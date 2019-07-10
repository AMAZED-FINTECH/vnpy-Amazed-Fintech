from vnpy.app.cta_strategy import (
    CtaTemplate,
    StopOrder,
    TickData,
    BarData,
    TradeData,
    OrderData,
    BarGenerator,
    ArrayManager,
)
from vnpy.trader.object import (
    HistoryRequest,
)
from vnpy.trader.constant import (
    Exchange,
    Interval,
    Direction,
)

from datetime import datetime
import time


class DoubleMaStrategy(CtaTemplate):
    author = "用Python的交易员"

    fast_window = 10
    slow_window = 20

    fast_ma0 = 0.0
    fast_ma1 = 0.0

    slow_ma0 = 0.0
    slow_ma1 = 0.0

    parameters = ["fast_window", "slow_window"]
    variables = ["fast_ma0", "fast_ma1", "slow_ma0", "slow_ma1"]

    def __init__(self, cta_engine, strategy_name, vt_symbol, setting):
        """"""
        super(DoubleMaStrategy, self).__init__(
            cta_engine, strategy_name, vt_symbol, setting
        )
        self.am1 = ArrayManager()

        self.bg5 = BarGenerator(self.on_bar, 5, self.on_5min_bar)
        self.am5 = ArrayManager()

        self.bg30 = BarGenerator(self.on_bar, 30, self.on_30min_bar)
        self.am30 = ArrayManager()

        self.bg60 = BarGenerator(self.on_bar, 60, self.on_1hour_bar)
        self.am60 = ArrayManager()

        self.bg_4 = BarGenerator(self.on_bar, 4, self.on_4hour_bar)
        self.am_4 = ArrayManager()

    def on_init(self):
        """
        Callback when strategy is inited.
        """
        self.write_log("策略初始化")
        # 这里在实盘的时候,由每个策略在init的时候,查询历史数据,不在本地查询,对于数字货币,直接用交易所的数据
        """
        req = HistoryRequest(symbol="BTC-USD-190927",
                             exchange=Exchange.OKEXF,
                             start=datetime.now(),
                             interval=Interval.HOUR4)

        okexgateway = self.cta_engine.main_engine.get_gateway("OKEXF")
        okexgateway.query_history(req)
        time.sleep(5)

        req = HistoryRequest(symbol="BTC-USD-190927",
                             exchange=Exchange.OKEXF,
                             start=datetime.now(),
                             interval=Interval.HOUR)
    
        okexgateway = self.cta_engine.main_engine.get_gateway("OKEXF")
        okexgateway.query_history(req)
        time.sleep(5)
        """
        req = HistoryRequest(symbol="BTC-USD-190927",
                             exchange=Exchange.OKEXF,
                             start=datetime.now(),
                             interval=Interval.MINUTE5)
        okexgateway = self.cta_engine.main_engine.get_gateway("OKEXF")
        okexgateway.query_history(req)
        time.sleep(5)

        # 查询一个之后,要暂停一下,等待长周期历史K线读取完毕
        req = HistoryRequest(symbol="BTC-USD-190927",
                             exchange=Exchange.OKEXF,
                             start=datetime.now(),
                             interval=Interval.MINUTE)
        okexgateway = self.cta_engine.main_engine.get_gateway("OKEXF")
        okexgateway.query_history(req)
        time.sleep(5)
        self.write_log("查询完毕初始化数据")

    def on_start(self):
        """
        Callback when strategy is started.
        """
        self.write_log("策略启动")
        self.put_event()

    def on_stop(self):
        """
        Callback when strategy is stopped.
        """
        self.write_log("策略停止")

        self.put_event()

    def on_tick(self, tick: TickData):
        """
        Callback of new tick data update.
        数字货币交易,tick用来做风控事件,止损,止盈等.
        """
        # print(tick.__dict__)
        # self.bg.update_tick(tick)
        pass

    def on_bar(self, bar: BarData):
        """
        Callback of new bar data update.
        这里在初始化回调的时候,会有不同的周期进来,因此需要整理
        """

        # 持久化运行期间,策略从gateway收到分钟数据->5分钟 ->30分钟 -> 60分钟
        if bar.interval == Interval.MINUTE:
            # 更新一分钟的am1
            am1 = self.am1
            if am1.count != 0:
                # 只有时间符合的才更新
                if bar.datetime > am1.time_array[-1]:
                    am1.update_bar(bar)
            else:
                am1.update_bar(bar)
            self.write_log("double_ma_strategy 1分钟Bar" + str(bar.__dict__))
            # 产生5分钟的bar
            self.bg5.update_bar(bar)
            # 产生30分钟的bar
            self.bg30.update_bar(bar)
            # 产生60分钟的bar
            self.bg60.update_bar(bar)
            if not am1.inited:
                return
            # 小时线收到了之后,在小时线那里更新4小时线
        # requestHistory,收到一小时的K线之后,更新四小时的K线,并记录一小时的K线
        elif bar.interval == Interval.MINUTE30:
            self.on_30min_bar(bar)
        elif bar.interval == Interval.MINUTE5:
            self.on_5min_bar(bar)
        elif bar.interval == Interval.HOUR:
            self.on_1hour_bar(bar)
        elif bar.interval == Interval.HOUR4:
            self.on_4hour_bar(bar)

    def on_5min_bar(self, bar: BarData):

        am5 = self.am5
        if am5.count != 0:
            # 只有时间符合的才更新
            if bar.datetime > am5.time_array[-1]:
                am5.update_bar(bar)
                self.write_log("double_ma_strategy 5分钟Bar" + str(bar.__dict__))
            if not am5.inited:
                return
        else:
            am5.update_bar(bar)

        am = self.am5

        fast_ma = am.sma(self.fast_window, array=True)
        self.fast_ma0 = round(fast_ma[-1], 2)
        self.fast_ma1 = round(fast_ma[-2], 2)
        self.write_log(datetime.now().strftime("%H:%M:%S") + " fastma0: " +str(self.fast_ma0))
        self.write_log(datetime.now().strftime("%H:%M:%S") + " fastma1: " + str(self.fast_ma1))
        slow_ma = am.sma(self.slow_window, array=True)
        self.slow_ma0 = round(slow_ma[-1], 2)
        self.slow_ma1 = round(slow_ma[-2], 2)
        self.write_log(datetime.now().strftime("%H:%M:%S") + " slowma0: " + str(self.slow_ma0))
        self.write_log(datetime.now().strftime("%H:%M:%S") + " slowma1: " + str(self.slow_ma1))
        cross_over = self.fast_ma0 > self.slow_ma0 and self.fast_ma1 < self.slow_ma1
        cross_below = self.fast_ma0 < self.slow_ma0 and self.fast_ma1 > self.slow_ma1

        if cross_over:
            if self.pos == 0:
                # 买 开
                self.buy(bar.close_price + 20, 1)
            elif self.pos < 0:
                # 平 空
                self.cover(bar.close_price + 20, 1)
                # 买 开
                self.buy(bar.close_price + 20, 1)

        elif cross_below:
            if self.pos == 0:
                # 空 开
                self.short(bar.close_price - 20, 1)
            elif self.pos > 0:
                # 平 多
                self.sell(bar.close_price - 20, 1)
                # 空 开
                self.short(bar.close_price - 20, 1)
        self.put_event()

    def on_30min_bar(self, bar: BarData):
        # 策略是在30分钟的周期上进行逻辑判断的
        # self.write_log("double_ma_strategy 30分钟Bar" + str(bar.__dict__))
        # 先检查数据收录情况
        am30 = self.am30
        if am30.count != 0:
            # 只有时间符合的才更新
            if bar.datetime > am30.time_array[-1]:
                am30.update_bar(bar)
        else:
            am30.update_bar(bar)
        if not am30.inited:
            return

    def on_1hour_bar(self, bar: BarData):
        self.bg_4.update_bar_hour(bar)
        # self.write_log("double_ma_strategy 1小时Bar" + str(bar.__dict__))
        # 先检查数据收录情况
        am60 = self.am60
        if am60.count != 0:
            # 只有时间符合的才更新
            if bar.datetime > am60.time_array[-1]:
                am60.update_bar(bar)
        else:
            am60.update_bar(bar)
        if not am60.inited:
            return

    def on_4hour_bar(self, bar: BarData):
        # self.write_log("double_ma_strategy 4小时Bar" + str(bar.__dict__))
        # 先检查数据收录情况
        # 收到4小时K线之后,要先看是否满足最新时间,不满足不更新
        am_4 = self.am_4
        if am_4.count != 0:
            # 只有时间符合的才更新
            if bar.datetime > am_4.time_array[-1]:
                am_4.update_bar(bar)
        else:
            am_4.update_bar(bar)
        if not am_4.inited:
            return

    def on_order(self, order: OrderData):
        """
        Callback of new order data update.
        """
        # print(order.__dict__)
        pass

    def on_trade(self, trade: TradeData):
        """
        Callback of new trade data update.
        """
        # 如果是多单，计算持仓时符号为正，如果是空单，计算持仓时符号为负
        self.put_event()

    def on_stop_order(self, stop_order: StopOrder):
        """
        Callback of stop order update.
        """
        pass
