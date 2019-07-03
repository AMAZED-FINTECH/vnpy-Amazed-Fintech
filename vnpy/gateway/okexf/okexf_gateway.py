# encoding: UTF-8
"""
Author: qqqlyx
"""

import hashlib
import hmac
import sys
import time
import json
import base64
import zlib
from copy import copy, deepcopy
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
    Offset,
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
    PositionData,
    HistoryRequest,
    BarData,
)
REST_HOST = "https://www.okex.com"
WEBSOCKET_HOST = "wss://real.okex.com:10442/ws/v3"

STATUS_OKEXF2VT = {
    "-1": Status.CANCELLED,
    "0": Status.NOTTRADED,
    "1": Status.PARTTRADED,
    "2": Status.ALLTRADED,
    "3": Status.NOTTRADED,
    "6": Status.PARTTRADED,
    "7": Status.COMPLTE,
}

ORDERTYPE_OKEXF2VT = {
    "0": OrderType.LIMIT,
    "1": OrderType.MARKET,
}

TYPE_OKEXF2VT = {
    "1": (Offset.OPEN, Direction.LONG),
    "2": (Offset.OPEN, Direction.SHORT),
    "3": (Offset.CLOSE, Direction.SHORT),
    "4": (Offset.CLOSE, Direction.LONG),
}
TYPE_VT2OKEXF = {v: k for k, v in TYPE_OKEXF2VT.items()}

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


instruments = set()
currencies = set()
sub_instruments = ["BTC-USD-190927",
                   "ETH-USD-190927",
                   "LTC-USD-190927",
                   "EOS-USD-190927",
                   "BCH-USD-190927",
                   ]


class OkexfGateway(BaseGateway):
    """
    OKEX FUTURES 交割合约 connection.
    """

    default_setting = {
        "API Key": "",
        "Secret Key": "",
        "Passphrase": "",
        "Leverage": 10,  
        "会话数": 3,
        "代理地址": "",
        "代理端口": "",
    }

    exchanges = [Exchange.OKEXF]

    def __init__(self, event_engine):
        """Constructor"""
        super(OkexfGateway, self).__init__(event_engine, "OKEXF")

        self.rest_api = OkexfRestApi(self)
        self.ws_api = OkexfWebsocketApi(self)

        self.orders = {}

    def connect(self, setting: dict):
        """"""
        key = setting["API Key"]
        secret = setting["Secret Key"]
        passphrase = setting["Passphrase"]
        session_number = setting["会话数"]
        leverage = setting["Leverage"]
        proxy_host = setting["代理地址"]
        proxy_port = setting["代理端口"]

        if proxy_port.isdigit():
            proxy_port = int(proxy_port)
        else:
            proxy_port = 0

        self.rest_api.connect(key, secret, passphrase, leverage,
                              session_number, proxy_host, proxy_port)
        self.ws_api.connect(key, secret, passphrase, proxy_host, proxy_port)

    def subscribe(self, req: SubscribeRequest):
        """websocket订阅"""
        self.ws_api.subscribe(req)

    def send_order(self, req: OrderRequest):
        """restful发单"""
        return self.rest_api.send_order(req)

    def cancel_order(self, req: CancelRequest):
        """restful取消发单"""
        self.rest_api.cancel_order(req)

    def query_account(self):
        """查询账户"""
        self.rest_api.query_account()

    def query_position(self):
        """查询持仓"""
        self.rest_api.query_position()

    def query_contract(self):
        """查询合约"""
        self.rest_api.query_contract()

    def close(self):
        """"""
        self.rest_api.stop()
        self.ws_api.stop()

    def on_order(self, order: OrderData):
        """"""
        self.orders[order.orderid] = order
        super().on_order(order)

    def get_order(self, orderid: str):
        """"""
        return self.orders.get(orderid, None)

    def query_history(self, req: HistoryRequest):
        """"""
        self.rest_api.query_history(req)


class OkexfRestApi(RestClient):
    """
    OKEXF REST API
    """

    def __init__(self, gateway: BaseGateway):
        """"""
        super(OkexfRestApi, self).__init__()

        self.gateway = gateway
        self.gateway_name = gateway.gateway_name

        self.key = ""
        self.secret = ""
        self.passphrase = ""
        self.leverage = 0

        self.order_count = 10000
        self.order_count_lock = Lock()

        self.connect_time = 0

    def sign(self, request):
        """
        Generate OKEXF signature.
        """
        # Sign
        timestamp = get_timestamp()
        request.data = json.dumps(request.data)

        if request.params:
            path = request.path + "?" + urlencode(request.params)
        else:
            path = request.path

        msg = timestamp + request.method + path + request.data
        signature = generate_signature(msg, self.secret)

        # Add headers
        request.headers = {
            "OK-ACCESS-KEY": self.key,
            "OK-ACCESS-SIGN": signature,
            "OK-ACCESS-TIMESTAMP": timestamp,
            "OK-ACCESS-PASSPHRASE": self.passphrase,
            "Content-Type": "application/json"
        }
        return request

    def connect(
        self,
        key: str,
        secret: str,
        passphrase: str,
        leverage: int,
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
        self.leverage = leverage

        self.connect_time = int(datetime.now().strftime("%Y%m%d%H%M%S"))

        self.init(REST_HOST, proxy_host, proxy_port)
        self.start(session_number)
        self.gateway.write_log("REST API启动成功")

        # 查询时间
        self.query_time()
        # 查询合约
        self.query_contract()
        # 查询账户
        self.query_account()
        # 查询持仓
        self.query_position()

    def _new_order_id(self):
        with self.order_count_lock:
            self.order_count += 1
            return self.order_count

    def send_order(self, req: OrderRequest):
        """下单"""
        # Need both offset and direction for sending order.
        if (req.offset, req.direction) not in TYPE_VT2OKEXF:
            return ""

        orderid = f"futures{self.connect_time}{self._new_order_id()}"
        
        data = {
            "client_oid": orderid,
            "type": TYPE_VT2OKEXF[(req.offset, req.direction)],
            "instrument_id": req.symbol,
            "price": str(req.price),
            "size": str(int(req.volume)),
            "leverage": self.leverage,
        }

        if req.type == OrderType.MARKET:
            data["match_price"] = "1"
        else:
            data["match_price"] = "0"

        order = req.create_order_data(orderid, self.gateway_name)

        self.add_request(
            "POST",
            "/api/futures/v3/order",
            callback=self.on_send_order,
            data=data,
            extra=order,
            on_failed=self.on_send_order_failed,
            on_error=self.on_send_order_error,
        )

        self.gateway.on_order(copy(order))
        return order.vt_orderid

    def cancel_order(self, req: CancelRequest):
        """取消下单"""
        path = "/api/futures/v3/cancel_order/%s/%s" % (req.symbol, req.orderid)
        self.add_request(
            "POST",
            path,
            callback=self.on_cancel_order,
            on_error=self.on_cancel_order_error,
            on_failed=self.on_cancel_order_failed,
            extra=req
        )

    def query_contract(self):
        """查询合约，这里和现货不一样，每周会断开，断开之后要重连，重连合约会变
        重置合约，重置订阅"""
        self.add_request(
            "GET",
            "/api/futures/v3/instruments",
            callback=self.on_query_contract
        )

    def query_account(self):
        """查询账户"""
        self.add_request(
            "GET",
            "/api/futures/v3/accounts",
            callback=self.on_query_account
        )

    def query_order(self):
        """查询订单"""
        for code in instruments:

            # get waiting orders
            self.add_request(
                "GET",
                "/api/futures/v3/orders/%s?state=0" % (code),
                callback=self.on_query_order
            )

            # get part traded orders
            self.add_request(
                "GET",
                "/api/futures/v3/orders/%s?state=1" % (code),
                callback=self.on_query_order
            )

    def query_position(self):
        """查询持仓"""
        # 这里要修改一下，每次查询持仓之前，要把当前持仓清空
        instruments = set()
        self.add_request(
            "GET",
            "/api/futures/v3/position",
            callback=self.on_query_position
        )

    def query_time(self):
        """查询时间"""
        self.add_request(
            "GET",
            "/api/general/v3/time",
            callback=self.on_query_time
        )

    def on_query_contract(self, data, request):
        """查询合约,这里是每次调用查询合约，要清空合约和货币"""

        instruments.clear()
        currencies.clear()

        for instrument_data in data:
            symbol = instrument_data["instrument_id"]
            contract = ContractData(
                symbol=symbol,
                exchange=Exchange.OKEXF,
                name=symbol,
                product=Product.FUTURES,
                size=int(instrument_data["trade_increment"]),
                pricetick=float(instrument_data["tick_size"]),
                gateway_name=self.gateway_name,
            )
            self.gateway.on_contract(contract)

            instruments.add(instrument_data["instrument_id"])
            currencies.add(instrument_data["underlying_index"])
            currencies.add(instrument_data["quote_currency"])

        self.gateway.write_log("合约信息查询成功")

    def on_query_account(self, data, request):
        """查询账户"""

        for currency, d in data["info"].items():
            account = AccountData(
                accountid=currency.upper(),
                balance=float(d["equity"]),
                frozen=float(d.get("margin_for_unfilled", 0)),
                gateway_name=self.gateway_name,
            )  
            self.gateway.on_account(account)      
        self.gateway.write_log("账户资金查询成功")

        # Start websocket api after instruments data collected
        # 这里本来是查询合约之后可以订阅，但是，由于OKEX FUTURES合约变更
        # 导致重连之后，立刻就需要重新查询合约
        # 否则，在subscribe_topic那里就会出现订阅不存在的合约问题
        # 所以，将启动websocket线程的功能设置在查询账户之后，也就是restapi启动之后
        self.gateway.ws_api.start()

        # and query pending orders
        self.query_order()

    def on_query_position(self, data, request):
        """查询持仓"""
        if not data["holding"]:
            return

        for pos_data in data["holding"][0]:
            if float(pos_data["long_qty"]) > 0:
                pos = PositionData(
                    symbol=pos_data["instrument_id"].upper(),
                    exchange=Exchange.OKEXF,
                    direction=Direction.LONG,
                    volume=float(pos_data["long_qty"]),
                    frozen=float(pos_data["long_qty"]) - float(pos_data["long_avail_qty"]),
                    price=float(pos_data["long_avg_cost"]),
                    pnl=float(pos_data["realised_pnl"]),
                    gateway_name=self.gateway_name,
                )
                self.gateway.on_position(pos)

            if float(pos_data["short_qty"]) > 0:
                pos = PositionData(
                    symbol=pos_data["instrument_id"],
                    exchange=Exchange.OKEXF,
                    direction=Direction.SHORT,
                    volume=float(pos_data["short_qty"]),
                    frozen=float(pos_data["short_qty"]) - float(pos_data["short_avail_qty"]),
                    price=float(pos_data["short_avg_cost"]),
                    pnl=float(pos_data["realised_pnl"]),
                    gateway_name=self.gateway_name,
                )
                self.gateway.on_position(copy(pos))

    def on_query_order(self, data, request):
        """查询订单"""
        # 这里是查询订单，order_data是OKEX的订单，OrderData是本地需要保存的订单
        for order_data in data["order_info"]:
            offset, direction = TYPE_OKEXF2VT[order_data["type"]]
            order = OrderData(
                symbol=order_data["instrument_id"],
                exchange=Exchange.OKEXF,
                type=ORDERTYPE_OKEXF2VT[order_data["order_type"]],
                orderid=order_data["client_oid"],
                direction=direction,
                offset=offset,
                traded=int(order_data["filled_qty"]),
                price=float(order_data["price"]),
                volume=float(order_data["size"]),
                time=utc_to_local(order_data["timestamp"]).strftime("%Y-%m-%d %H:%M:%S.%fZ"),
                status=STATUS_OKEXF2VT[order_data["state"]],
                gateway_name=self.gateway_name,
            )
            self.gateway.on_order(copy(order))

    def on_query_time(self, data, request):
        """查询时间"""
        server_time = data["iso"]
        local_time = datetime.utcnow().isoformat()
        msg = f"服务器时间：{server_time}，本机时间：{local_time}"
        self.gateway.write_log(msg)

    def on_send_order_failed(self, status_code: str, request: Request):
        """
        Callback when sending order failed on server.
        下单失败的回掉函数，这里也是本地的概念，无需修改status->state
        """

        order = request.extra
        order.status = Status.REJECTED
        order.time = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%fZ")
        self.gateway.on_order(order)
        msg = f"委托失败，状态码：{status_code}，信息：{request.response.text}"
        self.gateway.write_log(msg)

    def on_send_order_error(
        self, exception_type: type, exception_value: Exception, tb, request: Request
    ):
        """
        Callback when sending order caused exception.
        下单错误的回掉函数，这里也是本地的概念，无需修改status->state
        """

        order = request.extra
        order.status = Status.REJECTED
        self.gateway.on_order(order)

        # Record exception if not ConnectionError
        if not issubclass(exception_type, ConnectionError):
            self.on_error(exception_type, exception_value, tb, request)

    def on_send_order(self, data, request):
        """
        Websocket will push a new order status
        本地为Status，OKEX修改为state
        """
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
        """
        Websocket will push a new order status
        """
        pass

    def on_cancel_order_failed(self, status_code: int, request: Request):
        """
        If cancel failed, mark order status to be rejected.
        """
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
        """TODO 因为Okex推送2000条数据，但是这里默认只推送200条"""
        params = {
            "granularity": INTERVAL_VT2OKEX[req.interval],
            "start": "",
            "end": "",
        }

        path = "/api/futures/v3/instruments/" + req.symbol + "/candles"
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
        """调取历史数据"""
        # OKEX返回的是倒叙的K线,需要重新排列

        for i in range(len(data) - 1):
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
            self.gateway.on_bar(copy(bar))


class OkexfWebsocketApi(WebsocketClient):
    """"""

    def __init__(self, gateway):
        """"""
        super(OkexfWebsocketApi, self).__init__()
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
        self.bars = {}
        self.sub_flags = {}

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

        self.connect_time = int(datetime.now().strftime("%Y%m%d%H%M%S"))

        self.init(WEBSOCKET_HOST, proxy_host, proxy_port)

    def unpack_data(self, data):
        """"""
        return json.loads(zlib.decompress(data, -zlib.MAX_WBITS))

    def subscribe(self, req: SubscribeRequest):
        """
        Subscribe to tick data upate.
        订阅ticker和bar
        """
        tick = TickData(
            symbol=req.symbol,
            exchange=req.exchange,
            name=req.symbol,
            datetime=datetime.now(),
            gateway_name=self.gateway_name,
        )
        self.ticks[req.symbol] = tick

        bar = BarData(
            symbol=req.symbol,
            exchange=req.exchange,
            datetime=copy(datetime.now()),
            interval=Interval.MINUTE,
            gateway_name=self.gateway_name,
        )
        self.bars[req.symbol] = bar
        self.sub_flags[req.symbol] = True

        channel_ticker = f"futures/ticker:{req.symbol}"
        channel_depth = f"futures/depth5:{req.symbol}"
        channel_candle = f"futures/candle60s:{req.symbol}"

        self.callbacks[channel_ticker] = self.on_ticker
        self.callbacks[channel_depth] = self.on_depth
        self.callbacks[channel_candle] = self.on_candle

        req = {
            "op": "subscribe",
            "args": [channel_ticker, channel_depth, channel_candle]
        }
        self.send_packet(req)

    def on_connected(self):
        """OKEX 期货重连之后，要重新订阅"""
        self.gateway.write_log("Websocket API连接成功")
        self.login()

    def on_disconnected(self):
        """"""
        self.gateway.write_log("Websocket API连接断开")
        # 由于是restful查询合约，因此，断开websocket之后，重新查询用
        self.gateway.query_contract()
        # 由于这里是在原来的基础上新加的一个功能
        # 因此，为了避免login->on_login->subscribe_topic->instruments
        # 为了避免调用instruments时, instruments不存在的问题，因此要用time.sleep(1)
        # 由于OKEX的Ping是30秒，因此，1秒钟的影响，暂时先忽略不计

    def on_packet(self, packet: dict):
        """"""
        if "event" in packet:
            event = packet["event"]
            if event == "subscribe":
                return
            elif event == "error":
                msg = packet["message"]
                self.gateway.write_log(f"Websocket API请求异常：{msg}")
            elif event == "login":
                self.on_login(packet)
        else:
            channel = packet["table"]
            data = packet["data"]
            callback = self.callbacks.get(channel, None)

            if callback:
                for d in data:
                    callback(d)

    def on_error(self, exception_type: type, exception_value: Exception, tb):
        """"""
        msg = f"触发异常，状态码：{exception_type}，信息：{exception_value}"
        self.gateway.write_log(msg)

        sys.stderr.write(self.exception_detail(exception_type, exception_value, tb))

    def login(self):
        """
        Need to login befores subscribe to websocket topic.
        """
        timestamp = str(time.time())

        msg = timestamp + "GET" + "/users/self/verify"
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
        self.callbacks["login"] = self.on_login

    def subscribe_topic(self):
        """
        Subscribe to all private topics.
        """
        self.callbacks["futures/ticker"] = self.on_ticker
        self.callbacks["futures/depth5"] = self.on_depth
        self.callbacks["futures/account"] = self.on_account
        self.callbacks["futures/order"] = self.on_order
        self.callbacks["futures/position"] = self.on_position
        self.callbacks["futures/candle60s"] = self.on_candle

        # Subscribe to order update
        channels = []
        for instrument_id in instruments:
            channel = f"futures/order:{instrument_id}"
            channels.append(channel)

        req = {
            "op": "subscribe",
            "args": channels
        }
        self.send_packet(req)

        # Subscribe to account update
        channels = []
        for currency in currencies:
            if currency != "USD":
                channel = f"futures/account:{currency}"
                channels.append(channel)

        req = {
            "op": "subscribe",
            "args": channels
        }
        self.send_packet(req)

        # Subscribe to position update
        channels = []
        for instrument_id in instruments:
            channel = f"futures/position:{instrument_id}"
            channels.append(channel)

        req = {
            "op": "subscribe",
            "args": channels
        }
        self.send_packet(req)

        # 这里添加gateway的文件，以保证能够一开始就订阅，这里算是改动比较大的了
        # 为了减少服务器压力，这里只需要订阅需要的合约，也就是季度交割合约
        for futures in sub_instruments:
            req = SubscribeRequest(symbol=futures, exchange=Exchange.OKEXF)
            self.subscribe(req)

    def on_login(self, data: dict):
        """登录成功之后，自动订阅主题"""

        success = data.get("success", False)

        if success:
            self.gateway.write_log("Websocket API登录成功")
            self.subscribe_topic()
        else:
            self.gateway.write_log("Websocket API登录失败")

    def on_ticker(self, d):
        """"""
        symbol = d["instrument_id"]
        tick = self.ticks.get(symbol, None)
        if not tick:
            return

        tick.last_price = float(d["last"])
        tick.high_price = float(d["high_24h"])
        tick.low_price = float(d["low_24h"])
        tick.volume = float(d["volume_24h"])
        tick.datetime = utc_to_local(d["timestamp"])

        self.gateway.on_tick(copy(tick))

    def on_candle(self, d):
        """"""
        symbol = d["instrument_id"]
        bar = self.bars.get(symbol, None)
        if not bar:
            return

        # 由于OKEX每根Bar会推送很多次,不只一次,这里设置成,只有当这根Bar走完之后才推送
        candle = d["candle"]
        dt = utc_to_local(candle[0])

        # 如果时间戳和本地不等,推送Bar
        if dt != bar.datetime:
            if not self.sub_flags[symbol]:
                self.gateway.on_bar(copy(bar))
            else:
                self.sub_flags[symbol] = False
            bar.volume = float(candle[5])
            bar.open_price = float(candle[1])
            bar.high_price = float(candle[2])
            bar.low_price = float(candle[3])
            bar.close_price = float(candle[4])
            bar.datetime = dt
        else:
            bar.volume = float(candle[5])
            bar.open_price = float(candle[1])
            bar.high_price = float(candle[2])
            bar.low_price = float(candle[3])
            bar.close_price = float(candle[4])
            bar.datetime = dt

    def on_depth(self, d):
        """"""
        for tick_data in d:
            symbol = d["instrument_id"]
            tick = self.ticks.get(symbol, None)
            if not tick:
                return

            bids = d["bids"]
            asks = d["asks"]
            for n, buf in enumerate(bids):
                price, volume, _, __ = buf
                tick.__setattr__("bid_price_%s" % (n + 1), price)
                tick.__setattr__("bid_volume_%s" % (n + 1), volume)

            for n, buf in enumerate(asks):
                price, volume, _, __ = buf
                tick.__setattr__("ask_price_%s" % (n + 1), price)
                tick.__setattr__("ask_volume_%s" % (n + 1), volume)

            tick.datetime = utc_to_local(d["timestamp"])
            self.gateway.on_tick(copy(tick))

    def on_order(self, d):
        """这里是将收到的order转换成本地OrderData"""
        offset, direction = TYPE_OKEXF2VT[d["type"]]
        order = OrderData(
            symbol=d["instrument_id"],
            exchange=Exchange.OKEXF,
            type=ORDERTYPE_OKEXF2VT[d["order_type"]],
            orderid=d["client_oid"],
            offset=offset,
            direction=direction,
            price=float(d["price"]),
            volume=float(d["size"]),
            traded=float(d["filled_qty"]),
            time=utc_to_local(d["timestamp"]).strftime("%Y-%m-%d %H:%M:%S.%fZ"),
            status=STATUS_OKEXF2VT[d["state"]],
            gateway_name=self.gateway_name,
        )
        self.gateway.on_order(copy(order))

        trade_volume = d.get("last_fill_qty", 0)
        if not trade_volume or float(trade_volume) == 0:
            return

        self.trade_count += 1
        tradeid = f"{self.connect_time}{self.trade_count}"

        trade = TradeData(
            symbol=order.symbol,
            exchange=order.exchange,
            orderid=order.orderid,
            tradeid=tradeid,
            direction=order.direction,
            offset=order.offset,
            price=float(d["last_fill_px"]),
            volume=float(trade_volume),
            time=order.time,
            gateway_name=self.gateway_name,
        )
        self.gateway.on_trade(copy(trade))

    def on_account(self, d):
        """"""
        for key in d:
            account_data = d[key]
            account = AccountData(
                accountid=key,
                balance=float(account_data["equity"]),
                frozen=float(d.get("margin_for_unfilled", 0)),
                gateway_name=self.gateway_name
            )
            self.gateway.on_account(copy(account))

    def on_position(self, d):
        """"""
        pos = PositionData(
            symbol=d["instrument_id"],
            exchange=Exchange.OKEXF,
            direction=Direction.LONG,
            volume=float(d["long_qty"]),
            frozen=float(d["long_qty"]) - float(d["long_avail_qty"]),
            price=float(d["long_avg_cost"]),
            pnl=float(d["realised_pnl"]),
            gateway_name=self.gateway_name,
        )
        self.gateway.on_position(copy(pos))

        pos = PositionData(
            symbol=d["instrument_id"],
            exchange=Exchange.OKEXF,
            direction=Direction.SHORT,
            volume=float(d["short_qty"]),
            frozen=float(d["short_qty"]) - float(d["short_avail_qty"]),
            price=float(d["short_avg_cost"]),
            pnl=float(d["realised_pnl"]),
            gateway_name=self.gateway_name,
        )
        self.gateway.on_position(copy(pos))
        

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
