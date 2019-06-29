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
)

from datetime import datetime


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

        self.bg5 = BarGenerator(self.on_bar, 5, self.on_5min_bar)
        self.am5 = ArrayManager()

        self.bg30 = BarGenerator(self.on_bar, 30, self.on_30min_bar)
        self.am30 = ArrayManager()

    def on_init(self):
        """
        Callback when strategy is inited.
        """
        self.write_log("策略初始化")
        # 这里在实盘的时候,由每个策略在init的时候,查询历史数据,不在本地查询,对于数字货币,直接用交易所的数据

        req = HistoryRequest(symbol="BTC-USD-190927",
                             exchange=Exchange.OKEXF,
                             start=datetime.now(),
                             interval=Interval.MINUTE30)

        okexgateway = self.cta_engine.main_engine.get_gateway("OKEXF")
        okexgateway.query_history(req)

        req = HistoryRequest(symbol="BTC-USD-190927",
                             exchange=Exchange.OKEXF,
                             start=datetime.now(),
                             interval=Interval.MINUTE)

        okexgateway = self.cta_engine.main_engine.get_gateway("OKEXF")
        okexgateway.query_history(req)

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
        """
        self.bg.update_bar(bar)
        # print(bar.__dict__)
        # self.ding.send_text(str(bar.__dict__))
        am = self.am
        am.update_bar(bar)
        if not am.inited:
            return

        return

        fast_ma = am.sma(self.fast_window, array=True)
        self.fast_ma0 = fast_ma[-1]
        self.fast_ma1 = fast_ma[-2]

        slow_ma = am.sma(self.slow_window, array=True)
        self.slow_ma0 = slow_ma[-1]
        self.slow_ma1 = slow_ma[-2]

        cross_over = self.fast_ma0 > self.slow_ma0 and self.fast_ma1 < self.slow_ma1
        cross_below = self.fast_ma0 < self.slow_ma0 and self.fast_ma1 > self.slow_ma1

        if cross_over:
            if self.pos == 0:
                self.buy(bar.close_price, 1)
            elif self.pos < 0:
                self.cover(bar.close_price, 1)
                self.buy(bar.close_price, 1)

        elif cross_below:
            if self.pos == 0:
                self.short(bar.close_price, 1)
            elif self.pos > 0:
                self.sell(bar.close_price, 1)
                self.short(bar.close_price, 1)

        self.put_event()

    def on_5min_bar(self, bar: BarData):
        pass

    def on_30min_bar(self, bar: BarData):
        pass

    def on_order(self, order: OrderData):
        """
        Callback of new order data update.
        """
        pass

    def on_trade(self, trade: TradeData):
        """
        Callback of new trade data update.
        """
        self.put_event()

    def on_stop_order(self, stop_order: StopOrder):
        """
        Callback of stop order update.
        """
        pass
