# encoding: UTF-8
"""
"""

import hashlib
import hmac
import sys
import time
import json
import base64
import zlib
from copy import copy
from datetime import datetime, timedelta
from threading import Lock
from urllib.parse import urlencode

from requests import ConnectionError

from vnpy.api.rest import Request, RestClient
from vnpy.api.websocket import WebsocketClient
from vnpy.trader.constant import (
    Direction,
    Exchange,
    OrderType,
    Product,
    Status,
    Interval,
)
from vnpy.trader.gateway import BaseGateway
from vnpy.trader.object import (
    TickData,
    OrderData,
    TradeData,
    AccountData,
    ContractData,
    OrderRequest,
    CancelRequest,
    SubscribeRequest,
    HistoryRequest,
    BarData,
)

REST_HOST = "https://www.okex.com"
WEBSOCKET_HOST = "wss://real.okex.com:10442/ws/v3"

STATUS_OKEX2VT = {
    "ordering": Status.SUBMITTING,
    "open": Status.NOTTRADED,
    "part_filled": Status.PARTTRADED,
    "filled": Status.ALLTRADED,
    "cancelled": Status.CANCELLED,
    "cancelling": Status.CANCELLED,
    "failure": Status.REJECTED,
}

DIRECTION_VT2OKEX = {Direction.LONG: "buy", Direction.SHORT: "sell"}
DIRECTION_OKEX2VT = {v: k for k, v in DIRECTION_VT2OKEX.items()}

ORDERTYPE_VT2OKEX = {
    OrderType.LIMIT: "limit",
    OrderType.MARKET: "market"
}
ORDERTYPE_OKEX2VT = {v: k for k, v in ORDERTYPE_VT2OKEX.items()}

INTERVAL_OKEX2VT = {
    "60": Interval.MINUTE,
    "180": Interval.MINUTE3,
    "300": Interval.MINUTE5,
    "900": Interval.MINUTE15,
    "1800": Interval.MINUTE30,
    "3600": Interval.HOUR,
    "7200": Interval.HOUR2,
    "14400": Interval.HOUR4,
    "21600": Interval.HOUR6,
    "43200": Interval.HOUR12,
    "86400": Interval.DAILY,
}
INTERVAL_VT2OKEX = {v: k for k, v in INTERVAL_OKEX2VT.items()}

# 这里的两个变量都是instruments合约的内容
# 订阅的时候，订阅所有频道的成交
instruments = set()
currencies = set()

# 这里添加我自己的订阅，在subscribe_topic这里就有
mysubscribe = ["BTC-USDT", "ETH-USDT", "LTC-USDT", "EOS-USDT", "BCH-USDT"]


class OkexGateway(BaseGateway):
    """
    VN Trader Gateway for OKEX connection.
    """

    default_setting = {
        "API Key": "",
        "Secret Key": "",
        "Passphrase": "",
        "会话数": 3,
        "代理地址": "",
        "代理端口": "",
    }

    exchanges = [Exchange.OKEX]

    def __init__(self, event_engine):
        """Constructor"""
        super(OkexGateway, self).__init__(event_engine, "OKEX")

        self.rest_api = OkexRestApi(self)
        self.ws_api = OkexWebsocketApi(self)

        self.orders = {}

    def connect(self, setting: dict):
        """输入配置，进行连接，restful和websocket的连接"""
        key = setting["API Key"]
        secret = setting["Secret Key"]
        passphrase = setting["Passphrase"]
        session_number = setting["会话数"]
        proxy_host = setting["代理地址"]
        proxy_port = setting["代理端口"]

        if proxy_port.isdigit():
            proxy_port = int(proxy_port)
        else:
            proxy_port = 0

        self.rest_api.connect(key, secret, passphrase,
                              session_number, proxy_host, proxy_port)
        self.ws_api.connect(key, secret, passphrase, proxy_host, proxy_port)

    def subscribe(self, req: SubscribeRequest):
        """订阅，websocket订阅"""
        self.ws_api.subscribe(req)

    def send_order(self, req: OrderRequest):
        """发单，restful发单"""
        return self.rest_api.send_order(req)

    def cancel_order(self, req: CancelRequest):
        """取消发单，restful取消"""
        self.rest_api.cancel_order(req)

    def query_account(self):
        """查询账户, restful查询"""
        # 这里原来是pass，修改为restful查询
        self.rest_api.query_account()

    def query_position(self):
        """查询持仓，restful查询"""
        # 这里原来是pass，修改为restful查询
        self.rest_api.query_contract()

    def close(self):
        """关闭两个api"""
        self.rest_api.stop()
        self.ws_api.stop()

    def on_order(self, order: OrderData):
        """返回order"""
        self.orders[order.orderid] = order
        super().on_order(order)

    def get_order(self, orderid: str):
        """根据订单号，获取order"""
        return self.orders.get(orderid, None)

    def query_history(self, req: HistoryRequest):
        """"""
        self.rest_api.query_history(req)


class OkexRestApi(RestClient):
    """
    OKEX REST API
    """

    def __init__(self, gateway: BaseGateway):
        """"""
        super(OkexRestApi, self).__init__()

        self.gateway = gateway
        self.gateway_name = gateway.gateway_name

        self.key = ""
        self.secret = ""
        self.passphrase = ""

        self.order_count = 10000
        self.order_count_lock = Lock()

        # 在connect的时候修改
        self.connect_time = 0

    def sign(self, request):
        """
        Generate OKEX signature.
        """
        # Sign
        # timestamp = str(time.time())
        # Okex时间戳设计
        timestamp = get_timestamp()
        # 将请求变成json
        request.data = json.dumps(request.data)

        # 如果有参数，路径为
        if request.params:
            path = request.path + '?' + urlencode(request.params)
        else:
            path = request.path

        # 发送的消息为，时间戳 + 请求方式 + 路径 + 数据
        msg = timestamp + request.method + path + request.data
        # 签名为 消息 + secret_key
        signature = generate_signature(msg, self.secret)

        # Add headers
        # 添加头
        request.headers = {
            'OK-ACCESS-KEY': self.key,
            'OK-ACCESS-SIGN': signature,
            'OK-ACCESS-TIMESTAMP': timestamp,
            'OK-ACCESS-PASSPHRASE': self.passphrase,
            'Content-Type': 'application/json'
        }
        # 返回request
        return request

    def connect(
        self,
        key: str,
        secret: str,
        passphrase: str,
        session_number: int,
        proxy_host: str,
        proxy_port: int,
    ):
        """
        Initialize connection to REST server.
        """
        self.key = key
        self.secret = secret.encode()
        self.passphrase = passphrase

        self.connect_time = int(datetime.now().strftime("%y%m%d%H%M%S"))

        # 初始化，添加HOST，添加代理HOST， 添加代理PORT
        self.init(REST_HOST, proxy_host, proxy_port)
        # 开启进程，这里开始父类的进程，开始运行
        self.start(session_number)
        self.gateway.write_log("REST API启动成功")

        # 查询Okex官方时间，接口时间
        self.query_time()
        # 查询所有合约
        self.query_contract()
        # 查询所有账户
        self.query_account()
        # 查询所有订单
        self.query_order()

    def _new_order_id(self):
        """生成新的订单号，这个函数保证接口的订单号不会重复"""
        with self.order_count_lock:
            self.order_count += 1
            return self.order_count

    def send_order(self, req: OrderRequest):
        """币币下单"""

        # Id组成用spot做前缀来下单
        orderid = f"spot{self.connect_time}{self._new_order_id()}"

        # 公用字段：client_id, type, side, instrument_id, order_type
        data = {
            "client_oid": orderid,
            "type": ORDERTYPE_VT2OKEX[req.type],
            "side": DIRECTION_VT2OKEX[req.direction],
            "instrument_id": req.symbol
        }

        # 如果是市价单，卖出必填size，买入必填notional
        if req.type == OrderType.MARKET:
            if req.direction == Direction.LONG:
                data["notional"] = req.volume
            else:
                data["size"] = req.volume
        # 如果是限价单，必填price和size
        else:
            data["price"] = req.price
            data["size"] = req.volume

        order = req.create_order_data(orderid, self.gateway_name)

        self.add_request(
            "POST",
            "/api/spot/v3/orders",
            callback=self.on_send_order,
            data=data,
            extra=order,
            on_failed=self.on_send_order_failed,
            on_error=self.on_send_order_error,
        )

        self.gateway.on_order(order)
        return order.vt_orderid

    def cancel_order(self, req: CancelRequest):
        """取消订单"""
        data = {
            "instrument_id": req.symbol,
            "client_oid": req.orderid
        }

        path = "/api/spot/v3/cancel_orders/" + req.orderid
        self.add_request(
            "POST",
            path,
            callback=self.on_cancel_order,
            data=data,
            on_error=self.on_cancel_order_error,
            on_failed=self.on_cancel_order_failed,
            extra=req
        )

    def query_contract(self):
        """获取币对信息"""
        self.add_request(
            "GET",
            "/api/spot/v3/instruments",
            callback=self.on_query_contract
        )

    def query_account(self):
        """查询币币账户信息，spot accounts"""
        self.add_request(
            "GET",
            "/api/spot/v3/accounts",
            callback=self.on_query_account
        )

    def query_order(self):
        """获取所有未成交订单"""
        self.add_request(
            "GET",
            "/api/spot/v3/orders_pending",
            callback=self.on_query_order
        )

    def query_time(self):
        """查询Okex v3接口的时间"""
        self.add_request(
            "GET",
            "/api/general/v3/time",
            callback=self.on_query_time
        )

    def on_query_contract(self, data, request):
        """查询合约的回调函数"""
        for instrument_data in data:
            symbol = instrument_data["instrument_id"]
            contract = ContractData(
                symbol=symbol,
                exchange=Exchange.OKEX,
                name=symbol,
                product=Product.SPOT,
                size=1,
                pricetick=float(instrument_data["tick_size"]),
                min_volume=float(instrument_data["min_size"]),
                gateway_name=self.gateway_name
            )
            # 查询之后，推送合约
            self.gateway.on_contract(contract)

            # 添加合约
            instruments.add(instrument_data["instrument_id"])
            # 添加交易货币币种
            currencies.add(instrument_data["base_currency"])
            # 添加计价货币币种
            currencies.add(instrument_data["quote_currency"])

        self.gateway.write_log("合约信息查询成功")

        # Start websocket api after instruments data collected
        # 这里的设计，当查询完合约之后，要启动websocket api
        self.gateway.ws_api.start()

    def on_query_account(self, data, request):
        """查询账户的回调函数"""
        for account_data in data:
            account = AccountData(
                accountid=account_data["currency"],
                balance=float(account_data["balance"]),
                frozen=float(account_data["hold"]),
                gateway_name=self.gateway_name
            )
            self.gateway.on_account(account)

        self.gateway.write_log("账户资金查询成功")

    def on_query_order(self, data, request):
        """查询订单的回调函数"""
        for order_data in data:
            order = OrderData(
                symbol=order_data["instrument_id"],
                exchange=Exchange.OKEX,
                type=ORDERTYPE_OKEX2VT[order_data["type"]],
                orderid=order_data["client_oid"],
                direction=DIRECTION_OKEX2VT[order_data["side"]],
                price=float(order_data["price"]),
                volume=float(order_data["size"]),
                traded=float(order_data["filled_size"]),
                time=order_data["timestamp"][11:19],
                status=STATUS_OKEX2VT[order_data["status"]],
                gateway_name=self.gateway_name,
            )
            self.gateway.on_order(order)

        self.gateway.write_log("委托信息查询成功")

    def on_query_time(self, data, request):
        """查询时间的回调函数，写入日志，msg包括服务器时间和本机时间"""
        server_time = data["iso"]
        local_time = datetime.utcnow().isoformat()
        msg = f"服务器时间：{server_time}，本机时间：{local_time}"
        self.gateway.write_log(msg)

    def on_send_order_failed(self, status_code: str, request: Request):
        """
        Callback when sending order failed on server.
        当下单失败的时候，调用，一般是下单有误，发送这个东西
        """
        order = request.extra
        order.status = Status.REJECTED
        self.gateway.on_order(order)

        msg = f"委托失败，状态码：{status_code}，信息：{request.response.text}"
        self.gateway.write_log(msg)

    def on_send_order_error(
        self, exception_type: type, exception_value: Exception, tb, request: Request
    ):
        """
        Callback when sending order caused exception.
        发单失败，一般是交易所那边出现问题。
        """
        order = request.extra
        order.status = Status.REJECTED
        self.gateway.on_order(order)

        # Record exception if not ConnectionError
        if not issubclass(exception_type, ConnectionError):
            self.on_error(exception_type, exception_value, tb, request)

    def on_send_order(self, data, request):
        """Websocket will push a new order status
        正确发单返回，包括拒绝单"""
        order = request.extra

        error_msg = data["error_message"]
        if error_msg:
            order.status = Status.REJECTED
            self.gateway.on_order(order)

            self.gateway.write_log(f"委托失败：{error_msg}")

    def on_cancel_order_error(
        self, exception_type: type, exception_value: Exception, tb, request: Request
    ):
        """
        Callback when cancelling order failed on server.
        """
        # Record exception if not ConnectionError
        if not issubclass(exception_type, ConnectionError):
            self.on_error(exception_type, exception_value, tb, request)

    def on_cancel_order(self, data, request):
        """Websocket will push a new order status
        这里订阅websocket的order频道，所有正常order状态返回，都是由websocket来推送"""
        pass

    def on_cancel_order_failed(self, status_code: int, request: Request):
        """If cancel failed, mark order status to be rejected.
        当取消订单错误的时候，回调这个"""
        req = request.extra
        order = self.gateway.get_order(req.orderid)
        if order:
            order.status = Status.REJECTED
            self.gateway.on_order(order)

    def on_failed(self, status_code: int, request: Request):
        """
        Callback to handle request failed.
        """
        msg = f"请求失败，状态码：{status_code}，信息：{request.response.text}"
        self.gateway.write_log(msg)

    def on_error(
        self, exception_type: type, exception_value: Exception, tb, request: Request
    ):
        """
        Callback to handler request exception.
        """
        msg = f"触发异常，状态码：{exception_type}，信息：{exception_value}"
        self.gateway.write_log(msg)

        sys.stderr.write(
            self.exception_detail(exception_type, exception_value, tb, request)
        )

    def query_history(self, req: HistoryRequest):
        """这里有待改进，因为Okex推送2000条数据，但是这里默认只推送200条"""
        params = {
            "granularity": INTERVAL_VT2OKEX[req.interval],
            "start": "",
            "end": "",
        }

        path = "/api/spot/v3/instruments/" + req.symbol + "/candles"
        self.add_request(
            "GET",
            path,
            callback=self.on_query_history,
            params=params,
            on_error=self.on_error,
            on_failed=self.on_failed,
            extra=req
        )

    def on_query_history(self, data, req):
        """"""
        # OKEX返回的是倒叙的K线,需要重新排列
        for i in range(len(data)):
            d = data[-i - 1]
            dt = utc_to_local(d[0])
            bar = BarData(
                symbol=req.extra.symbol,
                exchange=req.extra.exchange,
                datetime=dt,
                interval=req.extra.interval,
                volume=float(d[5]),
                open_price=float(d[1]),
                high_price=float(d[2]),
                low_price=float(d[3]),
                close_price=float(d[4]),
                gateway_name=self.gateway_name
            )
            self.gateway.on_bar(bar)


class OkexWebsocketApi(WebsocketClient):
    """websocket api"""

    def __init__(self, gateway):
        """"""
        super(OkexWebsocketApi, self).__init__()
        self.ping_interval = 30     # OKEX use 30 seconds for ping

        self.gateway = gateway
        self.gateway_name = gateway.gateway_name

        self.key = ""
        self.secret = ""
        self.passphrase = ""

        self.trade_count = 10000
        self.connect_time = 0

        self.callbacks = {}
        self.ticks = {}

    def connect(
        self,
        key: str,
        secret: str,
        passphrase: str,
        proxy_host: str,
        proxy_port: int
    ):
        """"""
        self.key = key
        self.secret = secret.encode()
        self.passphrase = passphrase

        # 连接时间
        self.connect_time = int(datetime.now().strftime("%y%m%d%H%M%S"))

        self.init(WEBSOCKET_HOST, proxy_host, proxy_port)
        # 这里本来是连接之后启动，重新构建之后，选择在restapi查询合约之后，启动websocket
        # self.start()

    def on_connected(self):
        """连接成功之后，登录"""
        self.gateway.write_log("Websocket API连接成功")
        self.login()

    def on_disconnected(self):
        """这里并不是Api连接断开的概念，而是交易所不让连接"""
        self.gateway.write_log("Websocket API连接断开")

    def login(self):
        """
        Need to login befores subscribe to websocket topic.
        先登录，后订阅，连接之后就登录
        """
        timestamp = str(time.time())

        msg = timestamp + 'GET' + '/users/self/verify'
        signature = generate_signature(msg, self.secret)

        req = {
            "op": "login",
            "args": [
                self.key,
                self.passphrase,
                timestamp,
                signature.decode("utf-8")
            ]
        }
        self.send_packet(req)
        self.callbacks['login'] = self.on_login

    def on_login(self, data: dict):
        """登录情况返回"""
        success = data.get("success", False)

        # 如果登录成功，订阅主题
        if success:
            self.gateway.write_log("Websocket API登录成功")
            self.subscribe_topic()
        else:
            self.gateway.write_log("Websocket API登录失败")

    def subscribe_topic(self):
        """
        Subscribe to all private topics.
        订阅ticker，5档行情，account账户，order订单，成交
        因此，账户信息的变动和order信息的变动，都在websocket里面订阅，不是restful里面订阅
        """
        self.callbacks["spot/ticker"] = self.on_ticker
        self.callbacks["spot/depth5"] = self.on_depth
        self.callbacks["spot/account"] = self.on_account
        self.callbacks["spot/order"] = self.on_order

        # Subscribe to order update
        # 在gateway的最上面写要订阅哪些合约，这里会自动订阅ticker和depth5
        channels = []
        for instrument_id in instruments:
            channel = f"spot/order:{instrument_id}"
            channels.append(channel)

        req = {
            "op": "subscribe",
            "args": channels
        }
        self.send_packet(req)

        # Subscribe to account update
        # 这里写订阅的账户信息，主要为币种的变化
        channels = []
        for currency in currencies:
            channel = f"spot/account:{currency}"
            channels.append(channel)

        req = {
            "op": "subscribe",
            "args": channels
        }
        self.send_packet(req)

        # Subscribe to BTC/USDT trade for keep connection alive
        # 这里一直订阅BTC-USDT以保证连接
        req = {
            "op": "subscribe",
            "args": ["spot/trade:BTC-USDT"]
        }
        self.send_packet(req)

        # 这里添加gateway的文件，以保证能够一开始就订阅，这里算是改动比较大的了
        for spot in mysubscribe:
            req = SubscribeRequest(symbol=spot, exchange=Exchange.OKEX)
            self.subscribe(req)

    def subscribe(self, req: SubscribeRequest):
        """
        Subscribe to tick data upate.
        """
        tick = TickData(
            symbol=req.symbol,
            exchange=req.exchange,
            name=req.symbol,
            datetime=datetime.now(),
            gateway_name=self.gateway_name,
        )
        # 先在websocket gateway这里面存储tick
        self.ticks[req.symbol] = tick

        # 订阅ticker和5档深度行情频道
        channel_ticker = f"spot/ticker:{req.symbol}"
        channel_depth = f"spot/depth5:{req.symbol}"

        # 设置回调函数
        self.callbacks[channel_ticker] = self.on_ticker
        self.callbacks[channel_depth] = self.on_depth

        # 订阅行情，这里的订阅行情，是一个一个订阅的。
        req = {
            "op": "subscribe",
            "args": [channel_ticker, channel_depth]
        }
        self.send_packet(req)

    def unpack_data(self, data):
        """okex的解压数据的方式"""
        return json.loads(zlib.decompress(data, -zlib.MAX_WBITS))

    def on_packet(self, packet: dict):
        """收到数据的解压"""
        if "event" in packet:
            event = packet["event"]
            # 订阅返回
            if event == "subscribe":
                return
            # 错误返回
            elif event == "error":
                msg = packet["message"]
                self.gateway.write_log(f"Websocket API请求异常：{msg}")
            # 登录返回
            elif event == "login":
                self.on_login(packet)
        else:
            # 订阅的频道，table
            channel = packet["table"]
            # 订阅的数据
            data = packet["data"]
            # 订阅的频道
            callback = self.callbacks.get(channel, None)

            # 如果有回调函数，对每个data，调用回调函数
            if callback:
                for d in data:
                    callback(d)

    def on_ticker(self, d):
        """tick的回调函数"""
        symbol = d["instrument_id"]
        tick = self.ticks.get(symbol, None)
        if not tick:
            return

        tick.last_price = float(d["last"])
        tick.open = float(d["open_24h"])
        tick.high = float(d["high_24h"])
        tick.low = float(d["low_24h"])
        tick.volume = float(d["base_volume_24h"])
        tick.datetime = utc_to_local(d["timestamp"])
        """
        tick.datetime = datetime.strptime(
            d["timestamp"], "%Y-%m-%dT%H:%M:%S.%fZ")
        """
        self.gateway.on_tick(copy(tick))

    def on_depth(self, d):
        """5档行情的回调函数"""
        for tick_data in d:
            symbol = d["instrument_id"]
            tick = self.ticks.get(symbol, None)
            if not tick:
                return

            bids = d["bids"]
            asks = d["asks"]
            for n, buf in enumerate(bids):
                price, volume, _ = buf
                tick.__setattr__("bid_price_%s" % (n + 1), float(price))
                tick.__setattr__("bid_volume_%s" % (n + 1), float(volume))

            for n, buf in enumerate(asks):
                price, volume, _ = buf
                tick.__setattr__("ask_price_%s" % (n + 1), float(price))
                tick.__setattr__("ask_volume_%s" % (n + 1), float(volume))

            tick.datetime = utc_to_local(d["timestamp"])
            """
            tick.datetime = datetime.strptime(
                d["timestamp"], "%Y-%m-%dT%H:%M:%S.%fZ")
            """
            self.gateway.on_tick(copy(tick))

    def on_order(self, d):
        """order的回调函数"""
        order = OrderData(
            symbol=d["instrument_id"],
            exchange=Exchange.OKEX,
            type=ORDERTYPE_OKEX2VT[d["type"]],
            orderid=d["client_oid"],
            direction=DIRECTION_OKEX2VT[d["side"]],
            price=float(d["price"]),
            volume=float(d["size"]),
            traded=float(d["filled_size"]),
            time=d["timestamp"][11:19],
            status=STATUS_OKEX2VT[d["status"]],
            gateway_name=self.gateway_name,
        )
        # 向gateway里面推送order
        self.gateway.on_order(copy(order))

        # 这个字段的意思就是最新成交数量
        trade_volume = d.get("last_fill_qty", 0)
        if not trade_volume or float(trade_volume) == 0:
            return

        # 如果有成交，数量不为0，则进行以下操作,推送成交
        self.trade_count += 1
        tradeid = f"spot{self.connect_time}{self.trade_count}"

        # last_fill_px 最近成交价格，last_fill_time 最新成交时间
        trade = TradeData(
            symbol=order.symbol,
            exchange=order.exchange,
            orderid=order.orderid,
            tradeid=tradeid,
            direction=order.direction,
            price=float(d["last_fill_px"]),
            volume=float(trade_volume),
            time=d["last_fill_time"][11:19],
            gateway_name=self.gateway_name
        )
        self.gateway.on_trade(trade)

    def on_account(self, d):
        """"""
        account = AccountData(
            accountid=d["currency"],
            balance=float(d["balance"]),
            frozen=float(d["hold"]),
            gateway_name=self.gateway_name
        )

        # 推送账户信息，这里on_account的概念
        self.gateway.on_account(copy(account))

    def on_error(self, exception_type: type, exception_value: Exception, tb):
        """如果触发异常"""
        msg = f"触发异常，状态码：{exception_type}，信息：{exception_value}"
        self.gateway.write_log(msg)

        sys.stderr.write(self.exception_detail(
            exception_type, exception_value, tb))


def generate_signature(msg: str, secret_key: str):
    """OKEX V3 signature"""
    return base64.b64encode(hmac.new(secret_key, msg.encode(), hashlib.sha256).digest())


def get_timestamp():
    """"""
    now = datetime.utcnow()
    timestamp = now.isoformat("T", "milliseconds")
    return timestamp + "Z"


def utc_to_local(timestamp):
    time = datetime.strptime(timestamp, "%Y-%m-%dT%H:%M:%S.%fZ")
    utc_time = time + timedelta(hours=8)
    return utc_time