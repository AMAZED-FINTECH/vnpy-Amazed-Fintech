""""""
from abc import ABC
from typing import Any, Callable

from vnpy.trader.constant import Interval, Direction, Offset
from vnpy.trader.object import BarData, TickData, OrderData, TradeData
from vnpy.trader.utility import virtual

from .base import StopOrder, EngineType


class CtaTemplate(ABC):
    """Cta模板"""

    author = ""
    # 参数，例如 长短均线中的10，是这个指标
    parameters = []
    # 变量，在程序中，fast_ma0,fast_ma1,用来判断当前是否穿线的单一值，并不是list的概念，是一个点的意思
    variables = []

    def __init__(
        self,
        cta_engine: Any,
        strategy_name: str,
        vt_symbol: str,
        setting: dict,
    ):
        """"""
        # 模板里面添加cta_engine,cta_engine内添加main_engine,event_engine,和其他木块一样
        self.cta_engine = cta_engine
        # 每个策略，都是一个策略模板 + 模板中的具体实现
        self.strategy_name = strategy_name
        # 策略的监听vt_symbol,也就是，parameters,variables,vt_symbol,strategy_name每个改变,都是不同的策略
        self.vt_symbol = vt_symbol

        # 是否初始化，是否交易，当前的仓位
        self.inited = False
        self.trading = False
        self.pos = 0

        # 添加策略变量，所有策略中都需要添加inited,trading,pos
        self.variables.insert(0, "inited")
        self.variables.insert(1, "trading")
        self.variables.insert(2, "pos")

        # 更新配置,配置添加进来
        self.update_setting(setting)

    def update_setting(self, setting: dict):
        """
        Update strategy parameter with value in setting dict.
        """
        # 这里的意思也就是,策略只关注策略本身,并不用对参数有太多的想法,只要添加setting配置文件就行
        for name in self.parameters:
            if name in setting:
                setattr(self, name, setting[name])

    @classmethod
    def get_class_parameters(cls):
        """
        Get default parameters dict of strategy class.
        这也是获取策略参数的办法,但是并不需要实例化类,具体看cls和self的区别
        """
        #
        class_parameters = {}
        for name in cls.parameters:
            class_parameters[name] = getattr(cls, name)
        return class_parameters

    def get_parameters(self):
        """
        Get strategy parameters dict.
        通过实例的类获取参数
        """
        strategy_parameters = {}
        for name in self.parameters:
            strategy_parameters[name] = getattr(self, name)
        return strategy_parameters

    def get_variables(self):
        """
        Get strategy variables dict.
        获取策略所有变量
        """
        strategy_variables = {}
        for name in self.variables:
            strategy_variables[name] = getattr(self, name)
        return strategy_variables

    def get_data(self):
        """
        Get strategy data.
        获取策略的数据,一个策略,这里显示变量有6个:
        strategy_name
        vt_symbol
        class_name
        author
        parameters
        variables
        其中任何一个改变，都视为不同的策略
        """
        strategy_data = {
            "strategy_name": self.strategy_name,
            "vt_symbol": self.vt_symbol,
            "class_name": self.__class__.__name__,
            "author": self.author,
            "parameters": self.get_parameters(),
            "variables": self.get_variables(),
        }
        return strategy_data

    @virtual
    def on_init(self):
        """
        Callback when strategy is inited.
        每个策略初始化，都要有对应的初始化，包括提取历史数据等
        """
        pass

    @virtual
    def on_start(self):
        """
        Callback when strategy is started.
        策略启动,和Init有一定的功能重合
        """
        pass

    @virtual
    def on_stop(self):
        """
        Callback when strategy is stopped.
        策略停止
        """
        pass

    @virtual
    def on_tick(self, tick: TickData):
        """
        Callback of new tick data update.
        收到Tick数据之后的操作，高频策略或风控使用
        """
        pass

    @virtual
    def on_bar(self, bar: BarData):
        """
        Callback of new bar data update.
        收到Bar数据之后的操作，中低频策略使用
        """
        pass

    @virtual
    def on_trade(self, trade: TradeData):
        """
        Callback of new trade data update.
        收到成交之后的处理
        """
        pass

    @virtual
    def on_order(self, order: OrderData):
        """
        Callback of new order data update.
        收到order之后的处理
        """
        pass

    @virtual
    def on_stop_order(self, stop_order: StopOrder):
        """
        Callback of stop order update.
        收到stop order的处理
        """
        pass

    def buy(self, price: float, volume: float, stop: bool = False, lock: bool = False):
        """
        Send buy order to open a long position.
        开  多单
        """
        return self.send_order(Direction.LONG, Offset.OPEN, price, volume, stop, lock)

    def sell(self, price: float, volume: float, stop: bool = False, lock: bool = False):
        """
        Send sell order to close a long position.
        平  多单 简化发单
        """
        return self.send_order(Direction.SHORT, Offset.CLOSE, price, volume, stop, lock)

    def short(self, price: float, volume: float, stop: bool = False, lock: bool = False):
        """
        Send short order to open as short position.
        开  空单 简化发单
        """
        return self.send_order(Direction.SHORT, Offset.OPEN, price, volume, stop, lock)

    def cover(self, price: float, volume: float, stop: bool = False, lock: bool = False):
        """
        Send cover order to close a short position.
        平  空单 简化发单
        """
        return self.send_order(Direction.LONG, Offset.CLOSE, price, volume, stop, lock)

    def send_order(
        self,
        direction: Direction,
        offset: Offset,
        price: float,
        volume: float,
        stop: bool = False,
        lock: bool = False
    ):
        """
        Send a new order.
        发单
        """
        if self.trading:
            vt_orderids = self.cta_engine.send_order(
                self, direction, offset, price, volume, stop, lock
            )
            return vt_orderids
        else:
            return []

    def cancel_order(self, vt_orderid: str):
        """
        Cancel an existing order.
        取消发单
        """
        if self.trading:
            self.cta_engine.cancel_order(self, vt_orderid)

    def cancel_all(self):
        """
        Cancel all orders sent by strategy.
        取消由本策略推送的所有的单
        """
        if self.trading:
            self.cta_engine.cancel_all(self)

    def write_log(self, msg: str):
        """
        Write a log message.
        Cta engine写日志
        """
        self.cta_engine.write_log(msg, self)

    def get_engine_type(self):
        """
        Return whether the cta_engine is backtesting or live trading.
        获取策略的type，主要是回测和实盘的区别
        """
        return self.cta_engine.get_engine_type()

    def load_bar(
        self,
        days: int,
        interval: Interval = Interval.MINUTE,
        callback: Callable = None,
    ):
        """
        Load historical bar data for initializing strategy.
        载入历史数据，逻辑都是载入一分钟K线，然后合成
        """
        if not callback:
            callback = self.on_bar

        self.cta_engine.load_bar(self.vt_symbol, days, interval, callback)

    def load_tick(self, days: int):
        """
        Load historical tick data for initializing strategy.
        载入历史tick数据
        """
        self.cta_engine.load_tick(self.vt_symbol, days, self.on_tick)

    def put_event(self):
        """
        Put an strategy data event for ui update.
        这里更新event驱动，是为了可视化的更新
        策略事件，将CtaTemplate整个放进到策略中去
        每个具体的策略，继承自一个CtaTemplate，每次发出策略，将具体策略推送
        """
        if self.inited:
            self.cta_engine.put_strategy_event(self)

    def send_email(self, msg):
        """
        Send email to default receiver.
        严重bug问题，需要发送邮件
        """
        if self.inited:
            self.cta_engine.send_email(msg, self)

    def sync_data(self):
        """
        Sync strategy variables value into disk storage.
        """
        if self.trading:
            self.cta_engine.sync_strategy_data(self)


class CtaSignal(ABC):
    """CtaSignal信号 为了多策略"""

    def __init__(self):
        """"""
        self.signal_pos = 0

    @virtual
    def on_tick(self, tick: TickData):
        """
        Callback of new tick data update.
        """
        pass

    @virtual
    def on_bar(self, bar: BarData):
        """
        Callback of new bar data update.
        """
        pass

    def set_signal_pos(self, pos):
        """"""
        self.signal_pos = pos

    def get_signal_pos(self):
        """"""
        return self.signal_pos


class TargetPosTemplate(CtaTemplate):
    """适合不习惯使用下单的人"""
    tick_add = 1

    last_tick = None
    last_bar = None
    target_pos = 0
    vt_orderids = []

    def __init__(self, cta_engine, strategy_name, vt_symbol, setting):
        """"""
        super(TargetPosTemplate, self).__init__(
            cta_engine, strategy_name, vt_symbol, setting
        )
        self.variables.append("target_pos")

    @virtual
    def on_tick(self, tick: TickData):
        """
        Callback of new tick data update.
        """
        self.last_tick = tick

        if self.trading:
            self.trade()

    @virtual
    def on_bar(self, bar: BarData):
        """
        Callback of new bar data update.
        """
        self.last_bar = bar

    @virtual
    def on_order(self, order: OrderData):
        """
        Callback of new order data update.
        """
        vt_orderid = order.vt_orderid

        if not order.is_active() and vt_orderid in self.vt_orderids:
            self.vt_orderids.remove(vt_orderid)

    def set_target_pos(self, target_pos):
        """"""
        self.target_pos = target_pos
        self.trade()

    def trade(self):
        """"""
        self.cancel_all()

        pos_change = self.target_pos - self.pos
        if not pos_change:
            return

        long_price = 0
        short_price = 0

        if self.last_tick:
            if pos_change > 0:
                long_price = self.last_tick.ask_price_1 + self.tick_add
                if self.last_tick.limit_up:
                    long_price = min(long_price, self.last_tick.limit_up)
            else:
                short_price = self.last_tick.bid_price_1 - self.tick_add
                if self.last_tick.limit_down:
                    short_price = max(short_price, self.last_tick.limit_down)

        else:
            if pos_change > 0:
                long_price = self.last_bar.close_price + self.tick_add
            else:
                short_price = self.last_bar.close_price - self.tick_add

        if self.get_engine_type() == EngineType.BACKTESTING:
            if pos_change > 0:
                vt_orderids = self.buy(long_price, abs(pos_change))
            else:
                vt_orderids = self.short(short_price, abs(pos_change))
            self.vt_orderids.extend(vt_orderids)

        else:
            if self.vt_orderids:
                return

            if pos_change > 0:
                if self.pos < 0:
                    if pos_change < abs(self.pos):
                        vt_orderids = self.cover(long_price, pos_change)
                    else:
                        vt_orderids = self.cover(long_price, abs(self.pos))
                else:
                    vt_orderids = self.buy(long_price, abs(pos_change))
            else:
                if self.pos > 0:
                    if abs(pos_change) < self.pos:
                        vt_orderids = self.sell(short_price, abs(pos_change))
                    else:
                        vt_orderids = self.sell(short_price, abs(self.pos))
                else:
                    vt_orderids = self.short(short_price, abs(pos_change))
            self.vt_orderids.extend(vt_orderids)
