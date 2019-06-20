# noinspection PyUnresolvedReferences
import typing
if typing.TYPE_CHECKING:
    from typing import *
    # noinspection PyUnresolvedReferences
    from enum import Enum
    from .vntap import *


def set_async_callback_exception_handler(handler: Callable[[AsyncDispatchException], None]):
    """
    set a customize exception handler for async callback in this module(pyd)
    \a handler should return True if it handles that exception,

    :note: If the return value of \a handler is not True, exception will be re-thrown.
    """
    ...


class AsyncDispatchException:
    what: str
    instance: object
    function_name: str


class TapAPIApplicationInfo():
    
    
    AuthCode: str
    KeyOperationLogPath: str
    
    
class TapAPICommodity():
    
    
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    
    
class TapAPIContract():
    
    
    Commodity: ITapTrade.TapAPICommodity
    ContractNo1: str
    StrikePrice1: str
    CallOrPutFlag1: str
    ContractNo2: str
    StrikePrice2: str
    CallOrPutFlag2: str
    
    
class TapAPIExchangeInfo():
    
    
    ExchangeNo: str
    ExchangeName: str
    
    
class TapAPIChangePasswordReq():
    
    
    AccountNo: str
    PasswordType: str
    OldPassword: str
    NewPassword: str
    
    
class TapAPIAuthPasswordReq():
    
    
    AccountNo: str
    PasswordType: str
    Password: str
    
    
class TapAPITradeLoginAuth():
    
    
    UserNo: str
    ISModifyPassword: str
    Password: str
    NewPassword: str
    
    
class TapAPITradeLoginRspInfo():
    
    
    UserNo: str
    UserType: int
    UserName: str
    ReservedInfo: str
    LastLoginIP: str
    LastLoginProt: int
    LastLoginTime: str
    LastLogoutTime: str
    TradeDate: str
    LastSettleTime: str
    StartTime: str
    InitTime: str
    
    
class TapAPIRequestVertificateCodeRsp():
    
    
    SecondSerialID: str
    Effective: int
    
    
class TapAPIAccQryReq():
    
    
    
    
    
class TapAPIAccountInfo():
    
    
    AccountNo: str
    AccountType: str
    AccountState: str
    AccountTradeRight: str
    CommodityGroupNo: str
    AccountShortName: str
    AccountEnShortName: str
    
    
class TapAPINewOrder():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    ContractNo2: str
    StrikePrice2: str
    CallOrPutFlag2: str
    OrderType: str
    OrderSource: str
    TimeInForce: str
    ExpireTime: str
    IsRiskOrder: str
    OrderSide: str
    PositionEffect: str
    PositionEffect2: str
    InquiryNo: str
    HedgeFlag: str
    OrderPrice: float
    OrderPrice2: float
    StopPrice: float
    OrderQty: int
    OrderMinQty: int
    MinClipSize: int
    MaxClipSize: int
    RefInt: int
    RefDouble: float
    RefString: str
    ClientID: str
    TacticsType: str
    TriggerCondition: str
    TriggerPriceType: str
    AddOneIsValid: str
    
    
class TapAPIOrderInfo():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    ContractNo2: str
    StrikePrice2: str
    CallOrPutFlag2: str
    OrderType: str
    OrderSource: str
    TimeInForce: str
    ExpireTime: str
    IsRiskOrder: str
    OrderSide: str
    PositionEffect: str
    PositionEffect2: str
    InquiryNo: str
    HedgeFlag: str
    OrderPrice: float
    OrderPrice2: float
    StopPrice: float
    OrderQty: int
    OrderMinQty: int
    RefInt: int
    RefDouble: float
    RefString: str
    MinClipSize: int
    MaxClipSize: int
    LicenseNo: str
    ServerFlag: str
    OrderNo: str
    ClientOrderNo: str
    ClientID: str
    TacticsType: str
    TriggerCondition: str
    TriggerPriceType: str
    AddOneIsValid: str
    ClientLocalIP: str
    ClientMac: str
    ClientIP: str
    OrderStreamID: int
    UpperNo: str
    UpperChannelNo: str
    OrderLocalNo: str
    UpperStreamID: int
    OrderSystemNo: str
    OrderExchangeSystemNo: str
    OrderParentSystemNo: str
    OrderInsertUserNo: str
    OrderInsertTime: str
    OrderCommandUserNo: str
    OrderUpdateUserNo: str
    OrderUpdateTime: str
    OrderState: str
    OrderMatchPrice: float
    OrderMatchPrice2: float
    OrderMatchQty: int
    OrderMatchQty2: int
    ErrorCode: int
    ErrorText: str
    IsBackInput: str
    IsDeleted: str
    IsAddOne: str
    
    
class TapAPIOrderInfoNotice():
    
    
    SessionID: int
    ErrorCode: int
    OrderInfo: ITapTrade.TapAPIOrderInfo
    
    
class TapAPIOrderActionRsp():
    
    
    ActionType: str
    OrderInfo: ITapTrade.TapAPIOrderInfo
    
    
class TapAPIAmendOrder():
    
    
    ReqData: ITapTrade.TapAPINewOrder
    ServerFlag: str
    OrderNo: str
    
    
class TapAPIOrderCancelReq():
    
    
    RefInt: int
    RefDouble: float
    RefString: str
    ServerFlag: str
    OrderNo: str
    
    
class TapAPIOrderQryReq():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    OrderType: str
    OrderSource: str
    TimeInForce: str
    ExpireTime: str
    IsRiskOrder: str
    ServerFlag: str
    OrderNo: str
    IsBackInput: str
    IsDeleted: str
    IsAddOne: str
    
    
class TapAPIOrderProcessQryReq():
    
    
    ServerFlag: str
    OrderNo: str
    
    
class TapAPIFillQryReq():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    MatchSource: str
    MatchSide: str
    PositionEffect: str
    ServerFlag: str
    OrderNo: str
    UpperNo: str
    IsDeleted: str
    IsAddOne: str
    
    
class TapAPIFillInfo():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    MatchSource: str
    MatchSide: str
    PositionEffect: str
    ServerFlag: str
    OrderNo: str
    OrderSystemNo: str
    MatchNo: str
    UpperMatchNo: str
    ExchangeMatchNo: str
    MatchDateTime: str
    UpperMatchDateTime: str
    UpperNo: str
    MatchPrice: float
    MatchQty: int
    IsDeleted: str
    IsAddOne: str
    FeeCurrencyGroup: str
    FeeCurrency: str
    FeeValue: float
    IsManualFee: str
    ClosePrositionPrice: float
    
    
class TapAPICloseQryReq():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    
    
class TapAPICloseInfo():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    CloseSide: str
    CloseQty: int
    OpenPrice: float
    ClosePrice: float
    OpenMatchNo: str
    OpenMatchDateTime: str
    CloseMatchNo: str
    CloseMatchDateTime: str
    CloseStreamId: int
    CommodityCurrencyGroup: str
    CommodityCurrency: str
    CloseProfit: float
    
    
class TapAPIPositionQryReq():
    
    
    AccountNo: str
    
    
class TapAPIPositionInfo():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    MatchSide: str
    HedgeFlag: str
    PositionNo: str
    ServerFlag: str
    OrderNo: str
    MatchNo: str
    UpperNo: str
    PositionPrice: float
    PositionQty: int
    PositionStreamId: int
    CommodityCurrencyGroup: str
    CommodityCurrency: str
    CalculatePrice: float
    AccountInitialMargin: float
    AccountMaintenanceMargin: float
    UpperInitialMargin: float
    UpperMaintenanceMargin: float
    PositionProfit: float
    LMEPositionProfit: float
    OptionMarketValue: float
    IsHistory: str
    
    
class TapAPIPositionProfit():
    
    
    PositionNo: str
    PositionStreamId: int
    PositionProfit: float
    LMEPositionProfit: float
    OptionMarketValue: float
    CalculatePrice: float
    
    
class TapAPIPositionProfitNotice():
    
    
    IsLast: str
    Data: ITapTrade.TapAPIPositionProfit
    
    
class TapAPIPositionSummary():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    MatchSide: str
    PositionPrice: float
    PositionQty: int
    HisPositionQty: int
    
    
class TapAPIFundReq():
    
    
    AccountNo: str
    
    
class TapAPIFundData():
    
    
    AccountNo: str
    CurrencyGroupNo: str
    CurrencyNo: str
    TradeRate: float
    FutureAlg: str
    OptionAlg: str
    PreBalance: float
    PreUnExpProfit: float
    PreLMEPositionProfit: float
    PreEquity: float
    PreAvailable1: float
    PreMarketEquity: float
    CashInValue: float
    CashOutValue: float
    CashAdjustValue: float
    CashPledged: float
    FrozenFee: float
    FrozenDeposit: float
    AccountFee: float
    SwapInValue: float
    SwapOutValue: float
    PremiumIncome: float
    PremiumPay: float
    CloseProfit: float
    FrozenFund: float
    UnExpProfit: float
    ExpProfit: float
    PositionProfit: float
    LmePositionProfit: float
    OptionMarketValue: float
    AccountIntialMargin: float
    AccountMaintenanceMargin: float
    UpperInitalMargin: float
    UpperMaintenanceMargin: float
    Discount: float
    Balance: float
    Equity: float
    Available: float
    CanDraw: float
    MarketEquity: float
    AuthMoney: float
    
    
class TapAPICommodityInfo():
    
    
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    CommodityName: str
    CommodityEngName: str
    RelateExchangeNo: str
    RelateCommodityType: str
    RelateCommodityNo: str
    RelateExchangeNo2: str
    RelateCommodityType2: str
    RelateCommodityNo2: str
    CurrencyGroupNo: str
    TradeCurrency: str
    ContractSize: float
    OpenCloseMode: str
    StrikePriceTimes: float
    CommodityTickSize: float
    CommodityDenominator: int
    CmbDirect: str
    DeliveryMode: str
    DeliveryDays: int
    AddOneTime: str
    CommodityTimeZone: int
    IsAddOne: str
    
    
class TapAPITradeContractInfo():
    
    
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo1: str
    StrikePrice1: str
    CallOrPutFlag1: str
    ContractNo2: str
    StrikePrice2: str
    CallOrPutFlag2: str
    ContractType: str
    QuoteUnderlyingContract: str
    ContractName: str
    ContractExpDate: str
    LastTradeDate: str
    FirstNoticeDate: str
    
    
class TapAPICurrencyInfo():
    
    
    CurrencyNo: str
    CurrencyGroupNo: str
    TradeRate: float
    TradeRate2: float
    FutureAlg: str
    OptionAlg: str
    
    
class TapAPITradeMessageReq():
    
    
    AccountNo: str
    AccountAttributeNo: str
    BenginSendDateTime: str
    EndSendDateTime: str
    
    
class TapAPITradeMessage():
    
    
    SerialID: int
    AccountNo: str
    TMsgValidDateTime: str
    TMsgTitle: str
    TMsgContent: str
    TMsgType: str
    TMsgLevel: str
    IsSendBySMS: str
    IsSendByEMail: str
    Sender: str
    SendDateTime: str
    
    
class TapAPIBillQryReq():
    
    
    UserNo: str
    BillType: str
    BillDate: str
    BillFileType: str
    
    
class TapAPIBillQryRsp():
    
    
    Reqdata: ITapTrade.TapAPIBillQryReq
    BillLen: int
    BillText: List[str]
    
    
class TapAPIHisOrderQryReq():
    
    
    AccountNo: str
    AccountAttributeNo: str
    BeginDate: str
    EndDate: str
    
    
class TapAPIHisOrderQryRsp():
    
    
    Date: str
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    ContractNo2: str
    StrikePrice2: str
    CallOrPutFlag2: str
    OrderType: str
    OrderSource: str
    TimeInForce: str
    ExpireTime: str
    IsRiskOrder: str
    OrderSide: str
    PositionEffect: str
    PositionEffect2: str
    InquiryNo: str
    HedgeFlag: str
    OrderPrice: float
    OrderPrice2: float
    StopPrice: float
    OrderQty: int
    OrderMinQty: int
    OrderCanceledQty: int
    RefInt: int
    RefDouble: float
    RefString: str
    ServerFlag: str
    OrderNo: str
    OrderStreamID: int
    UpperNo: str
    UpperChannelNo: str
    OrderLocalNo: str
    UpperStreamID: int
    OrderSystemNo: str
    OrderExchangeSystemNo: str
    OrderParentSystemNo: str
    OrderInsertUserNo: str
    OrderInsertTime: str
    OrderCommandUserNo: str
    OrderUpdateUserNo: str
    OrderUpdateTime: str
    OrderState: str
    OrderMatchPrice: float
    OrderMatchPrice2: float
    OrderMatchQty: int
    OrderMatchQty2: int
    ErrorCode: int
    ErrorText: str
    IsBackInput: str
    IsDeleted: str
    IsAddOne: str
    AddOneIsValid: str
    MinClipSize: int
    MaxClipSize: int
    LicenseNo: str
    TacticsType: str
    TriggerCondition: str
    TriggerPriceType: str
    
    
class TapAPIHisMatchQryReq():
    
    
    AccountNo: str
    AccountAttributeNo: str
    BeginDate: str
    EndDate: str
    CountType: str
    
    
class TapAPIHisMatchQryRsp():
    
    
    SettleDate: str
    TradeDate: str
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    MatchSource: str
    MatchSide: str
    PositionEffect: str
    HedgeFlag: str
    MatchPrice: float
    MatchQty: int
    OrderNo: str
    MatchNo: str
    MatchStreamID: int
    UpperNo: str
    MatchCmbNo: str
    ExchangeMatchNo: str
    MatchUpperStreamID: int
    CommodityCurrencyGroup: str
    CommodityCurrency: str
    Turnover: float
    PremiumIncome: float
    PremiumPay: float
    AccountFee: float
    AccountFeeCurrencyGroup: str
    AccountFeeCurrency: str
    IsManualFee: str
    AccountOtherFee: float
    UpperFee: float
    UpperFeeCurrencyGroup: str
    UpperFeeCurrency: str
    IsUpperManualFee: str
    UpperOtherFee: float
    MatchDateTime: str
    UpperMatchDateTime: str
    CloseProfit: float
    ClosePrice: float
    CloseQty: int
    SettleGroupNo: str
    OperatorNo: str
    OperateTime: str
    
    
class TapAPIHisOrderProcessQryReq():
    
    
    Date: str
    OrderNo: str
    
    
class TapAPIHisPositionQryReq():
    
    
    AccountNo: str
    Date: str
    SettleFlag: str
    
    
class TapAPIHisPositionQryRsp():
    
    
    SettleDate: str
    OpenDate: str
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    MatchSide: str
    HedgeFlag: str
    PositionPrice: float
    PositionQty: int
    OrderNo: str
    PositionNo: str
    UpperNo: str
    CurrencyGroup: str
    Currency: str
    PreSettlePrice: float
    SettlePrice: float
    PositionDProfit: float
    LMEPositionProfit: float
    OptionMarketValue: float
    AccountInitialMargin: float
    AccountMaintenanceMargin: float
    UpperInitialMargin: float
    UpperMaintenanceMargin: float
    SettleGroupNo: str
    
    
class TapAPIHisDeliveryQryReq():
    
    
    AccountNo: str
    AccountAttributeNo: str
    BeginDate: str
    EndDate: str
    CountType: str
    
    
class TapAPIHisDeliveryQryRsp():
    
    
    DeliveryDate: str
    OpenDate: str
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    MatchSource: str
    OpenSide: str
    OpenPrice: float
    DeliveryPrice: float
    DeliveryQty: int
    FrozenQty: int
    OpenNo: str
    UpperNo: str
    CommodityCurrencyGroupy: str
    CommodityCurrency: str
    PreSettlePrice: float
    DeliveryProfit: float
    AccountFrozenInitialMargin: float
    AccountFrozenMaintenanceMargin: float
    UpperFrozenInitialMargin: float
    UpperFrozenMaintenanceMargin: float
    AccountFeeCurrencyGroup: str
    AccountFeeCurrency: str
    AccountDeliveryFee: float
    UpperFeeCurrencyGroup: str
    UpperFeeCurrency: str
    UpperDeliveryFee: float
    DeliveryMode: str
    OperatorNo: str
    OperateTime: str
    SettleGourpNo: str
    
    
class TapAPIAccountCashAdjustQryReq():
    
    
    SerialID: int
    AccountNo: str
    AccountAttributeNo: str
    BeginDate: str
    EndDate: str
    
    
class TapAPIAccountCashAdjustQryRsp():
    
    
    Date: str
    AccountNo: str
    CashAdjustType: str
    CurrencyGroupNo: str
    CurrencyNo: str
    CashAdjustValue: float
    CashAdjustRemark: str
    OperateTime: str
    OperatorNo: str
    AccountBank: str
    BankAccount: str
    AccountLWFlag: str
    CompanyBank: str
    InternalBankAccount: str
    CompanyLWFlag: str
    
    
class TapAPIAccountFeeRentQryReq():
    
    
    AccountNo: str
    
    
class TapAPIAccountFeeRentQryRsp():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    MatchSource: str
    CalculateMode: str
    CurrencyGroupNo: str
    CurrencyNo: str
    OpenCloseFee: float
    CloseTodayFee: float
    
    
class TapAPIAccountMarginRentQryReq():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    
    
class TapAPIAccountMarginRentQryRsp():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    CalculateMode: str
    CurrencyGroupNo: str
    CurrencyNo: str
    InitialMargin: float
    MaintenanceMargin: float
    SellInitialMargin: float
    SellMaintenanceMargin: float
    LockMargin: float
    
    
class TapAPIOrderQuoteMarketNotice():
    
    
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    OrderSide: str
    OrderQty: int
    
    
class TapAPIOrderMarketInsertReq():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    OrderType: str
    TimeInForce: str
    ExpireTime: str
    OrderSource: str
    BuyPositionEffect: str
    SellPositionEffect: str
    AddOneIsValid: str
    OrderBuyPrice: float
    OrderSellPrice: float
    OrderBuyQty: int
    OrderSellQty: int
    ClientBuyOrderNo: str
    ClientSellOrderNo: str
    RefInt: int
    RefDouble: float
    RefString: str
    Remark: str
    
    
class TapAPIOrderMarketInsertRsp():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    OrderType: str
    TimeInForce: str
    ExpireTime: str
    OrderSource: str
    BuyPositionEffect: str
    SellPositionEffect: str
    OrderBuyPrice: float
    OrderSellPrice: float
    OrderBuyQty: int
    OrderSellQty: int
    ServerFlag: str
    OrderBuyNo: str
    OrderSellNo: str
    AddOneIsValid: str
    OrderMarketUserNo: str
    OrderMarketTime: str
    RefInt: int
    RefDouble: float
    RefString: str
    ClientBuyOrderNo: str
    ClientSellOrderNo: str
    ErrorCode: int
    ErrorText: str
    ClientLocalIP: str
    ClientMac: str
    ClientIP: str
    Remark: str
    
    
class TapAPIOrderMarketDeleteReq():
    
    
    ServerFlag: str
    OrderBuyNo: str
    OrderSellNo: str
    
    
class TapAPIOrderLocalRemoveReq():
    
    
    ServerFlag: str
    OrderNo: str
    
    
class TapAPIOrderLocalRemoveRsp():
    
    
    req: ITapTrade.TapAPIOrderLocalRemoveReq
    ClientLocalIP: str
    ClientMac: str
    ClientIP: str
    
    
class TapAPIOrderLocalInputReq():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    ContractNo2: str
    StrikePrice2: str
    CallOrPutFlag2: str
    OrderType: str
    OrderSource: str
    TimeInForce: str
    ExpireTime: str
    IsRiskOrder: str
    OrderSide: str
    PositionEffect: str
    PositionEffect2: str
    InquiryNo: str
    HedgeFlag: str
    OrderPrice: float
    OrderPrice2: float
    StopPrice: float
    OrderQty: int
    OrderMinQty: int
    OrderSystemNo: str
    OrderExchangeSystemNo: str
    UpperNo: str
    OrderMatchPrice: float
    OrderMatchPrice2: float
    OrderMatchQty: int
    OrderMatchQty2: int
    OrderState: str
    IsAddOne: str
    
    
class TapAPIOrderLocalModifyReq():
    
    
    req: ITapTrade.TapAPIOrderLocalInputReq
    ServerFlag: str
    OrderNo: str
    
    
class TapAPIOrderLocalTransferReq():
    
    
    AccountNo: str
    ServerFlag: str
    OrderNo: str
    
    
class TapAPIFillLocalInputReq():
    
    
    AccountNo: str
    ExchangeNo: str
    CommodityType: str
    CommodityNo: str
    ContractNo: str
    StrikePrice: str
    CallOrPutFlag: str
    MatchSide: str
    PositionEffect: str
    HedgeFlag: str
    MatchPrice: float
    MatchQty: int
    OrderSystemNo: str
    UpperMatchNo: str
    MatchDateTime: str
    UpperMatchDateTime: str
    UpperNo: str
    IsAddOne: str
    FeeCurrencyGroup: str
    FeeCurrency: str
    FeeValue: float
    IsManualFee: str
    ClosePositionPrice: float
    
    
class TapAPIFillLocalRemoveReq():
    
    
    ServerFlag: str
    MatchNo: str
    
    
class TapAPITradingCalendarQryRsp():
    
    
    CurrTradeDate: str
    LastSettlementDate: str
    PromptDate: str
    LastPromptDate: str
    
    
class ITapTradeAPINotify():
    
    
    
    
    def OnConnect(self, )->None:
        ...
    def OnRspLogin(self, errorCode: int, loginRspInfo: ITapTrade.TapAPITradeLoginRspInfo)->None:
        ...
    def OnRtnContactInfo(self, errorCode: int, isLast: str, ContactInfo: str)->None:
        ...
    def OnRspRequestVertificateCode(self, sessionID: int, errorCode: int, rsp: ITapTrade.TapAPIRequestVertificateCodeRsp)->None:
        ...
    def OnExpriationDate(self, date: str, days: int)->None:
        ...
    def OnAPIReady(self, errorCode: int)->None:
        ...
    def OnDisconnect(self, reasonCode: int)->None:
        ...
    def OnRspChangePassword(self, sessionID: int, errorCode: int)->None:
        ...
    def OnRspAuthPassword(self, sessionID: int, errorCode: int)->None:
        ...
    def OnRspQryTradingDate(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPITradingCalendarQryRsp)->None:
        ...
    def OnRspSetReservedInfo(self, sessionID: int, errorCode: int, info: str)->None:
        ...
    def OnRspQryAccount(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIAccountInfo)->None:
        ...
    def OnRspQryFund(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIFundData)->None:
        ...
    def OnRtnFund(self, info: ITapTrade.TapAPIFundData)->None:
        ...
    def OnRspQryExchange(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIExchangeInfo)->None:
        ...
    def OnRspQryCommodity(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPICommodityInfo)->None:
        ...
    def OnRspQryContract(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPITradeContractInfo)->None:
        ...
    def OnRtnContract(self, info: ITapTrade.TapAPITradeContractInfo)->None:
        ...
    def OnRspOrderAction(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPIOrderActionRsp)->None:
        ...
    def OnRtnOrder(self, info: ITapTrade.TapAPIOrderInfoNotice)->None:
        ...
    def OnRspQryOrder(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIOrderInfo)->None:
        ...
    def OnRspQryOrderProcess(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIOrderInfo)->None:
        ...
    def OnRspQryFill(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIFillInfo)->None:
        ...
    def OnRtnFill(self, info: ITapTrade.TapAPIFillInfo)->None:
        ...
    def OnRspQryPosition(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIPositionInfo)->None:
        ...
    def OnRtnPosition(self, info: ITapTrade.TapAPIPositionInfo)->None:
        ...
    def OnRspQryPositionSummary(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIPositionSummary)->None:
        ...
    def OnRtnPositionSummary(self, info: ITapTrade.TapAPIPositionSummary)->None:
        ...
    def OnRtnPositionProfit(self, info: ITapTrade.TapAPIPositionProfitNotice)->None:
        ...
    def OnRspQryCurrency(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPICurrencyInfo)->None:
        ...
    def OnRspQryTradeMessage(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPITradeMessage)->None:
        ...
    def OnRtnTradeMessage(self, info: ITapTrade.TapAPITradeMessage)->None:
        ...
    def OnRspQryHisOrder(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIHisOrderQryRsp)->None:
        ...
    def OnRspQryHisOrderProcess(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIHisOrderQryRsp)->None:
        ...
    def OnRspQryHisMatch(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIHisMatchQryRsp)->None:
        ...
    def OnRspQryHisPosition(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIHisPositionQryRsp)->None:
        ...
    def OnRspQryHisDelivery(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIHisDeliveryQryRsp)->None:
        ...
    def OnRspQryAccountCashAdjust(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIAccountCashAdjustQryRsp)->None:
        ...
    def OnRspQryBill(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIBillQryRsp)->None:
        ...
    def OnRspQryAccountFeeRent(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIAccountFeeRentQryRsp)->None:
        ...
    def OnRspQryAccountMarginRent(self, sessionID: int, errorCode: int, isLast: str, info: ITapTrade.TapAPIAccountMarginRentQryRsp)->None:
        ...
    def OnRspHKMarketOrderInsert(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPIOrderMarketInsertRsp)->None:
        ...
    def OnRspHKMarketOrderDelete(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPIOrderMarketInsertRsp)->None:
        ...
    def OnHKMarketQuoteNotice(self, info: ITapTrade.TapAPIOrderQuoteMarketNotice)->None:
        ...
    def OnRspOrderLocalRemove(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPIOrderLocalRemoveRsp)->None:
        ...
    def OnRspOrderLocalInput(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPIOrderInfo)->None:
        ...
    def OnRspOrderLocalModify(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPIOrderInfo)->None:
        ...
    def OnRspOrderLocalTransfer(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPIOrderInfo)->None:
        ...
    def OnRspFillLocalInput(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPIFillLocalInputReq)->None:
        ...
    def OnRspFillLocalRemove(self, sessionID: int, errorCode: int, info: ITapTrade.TapAPIFillLocalRemoveReq)->None:
        ...
class ITapTradeAPI():
    
    
    
    
    def SetAPINotify(self, pSpi: ITapTrade.ITapTradeAPINotify)->int:
        ...
    def SetHostAddress(self, IP: str, port: int, bSSL: bool)->int:
        ...
    def Login(self, loginAuth: ITapTrade.TapAPITradeLoginAuth)->int:
        ...
    def RequestVertificateCode(self, ContactInfo: str)->Tuple[int,int]:
        return "retv","sessionID"
    def SetVertificateCode(self, VertificateCode: str)->int:
        ...
    def Disconnect(self, )->int:
        ...
    def ChangePassword(self, req: ITapTrade.TapAPIChangePasswordReq)->Tuple[int,int]:
        return "retv","sessionID"
    def AuthPassword(self, req: ITapTrade.TapAPIAuthPasswordReq)->Tuple[int,int]:
        return "retv","sessionID"
    def HaveCertainRight(self, rightID: int)->int:
        ...
    def QryTradingDate(self, )->Tuple[int,int]:
        return "retv","sessionID"
    def SetReservedInfo(self, info: str)->Tuple[int,int]:
        return "retv","sessionID"
    def QryAccount(self, qryReq: ITapTrade.TapAPIAccQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryFund(self, qryReq: ITapTrade.TapAPIFundReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryExchange(self, )->Tuple[int,int]:
        return "retv","sessionID"
    def QryCommodity(self, )->Tuple[int,int]:
        return "retv","sessionID"
    def QryContract(self, qryReq: ITapTrade.TapAPICommodity)->Tuple[int,int]:
        return "retv","sessionID"
    def InsertOrder(self, order: ITapTrade.TapAPINewOrder)->Tuple[int,int,"level 2 pointer:ITapTrade::TAPISTR_50 *"]:
        return "retv","sessionID"
    def CancelOrder(self, order: ITapTrade.TapAPIOrderCancelReq)->Tuple[int,int]:
        return "retv","sessionID"
    def AmendOrder(self, order: ITapTrade.TapAPIAmendOrder)->Tuple[int,int]:
        return "retv","sessionID"
    def ActivateOrder(self, order: ITapTrade.TapAPIOrderCancelReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryOrder(self, qryReq: ITapTrade.TapAPIOrderQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryOrderProcess(self, qryReq: ITapTrade.TapAPIOrderProcessQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryFill(self, qryReq: ITapTrade.TapAPIFillQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryPosition(self, qryReq: ITapTrade.TapAPIPositionQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryPositionSummary(self, qryReq: ITapTrade.TapAPIPositionQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryCurrency(self, )->Tuple[int,int]:
        return "retv","sessionID"
    def QryAccountCashAdjust(self, qryReq: ITapTrade.TapAPIAccountCashAdjustQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryTradeMessage(self, qryReq: ITapTrade.TapAPITradeMessageReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryBill(self, qryReq: ITapTrade.TapAPIBillQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryHisOrder(self, qryReq: ITapTrade.TapAPIHisOrderQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryHisOrderProcess(self, qryReq: ITapTrade.TapAPIHisOrderProcessQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryHisMatch(self, qryReq: ITapTrade.TapAPIHisMatchQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryHisPosition(self, qryReq: ITapTrade.TapAPIHisPositionQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryHisDelivery(self, qryReq: ITapTrade.TapAPIHisDeliveryQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryAccountFeeRent(self, qryReq: ITapTrade.TapAPIAccountFeeRentQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def QryAccountMarginRent(self, qryReq: ITapTrade.TapAPIAccountMarginRentQryReq)->Tuple[int,int]:
        return "retv","sessionID"
    def InsertHKMarketOrder(self, order: ITapTrade.TapAPIOrderMarketInsertReq)->Tuple[int,int,"level 2 pointer:ITapTrade::TAPISTR_50 *","level 2 pointer:ITapTrade::TAPISTR_50 *"]:
        return "retv","sessionID","ClientBuyOrderNo"
    def CancelHKMarketOrder(self, order: ITapTrade.TapAPIOrderMarketDeleteReq)->Tuple[int,int]:
        return "retv","sessionID"
    def OrderLocalRemove(self, order: ITapTrade.TapAPIOrderLocalRemoveReq)->Tuple[int,int]:
        return "retv","sessionID"
    def OrderLocalInput(self, order: ITapTrade.TapAPIOrderLocalInputReq)->Tuple[int,int]:
        return "retv","sessionID"
    def OrderLocalModify(self, order: ITapTrade.TapAPIOrderLocalModifyReq)->Tuple[int,int]:
        return "retv","sessionID"
    def OrderLocalTransfer(self, order: ITapTrade.TapAPIOrderLocalTransferReq)->Tuple[int,int]:
        return "retv","sessionID"
    def FillLocalInput(self, fill: ITapTrade.TapAPIFillLocalInputReq)->Tuple[int,int]:
        return "retv","sessionID"
    def FillLocalRemove(self, fill: ITapTrade.TapAPIFillLocalRemoveReq)->Tuple[int,int]:
        return "retv","sessionID"
TAPISTR_10 = str
TAPISTR_20 = str
TAPISTR_30 = str
TAPISTR_40 = str
TAPISTR_50 = str
TAPISTR_70 = str
TAPISTR_100 = str
TAPISTR_300 = str
TAPISTR_500 = str
TAPISTR_2000 = str
TAPIAUTHCODE = str
TAPIMACTYPE = str
TAPISecondSerialIDType = str
TAPICHAR = str
TAPIClientIDType = str
TAPIINT32 = int
TAPIUINT32 = int
TAPIINT64 = int
TAPIUINT64 = int
TAPIUINT16 = int
TAPIUINT8 = int
TAPIREAL64 = float
TAPIYNFLAG = str
TAPIPasswordType = str
TAPIDTSTAMP = str
TAPIDATETIME = str
TAPIDATE = str
TAPITIME = str
TAPILOGLEVEL = str
TAPIOptionType = str
TAPICommodityType = str
TAPICallOrPutFlagType = str
TAPIAccountType = str
TAPIRightIDType = int
TAPIUserTypeType = int
TAPIAccountState = str
TAPIAccountRightType = str
TAPIOrderTypeType = str
TAPIOrderSourceType = str
TAPITimeInForceType = str
TAPISideType = str
TAPIPositionEffectType = str
TAPIHedgeFlagType = str
TAPIOrderStateType = str
TAPICalculateModeType = str
TAPIMatchSourceType = str
TAPIOpenCloseModeType = str
TAPIFutureAlgType = str
TAPIOptionAlgType = str
TAPISendType = str
TAPIBankAccountLWFlagType = str
TAPICashAdjustTypeType = str
TAPIMarginCalculateModeType = str
TAPIOptionMarginCalculateModeType = str
TAPICmbDirectType = str
TAPIDeliveryModeType = str
TAPIContractTypeType = str
TAPITacticsTypeType = str
TAPIORDERACT = str
TAPITriggerConditionType = str
TAPITriggerPriceTypeType = str
TAPITradingStateType = str
TAPINoticeIgnoreFlagType = int
TAPIOrderQryTypeType = str
TapAPIOrderDeactivateReq = ITapTrade.TapAPIOrderCancelReq
TapAPIOrderActivateReq = ITapTrade.TapAPIOrderCancelReq
TapAPIOrderDeleteReq = ITapTrade.TapAPIOrderCancelReq
TAPIMsgReceiverType = str
TAPIMsgLevelType = str
TAPIMsgTypeType = str
TAPIBillTypeType = str
TAPIBillFileTypeType = str
TapAPIHisOrderProcessQryRsp = ITapTrade.TapAPIHisOrderQryRsp
TAPISettleFlagType = str
TapAPIOrderMarketDeleteRsp = ITapTrade.TapAPIOrderMarketInsertRsp
TapAPIOrderLocalInputRsp = ITapTrade.TapAPIOrderInfo
TapAPIOrderLocalModifyRsp = ITapTrade.TapAPIOrderInfo
TapAPIOrderLocalTransferRsp = ITapTrade.TapAPIOrderInfo
TapAPIFillLocalInputRsp = ITapTrade.TapAPIFillLocalInputReq
TapAPIFillLocalRemoveRsp = ITapTrade.TapAPIFillLocalRemoveReq
APIYNFLAG_YES: str
APIYNFLAG_NO: str
APIPASSWORD_TRADE: str
APIPASSWORD_PHONE: str
APILOGLEVEL_NONE: str
APILOGLEVEL_ERROR: str
APILOGLEVEL_WARNING: str
APILOGLEVEL_DEBUG: str
TAPI_OPTION_TYPE_AMERICAN: str
TAPI_OPTION_TYPE_AMERICAN2: str
TAPI_OPTION_TYPE_EUROPEAN: str
TAPI_OPTION_TYPE_EUROPEAN2: str
TAPI_COMMODITY_TYPE_NONE: str
TAPI_COMMODITY_TYPE_SPOT: str
TAPI_COMMODITY_TYPE_FUTURES: str
TAPI_COMMODITY_TYPE_OPTION: str
TAPI_COMMODITY_TYPE_SPREAD_MONTH: str
TAPI_COMMODITY_TYPE_SPREAD_COMMODITY: str
TAPI_COMMODITY_TYPE_BUL: str
TAPI_COMMODITY_TYPE_BER: str
TAPI_COMMODITY_TYPE_STD: str
TAPI_COMMODITY_TYPE_STG: str
TAPI_COMMODITY_TYPE_PRT: str
TAPI_COMMODITY_TYPE_DIRECTFOREX: str
TAPI_COMMODITY_TYPE_INDIRECTFOREX: str
TAPI_COMMODITY_TYPE_CROSSFOREX: str
TAPI_COMMODITY_TYPE_INDEX: str
TAPI_COMMODITY_TYPE_STOCK: str
TAPI_CALLPUT_FLAG_CALL: str
TAPI_CALLPUT_FLAG_PUT: str
TAPI_CALLPUT_FLAG_NONE: str
TAPI_ACCOUNT_TYPE_PERSON: str
TAPI_ACCOUNT_TYPE_ORGANIZATION: str
TAPI_ACCOUNT_TYPE_AGENT: str
TAPI_ACCOUNT_TYPE_MARGIN: str
TAPI_ACCOUNT_TYPE_HOUSE: str
TAPI_ACCOUNT_TYPE_STOCK: str
TAPI_RIGHT_ORDER_DEL: int
TAPI_RIGHT_ORDER_CHECK: int
TAPI_RIGHT_ONLY_QRY: int
TAPI_RIGHT_ONLY_OPEN: int
TAPI_USERTYPE_CLIENT: int
TAPI_USERTYPE_BROKER: int
TAPI_USERTYPE_TRADER: int
TAPI_USERTYPE_RISK: int
TAPI_USERTYPE_MANAGER: int
TAPI_USERTYPE_QUOTE: int
TAPI_ACCOUNT_STATE_NORMAL: str
TAPI_ACCOUNT_STATE_CANCEL: str
TAPI_ACCOUNT_STATE_SLEEP: str
TAPI_ACCOUNT_TRADING_RIGHT_NORMAL: str
TAPI_ACCOUNT_TRADING_RIGHT_NOTRADE: str
TAPI_ACCOUNT_TRADING_RIGHT_CLOSE: str
TAPI_ORDER_TYPE_MARKET: str
TAPI_ORDER_TYPE_LIMIT: str
TAPI_ORDER_TYPE_STOP_MARKET: str
TAPI_ORDER_TYPE_STOP_LIMIT: str
TAPI_ORDER_TYPE_OPT_EXEC: str
TAPI_ORDER_TYPE_OPT_ABANDON: str
TAPI_ORDER_TYPE_REQQUOT: str
TAPI_ORDER_TYPE_RSPQUOT: str
TAPI_ORDER_TYPE_ICEBERG: str
TAPI_ORDER_TYPE_GHOST: str
TAPI_ORDER_TYPE_HKEX_AUCTION: str
TAPI_ORDER_TYPE_SWAP: str
TAPI_ORDER_SOURCE_SELF_ETRADER: str
TAPI_ORDER_SOURCE_PROXY_ETRADER: str
TAPI_ORDER_SOURCE_JTRADER: str
TAPI_ORDER_SOURCE_MANUAL: str
TAPI_ORDER_SOURCE_CARRY: str
TAPI_ORDER_SOURCE_PROGRAM: str
TAPI_ORDER_SOURCE_DELIVERY: str
TAPI_ORDER_SOURCE_ABANDON: str
TAPI_ORDER_SOURCE_CHANNEL: str
TAPI_ORDER_TIMEINFORCE_GFD: str
TAPI_ORDER_TIMEINFORCE_GTC: str
TAPI_ORDER_TIMEINFORCE_GTD: str
TAPI_ORDER_TIMEINFORCE_FAK: str
TAPI_ORDER_TIMEINFORCE_FOK: str
TAPI_SIDE_NONE: str
TAPI_SIDE_BUY: str
TAPI_SIDE_SELL: str
TAPI_PositionEffect_NONE: str
TAPI_PositionEffect_OPEN: str
TAPI_PositionEffect_COVER: str
TAPI_PositionEffect_COVER_TODAY: str
TAPI_HEDGEFLAG_NONE: str
TAPI_HEDGEFLAG_T: str
TAPI_HEDGEFLAG_B: str
TAPI_HEDGEFLAG_L: str
TAPI_ORDER_STATE_SUBMIT: str
TAPI_ORDER_STATE_ACCEPT: str
TAPI_ORDER_STATE_TRIGGERING: str
TAPI_ORDER_STATE_EXCTRIGGERING: str
TAPI_ORDER_STATE_QUEUED: str
TAPI_ORDER_STATE_PARTFINISHED: str
TAPI_ORDER_STATE_FINISHED: str
TAPI_ORDER_STATE_CANCELING: str
TAPI_ORDER_STATE_MODIFYING: str
TAPI_ORDER_STATE_CANCELED: str
TAPI_ORDER_STATE_LEFTDELETED: str
TAPI_ORDER_STATE_FAIL: str
TAPI_ORDER_STATE_DELETED: str
TAPI_ORDER_STATE_SUPPENDED: str
TAPI_ORDER_STATE_DELETEDFOREXPIRE: str
TAPI_ORDER_STATE_EFFECT: str
TAPI_ORDER_STATE_APPLY: str
TAPI_CALULATE_MODE_COMBINE: str
TAPI_CALCULATE_MODE_PERCENTAGE: str
TAPI_CALCULATE_MODE_QUOTA: str
TAPI_CALCULATE_MODE_CHAPERCENTAGE: str
TAPI_CALCULATE_MODE_CHAQUOTA: str
TAPI_CALCULATE_MODE_DISCOUNT: str
TAPI_MATCH_SOURCE_ALL: str
TAPI_MATCH_SOURCE_SELF_ETRADER: str
TAPI_MATCH_SOURCE_PROXY_ETRADER: str
TAPI_MATCH_SOURCE_JTRADER: str
TAPI_MATCH_SOURCE_MANUAL: str
TAPI_MATCH_SOURCE_CARRY: str
TAPI_MATCH_SOURCE_PROGRAM: str
TAPI_MATCH_SOURCE_DELIVERY: str
TAPI_MATCH_SOURCE_ABANDON: str
TAPI_MATCH_SOURCE_CHANNEL: str
TAPI_MATCH_SOURCE_ESUNNY_API: str
TAPI_CLOSE_MODE_NONE: str
TAPI_CLOSE_MODE_UNFINISHED: str
TAPI_CLOSE_MODE_OPENCOVER: str
TAPI_CLOSE_MODE_CLOSETODAY: str
TAPI_FUTURES_ALG_ZHUBI: str
TAPI_FUTURES_ALG_DINGSHI: str
TAPI_OPTION_ALG_FUTURES: str
TAPI_OPTION_ALG_OPTION: str
SENDTYPE_SMS: str
SENDTYPE_MAIL: str
TAPI_LWFlag_L: str
TAPI_LWFlag_W: str
TAPI_CASHINOUT_MODE_FEEADJUST: str
TAPI_CASHINOUT_MODE_YKADJUST: str
TAPI_CASHINOUT_MODE_PLEDGE: str
TAPI_CASHINOUT_MODE_INTERESTREVENUE: str
TAPI_CASHINOUT_MODE_COLLECTIONCOST: str
TAPI_CASHINOUT_MODE_OTHER: str
TAPI_CASHINOUT_MODE_COMPANY: str
TAPI_DEPOSITCALCULATE_MODE_FEN: str
TAPI_DEPOSITCALCULATE_MODE_SUO: str
TAPI_CMB_DIRECT_FIRST: str
TAPI_CMB_DIRECT_SECOND: str
TAPI_DELIVERY_MODE_GOODS: str
TAPI_DELIVERY_MODE_CASH: str
TAPI_DELIVERY_MODE_EXECUTE: str
TAPI_DELIVERY_MODE_ABANDON: str
TAPI_DELIVERY_MODE_HKF: str
TAPI_CONTRACT_TYPE_TRADEQUOTE: str
TAPI_CONTRACT_TYPE_QUOTE: str
TAPI_TACTICS_TYPE_NONE: str
TAPI_TACTICS_TYPE_READY: str
TAPI_TACTICS_TYPE_ATUO: str
TAPI_TACTICS_TYPE_CONDITION: str
APIORDER_INSERT: str
APIORDER_MODIFY: str
APIORDER_DELETE: str
APIORDER_SUSPEND: str
APIORDER_ACTIVATE: str
APIORDER_SYSTEM_DELETE: str
TAPI_TRIGGER_CONDITION_NONE: str
TAPI_TRIGGER_CONDITION_GREAT: str
TAPI_TRIGGER_CONDITION_LITTLE: str
TAPI_TRIGGER_PRICE_NONE: str
TAPI_TRIGGER_PRICE_BUY: str
TAPI_TRIGGER_PRICE_SELL: str
TAPI_TRIGGER_PRICE_LAST: str
TAPI_TRADE_STATE_BID: str
TAPI_TRADE_STATE_MATCH: str
TAPI_TRADE_STATE_CONTINUOUS: str
TAPI_TRADE_STATE_PAUSED: str
TAPI_TRADE_STATE_CLOSE: str
TAPI_TRADE_STATE_DEALLAST: str
TAPI_TRADE_STATE_GWDISCONNECT: str
TAPI_TRADE_STATE_UNKNOWN: str
TAPI_TRADE_STATE_INITIALIZE: str
TAPI_TRADE_STATE_READY: str
TAPI_NOTICE_IGNORE_NONE: int
TAPI_NOTICE_IGNORE_ALL: int
TAPI_NOTICE_IGNORE_FUND: int
TAPI_NOTICE_IGNORE_ORDER: int
TAPI_NOTICE_IGNORE_FILL: int
TAPI_NOTICE_IGNORE_POSITION: int
TAPI_NOTICE_IGNORE_CLOSE: int
TAPI_NOTICE_IGNORE_POSITIONPROFIT: int
TAPI_ORDER_QRY_TYPE_ALL: str
TAPI_ORDER_QRY_TYPE_UNENDED: str
TAPI_MSG_RECEIVER_ACCOUNTNO: str
TAPI_MSG_RECEIVER_ACCOUNTGROUPNO: str
TAPI_MSG_RECEIVER_ATTRIBUTE: str
TAPI_MSG_RECEIVER_USERNO: str
TAPI_MSG_LEVEL_NORMAL: str
TAPI_MSG_LEVEL_IMPORTANT: str
TAPI_MSG_LEVEL_IMERGENCY: str
TAPI_Msg_TYPE_MANAGER: str
TAPI_Msg_TYPE_RISKCONTROL: str
TAPI_BILL_DATE: str
TAPI_BILL_MONTH: str
TAPI_BILL_FILE_TXT: str
TAPI_BILL_FILE_PDF: str
SettleFlag_AutoSettle: str
SettleFlagh_Manual: str
TAPIERROR_LOGIN: int
TAPIERROR_LOGIN_USER: int
TAPIERROR_LOGIN_DDA: int
TAPIERROR_LOGIN_LICENSE: int
TAPIERROR_LOGIN_MODULE: int
TAPIERROR_LOGIN_FORCE: int
TAPIERROR_LOGIN_STATE: int
TAPIERROR_LOGIN_PASS: int
TAPIERROR_LOGIN_RIGHT: int
TAPIERROR_LOGIN_COUNT: int
TAPIERROR_LOGIN_NOTIN_SERVERFLAGUSRES: int
TAPIERROR_LOGIN_FREEZE: int
TAPIERROR_LOGIN_TOFREEZE: int
TAPIERROR_LOGIN_ACCOUNTSTATE: int
TAPIERROR_LOGIN_SECCERTIFI: int
TAPIERROR_LOGIN_NOSECONDSET: int
TAPIERROR_LOGIN_NOTURSTHOST: int
TAPITAPIERROR_SECONDCERTIFICATION_FAIL: int
TAPITAPIERROR_SECONDCERTIFICATION_TIMEOVER: int
TAPIERROR_CONN_DATABASE: int
TAPIERROR_OPER_DATABASE: int
TAPIERROR_NEED_ONETOONE: int
TAPIERROR_EXIST_RELATEINFO: int
TAPIERROR_EXIST_RELATEINFOOFGROUP: int
TAPIERROR_USERPASSWORD_MOD_SOURCE: int
TAPIERROR_USERPASSWORD_MOD_SAME: int
TAPIERROR_USERPASSWORD_MOD_COMPLEXITY: int
TAPIERROR_CURRENCY_ONLY_ONEBASE: int
TAPIERROR_CURRENCY_ONLY_USDHKD: int
TAPIERROR_ORDERINSERT_ACCOUNT: int
TAPIERROR_ORDERINSERT_ACCOUNT_STATE: int
TAPIERROR_ORDERINSERT_TRADECENT_ERROR: int
TAPIERROR_ORDERINSERT_CONTRACT: int
TAPIERROR_ORDERINSERT_LME_NOTREADY: int
TAPIERROR_ORDERINSERT_ERROR_ORDER_TYPE: int
TAPIERROR_ORDERINSERT_READY_TYPE_ERROR: int
TAPIERROR_ORDERINSERT_ORDER_TYPE_ERROR: int
TAPIERROR_ORDER_NOTRADE_ACCOUNT: int
TAPIERROR_ORDER_NOTRADE_COM_GROUP: int
TAPIERROR_ORDER_NOTRADE_ACC_CONTRACT: int
TAPIERROR_ORDER_NOTRADE_SYSTEM: int
TAPIERROR_ORDER_CLOSE_ACCOUNT: int
TAPIERROR_ORDER_CLOSE_ACC_CONTRACT: int
TAPIERROR_ORDER_CLOSE_SYSTEM: int
TAPIERROR_ORDER_CLOSE_DAYS: int
TAPIERROR_ORDER_NOTRADE_RISK: int
TAPIERROR_ORDER_CLOSE_RISK: int
TAPIERROR_ORDERINSERT_POSITIONMAX: int
TAPIERROR_ORDERINSERT_ONCEMAX: int
TAPIERROR_ORDERINSERT_TRADEROUTE: int
TAPIERROR_ORDER_IN_MOD_PRICE_ERROR: int
TAPIERROR_ORDER_IN_GIVEUP_POS_MAX: int
TAPIERROR_UPPERCHANNEL_NOT_LOGIN: int
TAPIERROR_UPPERCHANNEL_NOT_FOUND: int
TAPIERROR_ORDERINSERT_NOTENOUGHFUND: int
TAPIERROR_ORDERINSERT_FEE: int
TAPIERROR_ORDERINSERT_MARGIN: int
TAPIERROR_ORDERINSERT_BASENOFUND: int
TAPIERROR_ORDERINSERT_MARGINAMOUNT: int
TAPIERROR_ORDERINSERT_OPENRATIO: int
TAPIERROR_ORDERINSERT_GROUP_OPENRATIO: int
TAPIERROR_ORDERINSERT_RISKARRAY: int
TAPIERROR_ORDERDELETE_NOT_SYSNO: int
TAPIERROR_ORDERDELETE_NOT_STATE: int
TAPIERROR_ORDERDELETE_NO_INPUT: int
TAPIERROR_ORDERMODIFY_NOT_STATE: int
TAPIERROR_ORDERMODIFY_BACK_INPUT: int
TAPIERROR_ORDERMODIFY_RISK_ORDER: int
TAPIERROR_ORDERMODIFY_ERROR_QTY: int
TAPIERROR_ORDERMODIFY_ERROR_READY: int
TAPIERROR_ORDERINPUT_CANNOTMOVE: int
TAPIERROR_ORDERINPUT_REPEAT: int
TAPIERROR_CONTRACT_QUOTE: int
TAPIERROR_UPPER_ONCEMAX: int
TAPIERROR_UPPER_POSITIONMAX: int
TAPIERROR_ORDERINSERT_CLOSEMODE: int
TAPIERROR_CLOSE_ORDER: int
TAPIERROR_CLOSE_MATCH: int
TAPIERROR_MOD_DEL_NO_ORDER: int
TAPIERROR_MOD_DEL_GATEWAY_DISCON: int
TAPIERROR_MATCHINPUT_REPEAT: int
TAPIERROR_MATCHINPUT_NO_ORDER: int
TAPIERROR_MATCHINPUT_NO_CONTRACT: int
TAPIERROR_MATCHINPUT_PARM_ERROR: int
TAPIERROR_MATCHINPUT_OSTATE_ERROR: int
TAPIERROR_MATCHREMOVE_NO_MATCH: int
TAPIERROR_MATCHREMOVE_STATE_ERROR: int
TAPIERROR_ORDERINPUT_STATE_ERROR: int
TAPIERROR_ORDERINPUT_MOD_ERROR: int
TAPIERROR_ORDERREMOVE_ERROR: int
TAPIERROR_ORDERINPUT_MOD_STATE_ERROR: int
TAPIERROR_ORDEREXCHANGE_STATE_ERROR: int
TAPIERROR_ORDERREMOVE_NOT_ERROR: int
TAPIERROR_ORDERMARKET_DELETE_NOTFOUND: int
TAPIERROR_ORDERMARKET_DEL_ACCOUNT_NE: int
TAPIERROR_ORDERMARKET_DEL_COMMODITY_NE: int
TAPIERROR_ORDERMARKET_DEL_CONTRACT_NE: int
TAPIERROR_ORDERMARKET_DEL_SIDE_EQ: int
TAPIERROR_ORDERMARKET_DEL_SIDE_ERROR: int
TAPIERROR_ORDERMARKET_OTHER_SIDE_ERROR: int
TAPIERROR_ORDERACTIVATE_NOTFOUND_ERROR: int
TAPIERROR_ORDERACTIVATE_STATE_ERROR: int
TAPIERROR_GW_NOT_READY: int
TAPIERROR_GW_INVALID_COMMODITY: int
TAPIERROR_GW_INVALID_CONTRACT: int
TAPIERROR_GW_INVALID_FIELD: int
TAPIERROR_GW_INVALID_PRICE: int
TAPIERROR_GW_INVALID_VOLUME: int
TAPIERROR_GW_INVALID_TYPE: int
TAPIERROR_GW_INVALID_MODE: int
TAPIERROR_GW_ORDER_NOT_EXIST: int
TAPIERROR_GW_SEND_FAIL: int
TAPIERROR_GW_REJ_BYUPPER: int
TAPIERROR_TRADEFRONT_MODULETYPEERR: int
TAPIERROR_TRADEFRONT_TOOMANYDATA: int
TAPIERROR_TRADEFRONT_NODATA: int
TAPIERROT_TRADEFRONT_NOUSER: int
TAPIERROR_TRADEFRONT_DISCONNECT_TRADE: int
TAPIERROR_TRADEFRONT_DISCONNECT_MANAGE: int
TAPIERROR_TRADEFRONT_ACCOUNT: int
TAPIERROR_TRADEFRONT_ORDER: int
TAPIERROR_TRADEFRONT_FREQUENCY: int
TAPIERROR_TRADEFRONT_RUFUSE: int
TAPIERROR_TRADEFRONT_SELFMATCH: int
TAPIERROR_SUCCEED: int
TAPIERROR_ConnectFail: int
TAPIERROR_LinkAuthFail: int
TAPIERROR_HostUnavailable: int
TAPIERROR_SendDataError: int
TAPIERROR_TestIDError: int
TAPIERROR_NotReadyTestNetwork: int
TAPIERROR_CurTestNotOver: int
TAPIERROR_NOFrontAvailable: int
TAPIERROR_DataPathAvaiable: int
TAPIERROR_RepeatLogin: int
TAPIERROR_InnerError: int
TAPIERROR_LastReqNotFinish: int
TAPIERROR_InputValueError: int
TAPIERROR_AuthCode_Invalid: int
TAPIERROR_AuthCode_Expired: int
TAPIERROR_AuthCode_TypeNotMatch: int
TAPIERROR_API_NotReady: int
TAPIERROR_UDP_LISTEN_FAILED: int
TAPIERROR_UDP_LISTENING: int
TAPIERROR_NotImplemented: int
TAPIERROR_CallOneTimeOnly: int
TAPIERROR_ORDER_FREQUENCY: int
TAPIERROR_RENTQRY_TOOFAST: int
TAPIERROR_CALL_NOCONDITION: int
TAPIERROR_ORDER_NOTFOUND: int
TAPIERROR_LOGPATH_EMPTY: int
TAPIERROR_LOGPATH_FAILOPEN: int
TAPIERROR_RIGHT_TRADER: int
TAPIERROR_RIGHT_ORDERINPUT: int
TAPIERROR_RIGHT_LOCALOPERATION: int
TAPIERROR_RIGHT_ORDERTRANSFER: int
TAPIERROR_FILLINPUT_SYSTEMNO: int
TAPIERROR_FILLREMOVE_MATCHNO: int
TAPIERROR_FILLREQMOVE_NOFUND: int
TAPIERROR_LOCALMODIFY_ACCOUNT: int
TAPIERROR_LOCALTRANSFER_ACCOUNT: int
TAPIERROR_INPUTERROR_PHONE: int
TAPIERROR_ERROR_CONTACT: int
TAPIERROR_ERROR_REJESTVERTIFICATE: int
TAPIERROR_RIGHT_SETPASSWORD: int
TAPIERROR_RISK_OPERERROR: int
TAPIERROR_ORDER_MODACCOUNT: int
TAPIERROR_INPUTERROR_NULL: int
TAPIERROR_INPUTERROR_TAPIYNFLAG: int
TAPIERROR_INPUTERROR_TAPILOGLEVEL: int
TAPIERROR_INPUTERROR_TAPICommodityType: int
TAPIERROR_INPUTERROR_TAPICallOrPutFlagType: int
TAPIERROR_INPUTERROR_TAPIBucketDateFlag: int
TAPIERROR_INPUTERROR_TAPIHisQuoteType: int
TAPIERROR_INPUTERROR_TAPIAccountType: int
TAPIERROR_INPUTERROR_TAPIUserTypeType: int
TAPIERROR_INPUTERROR_TAPIAccountState: int
TAPIERROR_INPUTERROR_TAPIAccountFamilyType: int
TAPIERROR_INPUTERROR_TAPIOrderTypeType: int
TAPIERROR_INPUTERROR_TAPIOrderSourceType: int
TAPIERROR_INPUTERROR_TAPITimeInForceType: int
TAPIERROR_INPUTERROR_TAPISideType: int
TAPIERROR_INPUTERROR_TAPIPositionEffectType: int
TAPIERROR_INPUTERROR_TAPIHedgeFlagType: int
TAPIERROR_INPUTERROR_TAPIOrderStateType: int
TAPIERROR_INPUTERROR_TAPICalculateModeType: int
TAPIERROR_INPUTERROR_TAPIMatchSourceType: int
TAPIERROR_INPUTERROR_TAPIOpenCloseModeType: int
TAPIERROR_INPUTERROR_TAPIFutureAlgType: int
TAPIERROR_INPUTERROR_TAPIOptionAlgType: int
TAPIERROR_INPUTERROR_TAPIBankAccountLWFlagType: int
TAPIERROR_INPUTERROR_TAPIBankAccountStateType: int
TAPIERROR_INPUTERROR_TAPIBankAccountSwapStateType: int
TAPIERROR_INPUTERROR_TAPIBankAccountTransferStateType: int
TAPIERROR_INPUTERROR_TAPIMarginCalculateModeType: int
TAPIERROR_INPUTERROR_TAPIOptionMarginCalculateModeType: int
TAPIERROR_INPUTERROR_TAPICmbDirectType: int
TAPIERROR_INPUTERROR_TAPIDeliveryModeType: int
TAPIERROR_INPUTERROR_TAPIContractTypeType: int
TAPIERROR_INPUTERROR_TAPIPartyTypeType: int
TAPIERROR_INPUTERROR_TAPIPartyCertificateTypeType: int
TAPIERROR_INPUTERROR_TAPIMsgReceiverType: int
TAPIERROR_INPUTERROR_TAPIMsgTypeType: int
TAPIERROR_INPUTERROR_TAPIMsgLevelType: int
TAPIERROR_INPUTERROR_TAPITransferDirectType: int
TAPIERROR_INPUTERROR_TAPITransferStateType: int
TAPIERROR_INPUTERROR_TAPITransferTypeType: int
TAPIERROR_INPUTERROR_TAPITransferDeviceIDType: int
TAPIERROR_INPUTERROR_TAPITacticsTypeType: int
TAPIERROR_INPUTERROR_TAPIORDERACT: int
TAPIERROR_INPUTERROR_TAPIBillTypeType: int
TAPIERROR_INPUTERROR_TAPIBillFileTypeType: int
TAPIERROR_INPUTERROR_TAPIOFFFlagType: int
TAPIERROR_INPUTERROR_TAPICashAdjustTypeType: int
TAPIERROR_INPUTERROR_TAPITriggerConditionType: int
TAPIERROR_INPUTERROR_TAPITriggerPriceTypeType: int
TAPIERROR_INPUTERROR_TAPITradingStateType: int
TAPIERROR_INPUTERROR_TAPIMarketLevelType: int
TAPIERROR_INPUTERROR_TAPIOrderQryTypeType: int
TAPIERROR_INPUTERROR_TAPIClientID: int
TAPIERROR_INPUTERROR_QryHisQuoteParam: int
TAPIERROR_INPUTERROR_TAPIIncludeNAN: int
TAPIERROR_INPUTERROR_TAPIExpireTime: int
TAPIERROR_INPUTERROR_TAPIPasswordType: int
TAPIERROR_INPUTERROR_TAPISettleFlagType: int
TAPIERROR_DISCONNECT_CLOSE_INIT: int
TAPIERROR_DISCONNECT_CLOSE_PASS: int
TAPIERROR_DISCONNECT_READ_ERROR: int
TAPIERROR_DISCONNECT_WRITE_ERROR: int
TAPIERROR_DISCONNECT_BUF_FULL: int
TAPIERROR_DISCONNECT_IOCP_ERROR: int
TAPIERROR_DISCONNECT_PARSE_ERROR: int
TAPIERROR_DISCONNECT_CONNECT_TIMEOUT: int
TAPIERROR_DISCONNECT_INIT_ERROR: int
TAPIERROR_DISCONNECT_HAS_CONNECTED: int
TAPIERROR_DISCONNECT_HAS_EXIT: int
TAPIERROR_DISCONNECT_TRY_LATER: int

