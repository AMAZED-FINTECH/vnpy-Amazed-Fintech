""""""
from copy import copy

from vnpy.trader.engine import MainEngine
from vnpy.trader.object import (
    ContractData,
    OrderData,
    TradeData,
    PositionData,
    OrderRequest
)
from vnpy.trader.constant import (Direction, Offset, Exchange)


class OffsetConverter:
    """委托转换"""

    def __init__(self, main_engine: MainEngine):
        """初始化,添加主引擎"""
        self.main_engine = main_engine
        self.holdings = {}

    def update_position(self, position: PositionData):
        """更新持仓"""
        # 检查是否需要转换
        if not self.is_convert_required(position.vt_symbol):
            return

        # 查看当前持仓
        holding = self.get_position_holding(position.vt_symbol)
        # 更新当前持仓
        holding.update_position(position)

    def update_trade(self, trade: TradeData):
        """更新成交"""
        # 检查是否需要转换
        if not self.is_convert_required(trade.vt_symbol):
            return

        # 获取当前持仓
        holding = self.get_position_holding(trade.vt_symbol)
        # 更新持仓
        holding.update_trade(trade)

    def update_order(self, order: OrderData):
        """"""
        if not self.is_convert_required(order.vt_symbol):
            return

        holding = self.get_position_holding(order.vt_symbol)
        holding.update_order(order)

    def update_order_request(self, req: OrderRequest, vt_orderid: str):
        """OrderRequest也需要更新"""
        if not self.is_convert_required(req.vt_symbol):
            return

        holding = self.get_position_holding(req.vt_symbol)
        holding.update_order_request(req, vt_orderid)

    def get_position_holding(self, vt_symbol: str):
        """"""
        # 获取当前持仓
        holding = self.holdings.get(vt_symbol, None)
        if not holding:
            contract = self.main_engine.get_contract(vt_symbol)
            holding = PositionHolding(contract)
            self.holdings[vt_symbol] = holding
        return holding

    def convert_order_request(self, req: OrderRequest, lock: bool):
        """"""
        # 转换order_request
        if not self.is_convert_required(req.vt_symbol):
            return [req]

        # 持仓
        holding = self.get_position_holding(req.vt_symbol)

        # 如果设置锁,返回转换Order_Request锁
        if lock:
            return holding.convert_order_request_lock(req)
        # 如果请求为上期所,转换
        elif req.exchange == Exchange.SHFE:
            return holding.convert_order_request_shfe(req)
        # 否则，只返回convert_order_request
        else:
            return [req]

    def is_convert_required(self, vt_symbol: str):
        """
        Check if the contract needs offset convert.
        检查合约,是否有转换的需要
        只有是多空这种需要转换,如果合约都是净持仓,无需转换
        """
        contract = self.main_engine.get_contract(vt_symbol)

        # Only contracts with long-short position mode requires convert
        # 如果交易所是净持仓的概念,没有多空的概念,则返回净持仓
        if not contract:
            return False
        elif contract.net_position:
            return False
        else:
            return True


class PositionHolding:
    """本地持仓"""

    def __init__(self, contract: ContractData):
        """将合约信息输入，进行初始化"""
        self.vt_symbol = contract.vt_symbol
        self.exchange = contract.exchange

        # 正在委托的订单
        self.active_orders = {}

        # 多单数量
        self.long_pos = 0
        # 昨 多单
        self.long_yd = 0
        # 今 多单
        self.long_td = 0

        # 空单数量
        self.short_pos = 0
        # 昨 空单
        self.short_yd = 0
        # 今 空单
        self.short_td = 0

        # 多单->冻结
        self.long_pos_frozen = 0
        # 多单->昨天被冻结
        self.long_yd_frozen = 0
        # 多单->今天被冻结
        self.long_td_frozen = 0

        # 空单->冻结
        self.short_pos_frozen = 0
        # 空单->昨天被冻结
        self.short_yd_frozen = 0
        # 空单->今天被冻结
        self.short_td_frozen = 0

    def update_position(self, position: PositionData):
        """用于初始化仓位,查到一个仓位,"""
        # 如果是多单
        if position.direction == Direction.LONG:
            self.long_pos = position.volume
            self.long_yd = position.yd_volume
            self.long_td = self.long_pos - self.long_yd
        # 如果是空单
        else:
            self.short_pos = position.volume
            self.short_yd = position.yd_volume
            self.short_td = self.short_pos - self.short_yd

    def update_order(self, order: OrderData):
        """"""
        # 如果是活动单,还未成交,只记录下来就可以了
        if order.is_active():
            self.active_orders[order.vt_orderid] = order
        # 如果这个订单已经成交了,删除活动订单,并计算冻结的订单
        else:
            if order.vt_orderid in self.active_orders:
                self.active_orders.pop(order.vt_orderid)

        # 计算冻结的订单
        self.calculate_frozen()

    def update_order_request(self, req: OrderRequest, vt_orderid: str):
        """更新本地委托的订单"""
        gateway_name, orderid = vt_orderid.split(".")

        # 从本地的订单到gateway订单的转换
        order = req.create_order_data(orderid, gateway_name)
        # 更新订单
        self.update_order(order)

    def update_trade(self, trade: TradeData):
        """根据成交更新持仓"""
        # 方向是多
        if trade.direction == Direction.LONG:
            # 开仓
            if trade.offset == Offset.OPEN:
                self.long_td += trade.volume
            # 平今
            elif trade.offset == Offset.CLOSETODAY:
                self.short_td -= trade.volume
            # 平昨
            elif trade.offset == Offset.CLOSEYESTERDAY:
                self.short_yd -= trade.volume
            # 平单
            elif trade.offset == Offset.CLOSE:
                # 上期所的平单就是平昨
                if trade.exchange == Exchange.SHFE:
                    self.short_yd -= trade.volume
                # 其他交易所的平单就是平今
                else:
                    self.short_td -= trade.volume

                    # 如果今天没有那么多单平
                    if self.short_td < 0:
                        # 平昨,今单为0
                        self.short_yd += self.short_td
                        self.short_td = 0
        else:
            if trade.offset == Offset.OPEN:
                self.short_td += trade.volume
            elif trade.offset == Offset.CLOSETODAY:
                self.long_td -= trade.volume
            elif trade.offset == Offset.CLOSEYESTERDAY:
                self.long_yd -= trade.volume
            elif trade.offset == Offset.CLOSE:
                if trade.exchange == Exchange.SHFE:
                    self.long_yd -= trade.volume
                else:
                    self.long_td -= trade.volume

                    if self.long_td < 0:
                        self.long_yd += self.long_td
                        self.long_td = 0

        self.long_pos = self.long_td + self.long_yd
        self.short_pos = self.short_td + self.short_yd

    def calculate_frozen(self):
        """计算委托单"""
        self.long_pos_frozen = 0
        self.long_yd_frozen = 0
        self.long_td_frozen = 0

        self.short_pos_frozen = 0
        self.short_yd_frozen = 0
        self.short_td_frozen = 0

        for order in self.active_orders.values():
            # Ignore position open orders
            if order.offset == Offset.OPEN:
                continue

            frozen = order.volume - order.traded

            if order.direction == Direction.LONG:
                if order.offset == Offset.CLOSETODAY:
                    self.short_td_frozen += frozen
                elif order.offset == Offset.CLOSEYESTERDAY:
                    self.short_yd_frozen += frozen
                elif order.offset == Offset.CLOSE:
                    self.short_td_frozen += frozen

                    if self.short_td_frozen > self.short_td:
                        self.short_yd_frozen += (self.short_td_frozen
                                                 - self.short_td)
                        self.short_td_frozen = self.short_td
            elif order.direction == Direction.SHORT:
                if order.offset == Offset.CLOSETODAY:
                    self.long_td_frozen += frozen
                elif order.offset == Offset.CLOSEYESTERDAY:
                    self.long_yd_frozen += frozen
                elif order.offset == Offset.CLOSE:
                    self.long_td_frozen += frozen

                    if self.long_td_frozen > self.long_td:
                        self.long_yd_frozen += (self.long_td_frozen
                                                - self.long_td)
                        self.long_td_frozen = self.long_td

            self.long_pos_frozen = self.long_td_frozen + self.long_yd_frozen
            self.short_pos_frozen = self.short_td_frozen + self.short_yd_frozen

    def convert_order_request_shfe(self, req: OrderRequest):
        """"""
        if req.offset == Offset.OPEN:
            return [req]

        if req.direction == Direction.LONG:
            pos_available = self.short_pos - self.short_pos_frozen
            td_available = self.short_td - self.short_td_frozen
        else:
            pos_available = self.long_pos - self.long_pos_frozen
            td_available = self.long_td - self.long_td_frozen

        if req.volume > pos_available:
            return []
        elif req.volume <= td_available:
            req_td = copy(req)
            req_td.offset = Offset.CLOSETODAY
            return [req_td]
        else:
            req_list = []

            if td_available > 0:
                req_td = copy(req)
                req_td.offset = Offset.CLOSETODAY
                req_td.volume = td_available
                req_list.append(req_td)

            req_yd = copy(req)
            req_yd.offset = Offset.CLOSEYESTERDAY
            req_yd.volume = req.volume - td_available
            req_list.append(req_yd)

            return req_list

    def convert_order_request_lock(self, req: OrderRequest):
        """"""
        if req.direction == Direction.LONG:
            td_volume = self.short_td
            yd_available = self.short_yd - self.short_yd_frozen
        else:
            td_volume = self.long_td
            yd_available = self.long_yd - self.long_yd_frozen

        # If there is td_volume, we can only lock position
        if td_volume:
            req_open = copy(req)
            req_open.offset = Offset.OPEN
            return [req_open]
        # If no td_volume, we close opposite yd position first
        # then open new position
        else:
            open_volume = max(0, req.volume - yd_available)
            req_list = []

            if yd_available:
                req_yd = copy(req)
                if self.exchange == Exchange.SHFE:
                    req_yd.offset = Offset.CLOSEYESTERDAY
                else:
                    req_yd.offset = Offset.CLOSE
                req_list.append(req_yd)

            if open_volume:
                req_open = copy(req)
                req_open.offset = Offset.OPEN
                req_open.volume = open_volume
                req_list.append(req_open)

            return req_list
