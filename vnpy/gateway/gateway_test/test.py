
from datetime import datetime

from vnpy.gateway.gateway_test.okex_gateway_test import OkexGateway
from vnpy.gateway.gateway_test.websocket_client_test import WebsocketClient
from vnpy.event import EventEngine
from vnpy.trader.engine import MainEngine
from vnpy.trader.object import (
    TickData,
    OrderData,
    TradeData,
    AccountData,
    ContractData,
    OrderRequest,
    CancelRequest,
    SubscribeRequest,
)


if __name__ == "__main__":
    event_engine = EventEngine()

    main_engine = MainEngine(event_engine)
    # 添加Okexgateway模块
    main_engine.add_gateway(OkexGateway)
    # 连接
    setting = {
        "API Key": "",
        "Secret Key": "",
        "Passphrase": "",
        "会话数": 3,
        "代理地址": "",
        "代理端口": "",
    }

    main_engine.connect(setting, "OKEX")








