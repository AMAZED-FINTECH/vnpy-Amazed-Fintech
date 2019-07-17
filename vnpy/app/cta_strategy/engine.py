""""""

import importlib
import os
import traceback
from collections import defaultdict
from pathlib import Path
from typing import Any, Callable
from datetime import datetime, timedelta
from threading import Thread
from queue import Queue, Empty
from copy import copy, deepcopy
import time
import psutil
import os

from vnpy.event import Event, EventEngine
from vnpy.trader.engine import BaseEngine, MainEngine
from vnpy.trader.object import (
    OrderRequest,
    SubscribeRequest,
    HistoryRequest,
    LogData,
    TickData,
    BarData,
    ContractData
)
from vnpy.trader.event import (
    EVENT_TICK, 
    EVENT_ORDER, 
    EVENT_TRADE,
    EVENT_POSITION,
    EVENT_BAR,
    EVENT_ACCOUNT
)
from vnpy.trader.constant import (
    Direction, 
    OrderType, 
    Interval, 
    Exchange, 
    Offset, 
    Status
)
from vnpy.trader.utility import load_json, save_json, extract_vt_symbol, round_to
from vnpy.trader.database import database_manager
# from vnpy.trader.rqdata import rqdata_client

from .base import (
    APP_NAME,
    EVENT_CTA_LOG,
    EVENT_CTA_STRATEGY,
    EVENT_CTA_STOPORDER,
    EngineType,
    StopOrder,
    StopOrderStatus,
    STOPORDER_PREFIX
)
from .template import CtaTemplate
from .converter import OffsetConverter
from .DBMongo import dbMongo

STOP_STATUS_MAP = {
    Status.SUBMITTING: StopOrderStatus.WAITING,
    Status.NOTTRADED: StopOrderStatus.WAITING,
    Status.PARTTRADED: StopOrderStatus.TRIGGERED,
    Status.ALLTRADED: StopOrderStatus.TRIGGERED,
    Status.CANCELLED: StopOrderStatus.CANCELLED,
    Status.REJECTED: StopOrderStatus.CANCELLED
}


class CtaEngine(BaseEngine):
    """Cta引擎，提供Cta功能与主引擎的交互"""

    engine_type = EngineType.LIVE  # live trading engine

    # 配置文件
    setting_filename = "cta_strategy_setting.json"
    data_filename = "cta_strategy_data.json"
    setting_dbname = "cta_strategy_setting"
    data_dbname = "cta_strategy_data"
    account_id = "mytest"

    def __init__(self, main_engine: MainEngine, event_engine: EventEngine):
        """初始化，与其他模块一样，提供与主引擎交互的方法"""
        super(CtaEngine, self).__init__(
            main_engine, event_engine, APP_NAME)

        # 配置dict，数据dict
        self.strategy_setting = {}  # strategy_name: dict
        self.strategy_data = {}     # strategy_name: dict

        # 策略的类，策略名称
        self.classes = {}           # class_name: stategy_class
        self.strategies = {}        # strategy_name: strategy

        # 订阅的合约vt_symbol
        self.symbol_strategy_map = defaultdict(
            list)                   # vt_symbol: strategy list
        # 策略的order哪个orderid对应哪个strategy
        self.orderid_strategy_map = {}  # vt_orderid: strategy
        # 策略名称对应orderid，一个策略对应多个id
        self.strategy_orderid_map = defaultdict(
            set)                    # strategy_name: orderid list

        # 停止单下单数量
        self.stop_order_count = 0   # for generating stop_orderid
        # 停止单id
        self.stop_orders = {}       # stop_orderid: stop_order

        # 初始化线程
        self.init_thread = None
        # 初始化队列
        self.init_queue = Queue()

        self.rq_client = None
        self.rq_symbols = set()

        # 策略成交
        self.vt_tradeids = set()    # for filtering duplicate trade

        # 转换
        self.offset_converter = OffsetConverter(self.main_engine)

        # 自主添加
        # DB 数据库
        self.db_mongo = dbMongo()
        self.db_thread = None
        self.db_queue = Queue()
        self.db_active = False
        self.db_count = 0

    def init_engine(self):
        """
        初始化引擎，初始化米匡
        载入所有策略（类）
        载入所有策略配置
        载入所有策略数据
        注册事件
        引擎初始化
        """
        # 开启策略线程
        self.db_start()
        
        # self.init_rqdata()
        self.load_strategy_class()
        self.load_strategy_setting()
        self.load_strategy_data()
        self.register_event()
        self.write_log("CTA策略引擎初始化成功")

    def account_id_change(self, new_id):
        self.account_id = new_id

    def init_dbmongo(self, name=None, password=None, ip="localhost", port="27017"):
        self.db_name = name
        self.db_pwd = password
        self.db_mongo = dbMongo(name, password, ip, port)

    def close(self):
        """
        关闭所有策略
        """
        self.stop_all_strategies()

    def register_event(self):
        """注册事件，tick, order, trade, position, 少了一个account？"""
        self.event_engine.register(EVENT_TICK, self.process_tick_event)
        self.event_engine.register(EVENT_ORDER, self.process_order_event)
        self.event_engine.register(EVENT_TRADE, self.process_trade_event)
        self.event_engine.register(EVENT_POSITION, self.process_position_event)
        self.event_engine.register(EVENT_BAR, self.process_bar_event)
        self.event_engine.register(EVENT_ACCOUNT, self.process_account_event)

    def init_rqdata(self):
        """
        Init RQData client.
        """
        result = rqdata_client.init()
        if result:
            self.write_log("RQData数据接口初始化成功")

    def query_bar_from_rq(
        self, symbol: str, exchange: Exchange, interval: Interval, start: datetime, end: datetime
    ):
        """
        Query bar data from RQData.
        """
        req = HistoryRequest(
            symbol=symbol,
            exchange=exchange,
            interval=interval,
            start=start,
            end=end
        )
        data = rqdata_client.query_history(req)
        return data

    def process_tick_event(self, event: Event):
        """处理tick事件，主要是向订阅了tick的策略推送"""
        tick = event.data

        strategies = self.symbol_strategy_map[tick.vt_symbol]
        if not strategies:
            return

        # 先根据tick检查本地停止单
        self.check_stop_order(tick)

        # 如果有策略，推送至策略的on_tick情况，由tick合成bar，进而由策略自己使用
        for strategy in strategies:
            if strategy.inited:
                self.call_strategy_func(strategy, strategy.on_tick, tick)

    def process_bar_event(self, event: Event):
        """处理bar事件，主要是向订阅了bar的策略推送"""
        bar = deepcopy(event.data)

        strategies = self.symbol_strategy_map[bar.vt_symbol]
        if not strategies:
            return

        # Bar不检查停止单
        # self.check_stop_order(bar)

        # 如果有策略，推送至策略的on_bar情况，由分钟bar合成更大级别的bar，进而由策略自己使用
        for strategy in strategies:
            #if strategy.inited:
            # 日志记录时,要先显示发出,再由策略收到
            # self.write_log("engine process Bar_Data:" + str(bar.__dict__), strategy)
            self.call_strategy_func(strategy, strategy.on_bar, bar)

            # =================================
            # 这里在测试的时候,写入数据库和log两种形式

            # 实盘中,只写入数据库中
            d = deepcopy(bar.__dict__)
            d["account_id"] = self.account_id
            d["strategy_name"] = strategy.strategy_name
            d["exchange"] = d["exchange"].value
            d["interval"] = d["interval"].value
            flt = {
                "vt_symbol": d["vt_symbol"],
                "interval": d["interval"],
                "datetime": d["datetime"],
                   }
            self.db_queue.put(["update", self.account_id, "Bar_Data", d, flt])

            # =================================

    def process_order_event(self, event: Event):
        """处理order事件"""
        order = event.data
        d = deepcopy(order.__dict__)

        # 先转换order
        self.offset_converter.update_order(order)

        # 先根据订单号返回对应的策略
        strategy = self.orderid_strategy_map.get(order.vt_orderid, None)
        if not strategy:
            self.write_log("非程序化策略订单:" + str(d))
            return

        # =================================
        # 这里在测试的时候,写入数据库和log两种形式

        # 实盘中,只写入数据库中
        d["account_id"] = self.account_id
        d["strategy_name"] = strategy.strategy_name
        d["exchange"] = d["exchange"].value
        d["type"] = d["type"].value
        d["direction"] = d["direction"].value
        d["offset"] = d["offset"].value
        d["status"] = d["status"].value

        flt = {
            "vt_orderid": d["vt_orderid"],
            "volume": d["volume"],
            "status": d["status"],
        }
        self.db_queue.put(["update", self.account_id, "Order_Data", d, flt])

        self.write_log("Order_Data:" + str(d), strategy)
        # =================================

        # Remove vt_orderid if order is no longer active.
        # 如果order_id已经成交了或者撤销了，删除这个订单。
        # 先返回所有这个策略的订单号
        vt_orderids = self.strategy_orderid_map[strategy.strategy_name]
        if order.vt_orderid in vt_orderids and not order.is_active():
            vt_orderids.remove(order.vt_orderid)

        # For server stop order, call strategy on_stop_order function
        # 如果是停止单，推送停止单到对应的策略中去
        if order.type == OrderType.STOP:
            so = StopOrder(
                vt_symbol=order.vt_symbol,
                direction=order.direction,
                offset=order.offset,
                price=order.price,
                volume=order.volume,
                stop_orderid=order.vt_orderid,
                strategy_name=strategy.strategy_name,
                status=STOP_STATUS_MAP[order.status],
                vt_orderids=[order.vt_orderid],
            )
            self.call_strategy_func(strategy, strategy.on_stop_order, so)  

        # Call strategy on_order function
        # 最后，不管是停止单，还是删除编号的操作，最后都要调用策略的on_order操作
        self.call_strategy_func(strategy, strategy.on_order, order)

    def process_trade_event(self, event: Event):
        """处理成交事件"""
        trade = event.data

        d = deepcopy(trade.__dict__)
        # Filter duplicate trade push
        # 如果推送过来的成交，不是此次运行期间的单子
        if trade.vt_tradeid in self.vt_tradeids:
            return
        # 将成交的订单添加至vt_tradeids 添加至本引擎中去
        self.vt_tradeids.add(trade.vt_tradeid)

        # 转换成交
        self.offset_converter.update_trade(trade)

        # 获取这个成交对应的策略
        strategy = self.orderid_strategy_map.get(trade.vt_orderid, None)
        if not strategy:
            self.write_log("非程序化策略成交:" + str(d))
            return

        # =================================
        # 这里在测试的时候,写入数据库和log两种形式

        # 实盘中,只写入数据库中
        d["account_id"] = self.account_id
        d["strategy_name"] = strategy.strategy_name

        d["exchange"] = d["exchange"].value
        d["direction"] = d["direction"].value
        d["offset"] = d["offset"].value

        flt = {
            "vt_orderid": d["vt_orderid"],
            "vt_tradeid": d["vt_tradeid"],
        }
        self.db_queue.put(["update", self.account_id, "Trade_Data", d, flt])

        self.write_log("Trade_Data:" + str(d), strategy)
        # =================================

        # 如果是多单，计算持仓时符号为正，如果是空单，计算持仓时符号为负
        if trade.direction == Direction.LONG:
            strategy.pos += trade.volume
        else:
            strategy.pos -= trade.volume

        # 仓位variable每次更新完都要在本地更新
        self.sync_strategy_data(strategy)

        # 调用策略函数
        self.call_strategy_func(strategy, strategy.on_trade, trade)
        # 发送策略事件，也就是策略有任何一项改变，就要发出，这个在double_ma_strategy内中
        # on_start on_bar on_init on_stop中，只要策略发生一些东西的变化，就触发put_strategy_event这个函数
        self.put_strategy_event(strategy)

    def process_position_event(self, event: Event):
        """处理仓位 事件"""
        position = event.data

        d = deepcopy(position.__dict__)

        # 就是把他转换，然后更新，就好了
        self.offset_converter.update_position(position)

        # =================================
        # 这里在测试的时候,写入数据库和log两种形式

        # 实盘中,只写入数据库中
        d["account_id"] = self.account_id
        d["exchange"] = d["exchange"].value
        d["direction"] = d["direction"].value
        d["datetime"] = copy(datetime.now().strftime("%Y-%m-%d %H:%M:%S.%fZ"))
        if d["volume"] > 0:
            self.db_queue.put(["insert", self.account_id, "Position_Data", d])

        self.write_log("Position_Data:" + str(d))
        # =================================

    def process_account_event(self, event: Event):
        """处理账户 事件"""
        account = event.data

        d = deepcopy(account.__dict__)

        # =================================
        # 这里在测试的时候,写入数据库和log两种形式

        # 实盘中,只写入数据库中
        d["account_id"] = self.account_id
        d["datetime"] = copy(datetime.now().strftime("%Y-%m-%d %H:%M:%S.%fZ"))
        if d["balance"] > 0:
            self.db_queue.put(["insert", self.account_id, "Account_Data", d])

        self.write_log("Account_Data:" + str(d))
        # =================================

    def check_stop_order(self, tick: TickData):
        """检查停止单，每次收到tick的时候都要检查"""
        # 对于所有的当前停止单
        for stop_order in list(self.stop_orders.values()):
            # 如果停止单的vt_symbol与tick不同
            if stop_order.vt_symbol != tick.vt_symbol:
                # 进行下一个循环
                continue
            # 为了保证下单,要查看tick内涨停价和5档价格,如果都没有,返回
            if not tick.limit_up and not tick.bid_price_5:
                continue

            # 多头停止单,tick价格上穿止损价
            # 如果是buy的LONG + OPEN 的STOPORDER,当价格向上突破某一个价格的时候开仓
            # 如果是cover的LONG + CLOSE 的STOPORDER,当价格向上突破某一个价格的时候平仓,意思是止损
            long_triggered = (
                stop_order.direction == Direction.LONG and tick.last_price >= stop_order.price
            )
            # 空头停止单,tick价格下穿止损价
            # 如果是short的SHORT + OPEN 的STOPORDER,当价格向下突破某一个价格的时候开仓
            # 如果是sell的SHORT + CLOSE 的STOPORDER,当价格向下突破某一个价格的时候平仓,意思是止损
            short_triggered = (
                stop_order.direction == Direction.SHORT and tick.last_price <= stop_order.price
            )

            # 如果有触发条件的话
            if long_triggered or short_triggered:
                # 是哪个策略的停止单
                strategy = self.strategies[stop_order.strategy_name]

                # To get excuted immediately after stop order is
                # triggered, use limit price if available, otherwise
                # use ask_price_5 or bid_price_5
                # 停止单的下一步处理,如果有涨跌停,按涨跌停下单,如果没有涨跌停,按买五卖五价格下单
                if stop_order.direction == Direction.LONG:
                    if tick.limit_up:
                        price = tick.limit_up
                    else:
                        price = tick.ask_price_5
                else:
                    if tick.limit_down:
                        price = tick.limit_down
                    else:
                        price = tick.bid_price_5

                # 获取主引擎中对应的合约,包括交易所和代码
                contract = self.main_engine.get_contract(stop_order.vt_symbol)

                # stoporder的本质也是转换stop_order->limit_order
                vt_orderids = self.send_limit_order(
                    strategy, 
                    contract,
                    stop_order.direction, 
                    stop_order.offset, 
                    price, 
                    stop_order.volume,
                    stop_order.lock
                )

                # 正常发单,会返回order_ids
                # Update stop order status if placed successfully
                if vt_orderids:
                    # Remove from relation map.
                    # 如果下的本地单被以限价单的形式取代了,就删掉本地停止单
                    self.stop_orders.pop(stop_order.stop_orderid)

                    # 获取下单的策略order_id
                    strategy_vt_orderids = self.strategy_orderid_map[strategy.strategy_name]
                    if stop_order.stop_orderid in strategy_vt_orderids:
                        strategy_vt_orderids.remove(stop_order.stop_orderid)

                    # Change stop order status to cancelled and update to strategy.
                    # 改变此下单的状态,变成已触发
                    stop_order.status = StopOrderStatus.TRIGGERED
                    stop_order.vt_orderids = vt_orderids

                    # 调用on_stop_order
                    self.call_strategy_func(
                        strategy, strategy.on_stop_order, stop_order
                    )
                    self.put_stop_order_event(stop_order)

    def send_server_order(
        self,
        strategy: CtaTemplate,
        contract: ContractData,
        direction: Direction,
        offset: Offset,
        price: float,
        volume: float,
        type: OrderType,
        lock: bool
    ):
        """
        Send a new order to server.
        服务器发单,有些服务器支持 云止损单 有些不支持 支持的就可以用这个
        """
        # Create request and send order.
        original_req = OrderRequest(
            symbol=contract.symbol,
            exchange=contract.exchange,
            direction=direction,
            offset=offset,
            type=type,
            price=price,
            volume=volume,
        )

        # Convert with offset converter
        req_list = self.offset_converter.convert_order_request(original_req, lock)

        # Send Orders
        vt_orderids = []

        for req in req_list:
            vt_orderid = self.main_engine.send_order(
                req, contract.gateway_name)
            vt_orderids.append(vt_orderid)

            self.offset_converter.update_order_request(req, vt_orderid)
            
            # Save relationship between orderid and strategy.
            self.orderid_strategy_map[vt_orderid] = strategy
            self.strategy_orderid_map[strategy.strategy_name].add(vt_orderid)

        return vt_orderids
    
    def send_limit_order(
        self,
        strategy: CtaTemplate,
        contract: ContractData,
        direction: Direction,
        offset: Offset,
        price: float,
        volume: float,
        lock: bool
    ):
        """
        Send a limit order to server.
        限价单永远可以向服务器发送
        """
        return self.send_server_order(
            strategy,
            contract,
            direction,
            offset,
            price,
            volume,
            OrderType.LIMIT,
            lock
        )
    
    def send_server_stop_order(
        self,
        strategy: CtaTemplate,
        contract: ContractData,
        direction: Direction,
        offset: Offset,
        price: float,
        volume: float,
        lock: bool
    ):
        """
        Send a stop order to server.
        
        Should only be used if stop order supported 
        on the trading server.
        向服务器发送停止单,需要服务器支持
        """
        return self.send_server_order(
            strategy,
            contract,
            direction,
            offset,
            price,
            volume,
            OrderType.STOP,
            lock
        )

    def send_local_stop_order(
        self,
        strategy: CtaTemplate,
        direction: Direction,
        offset: Offset,
        price: float,
        volume: float,
        lock: bool
    ):
        """
        Create a new local stop order.
        创建一个本地停止单,服务器不支持的时候用
        """
        self.stop_order_count += 1
        stop_orderid = f"{STOPORDER_PREFIX}.{self.stop_order_count}"

        stop_order = StopOrder(
            vt_symbol=strategy.vt_symbol,
            direction=direction,
            offset=offset,
            price=price,
            volume=volume,
            stop_orderid=stop_orderid,
            strategy_name=strategy.strategy_name,
            lock=lock
        )

        self.stop_orders[stop_orderid] = stop_order

        vt_orderids = self.strategy_orderid_map[strategy.strategy_name]
        vt_orderids.add(stop_orderid)

        self.call_strategy_func(strategy, strategy.on_stop_order, stop_order)
        self.put_stop_order_event(stop_order)

        return stop_orderid

    def cancel_server_order(self, strategy: CtaTemplate, vt_orderid: str):
        """
        Cancel existing order by vt_orderid.
        撤单,取消服务器的单子
        """
        order = self.main_engine.get_order(vt_orderid)
        if not order:
            self.write_log(f"撤单失败，找不到委托{vt_orderid}", strategy)
            return

        req = order.create_cancel_request()
        self.main_engine.cancel_order(req, order.gateway_name)

    def cancel_local_stop_order(self, strategy: CtaTemplate, stop_orderid: str):
        """
        Cancel a local stop order.
        撤销本地停止单
        """
        stop_order = self.stop_orders.get(stop_orderid, None)
        if not stop_order:
            return
        strategy = self.strategies[stop_order.strategy_name]

        # Remove from relation map.
        self.stop_orders.pop(stop_orderid)

        vt_orderids = self.strategy_orderid_map[strategy.strategy_name]
        if stop_orderid in vt_orderids:
            vt_orderids.remove(stop_orderid)

        # Change stop order status to cancelled and update to strategy.
        stop_order.status = StopOrderStatus.CANCELLED

        self.call_strategy_func(strategy, strategy.on_stop_order, stop_order)
        self.put_stop_order_event(stop_order)

    def send_order(
        self,
        strategy: CtaTemplate,
        direction: Direction,
        offset: Offset,
        price: float,
        volume: float,
        stop: bool,
        lock: bool
    ):
        """
        cta的下单
        """
        contract = self.main_engine.get_contract(strategy.vt_symbol)
        if not contract:
            self.write_log(f"委托失败，找不到合约：{strategy.vt_symbol}", strategy)
            return ""
        
        # Round order price and volume to nearest incremental value
        # 价格，首先要对价格处理，变成可下单的价格，最小变动要保持一致
        price = round_to(price, contract.pricetick)
        volume = round_to(volume, contract.min_volume)

        # 如果是停止单
        if stop:
            # 如果服务器支持，发服务器的单子，从这里可以看出，如果是支持服务器停止单的，要写在查询合约的回调函数内
            if contract.stop_supported:
                return self.send_server_stop_order(strategy, contract, direction, offset, price, volume, lock)
            # 如果服务器不支持，发本地的单子
            else:
                return self.send_local_stop_order(strategy, direction, offset, price, volume, lock)
        else:
            # 如果不是停止单，就是限价单
            return self.send_limit_order(strategy, contract, direction, offset, price, volume, lock)

    def cancel_order(self, strategy: CtaTemplate, vt_orderid: str):
        """
        取消下单
        取消本地停止单
        取消服务器停止单
        """
        if vt_orderid.startswith(STOPORDER_PREFIX):
            self.cancel_local_stop_order(strategy, vt_orderid)
        else:
            self.cancel_server_order(strategy, vt_orderid)

    def cancel_all(self, strategy: CtaTemplate):
        """
        Cancel all active orders of a strategy.
        一键取消所有的当前订单
        """
        vt_orderids = self.strategy_orderid_map[strategy.strategy_name]
        if not vt_orderids:
            return

        for vt_orderid in copy(vt_orderids):
            self.cancel_order(strategy, vt_orderid)

    def get_engine_type(self):
        """获取引擎模式,默认实盘模式"""
        return self.engine_type

    def load_bar(self, vt_symbol: str, days: int, interval: Interval, callback: Callable[[BarData], None]):
        """载入历史bar"""
        symbol, exchange = extract_vt_symbol(vt_symbol)
        end = datetime.now()
        start = end - timedelta(days)

        # Query bars from RQData by default, if not found, load from database.
        # TODO
        # 这里CTA的载入历史数据是从米匡那里拿，这个必须修改，没有米匡账号！
        # 初步建议修改为在主引擎中发送query_history拿数据，由gateway回调数据
        # OKEX的历史数据由OKEX提供，FUTURES的历史数据由数据库提供，每个都不一样，因此，不能在这里统一，要改成在gaetway中分发
        bars = self.query_bar_from_rq(symbol, exchange, interval, start, end)
        if not bars:
            bars = database_manager.load_bar_data(
                symbol=symbol,
                exchange=exchange,
                interval=interval,
                start=start,
                end=end,
            )

        for bar in bars:
            callback(bar)

    def load_tick(self, vt_symbol: str, days: int, callback: Callable[[TickData], None]):
        """同上"""
        symbol, exchange = extract_vt_symbol(vt_symbol)
        end = datetime.now()
        start = end - timedelta(days)

        ticks = database_manager.load_tick_data(
            symbol=symbol,
            exchange=exchange,
            start=start,
            end=end,
        )

        for tick in ticks:
            callback(tick)

    def call_strategy_func(self, strategy: CtaTemplate, func: Callable, params: Any = None):
        """
        Call function of a strategy and catch any exception raised.
        调用策略的函数,基本输入有:
        CtaTemplate,也就是每个策略实例,或者说策略模板
        Callable,回调函数,比如策略的strategy.on_tick或者strategy.on_order
        params,也就是Callable的参数,有些需要回调的参数,on_tick可以给tick on_order可以给order
        """
        try:
            if params:
                func(params)
            else:
                func()
        except Exception:
            strategy.trading = False
            strategy.inited = False

            msg = f"触发异常已停止\n{traceback.format_exc()}"
            self.write_log(msg, strategy)

    def add_strategy(self, class_name: str, strategy_name: str, vt_symbol: str, setting: dict):
        """
        Add a new strategy.
        添加一个策略,
        class_name,策略类的名称DoubleMaStrategy,这个是模板
        strategy_name,这个是实例名称，每个实例不同的名字,一个策略两个参数,就是不同的实例
        vt_symbol,这个是实例的品种
        """
        if strategy_name in self.strategies:
            self.write_log(f"创建策略失败，存在重名{strategy_name}")
            return

        # 策略类,模板的意思CtaTemplate
        strategy_class = self.classes[class_name]

        # 创建策略,本地保存,用唯一的策略名称来保存
        # 创建一个策略,用一个具体实例,一个策略名称,vt_symbol,setting来创建
        # setting更新的是params也就是策略参数,不是策略的variables
        # 初始化的时候,就添加策略的params
        strategy = strategy_class(self, strategy_name, vt_symbol, setting)
        self.strategies[strategy_name] = strategy

        # 创建策略需要的vt_symbol,用来做字典
        # Add vt_symbol to strategy map.
        strategies = self.symbol_strategy_map[vt_symbol]
        strategies.append(strategy)

        # Update to setting file.
        # 更新策略配置
        # 添加classname进setting和vt_symbol
        self.update_strategy_setting(strategy_name, setting)

        # 加入策略事件
        self.put_strategy_event(strategy)

    def init_strategy(self, strategy_name: str):
        """
        Init a strategy.
        """
        # 初始化策略,写成队列的形式,防止初始化时,同一时间初始化太多的策略
        self.init_queue.put(strategy_name)

        # 如果没有开启线程,开启线程
        if not self.init_thread:
            self.init_thread = Thread(target=self._init_strategy)
            self.init_thread.start()

    def _init_strategy(self):
        """
        Init strategies in queue.
        """
        # 初始化策略的内部接口,对外不暴露
        while not self.init_queue.empty():
            strategy_name = self.init_queue.get()
            strategy = self.strategies[strategy_name]

            if strategy.inited:
                self.write_log(f"{strategy_name}已经完成初始化，禁止重复操作")
                continue

            # 设置策略初始化为真,如果此处不开始,则query_history数据回调用不了
            # strategy.inited = True

            self.write_log(f"{strategy_name}开始执行初始化")

            # Call on_init function of strategy
            # 对每个策略调用回调函数on_init
            #self.call_strategy_func(strategy, strategy.on_init)
            #self.write_log("engine start flag")

            # Restore strategy data(variables)
            # 策略数据,获取策略数据
            # 这里的data指的是策略的variables

            data = self.strategy_data.get(strategy_name, None)
            if data:
                # 策略的variables
                for name in strategy.variables:
                    value = data.get(name, None)
                    if value:
                        # 设置策略,名称,值 = strategy.name = value
                        setattr(strategy, name, value)

            self.call_strategy_func(strategy, strategy.on_init)

            # Subscribe market data
            # 初始化,订阅合约
            # 由于OKEX Futures的本地机制,所有的订阅,不在这里写,在gateway里面写

            """
            contract = self.main_engine.get_contract(strategy.vt_symbol)
            if contract:
                req = SubscribeRequest(
                    symbol=contract.symbol, exchange=contract.exchange)
                self.main_engine.subscribe(req, contract.gateway_name)
            else:
                self.write_log(f"行情订阅失败，找不到合约{strategy.vt_symbol}", strategy)
            """

            # Put event to update init completed status.
            # 设置策略初始化为真
            strategy.inited = True

            self.put_strategy_event(strategy)
            self.write_log(f"{strategy_name}初始化完成")

        # 由于初始化时间较长,因此每个设置完成之后,都要把线程关闭,以免浪费内存资源,否则策略加载过多,造成资源浪费
        self.init_thread = None

    def start_strategy(self, strategy_name: str):
        """
        Start a strategy.
        开始运行策略
        """
        # 运行策略
        strategy = self.strategies[strategy_name]
        # 如果策略还没有启动,先启动初始化
        if not strategy.inited:
            self.write_log(f"策略{strategy.strategy_name}启动失败，请先初始化")
            return

        # 如果策略已经在运行,无需重复启动
        if strategy.trading:
            self.write_log(f"{strategy_name}已经启动，请勿重复操作")
            return

        # 开启策略
        self.call_strategy_func(strategy, strategy.on_start)
        strategy.trading = True

        # 发送策略事件
        self.put_strategy_event(strategy)

    def stop_strategy(self, strategy_name: str):
        """
        Stop a strategy.
        停止策略
        """
        # 读取策略
        strategy = self.strategies[strategy_name]
        if not strategy.trading:
            return

        # Call on_stop function of the strategy
        # 调用on_stop策略
        self.call_strategy_func(strategy, strategy.on_stop)

        # Change trading status of strategy to False
        # 先关闭策略开启状态
        strategy.trading = False

        # Cancel all orders of the strategy
        # 取消所有的订单
        self.cancel_all(strategy)

        # Update GUI
        # 向UI内发送事件
        self.put_strategy_event(strategy)

    def edit_strategy(self, strategy_name: str, setting: dict):
        """
        Edit parameters of a strategy.
        编辑策略,也就是策略配置更新,一般用不到,除非策略停止
        """
        strategy = self.strategies[strategy_name]
        strategy.update_setting(setting)

        self.update_strategy_setting(strategy_name, setting)
        self.put_strategy_event(strategy)

    def remove_strategy(self, strategy_name: str):
        """
        Remove a strategy.
        移除一个策略
        """
        strategy = self.strategies[strategy_name]
        # 移除前先停止,否则报错
        if strategy.trading:
            self.write_log(f"策略{strategy.strategy_name}移除失败，请先停止")
            return

        # Remove setting
        # 移除配置
        self.remove_strategy_setting(strategy_name)

        # Remove from symbol strategy map
        # 从策略字典里面移除
        strategies = self.symbol_strategy_map[strategy.vt_symbol]
        strategies.remove(strategy)

        # 从活动里面移除
        # Remove from active orderid map
        if strategy_name in self.strategy_orderid_map:
            vt_orderids = self.strategy_orderid_map.pop(strategy_name)

            # Remove vt_orderid strategy map
            for vt_orderid in vt_orderids:
                if vt_orderid in self.orderid_strategy_map:
                    self.orderid_strategy_map.pop(vt_orderid)

        # Remove from strategies
        self.strategies.pop(strategy_name)

        return True

    def load_strategy_class(self):
        """
        Load strategy class from source code.
        动态载入策略类,可以从两个地方载入:

        """
        # 这里提供两个路径,实盘中,可以存在一个路径,也可以存在另一个路径,都可以
        path1 = Path(__file__).parent.joinpath("strategies")
        self.load_strategy_class_from_folder(
            path1, "vnpy.app.cta_strategy.strategies")

        path2 = Path.cwd().joinpath("strategies")
        self.load_strategy_class_from_folder(path2, "strategies")

    def load_strategy_class_from_folder(self, path: Path, module_name: str = ""):
        """
        Load strategy class from certain folder.
        从文件夹载入策略
        所有带.py结尾的都是正常策略
        """
        # dirpath是文件的路径,dirnames不知道,filenames是文件内所有的策略名称
        for dirpath, dirnames, filenames in os.walk(str(path)):
            # 要提取的是策略
            for filename in filenames:
                # 如果以.py为结尾,这里有个问题,就是__init__.py也是以这个为结尾,不严谨,尽管在下面的过程中排除掉
                if filename.endswith(".py"):
                    # 策略名称,也就是.py的文件名称
                    strategy_module_name = ".".join(
                        [module_name, filename.replace(".py", "")])
                    self.load_strategy_class_from_module(strategy_module_name)

    def load_strategy_class_from_module(self, module_name: str):
        """
        Load strategy class from module file.
        从模块中载入策略,主要为improtlib模块的本地实现
        """
        try:
            module = importlib.import_module(module_name)

            # 取这个模块内的所有东西
            for name in dir(module):
                # 获取模块的值
                value = getattr(module, name)
                # 1.是个type类型
                # 2.是CtaTemplate的子类
                # 3.不是CtaTemplate
                if (isinstance(value, type) and
                        issubclass(value, CtaTemplate) and
                        value is not CtaTemplate):
                    # 每个值得名称就是他本身,比如AtrRsiStrategy
                    self.classes[value.__name__] = value
        except:  # noqa
            msg = f"策略文件{module_name}加载失败，触发异常：\n{traceback.format_exc()}"
            self.write_log(msg)

    def load_strategy_data(self):
        """
        Load strategy data from json file.
        策略数据,策略的数据指的是什么,如果
        """
        results = self.db_mongo.dbQuery(self.account_id, self.data_dbname, {})
        for result in results:
            self.strategy_data[result["strategy_name"]] = result["data"]
        # self.strategy_data = load_json(self.data_filename)

    def sync_strategy_data(self, strategy: CtaTemplate):
        """
        Sync strategy data into json file.
        同步策略数据到本地,I/O操作,要小心一点,每个成交都要修改
        """
        data = strategy.get_variables()
        data.pop("inited")      # Strategy status (inited, trading) should not be synced.
        data.pop("trading")

        self.strategy_data[strategy.strategy_name] = data
        d = {
            "strategy_name": strategy.strategy_name,
            "data": data,
        }
        flt = {"strategy_name": strategy.strategy_name}
        self.db_queue.put(["update", self.account_id, self.data_dbname, d, flt, True])

        #save_json(self.data_filename, self.strategy_data)

    def get_all_strategy_class_names(self):
        """
        Return names of strategy classes loaded.
        获取所有策略的类名
        """
        return list(self.classes.keys())

    def get_strategy_class_parameters(self, class_name: str):
        """
        Get default parameters of a strategy class.
        获取所有策略的参数类
        """
        strategy_class = self.classes[class_name]

        parameters = {}
        for name in strategy_class.parameters:
            parameters[name] = getattr(strategy_class, name)

        return parameters

    def get_strategy_parameters(self, strategy_name):
        """
        Get parameters of a strategy.
        获取策略参数
        """
        strategy = self.strategies[strategy_name]
        return strategy.get_parameters()

    def init_all_strategies(self):
        """
        初始化所有策略
        """
        for strategy_name in self.strategies.keys():
            self.init_strategy(strategy_name)

    def start_all_strategies(self):
        """
        启动所有策略
        """
        for strategy_name in self.strategies.keys():
            self.start_strategy(strategy_name)

    def stop_all_strategies(self):
        """
        停止所有策略
        """
        for strategy_name in self.strategies.keys():
            self.stop_strategy(strategy_name)

    def load_strategy_setting(self):
        """
        Load setting file.
        载入策略配置文件
        这里修改很大,变成与数据库交互
        """
        results = self.db_mongo.dbQuery(self.account_id, self.setting_dbname, {})
        for result in results:
            self.add_strategy(
                result["class_name"],
                result["strategy_name"],
                result["vt_symbol"],
                result["setting"]
            )

        """
        self.strategy_setting = load_json(self.setting_filename)

        # 策略配置文件,由于策略名称统一,因此策略配置统一
        for strategy_name, strategy_config in self.strategy_setting.items():
            # 添加策略
            # 策略名称唯一
            # 添加类名称
            # 添加vt_symbol
            # 添加setting配置
            self.add_strategy(
                strategy_config["class_name"], 
                strategy_name,
                strategy_config["vt_symbol"], 
                strategy_config["setting"]
            )
        """

    def update_strategy_setting(self, strategy_name: str, setting: dict):
        """
        Update setting file.
        更新策略配置到本地
        """
        strategy = self.strategies[strategy_name]

        self.strategy_setting[strategy_name] = {
            "class_name": strategy.__class__.__name__,
            "vt_symbol": strategy.vt_symbol,
            "setting": setting,
        }
        d = {
            "strategy_name": strategy_name,
            "class_name": strategy.__class__.__name__,
            "vt_symbol": strategy.vt_symbol,
            "setting": setting,
        }
        flt = {
            "strategy_name": strategy_name,
            "class_name": strategy.__class__.__name__,
        }
        self.db_queue.put(["update", self.account_id, self.setting_dbname, d, flt, True])
        # self.db_mongo.dbUpdate(self.account_id, self.setting_dbname, d, flt, True)
        """
        # 更新完策略配置之后,要save_json到配置文件内
        save_json(self.setting_filename, self.strategy_setting)
        """

    def remove_strategy_setting(self, strategy_name: str):
        """
        Remove setting file.
        移除策略配置
        """
        if strategy_name not in self.strategy_setting:
            return

        # 删除策略配置,同样也需要save_json
        self.strategy_setting.pop(strategy_name)
        flt = {
            "strategy_name": strategy_name
        }
        self.db_mongo.dbDelete(self.account_id, self.setting_dbname, flt)

        """
        #save_json(self.setting_filename, self.strategy_setting)
        """

    def put_stop_order_event(self, stop_order: StopOrder):
        """
        Put an event to update stop order status.
        """
        # 发送停止单事件
        event = Event(EVENT_CTA_STOPORDER, stop_order)
        self.event_engine.put(event)

    def put_strategy_event(self, strategy: CtaTemplate):
        """
        Put an event to update strategy status.
        """
        # 发送策略事件
        data = strategy.get_data()
        event = Event(EVENT_CTA_STRATEGY, data)
        self.event_engine.put(event)

    def write_log(self, msg: str, strategy: CtaTemplate = None):
        """
        Create cta engine log event.
        """
        if strategy:
            msg = f"{strategy.strategy_name} -> {msg}"
        d = {
            "datetime": datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f"),
            "gateway_name": "CtaStrategy",
            "msg": msg
        }
        self.db_queue.put(["insert", self.account_id, "Log", d])
        """
        log = LogData(msg=msg, gateway_name="CtaStrategy")
        event = Event(type=EVENT_CTA_LOG, data=log)
        # 写日志
        self.event_engine.put(event)
        """

    def send_email(self, msg: str, strategy: CtaTemplate = None):
        """
        Send email to default receiver.
        发送Email
        """
        if strategy:
            subject = f"{strategy.strategy_name}"
        else:
            subject = "CTA策略引擎"

        self.main_engine.send_email(subject, msg)

    def db_start(self):
        """开启DB的线程"""
        self.db_active = True
        self.db_thread = Thread(target=self.db_run)
        self.db_thread.start()

    def db_stop(self):
        self.db_active = False

    def db_run(self):
        """数据库线程的运行"""
        while self.db_active:
            try:
                task = self.db_queue.get(timeout=1)
                if task[0] == "update":
                    dbName = task[1]
                    collectionName = task[2]
                    d = task[3]
                    flt = task[4]
                    self.db_mongo.dbUpdate(dbName, collectionName, d, flt, True)
                elif task[0] == "insert":
                    dbName = task[1]
                    collectionName = task[2]
                    d = task[3]
                    self.db_mongo.dbInsert(dbName, collectionName, d)
                # task_type, data = task

            except Empty:
                self.db_count += 1
                # 设定每隔一小时左右
                if self.db_count >= 3600:
                    while True:
                        try:
                            info = psutil.virtual_memory()
                            self.write_log('重启内存使用：' + str(psutil.Process(os.getpid()).memory_info().rss))
                            self.write_log('重启总内存：' + str(info.total))
                            self.write_log('重启内存占比：' + str(info.percent))
                            self.write_log('重启cpu个数：' + str(psutil.cpu_count()))
                            self.write_log("数据库开始重启!!!")
                            # 先将run关闭
                            self.db_active = False
                            # 正常关闭Mongodb的连接
                            self.db_mongo.dbClient.close()
                            self.db_mongo = None
                            # 重新开启dbMongo()
                            self.db_mongo = dbMongo(self.db_name, self.db_pwd)
                            # 重新开启run
                            self.db_active = True
                            self.write_log("数据库重启成功!!!")
                            break
                        except Exception as e:
                            self.write_log("数据库问题" + str(e))
            except Exception as e:
                self.write_log(str(e))



