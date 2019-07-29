from pathlib import Path

from vnpy.trader.app import BaseApp
from vnpy.trader.constant import Direction
from vnpy.trader.object import TickData, BarData, TradeData, OrderData
from vnpy.trader.utility import BarGenerator, ArrayManager

from .base import APP_NAME, StopOrder
from .engine import MultiFactorEngine
# from .backtesting import BacktestingEngine, OptimizationSetting
from .template import MultiFactorTemplate, CtaSignal, TargetPosTemplate


class MultiFactorApp(BaseApp):
    """"""
    app_name = APP_NAME
    app_module = __module__
    app_path = Path(__file__).parent
    display_name = "MultiFactor策略"
    engine_class = MultiFactorEngine
    widget_name = "MultiFactorManager"
    icon_name = "multifactor.ico"
