"""
Defines constants and objects used in CtaStrategy App.
"""

from dataclasses import dataclass, field
from enum import Enum

from vnpy.trader.constant import Direction, Offset

APP_NAME = "MultiFactorStrategy"
STOPORDER_PREFIX = "MSTOP"


class StopOrderStatus(Enum):
    WAITING = "等待中"
    CANCELLED = "已撤销"
    TRIGGERED = "已触发"


class EngineType(Enum):
    LIVE = "实盘"
    BACKTESTING = "回测"


class BacktestingMode(Enum):
    BAR = 1
    TICK = 2

# 本地停止单
@dataclass
class StopOrder:
    vt_symbol: str
    direction: Direction
    offset: Offset
    price: float
    volume: float
    stop_orderid: str
    strategy_name: str
    lock: bool = False
    vt_orderids: list = field(default_factory=list)
    status: StopOrderStatus = StopOrderStatus.WAITING


EVENT_MULTIFACTOR_LOG = "eMultiFactorLog"
EVENT_MULTIFACTOR_STRATEGY = "eMultiFactorStrategy"
EVENT_MULTIFACTOR_STOPORDER = "eMultiFactorStopOrder"
