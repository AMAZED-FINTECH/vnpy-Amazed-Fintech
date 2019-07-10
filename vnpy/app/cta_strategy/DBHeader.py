
from vnpy.app.cta_strategy.DBMongo import dbMongo


# 设置数据库的表头,增加易读性
# 设置

def BarHeader(account_id):
    d = {
        "account_id": "",
        "strategy_name": "",
        "gateway_name": "",
        "symbol": "",
        "exchange": "",
        "datetime": "",
        "interval": "",
        "volume": "",
        "open_interest": "",
        "open_price": "",
        "high_price": "",
        "low_price": "",
        "close_price": "",
        "vt_symbol": ""
    }
    db.dbUpdate(account_id, "Bar_Data", d, {}, True)


def OrderHeader(account_id):
    d = {
        "account_id": "",
        "strategy_name": "",
        "gateway_name": "",
        "symbol": "",
        "exchange": "",
        "orderid": "",
        "type": "",
        "direction": "",
        "offset": "",
        "price": "",
        "volume": "",
        "traded": "",
        "status": "",
        "time": "",
        "vt_symbol": "",
        "vt_orderid": ""

    }
    db.dbUpdate(account_id, "Order_Data", d, {}, True)

def TradeHeader(account_id):
    d = {
        "account_id": "",
        "strategy_name": "",
        "gateway_name": "",
        "symbol": "",
        "exchange": "",
        "orderid": "",
        "tradeid": "",
        "direction": "",
        "offset": "",
        "price": "",
        "volume": "",
        "time": "",
        "vt_symbol": "",
        "vt_orderid": "",
        "vt_tradeid": ""

    }
    db.dbUpdate(account_id, "Trade_Data", d, {}, True)


def AccountHeader(account_id):
    d = {
        "account_id": "",
        "datetime": "",
        "gateway_name": "",
        "accountid": "",
        "balance": "",
        "frozen": "",
        "available": "",
        "vt_accountid": "",
    }
    db.dbUpdate(account_id, "Account_Data", d, {}, True)


def PositionHeader(account_id):
    d = {
        "account_id": "",
        "datetime": "",
        "gateway_name": "",
        "symbol": "",
        "exchange": "",
        "direction": "",
        "volume": "",
        "frozen": "",
        "price": "",
        "pnl": "",
        "yd_volume": "",
        "vt_symbol": "",
        "vt_positionid": "",
    }
    db.dbUpdate(account_id, "Position_Data", d, {}, True)


def cta_strategy_settingHeader(account_id):
    d = {
        "class_name": "DoubleMaStrategy",
        "strategy_name": "test_doublema",
        "vt_symbol": "BTC-USD-190927.OKEXF",
        "setting": {
            "class_name": "DoubleMaStrategy",
            "fast_window": 10,
            "slow_window": 30
        },
    }
    db.dbUpdate(account_id, "cta_strategy_setting", d, {}, True)

def cta_strategy_dataHeader(account_id):
    d = {
        "strategy_name": "test_doublema",

        "data": {
            "fast_ma0": 1,
            "fast_ma1": 2,
            "slow_ma0": 3,
            "slow_ma1": 4,
            "pos": 0,
        },
    }
    db.dbUpdate(account_id, "cta_strategy_data", d, {}, True)



if __name__ == "__main__":
    db = dbMongo()
    # PositionHeader("mytest")
    # AccountHeader("mytest")
    # TradeHeader("mytest")
    # OrderHeader("mytest")
    cta_strategy_dataHeader("mytest")
    cta_strategy_settingHeader("mytest")
    # BarHeader("mytest")


