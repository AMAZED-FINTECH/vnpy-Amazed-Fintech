/*
 * Copyright 2009-2015 the original author or authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file    oes_base_model.h
 *
 * 订单执行系统的基础领域模型(数据结构)定义
 *
 * @version 0.11.1      2016/11/17
 * @version 0.12        2016/11/30
 *          - 增加默认佣金模板配置; 为佣金模板配置项增加通配设置
 *          - 增加所有者(OwnerType)配置, 并替换原股东账户中的 optAcctLevel 字段
 *          - 重命名字段 etfId ==> fundId
 *          - 成交回报信息中增加累计成交金额、累计交易费用字段
 *          - 委托确认信息中增加累计成交金额、累计交易费用、冻结金额、冻结费用字段
 *          - 在登录报文中增加协议版本号信息, 并在登录时校验协议版本号的兼容性
 * @version 0.12.1      2016/12/19
 *          - 删除已不被使用的枚举定义 eOesEtfSubFlagSzT
 *          - 修改委托请求中ordType的数据字典 eOesOrdTypeShT eOesOrdTypeSzT
 *          - 委托请求、委托确认、成交回报中的userInfo联合体中加入i64、i32类型字段
 * @version 0.12.3      2017/01/10
 *          - 股票持仓基础信息结构体与期权持仓基础信息接结构体拆分
 *          - 删除 eOesHoldTypeT 枚举类型定义
 * @version 0.12.3.5    2017/02/20
 *          - 迁移报撤比相关的宏定义
 *          - 删除卖清仓阈值的宏定义
 *          - 新增债券、基金的证券类型, 调整ETF证券类型宏定义的取值
 *          - 新增证券子类型定义
 *          - 委托确认、成交回报中增加延迟统计字段
 *          - 现货产品基础信息中增加“证券子类型”字段, 并且重命名“买入单位”、“卖出单位”字段
 *          - ETF产品基础信息中增加“证券类型”、“证券子类型”字段
 *          - ETF成分股基础信息中增加“证券子类型”字段
 *          - 期权产品基础信息中增加“证券子类型”字段
 * @version 0.12.6.2    2017/03/16
 *          - 调整 ‘买卖类型’ 中期权标的锁定(OES_BS_TYPE_UF)、期权标的解锁(OES_BS_TYPE_UU)、
 *              指定登记(OES_BS_TYPE_SSE_DESIGNATION)、指定撤消(OES_BS_TYPE_SSE_RECALL_DESIGNATION)、
 *              托管注册(OES_BS_TYPE_SZSE_DESIGNATION)、托管撤消(OES_BS_TYPE_SZSE_CANCEL_DESIGNATION)
 *              的宏值
 *          - 新增 逆回购(OES_BS_TYPE_CS) 买卖类型
 *          - 调整 ‘费用计算模式’ 的宏定义 eOesCalFeeModeT => eOesCalcFeeModeT
 *              - 调整 OES_CAL_FEE_MODE_AMOUNT => OES_CALC_FEE_MODE_AMOUNT
 *              - 调整 OES_CAL_FEE_MODE_QTY => OES_CALC_FEE_MODE_QTY
 *              - 新增 OES_CALC_FEE_MODE_ORD (按笔数计算费用)
 *          - 重命名 ‘出入金方向’ 的宏定义 eOesCashDirectT => eOesFundTrsfDirectT
 *              - 调整 OES_CASH_DIRECT_IN => OES_FUND_TRSF_DIRECT_IN
 *              - 调整 OES_CASH_DIRECT_OUT => OES_FUND_TRSF_DIRECT_OUT
 *          - 重命名 ‘出入金委托状态’ 的宏定义
 *              - 调整 OES_CASH_TRSF_STS_RECV => OES_FUND_TRSF_STS_RECV
 *              - 调整 OES_CASH_TRSF_STS_DCLR => OES_FUND_TRSF_STS_DCLR
 *              - 调整 OES_CASH_TRSF_STS_DONE => OES_FUND_TRSF_STS_DONE
 *              - 调整 OES_CASH_TRSF_STS_INVALID_OES => OES_FUND_TRSF_STS_INVALID_OES
 *              - 调整 OES_CASH_TRSF_STS_INVALID_COUNTER => OES_FUND_TRSF_STS_INVALID_COUNTER
 *          - 调整 ‘产品级别’(eOesSecurityLevelT) 中的枚举值
 *          - 删除 无用的‘订单时间类型’(eOesOrdTimeTypeT) 的枚举定义
 *          - 删除 无用的‘权利类别’(eOesRightTypeT) 的枚举定义
 *          - ‘出入金委托基础信息’ 中增加 用户私有信息(userInfo) 字段
 *          - 增加 ‘出入金委托回报基础信息’
 * @version 0.12.8      2017/04/17
 *          - 调整 ‘委托确认信息(OesOrdCnfmT)’ 中部分字段的位置
 *          - 新增 ‘委托确认信息(OesOrdCnfmT)’ 中增加深圳多交易网关特性相关OES内部字段
 *            __tgwSetIdx、__tgwIdx、__declaredTgwIdx
 *          - 新增 ‘成交确认信息(OesTrdCnfmT)’ 中增加深圳多交易网关特性相关OES内部字段
 *            __tgwSetIdx
 * @version 0.12.8.1    2017/04/24
 *          - 重命名 ‘委托确认信息(OesOrdCnfmT)’、‘成交确认信息(OesTrdCnfmT)’ 中
 *            __tgwSetIdx => __tgwGroupIdx
 * @version 0.12.8.2    2017/05/16
 *          - 新增 ‘买卖类型’ 新股认购(OES_BS_TYPE_SUBSCRIPTION)
 *          - 重命名 如下‘买卖类型’ 定义, 原始买卖类型将被废弃
 *              - 买入 OES_BS_TYPE_B => OES_BS_TYPE_BUY
 *              - 卖出 OES_BS_TYPE_S => OES_BS_TYPE_SELL
 *              - 申购 OES_BS_TYPE_KB => OES_BS_TYPE_CREATION
 *              - 赎回 OES_BS_TYPE_KS => OES_BS_TYPE_REDEMPTION
 *              - 融资买入 OES_BS_TYPE_CB => OES_BS_TYPE_CREDIT_BUY
 *              - 融券卖出, 质押式逆回购 OES_BS_TYPE_CS => OES_BS_TYPE_CREDIT_SELL
 *              - 期权买入开仓 OES_BS_TYPE_BO => OES_BS_TYPE_BUY_OPEN
 *              - 期权买入平仓 OES_BS_TYPE_BC => OES_BS_TYPE_BUY_CLOSE
 *              - 期权卖出开仓 OES_BS_TYPE_SO => OES_BS_TYPE_SELL_OPEN
 *              - 期权卖出平仓 OES_BS_TYPE_SC => OES_BS_TYPE_SELL_CLOSE
 *              - 期权备兑开仓 OES_BS_TYPE_CO => OES_BS_TYPE_COVERED_OPEN
 *              - 期权备兑平仓 OES_BS_TYPE_CC => OES_BS_TYPE_COVERED_CLOSE
 *              - 期权行权 OES_BS_TYPE_TE => OES_BS_TYPE_OPTION_EXERCISE
 *              - 期权标的锁定 OES_BS_TYPE_UF => OES_BS_TYPE_UNDERLYING_FREEZE
 *              - 期权标的解锁 OES_BS_TYPE_UU => OES_BS_TYPE_UNDERLYING_UNFREEZE
 *          - 新增 ‘证券发行基础信息’
 *          - 新增 ‘产品基础信息(OesStockBaseInfoT)’ 中新增字段
 *              - 新增 是否需要适当性管理(isQualificationRequired)
 *              - 新增 是否支持当日回转交易(isDayTrading)
 *              - 新增 每百元面值债券应计利息(bondInterest), 精确到元后8位
 *          - 删除 ‘产品基础信息(OesStockBaseInfoT)’ 中删除字段
 *              - 删除 卖出资金是否可用(cashRealBack)
 *              - 删除 买入股是否可卖出(hldnRealBack)
 *          - 新增 ‘股东账户基础信息(OesInvAcctBaseInfoT)’ 中新增字段 新股认购限额(subscriptionQuota)
 *          - 新增 ‘委托确认信息(OesOrdCnfmT)’ 中新增字段
 *              - 新增 冻结利息(frzInterest)
 *              - 新增 已发生利息(cumInterest)
 *          - 新增 ‘成交确认信息(OesTrdCnfmT)’ 中新增字段
 *              - 新增 已发生利息(cumInterest)
 * @version 0.15.1      2017/06/26
 *          - ‘现货产品基础信息(OesStockBaseInfoT)’ 中
 *              - 调整字段 isQualificationRequired => qualificationClass,
 *                  取值请参考 eOesQualificationClassT
 *              - 新增字段 产品风险等级(securityRiskLevel), 取值请参考 eOesSecurityRiskLevelT
 *              - 新增字段 逆回购期限(repoExpirationDays)
 *              - 新增字段 占款天数(cashHoldDays) 字段
 *          - ‘客户资金基础信息(OesCashAssetBaseInfoT)’ 中
 *              - 字段重命名 期初余额(originalBal => beginningBal)
 *              - 新增字段 期初可用余额(beginningAvailableBal)
 *              - 新增字段 期初可取余额(beginningDrawableBal)
 *              - 新增字段 当前冲正金额(红冲蓝补的资金净额, reversalAmt)
 *          - ‘客户基础信息(OesCustBaseInfoT)’ 中
 *              - 新增字段 风险等级(riskLevel)
 *              - 新增字段 原始风险等级(originRiskLevel)
 *          - ‘证券账户基础信息(OesInvAcctBaseInfoT)’ 中
 *              - 调整字段 '(券账户权限限制, acctLimit)'
 *                  - 类型 uint32 => uint64
 *                  - 重命名 acctLimit => Limits
 *              - 调整字段 '(股东权限/客户权限, acctRight)'
 *                  - 类型 uint32 => uint64
 *                  - 重命名 acctRight => permissions
 *              - 新增字段 适当性管理分类(qualificationClass)
 *          - ‘股票持仓基础信息(OesStkHoldingBaseInfoT)’ 中
 *              - 新增字段 手动冻结持仓(管理员冻结持仓净额, manualFrzHld)
 *          - 添加交易权限的枚举值定义 eOesTradingPermissionT, eOesTradingLimitT
 *          - 交易权限的元数据定义 OesTradingPermissionEntryT
 *          - 重命名枚举类型 eOesExchangeTypeT => eOesExchangeIdT
 * @version 0.15.2      2017/07/18
 *          - 新增 OES中签、配号记录类型(eOesLotTypeT)
 *          - 新增 OES配号失败原因(eOesLotRejReasonT)
 *          - 出入金委托状态(eOesFundTrsfStatusT) 中新增如下状态
 *              - 指令已报到主柜前待回滚(OES_FUND_TRSF_STS_DCLR_ROLLBACK)
 *              - 指令已报到主柜后待回滚(OES_FUND_TRSF_STS_DONE_ROLLBACK)
 *              - 出入金指令完成, 等待事务结束(OES_FUND_TRSF_STS_WAIT_DONE)
 *          - 新增 新股配号、中签记录信息结构体定义(OesLotWinningBaseInfoT)
 * @version 0.15.2.1    2017/07/31
 *          - 新增 出入金业务拒绝的回报结构定义(OesFundTrsfRejectReportT)
 *          - 出入金委托回报的结构体(OesFundTrsfReportT) 中
 *              - 新增字段 错误信息(rejReasonInfo)
 *          - 重命名 ‘出入金委托状态’ 的宏定义
 *              - 调整 OES_FUND_TRSF_STS_RECV => OES_FUND_TRSF_STS_UNDECLARED
 *              - 调整 OES_FUND_TRSF_STS_DCLR => OES_FUND_TRSF_STS_DECLARED
 *              - 调整 OES_FUND_TRSF_STS_DCLR_ROLLBACK => OES_FUND_TRSF_STS_UNDECLARED_ROLLBACK
 *              - 调整 OES_FUND_TRSF_STS_DONE_ROLLBACK => OES_FUND_TRSF_STS_DECLARED_ROLLBACK
 * @version 0.15.2.2    2017/08/07
 *          - 新增 出入金执行的挂起状态(OES_FUND_TRSF_STS_SUSPENDED)
 *          - 出入金拒绝回报消息中调整‘错误信息’字段命名 rejReasonInfo => errorInfo
 *          - 出入金执行回报消息中调整‘错误信息’字段命名 rejReasonInfo => errorInfo
 * @version 0.15.3      2017/08/14
 *          - 新增 投资者分类(eOesInvestorClassT) 枚举定义
 *          - 新股配号、中签记录信息(OesLotWinningBaseInfoT) 中
 *              - 删除 客户代码(custId) 字段
 *              - 新增 证券名称(securityName) 字段
 *          - 证券发行基础信息(OesIssueBaseInfoT) 中
 *              - 新增 发行起始日(startDate) 字段
 *              - 新增 发行结束日(endDate) 字段
 *          - 客户基础信息(OesCustBaseInfoT) 中
 *              - 新增 机构标志(institutionFlag) 字段
 *              - 新增 投资者分类(investorClass) 字段
 *          - 证券账户基础信息(OesInvAcctBaseInfoT) 中
 *              - 删除 适当性管理分类(qualificationClass) 字段
 * @version 0.15.4      2017/09/04
 *          - '委托确认信息 (OesOrdCnfmT)' 中增加字段:
 *              - 客户端编号 (clientId)
 *              - 客户端环境号 (clEnvId)
 *              - 原始订单的客户委托流水号 (origClSeqNo)
 *              - 原始订单的客户端环境号 (origClEnvId)
 *              - 交易所订单编号 (exchOrdId)
 *          - '成交回报信息 (OesTrdCnfmT)' 中增加字段:
 *              - 客户端编号 (clientId)
 *              - 客户端环境号 (clEnvId)
 *          - '股票持仓基础信息 (OesStkHoldingBaseInfoT)' 中增加字段:
 *              - 持仓成本 (costAmount)
 *          - 整理以下结构体定义, 调整了字段顺序, 并删除了个别字段
 *              - 现货产品基础信息 (OesStockBaseInfoT)
 *              - 期权产品基础信息 (OesOptionBaseInfoT)
 *              - 证券账户基础信息 (OesInvAcctBaseInfoT)
 *          - 重命名 '出入金拒绝 (OesFundTrsfRejectReportT)' => OesFundTrsfRejectT
 *          - 调整枚举类型 '出入金委托状态 (eOesFundTrsfStatusT)' 的取值
 * @version 0.15.4.1    2017/09/19
 *          - 为委托拒绝、撤单拒绝、出入金拒绝分别定义独立的回报消息结构体, 并各自增加委托日期和委托时间字段
 *              - 委托拒绝 (OesOrdRejectT)
 *              - 撤单拒绝 (OesOrdCancelRejectT)
 *              - 出入金拒绝 (OesFundTrsfRejectT)
 *          - '出入金基础信息 (OesFundTrsfBaseInfoT)'中
 *              - 新增 是否仅调拨 (isAllotOnly) 字段
 *          - '出入金委托回报的结构体(OesFundTrsfReportT)' 中
 *              - 新增 是否仅调拨 (isAllotOnly) 字段
 *              - 新增 资金调拨流水号 (allotSerialNo) 字段
 *          - '股票持仓基础信息 (OesStkHoldingBaseInfoT)'中
 *              - 删除 持仓成本 (costAmount) 字段
 *              - 新增 日初总持仓成本 (originalCostAmt) 字段
 *              - 新增 日中累计买入金额 (totalBuyAmt) 字段
 *              - 新增 日中累计卖出金额 (totalSellAmt) 字段
 *              - 新增 日中累计买入费用 (totalBuyFee) 字段
 *              - 新增 日中累计卖出费用 (totalSellFee) 字段
 *          - '股票持仓查询结果 (OesStkHoldingItemT)' 中
 *              - 删除 持仓成本 (costAmount) 字段
 *              - 新增 日初总持仓成本 (originalCostAmt) 字段
 *              - 新增 日中累计买入金额 (totalBuyAmt) 字段
 *              - 新增 日中累计卖出金额 (totalSellAmt) 字段
 *              - 新增 日中累计买入费用 (totalBuyFee) 字段
 *              - 新增 日中累计卖出费用 (totalSellFee) 字段
 *          - '出入金回报和查询结果 (OesFundTrsfReportT/OesFundTransferSerialItemT)' 中
 *              - 新增 主柜错误码 (counterErrCode) 字段
 *                发生主柜错误时记录具体的主柜错误码, 而错误原因(rejReason)字段将统一设置为: OESERR_COUNTER_ERR
 *          - '委托回报和查询结果 (OesOrdCnfmT/OesOrdItemT)' 中
 *              - 新增 交易所错误码 (exchErrCode) 字段
 *                发生交易所错误时记录具体的交易所错误码, 而错误原因(ordRejReason)字段将统一设置为: OESERR_EXCHANGE_ERR
 *          - 调整固定费用和佣金的费率单位, 将精度从百万分之一修改为千万分之一
 *              - OES_FEE_RATE_UNIT 从 1000000 修改为 10000000
 *          - 默认开启用于统计延时的打点信息, 并将打点信息的时间戳类型改为 timeval32 (STimeval32T) 类型,
 *            同时将回报推送的打点信息, 从消息头迁移到消息体中 (__pushingTime)
 * @version 0.15.4.2    2017/10/16
 *          - 为委托拒绝、撤单拒绝回报消息结构增加错误原因(ordRejReason)字段
 * @version 0.15.4.3    2017/10/27
 *          - '委托拒绝 (OesOrdRejectT)' 结构体中增加字段:
 *              - 客户端编号 (clientId)
 *              - 客户端环境号 (clEnvId)
 *          - '撤单拒绝(OesOrdCancelRejectT)' 结构体中新增字段:
 *              - 客户端编号 (clientId)
 *              - 客户端环境号 (clEnvId)
 *          - '出入金拒绝 (OesFundTrsfRejectT)' 结构体中增加字段:
 *              - 客户端编号 (clientId)
 *              - 客户端环境号 (clEnvId)
 * @version 0.15.5      2017/11/03
 *          - 调整委托状态 eOesOrdStatusT:
 *              - 删除 'OES_ORD_STATUS_DECLARING (正报)' 状态
 *              - 重命名 OES_ORD_STATUS_NORMAL => OES_ORD_STATUS_NEW (新订单)
 *          - 删除结构体 '撤单拒绝 (OesOrdCancelRejectT)', 并将内容整合到 '委托拒绝 (OesOrdRejectT)' 结构体中
 *          - '委托拒绝 (OesOrdRejectT)' 结构体中增加字段:
 *              - 原始订单的客户委托流水号 (origClSeqNo)
 *              - 原始订单的客户端环境号 (origClEnvId)
 *          - '股票持仓基础信息 (OesStkHoldingBaseInfoT)' 结构体中增加字段:
 *              - 证券类型 (securityType)
 *              - 证券子类型 (subSecurityType)
 *          - '委托请求 (OesOrdReqT)' 和 '撤单请求(OesOrdCancelReqT)' 结构体中增加用于统计延时的时间戳
 *            字段 __ordReqOrigSendTime, 该字段由API在发送时自动填充, 并在委托回报中携带回客户端
 *          - 委托请求/撤单请求中增加用于统计延时的时间戳字段 __ordReqOrigSendTime,
 *            该字段由API在发送时自动填充, 并在委托回报中携带回客户端
 *          - 调整产品风险等级 eOesSecurityRiskLevelT 中 OES_RISK_LEVEL_VERY_LOW 字段含义:
 *              - 重定义 "极低风险" => "最低风险"
 *          - 调整客户状态/证券帐户/资金账户状态 eOesAcctStatusT:
 *              - 删除 OES_ACCT_STATUS_CLOSE    (销户) 等
 *              - 增加 OES_ACCT_STATUS_DISABLED (非正常)
 *          - 删除 eOesSourceTypeT 定义
 * @version 0.15.5.2    2018/01/07
 *          - 修复 'ETF证券基础信息 (OesEtfBaseInfoT)' 中 '前一交易 (preTradingDay)'
 *            字段的拼写错误
 *          - 新增 ‘出入金金额单位’ 的宏定义(OES_FUND_TRSF_UNIT)
 * @version 0.15.5.6    2018/04/01
 *          - 新增 交易平台类型定义(eOesPlatformIdT)
 *          - 新增 市场状态信息结构定义(OesMarketStateInfoT)
 * @version 0.15.5.12   2018/06/29
 *          - 新增 存托凭证对应的证券子类型 (OES_SUB_SECURITY_TYPE_STOCK_CDR)
 *          - 股东账户交易权限枚举(eOesTradingPermissionT)中新增
 *              - 存托凭证交易权限 (OES_PERMIS_CDR)
 *              - 创新企业股票交易权限 (OES_PERMIS_INNOVATION)
 * @version 0.15.5.14   2018/08/01
 *          - 删除委托信息中已不再使用的字段 __tgwIdx、__declaredTgwIdx (协议保持兼容)
 *          - 重命名字段 __tgwGroupIdx => __tgwGrpNo (协议保持兼容)
 *          - 证券子类别枚举(eOesSubSecurityTypeT) 中新增 存托凭证(OES_SUB_SECURITY_TYPE_STOCK_CDR) 子类别定义
 *          - 账户权限枚举(eOesTradingPermissionT) 中:
 *              - 新增 存托凭证交易权限(OES_PERMIS_CDR) 定义
 *              - 新增 创新企业股票交易权限(OES_PERMIS_INNOVATION) 定义
 *          - 出入金回报及查询结构体(OesFundTrsfReportT, OesFundTransferSerialItemT)中新增
 *            内部字段 '是否已转账到主柜(__hasCounterTransfered)'
 *          - 调整委托确认结构体中的内部字段定义, 调整了__declareFlag字段的位置, 并增加了__recNum字段
 *          - 新增 客户端状态定义(eOesClientStatusT) 枚举类型, 仅内部使用
 * @version 0.15.5.16   2018/08/31
 *          - 成交基础信息(OesTrdBaseInfoT) 中新增 证券子类别(subSecurityType) 字段
 *          - 客户资金基础信息(OesCashAssetBaseInfoT) 中新增 是否禁止出入金(isFundTrsfDisabled) 字段
 *          - 证券账户基础信息(OesInvAcctBaseInfoT) 中新增 是否禁止交易(isTradeDisabled) 字段
 *          - 新增 ‘出入金转账类型’ 定义(eOesFundTrsfTypeT)
 *          - 出入金请求(OesFundTrsfReqT) 中 '是否仅调拨(isAllotOnly)'字段含义调整为出入金转账类型
 *            取值在兼容原来基础上增加类型 '主柜和银行之间转账', 参见 'eOesFundTrsfTypeT' 定义
 *          - 出入金执行回报及查询结构(OesFundTrsfReportT / OesFundTrsfRejectT / OesFundTransferSerialItemT)
 *            中 '是否仅调拨(isAllotOnly)' 字段取值也同步调整
 * @version 0.15.5.16_u3 2018/09/28
 *          - 调整成交基础信息(OesTrdBaseInfoT)和成交回报信息(OesTrdCnfmT)的结构体字段
 *              - 调整 v0.15.5.16 版本新增的 '证券子类别(subSecurityType)' 字段的位置
 *              - 增加 '原始委托数量(origOrdQty)' 和 '原始委托价格(origOrdPrice)' 字段
 * @version 0.15.6.13   2018/07/16
 *          - 新增 OES执行类型定义 (eOesExecTypeT)
 *
 * @since   2015/07/30
 */


#ifndef _OES_BASE_MODEL_H
#define _OES_BASE_MODEL_H


#include    <sutil/types.h>


#ifdef __cplusplus
extern "C" {
#endif


/* ===================================================================
 * 预编译宏处理
 * =================================================================== */

/* 默认开启用于统计延时的打点信息 */
#if ! defined (_OES_DISABLE_LATENCY_STATS) \
        && ! defined (_OES_EXPORT_LATENCY_STATS)
#   define  _OES_EXPORT_LATENCY_STATS       1
#elif defined (_OES_DISABLE_LATENCY_STATS) \
        && defined (_OES_EXPORT_LATENCY_STATS)
#   undef   _OES_EXPORT_LATENCY_STATS
#endif
/* -------------------------           */


/* ===================================================================
 * 常量定义
 * =================================================================== */

/** 客户端名称最大长度 */
#define OES_CLIENT_NAME_MAX_LEN             (32)
/** 客户端说明最大长度 */
#define OES_CLIENT_DESC_MAX_LEN             (32)
/** 系统支持的最大客户端环境号数量 */
#define OES_MAX_CLIENT_ENVID_COUNT          (128)
/** 密码最大长度 */
#define OES_PWD_MAX_LEN                     (40)
/** 协议版本号的最大长度 */
#define OES_VER_ID_MAX_LEN                  (32)
/** 批量委托的每批次最大委托数量 */
#define OES_MAX_BATCH_ORDERS_COUNT          (500)

/** 客户代码最大长度 */
#define OES_CUST_ID_MAX_LEN                 (16)
/** 客户代码真实长度 */
#define OES_CUST_ID_REAL_LEN                (12)
/** 客户名称最大长度 */
#define OES_CUST_NAME_MAX_LEN               (64)

/** 资金账户代码最大长度 */
#define OES_CASH_ACCT_ID_MAX_LEN            (16)
/** 资金账户代码的实际长度 */
#define OES_CASH_ACCT_ID_REAL_LEN           (12)

/** 股东账户代码最大长度 */
#define OES_INV_ACCT_ID_MAX_LEN             (16)
/** 股东账户代码实际长度 */
#define OES_INV_ACCT_ID_REAL_LEN            (10)

/** 营业部代码最大长度 */
#define OES_BRANCH_ID_MAX_LEN               (8)
/** 营业部代码实际长度 */
#define OES_BRANCH_ID_REAL_LEN              (6)

/** 银行代码最大长度 */
#define OES_BANK_NO_MAX_LEN                 (8)
/** 银行代码实际使用长度 */
#define OES_BANK_NO_REAL_LEN                (4)

/** PBU域长度 */
#define OES_PBU_MAX_LEN                     (8)
/** PBU实际长度 */
#define OES_PBU_REAL_LEN                    (6)

/** 证券代码的最大长度 */
#define OES_SECURITY_ID_MAX_LEN             (16)
/** 实际的股票产品代码长度 */
#define OES_STOCK_ID_REAL_LEN               (6)
/** 实际的期权产品代码长度 */
#define OES_OPTION_ID_REAL_LEN              (8)

/** 市场描述字符串最大长度 */
#define OES_MKT_DESC_STR_MAX_LEN            (32)

/** 产品名称长度 */
#define OES_SECURITY_NAME_MAX_LEN           (24)
/** 产品名称实际长度 */
#define OES_SECURITY_NAME_REAL_LEN          (20)

/** 产品关联代码长度 */
#define OES_SECURITY_LINK_CODE_MAX_LEN      (16)
/** 产品关联代码实际长度 */
#define OES_SECURITY_LINK_CODE_REAL_LEN     (8)

/** 交易所订单编号的最大长度 */
#define OES_EXCH_ORDER_ID_MAX_LEN           (20)
/** 交易所订单编号的实际长度 (上证) */
#define OES_EXCH_ORDER_ID_SSE_LEN           (8)
/** 交易所订单编号的实际长度 (深证) */
#define OES_EXCH_ORDER_ID_SZSE_LEN          (16)

/** 点分十进制的IPv4, 字符串的最大长度 */
#define OES_MAX_IP_LEN                      (16)
/** MAC地址字符串的最大长度 */
#define OES_MAX_MAC_LEN                     (20)
/** MAC地址字符串的最大长度(按64位对齐的长度) */
#define OES_MAX_MAC_ALGIN_LEN               (24)
/** 设备序列号字符串的最大长度 */
#define OES_MAX_DRIVER_ID_LEN               (24)

/** 测试请求标识符的最大长度 */
#define OES_MAX_TEST_REQ_ID_LEN             (32)
/** 发送时间字段(YYYYMMDD-HH:mm:SS.sss (*C21))的最大长度 */
#define OES_MAX_SENDING_TIME_LEN            (22)
/** 发送时间字段(YYYYMMDD-HH:mm:SS.sss (*C21))的实际有效数据长度 */
#define OES_REAL_SENDING_TIME_LEN           (21)

/** 系统支持的最大权限数量 */
#define OES_MAX_TRADING_PERMISSION_COUNT    (64)
/** 权限说明(中文名称)的最大长度 */
#define OES_MAX_PERMISSION_MEMO_LEN         (64)

/** 错误描述信息长度 */
#define OES_MAX_ERROR_INFO_LEN              (64)

/** 主柜调拨流水号信息长度 */
#define OES_MAX_ALLOT_SERIALNO_LEN          (64)

/** 费用 (佣金/固定费用) 的费率单位 */
#define OES_FEE_RATE_UNIT                   (10000000)
/** ETF使用的资金百分比单位 */
#define OES_ETF_CASH_RATIO_UNIT             (100000)
/** 资金的转换单位 */
#define OES_CASH_UNIT                       (10000)
/** 债券每百元应计利息的转换单位 */
#define OES_BOND_INTEREST_UNIT              (100000000)
/** 期权保证金转换单位 */
#define OES_MARGIN_UNIT                     (100)

/** 最大买卖价格, 委托价格不能等于或超过此价格 */
#define OES_MAX_BS_PRICE                    (10000 * OES_CASH_UNIT)

/** 出入金的金额单位 */
#define OES_FUND_TRSF_UNIT                  (100)

/** 期权看涨方向限额 */
#define OES_HOLD_LIMIT_QUOTA_UP             (500)
/** 期权看跌方向限额 */
#define OES_HOLD_LIMIT_QUOTA_DOWN           (500)
/* -------------------------           */


/* ===================================================================
 * 枚举类型定义 (1. 市场相关)
 * =================================================================== */

/**
 * 交易所代码定义
 */
typedef enum _eOesExchangeId {
    OES_EXCH_UNDEFINE                       = 0,        /**< 未定义的交易所代码 */
    OES_EXCH_SSE                            = 1,        /**< 上海证券交易所 */
    OES_EXCH_SZSE                           = 2,        /**< 深圳证券交易所 */
    __MAX_OES_EXCH,

    /** 上海证券交易所 @depricated 已过时, 请使用 OES_EXCH_SSE */
    OES_EXCHANGE_TYPE_SSE                   = OES_EXCH_SSE,
    /** 深圳证券交易所 @depricated 已过时, 请使用 OES_EXCH_SZSE */
    OES_EXCHANGE_TYPE_SZSE                  = OES_EXCH_SZSE
} eOesExchangeIdT;


/**
 * 市场类型定义
 */
typedef enum _eOesMarketId {
    OES_MKT_UNDEFINE                        = 0,        /**< 未定义的市场类型 */
    OES_MKT_SH_ASHARE                       = 1,        /**< 上海A股 */
    OES_MKT_SZ_ASHARE                       = 2,        /**< 深圳A股 */
    OES_MKT_SH_OPTION                       = 3,        /**< 上海期权 */
    __OES_MKT_ID_MAX,                                   /**< 市场类型最大值 */

    /** 未定义的市场类型 @depricated 已过时, 请使用 OES_MKT_UNDEFINE  */
    OES_MKT_ID_UNDEFINE                     = OES_MKT_UNDEFINE,
    /** 上海A股 @depricated 已过时, 请使用 OES_MKT_SH_ASHARE */
    OES_MKT_ID_SH_A                         = OES_MKT_SH_ASHARE,
    /** 深圳A股 @depricated 已过时, 请使用 OES_MKT_SZ_ASHARE */
    OES_MKT_ID_SZ_A                         = OES_MKT_SZ_ASHARE,
    /** 上海期权 @depricated 已过时, 请使用 OES_MKT_SH_OPTION */
    OES_MKT_ID_SH_OPT                       = OES_MKT_SH_OPTION,

    __OES_MKT_ID_MAX_ALIGNED4               = 4,        /**< 市场类型最大值 (按4字节对齐的大小) */
    __OES_MKT_ID_MAX_ALIGNED8               = 8         /**< 市场类型最大值 (按8字节对齐的大小) */
} eOesMarketIdT;


/**
 * 市场交易平台类型定义
 */
typedef enum _eOesPlatformId {
    OES_PLATFORM_UNDEFINE                   = 0,        /**< 未定义的交易平台类型 */
    OES_PLATFORM_CASH_AUCTION               = 1,        /**< 现货集中竞价交易平台 */
    OES_PLATFORM_FINANCIAL_SERVICES         = 2,        /**< 综合金融服务平台 */
    OES_PLATFORM_NON_TRADE                  = 3,        /**< 非交易处理平台 */
    OES_PLATFORM_DERIVATIVE_AUCTION         = 4,        /**< 衍生品集中竞价交易平台 */
    __OES_PLATFORM_ID_MAX,                              /**< 平台号的最大值 */
    __OES_PLATFORM_ACTIVE_MAX               = 4         /**< 当前支持的平台最大值 */
} eOesPlatformIdT;


/**
 * 市场状态定义
 */
typedef enum _eOesMarketStatus {
    OES_MKT_STATE_UNDEFINE                  = 0,        /**< 未定义的市场状态 */
    OES_MKT_STATE_PRE_OPEN                  = 1,        /**< 未开放 (PreOpen) */
    OES_MKT_STATE_OPEN_UP_COMING            = 2,        /**< 即将开放 (OpenUpComing) */
    OES_MKT_STATE_OPEN                      = 3,        /**< 开放 (Open) */
    OES_MKT_STATE_HALT                      = 4,        /**< 暂停开放 (Halt) */
    OES_MKT_STATE_CLOSE                     = 5,        /**< 关闭 (Close) */
    __OES_MKT_STATE_MAX                                 /**< 市场状态最大值 */
} eOesMarketStatusT;


/**
 * OES 竞价时段定义
 */
typedef enum _eOesTrdSessType {
    OES_TRD_SESS_TYPE_O                     = 0,        /**< 开盘集合竞价时段 */
    OES_TRD_SESS_TYPE_T                     = 1,        /**< 连续竞价时段 */
    OES_TRD_SESS_TYPE_C                     = 2,        /**< 收盘集合竞价 */
    __OES_TRD_SESS_TYPE_MAX                             /**< 时段类型最大值 (时段类型数量) */
} eOesTrdSessTypeT;
/* -------------------------           */


/* ===================================================================
 * 枚举类型定义 (2. 产品相关)
 * =================================================================== */

/**
 * 证券类别
 */
typedef enum _eOesSecurityType {
    OES_SECURITY_TYPE_UNDEFINE              = 0,        /**< 未定义的证券类型 */
    OES_SECURITY_TYPE_STOCK                 = 1,        /**< 股票 */
    OES_SECURITY_TYPE_BOND                  = 2,        /**< 债券 */
    OES_SECURITY_TYPE_ETF                   = 3,        /**< ETF */
    OES_SECURITY_TYPE_FUND                  = 4,        /**< 基金 */
    OES_SECURITY_TYPE_OPTION                = 5,        /**< 期权 */
    __OES_SECURITY_TYPE_MAX,                            /**< 证券类型最大值 */

    __OES_SECURITY_TYPE_NOT_SUPPORT         = 100       /**< 不支持的证券类别 */
} eOesSecurityTypeT;


/**
 * 证券子类别
 */
typedef enum _eOesSubSecurityType {
    OES_SUB_SECURITY_TYPE_UNDEFINE          = 0,        /**< 未定义的证券子类型 */

    __OES_SUB_SECURITY_TYPE_STOCK_MIN       = 10,       /**< 股票类证券子类型最小值 */
    OES_SUB_SECURITY_TYPE_STOCK_ASH         = 11,       /**< A股股票, A Share */
    OES_SUB_SECURITY_TYPE_STOCK_SME         = 12,       /**< 中小板股票, Small & Medium Enterprise (SME) Board */
    OES_SUB_SECURITY_TYPE_STOCK_GEM         = 13,       /**< 创业板股票, Growth Enterprise Market (GEM) */
    OES_SUB_SECURITY_TYPE_STOCK_CDR         = 14,       /**< 存托凭证, Chinese Depository Receipt (CDR) */
    __OES_SUB_SECURITY_TYPE_STOCK_MAX,                  /**< 股票类证券子类型最大值 */

    __OES_SUB_SECURITY_TYPE_BOND_MIN        = 20,       /**< 债券类证券子类型最小值 */
    OES_SUB_SECURITY_TYPE_BOND_GBF          = 21,       /**< 国债 */
    OES_SUB_SECURITY_TYPE_BOND_CBF          = 22,       /**< 企业债 */
    OES_SUB_SECURITY_TYPE_BOND_CPF          = 23,       /**< 公司债 */
    OES_SUB_SECURITY_TYPE_BOND_CCF          = 24,       /**< 可转换债券 */
    OES_SUB_SECURITY_TYPE_BOND_FBF          = 25,       /**< 金融机构发行债券 */
    OES_SUB_SECURITY_TYPE_BOND_PRP          = 26,       /**< 债券质押式回购 */
    OES_SUB_SECURITY_TYPE_BOND_STD          = 27,       /**< 债券标准券 */
    __OES_SUB_SECURITY_TYPE_BOND_MAX,                   /**< 债券类证券子类型最大值 */

    __OES_SUB_SECURITY_TYPE_ETF_MIN         = 30,       /**< ETF类证券子类型最小值 */
    OES_SUB_SECURITY_TYPE_ETF_SINGLE_MKT    = 31,       /**< 单市场股票ETF */
    OES_SUB_SECURITY_TYPE_ETF_CROSS_MKT     = 32,       /**< 跨市场股票ETF */
    OES_SUB_SECURITY_TYPE_ETF_BOND          = 33,       /**< 实物债券ETF */
    OES_SUB_SECURITY_TYPE_ETF_CURRENCY      = 34,       /**< 货币ETF */
    OES_SUB_SECURITY_TYPE_ETF_CROSS_BORDER  = 35,       /**< 跨境ETF */
    OES_SUB_SECURITY_TYPE_ETF_GOLD          = 36,       /**< 黄金ETF */
    __OES_SUB_SECURITY_TYPE_ETF_MAX,                    /**< ETF类证券子类型最大值 */

    __OES_SUB_SECURITY_TYPE_FUND_MIN        = 40,       /**< 基金类证券子类型最小值 */
    OES_SUB_SECURITY_TYPE_FUND_LOF          = 41,       /**< LOF基金 */
    OES_SUB_SECURITY_TYPE_FUND_CEF          = 42,       /**< 封闭式基金, Close-end Fund */
    OES_SUB_SECURITY_TYPE_FUND_OEF          = 43,       /**< 开放式基金, Open-end Fund */
    OES_SUB_SECURITY_TYPE_FUND_GRADED       = 44,       /**< 分级子基金 */
    __OES_SUB_SECURITY_TYPE_FUND_MAX,                   /**< 基金类证券子类型最大值 */

    __OES_SUB_SECURITY_TYPE_OPTION_MIN      = 50,       /**< 期权类证券子类型最小值 */
    OES_SUB_SECURITY_TYPE_OPTION_STOCK      = 51,       /**< 个股期权 */
    OES_SUB_SECURITY_TYPE_OPTION_ETF        = 52,       /**< ETF期权 */
    __OES_SUB_SECURITY_TYPE_OPTION_MAX,                 /**< 期权类证券子类型最大值 */

    __OES_SUB_SECURITY_TYPE_MAX             = __OES_SUB_SECURITY_TYPE_OPTION_MAX
} eOesSubSecurityTypeT;


/**
 * 产品级别
 */
typedef enum _eOesSecurityLevel {
    OES_SECURITY_LEVEL_UNDEFINE             = 0,
    OES_SECURITY_LEVEL_N                    = 1,        /**< 正常证券 */
    OES_SECURITY_LEVEL_XST                  = 2,        /**< *ST股 */
    OES_SECURITY_LEVEL_ST                   = 3,        /**< ST股 */
    OES_SECURITY_LEVEL_P                    = 4,        /**< 退市整理证券 */
    OES_SECURITY_LEVEL_T                    = 5,        /**< 退市转让证券 */
    OES_SECURITY_LEVEL_U                    = 6,        /**< 优先股 */
    OES_SECURITY_LEVEL_B                    = 7,        /**< B级基金 */
    __OES_SECURITY_LEVEL_MAX
} eOesSecurityLevelT;


/**
 * 产品风险等级
 */
typedef enum _eOesSecurityRiskLevel {
    OES_RISK_LEVEL_VERY_LOW                 = 0,        /**< 最低风险 */
    OES_RISK_LEVEL_LOW                      = 1,        /**< 低风险 */
    OES_RISK_LEVEL_MEDIUM_LOW               = 2,        /**< 中低风险 */
    OES_RISK_LEVEL_MEDIUM                   = 3,        /**< 中风险 */
    OES_RISK_LEVEL_MEDIUM_HIGH              = 4,        /**< 中高风险 */
    OES_RISK_LEVEL_HIGH                     = 5,        /**< 高风险 */
    OES_RISK_LEVEL_VERY_HIGH                = 6,        /**< 极高风险 */
    __OES_RISK_LEVEL_MAX
} eOesSecurityRiskLevelT;


/**
 * 产品停复牌标识类别
 */
typedef enum _eOesSecuritySuspFlag {
    OES_SUSPFLAG_NONE                       = 0x0,      /**< 无停牌标识 */
    OES_SUSPFLAG_EXCHANGE                   = 0x1,      /**< 交易所连续停牌 */
    OES_SUSPFLAG_BROKER                     = 0x2,      /**< 券商人工停牌 */
    __OES_SUSPFLAG_OTHER
} eOesSecuritySuspFlagT;
/* -------------------------           */


/* ===================================================================
 * 枚举类型定义 (3. 订单相关)
 * =================================================================== */

/**
 * 订单执行状态定义
 */
typedef enum _eOesOrdStatus {
    OES_ORD_STATUS_UNDEFINE                 = 0,        /**< 未定义 */
    OES_ORD_STATUS_NEW                      = 1,        /**< 新订单 */

    OES_ORD_STATUS_DECLARED                 = 2,        /**< 已确认 */
    OES_ORD_STATUS_PARTIALLY_FILLED         = 3,        /**< 部分成交 */

    __OES_ORD_STATUS_FINAL_MIN              = 4,        /**< 订单终结状态判断标志 */
    OES_ORD_STATUS_CANCEL_DONE              = 5,        /**< 撤单指令已执行 (适用于撤单请求, 并做为撤单请求的终结状态) */
    OES_ORD_STATUS_PARTIALLY_CANCELED       = 6,        /**< 部分撤单 (部分成交, 剩余撤单) */
    OES_ORD_STATUS_CANCELED                 = 7,        /**< 已撤单 */
    OES_ORD_STATUS_FILLED                   = 8,        /**< 已成交 (全部成交) */
    __OES_ORD_STATUS_VALID_MAX,

    __OES_ORD_STATUS_INVALID_MIN            = 10,       /**< 废单判断标志 */
    OES_ORD_STATUS_INVALID_OES              = 11,       /**< OES内部废单 */
    OES_ORD_STATUS_INVALID_SH_F             = 12,       /**< 上证后台判断该订单为废单 */
    OES_ORD_STATUS_INVALID_SH_E             = 13,       /**< 上证前台判断该订单为废单 */
    OES_ORD_STATUS_INVALID_SH_COMM          = 14,       /**< 通信故障 */
    OES_ORD_STATUS_INVALID_SZ_F             = 15,       /**< 深证前台废单 */
    OES_ORD_STATUS_INVALID_SZ_E             = 16,       /**< 深证后台废单 */
    OES_ORD_STATUS_INVALID_SZ_REJECT        = 17,       /**< 深证业务拒绝 */
    OES_ORD_STATUS_INVALID_SZ_TRY_AGAIN     = 18,       /**< 深证平台未开放(需尝试重报) */
    __OES_ORD_STATUS_INVALID_MAX,

    /*
     * 以下订单状态定义已废弃, 只是为了兼容之前的版本而暂时保留
     */
    OES_ORD_STATUS_NORMAL                   = OES_ORD_STATUS_NEW,
    OES_ORD_STATUS_DECLARING                = OES_ORD_STATUS_NEW,
    __OES_ORD_STATUS_INVALID_OES            = OES_ORD_STATUS_INVALID_OES

} eOesOrdStatusT;


/**
 * 委托类型
 *
 * 部分缩写解释如下:
 *  - LMT (Limit)           : 限价
 *  - MTL (Market To Limit) : 剩余转限价(市价)
 *  - FAK (Fill and Kill)   : 剩余转撤销(市价)
 *  - FOK (Fill or Kill)    : 全部成交或全部撤销(市价/限价)
 *
 * 上海A股支持类型:
 *      1. OES_ORD_TYPE_LMT
 *      2. OES_ORD_TYPE_MTL_BEST_5
 *      3. OES_ORD_TYPE_FAK_BEST_5
 *
 * 上海期权支持市价类型:
 *      1. OES_ORD_TYPE_LMT
 *      2. OES_ORD_TYPE_LMT_FOK
 *      3. OES_ORD_TYPE_FOK
 *
 * 深圳A股支持市价类型:
 *      1. OES_ORD_TYPE_LMT
 *      2. OES_ORD_TYPE_MTL_BEST
 *      3. OES_ORD_TYPE_MTL_SAMEPARTY_BEST
 *      4. OES_ORD_TYPE_FAK_BEST_5
 *      5. OES_ORD_TYPE_FAK
 *      6. OES_ORD_TYPE_FOK
 */
typedef enum _eOesOrdType {
    OES_ORD_TYPE_LMT                        = 0,        /**< 限价 */
    OES_ORD_TYPE_LMT_FOK                    = 1,        /**< 限价FOK */
    __OES_ORD_TYPE_LMT_MAX,

    OES_ORD_TYPE_MTL_BEST_5                 = 10,       /**< 最优五档即时成交剩余转限价 */
    OES_ORD_TYPE_MTL_BEST                   = 11,       /**< 对手方最优价格申报 */
    OES_ORD_TYPE_MTL_SAMEPARTY_BEST         = 12,       /**< 本方最优价格申报 */
    __OES_ORD_TYPE_MTL_MAX,

    OES_ORD_TYPE_FAK_BEST_5                 = 20,       /**< 最优五档即时成交剩余撤销 */
    OES_ORD_TYPE_FAK                        = 21,       /**< 即时成交剩余撤销 */
    __OES_ORD_TYPE_FAK_MAX,

    OES_ORD_TYPE_FOK                        = 30,       /**< 市价全部成交或全部撤销 */
    __OES_ORD_TYPE_FOK_MAX,
    __OES_ORD_TYPE_MAX
} eOesOrdTypeT;


/**
 * 上证委托类型
 *
 * 部分缩写解释如下:
 *  - LMT (Limit)           : 限价
 *  - MTL (Market To Limit) : 剩余转限价(市价)
 *  - FAK (Fill and Kill)   : 剩余转撤销(市价)
 *  - FOK (Fill or Kill)    : 全部成交或全部撤销(市价/限价)
 */
typedef enum _eOesOrdTypeSh {
    /** 限价, 0 */
    OES_ORD_TYPE_SH_LMT                     = OES_ORD_TYPE_LMT,
    /** 最优五档即时成交剩余转限价, 10 */
    OES_ORD_TYPE_SH_MTL_BEST_5              = OES_ORD_TYPE_MTL_BEST_5,
    /** 最优五档即时成交剩余撤销, 20 */
    OES_ORD_TYPE_SH_FAK_BEST_5              = OES_ORD_TYPE_FAK_BEST_5,
    /** 限价FOK(期权), 1 */
    OES_ORD_TYPE_SH_LMT_FOK                 = OES_ORD_TYPE_LMT_FOK,
    /** 市价FOK(期权), 30 */
    OES_ORD_TYPE_SH_FOK                     = OES_ORD_TYPE_FOK
} eOesOrdTypeShT;


/**
 * 深证委托类型
 *
 * 部分缩写解释如下:
 *  - LMT (Limit)           : 限价
 *  - MTL (Market To Limit) : 剩余转限价(市价)
 *  - FAK (Fill and Kill)   : 剩余转撤销(市价)
 *  - FOK (Fill or Kill)    : 全部成交或全部撤销(市价/限价)
 */
typedef enum _eOesOrdTypeSz {
    /** 限价, 0 */
    OES_ORD_TYPE_SZ_LMT                     = OES_ORD_TYPE_LMT,
    /** 对手方最优价格申报, 11 */
    OES_ORD_TYPE_SZ_MTL_BEST                = OES_ORD_TYPE_MTL_BEST,
    /** 本方最优价格申报, 12 */
    OES_ORD_TYPE_SZ_MTL_SAMEPARTY_BEST      = OES_ORD_TYPE_MTL_SAMEPARTY_BEST,
    /** 最优五档即时成交剩余撤销, 20 */
    OES_ORD_TYPE_SZ_FAK_BEST_5              = OES_ORD_TYPE_FAK_BEST_5,
    /** 即时成交剩余撤销申报, 21 */
    OES_ORD_TYPE_SZ_FAK                     = OES_ORD_TYPE_FAK,
    /** 市价全额成交或撤销申报, 30 */
    OES_ORD_TYPE_SZ_FOK                     = OES_ORD_TYPE_FOK,
    /** 限价全额成交或撤销申报(只用于期权), 1 */
    OES_ORD_TYPE_SZ_LMT_FOK                 = OES_ORD_TYPE_LMT_FOK
} eOesOrdTypeSzT;


/**
 * 买卖类型
 */
typedef enum _eOesBuySellType {
    OES_BS_TYPE_UNDEFINE                    = 0,        /**< 未定义的买卖类型 */

    OES_BS_TYPE_BUY                         = 1,        /**< 买入 */
    OES_BS_TYPE_SELL                        = 2,        /**< 卖出 */
    OES_BS_TYPE_CREATION                    = 3,        /**< 申购 */
    OES_BS_TYPE_REDEMPTION                  = 4,        /**< 赎回 */
    OES_BS_TYPE_CREDIT_BUY                  = 5,        /**< 融资买入 */
    OES_BS_TYPE_CREDIT_SELL                 = 6,        /**< 融券卖出, 质押式逆回购 */
    OES_BS_TYPE_SUBSCRIPTION                = 7,        /**< 新股认购 */

    OES_BS_TYPE_BUY_OPEN                    = 11,       /**< 期权买入开仓 */
    OES_BS_TYPE_BUY_CLOSE                   = 12,       /**< 期权买入平仓 */
    OES_BS_TYPE_SELL_OPEN                   = 13,       /**< 期权卖出开仓 */
    OES_BS_TYPE_SELL_CLOSE                  = 14,       /**< 期权卖出平仓 */
    OES_BS_TYPE_COVERED_OPEN                = 15,       /**< 期权备兑开仓 */
    OES_BS_TYPE_COVERED_CLOSE               = 16,       /**< 期权备兑平仓 */
    OES_BS_TYPE_OPTION_EXERCISE             = 17,       /**< 期权行权 */
    OES_BS_TYPE_UNDERLYING_FREEZE           = 18,       /**< 期权标的锁定 */
    OES_BS_TYPE_UNDERLYING_UNFREEZE         = 19,       /**< 期权标的解锁 */
    /* -------------------------           */

    OES_BS_TYPE_CANCEL                      = 30,       /**< 撤单 */
    __OES_BS_TYPE_MAX_TRADING,                          /**< 对外开放的交易类业务的买卖类型最大值 */
    /* -------------------------           */

    OES_BS_TYPE_SSE_DESIGNATION             = 41,       /**< 指定登记 */
    OES_BS_TYPE_SSE_RECALL_DESIGNATION      = 42,       /**< 指定撤消 */
    OES_BS_TYPE_SZSE_DESIGNATION            = 43,       /**< 托管注册 */
    OES_BS_TYPE_SZSE_CANCEL_DESIGNATION     = 44,       /**< 托管撤消 */

    __OES_BS_TYPE_MAX,                                  /**< 买卖类型最大值 */
    /* -------------------------           */

    /*
     * 以下买卖类型定义即将被废弃
     */
    OES_BS_TYPE_B                           = OES_BS_TYPE_BUY,
    OES_BS_TYPE_S                           = OES_BS_TYPE_SELL,
    OES_BS_TYPE_KB                          = OES_BS_TYPE_CREATION,
    OES_BS_TYPE_KS                          = OES_BS_TYPE_REDEMPTION,
    OES_BS_TYPE_CB                          = OES_BS_TYPE_CREDIT_BUY,
    OES_BS_TYPE_CS                          = OES_BS_TYPE_CREDIT_SELL,

    OES_BS_TYPE_BO                          = OES_BS_TYPE_BUY_OPEN,
    OES_BS_TYPE_BC                          = OES_BS_TYPE_BUY_CLOSE,
    OES_BS_TYPE_SO                          = OES_BS_TYPE_SELL_OPEN,
    OES_BS_TYPE_SC                          = OES_BS_TYPE_SELL_CLOSE,
    OES_BS_TYPE_CO                          = OES_BS_TYPE_COVERED_OPEN,
    OES_BS_TYPE_CC                          = OES_BS_TYPE_COVERED_CLOSE,
    OES_BS_TYPE_TE                          = OES_BS_TYPE_OPTION_EXERCISE,
    OES_BS_TYPE_UF                          = OES_BS_TYPE_UNDERLYING_FREEZE,
    OES_BS_TYPE_UU                          = OES_BS_TYPE_UNDERLYING_UNFREEZE

} eOesBuySellTypeT;


/**
 * 订单的买卖方向 (内部使用)
 */
typedef enum _eOesOrdDir {
    OES_ORD_DIR_BUY                         = 0,        /**< 买 */
    OES_ORD_DIR_SELL                        = 1,        /**< 卖 */
    __OES_ORD_DIR_MAX                                   /**< 买卖方向最大值 */
} eOesOrdDirT;


/**
 * ETF成交回报记录的成交类型
 * 上证接口规范 (IS103_ETFInterface_CV14_20130123) 中规定如下:
 * - ETF 基金二级市场代码记录表示一笔申购/赎回交易连续记录的开始,对一笔申购/赎回交易而言,有且只有一条;
 * - 一级市场代码记录不再表示对应申购/赎回交易连续记录的结束,对一笔申购/赎回交易而言,有且只有一条。
 */
typedef enum _eOesEtfTrdCnfmType {
    OES_ETF_TRDCNFM_TYPE_NONE               = 0,        /**< 无意义 */
    OES_ETF_TRDCNFM_TYPE_ETF_FIRST          = 1,        /**< ETF二级市场记录 */
    OES_ETF_TRDCNFM_TYPE_CMPOENT            = 2,        /**< ETF成分证券成交记录 */
    OES_ETF_TRDCNFM_TYPE_CASH               = 3,        /**< ETF成交资金记录 */
    OES_ETF_TRDCNFM_TYPE_ETF_LAST           = 4,        /**< ETF一级市场记录 */
    __OES_ETF_TRDCNFM_TYPE_MAX                          /**< ETF成交的最大值 */
} eOesEtfTrdCnfmTypeT;


/**
 * ETF成分股现金替代标志
 */
typedef enum _eOesEtfSubFlag {
    OES_ETF_SUBFLAG_FORBID_SUB              = 0,        /**< 禁止现金替代 (必须有证券) */
    OES_ETF_SUBFLAG_ALLOW_SUB               = 1,        /**< 可以进行现金替代(先用证券,
                                                             如证券不足可用现金替代) */
    OES_ETF_SUBFLAG_MUST_SUB                = 2,        /**< 必须用现金替代 */
    OES_ETF_SUBFLAG_SZ_REFUND_SUB           = 3,        /**< 该证券为深市证券, 退补现金替代 */
    OES_ETF_SUBFLAG_SZ_MUST_SUB             = 4,        /**< 该证券为深市证券, 必须现金替代 */
    OES_ETF_SUBFLAG_OTHER_REFUND_SUB        = 5,        /**< 非沪深市场成分证券退补现金替代 */
    OES_ETF_SUBFLAG_OTHER_MUST_SUB          = 6         /**< 非沪深市场成份证券必须现金替代 */
} eOesEtfSubFlagT;


/**
 * OES中签、配号记录类型
 */
typedef enum _eOesLotType {
    OES_LOT_TYPE_UNDEFINE                   = 0,        /**< 未定义的买卖类型 */
    OES_LOT_TYPE_FAILED                     = 1,        /**< 认购失败记录 */
    OES_LOT_TYPE_ASSIGNMENT                 = 2,        /**< 配号记录 */
    OES_LOT_TYPE_LOTTERY                    = 3,        /**< 中签记录 */
    __OES_LOT_TYPE_MAX                                  /**< 中签、配号记录类型最大值 */
} eOesLotTypeT;


/**
 * OES配号失败原因
 */
typedef enum _eOesLotRejReason {
    OES_LOT_REJ_REASON_DUPLICATE            = 1,        /**< 配号失败-重复申购 */
    OES_LOT_REJ_REASON_INVALID_DUPLICATE    = 2,        /**< 配号失败-违规重复 */
    OES_LOT_REJ_REASON_OFFLINE_FIRST        = 3,        /**< 配号失败-网下在先 */
    OES_LOT_REJ_REASON_BAD_RECORD           = 4,        /**< 配号失败-不良记录 */
    OES_LOT_REJ_REASON_UNKNOW               = 5         /**< 配号失败-未知原因 */
} eOesLotRejReasonT;


/**
 * OES执行类型
 */
typedef enum _eOesExecType {
    OES_EXECTYPE_UNDEFINE                   = 0,        /**< 未定义的执行类型 */
    OES_EXECTYPE_INSERT                     = 1,        /**< 已接收 */
    OES_EXECTYPE_CONFIRMED                  = 2,        /**< 已确认 */
    OES_EXECTYPE_CANCELLED                  = 3,        /**< 撤单 */
    OES_EXECTYPE_AUTO_CANCELLED             = 4,        /**< 自动撤单 */
    OES_EXECTYPE_REJECT                     = 5,        /**< 拒绝 */
    OES_EXECTYPE_TRADE                      = 6,        /**< 成交 */
    __OES_EXECTYPE_MAX                                  /**< 执行类型最大值 */
} eOesExecTypeT;
/* -------------------------           */


/* ===================================================================
 * 枚举类型定义 (4. 资金/费用/出入金相关)
 * =================================================================== */

/**
 * 货币类型
 */
typedef enum _eOesCurrType {
    OES_CURR_TYPE_RMB                       = 0,        /**< 人民币 */
    OES_CURR_TYPE_HKD                       = 1,        /**< 港币 */
    OES_CURR_TYPE_USD                       = 2,        /**< 美元 */
    __OES_CURR_TYPE_MAX                                 /**< 货币种类最大值 */
} eOesCurrTypeT;


/**
 * 费用标识符
 */
typedef enum _eOesFeeType {
    OES_FEE_TYPE_EXCHANGE_STAMP             = 0x1,      /**< 交易所固定费用-印花税 */
    OES_FEE_TYPE_EXCHANGE_TRANSFER          = 0x2,      /**< 交易所固定费用-过户费 */
    OES_FEE_TYPE_EXCHANGE_SETTLEMENT        = 0x3,      /**< 交易所固定费用-结算费 */
    OES_FEE_TYPE_EXCHANGE_TRADE_RULE        = 0x4,      /**< 交易所固定费用-交易规费 */
    OES_FEE_TYPE_EXCHANGE_EXCHANGE          = 0x5,      /**< 交易所固定费用-经手费 */
    OES_FEE_TYPE_EXCHANGE_ADMINFER          = 0x6,      /**< 交易所固定费用-证管费 */
    OES_FEE_TYPE_EXCHANGE_OTHER             = 0x7,      /**< 交易所固定费用-其他费 */
    __OES_FEE_TYPE_EXCHANGE_MAX,                        /**< 交易所固定费用最大值 */

    OES_FEE_TYPE_BROKER_BACK_END            = 0x11      /**< 券商佣金-后台费用 */
} eOesFeeTypeT;


/**
 * 费用 (佣金/固定费用) 计算模式
 */
typedef enum _eOesCalcFeeMode {
    OES_CALC_FEE_MODE_AMOUNT                = 0,        /**< 按金额 */
    OES_CALC_FEE_MODE_QTY                   = 1,        /**< 按份额 */
    OES_CALC_FEE_MODE_ORD                   = 2         /**< 按笔数 */
} eOesCalcFeeModeT;


/**
 * 出入金方向定义
 */
typedef enum _eOesFundTrsfDirect {
    OES_FUND_TRSF_DIRECT_IN                 = 0,        /**< 转入OES (入金) */
    OES_FUND_TRSF_DIRECT_OUT                = 1         /**< 转出OES (出金) */
} eOesFundTrsfDirectT;


/**
 * 出入金转账类型定义
 */
typedef enum _eOesFundTrsfType {
    OES_FUND_TRSF_TYPE_OES_BANK             = 0,        /**< OES和银行之间转账 */
    OES_FUND_TRSF_TYPE_OES_COUNTER          = 1,        /**< OES和主柜之间划拨资金 */
    OES_FUND_TRSF_TYPE_COUNTER_BANK         = 2,        /**< 主柜和银行之间转账 */
    __OES_FUND_TRSF_TYPE_MAX                            /**< 出入金转账类型最大值 */
} eOesFundTrsfTypeT;


/**
 * 出入金委托状态
 */
typedef enum _eOesFundTrsfStatus {
    OES_FUND_TRSF_STS_UNDECLARED            = 0,        /**< 尚未上报到主柜 */
    OES_FUND_TRSF_STS_DECLARED              = 1,        /**< 已上报到主柜 */
    OES_FUND_TRSF_STS_WAIT_DONE             = 2,        /**< 主柜处理完成, 等待事务结束 */
    OES_FUND_TRSF_STS_DONE                  = 3,        /**< 出入金处理完成 */

    __OES_FUND_TRSF_STS_ROLLBACK_MIN        = 5,        /**< 废单判断标志 */
    OES_FUND_TRSF_STS_UNDECLARED_ROLLBACK   = 6,        /**< 待回滚(未上报到主柜前) */
    OES_FUND_TRSF_STS_DECLARED_ROLLBACK     = 7,        /**< 待回滚(已上报到主柜后) */

    __OES_FUND_TRSF_STS_INVALID_MIN         = 10,       /**< 废单判断标志 */
    OES_FUND_TRSF_STS_INVALID_OES           = 11,       /**< OES内部判断为废单 */
    OES_FUND_TRSF_STS_INVALID_COUNTER       = 12,       /**< 主柜判断为废单 */
    OES_FUND_TRSF_STS_SUSPENDED             = 13        /**< 挂起状态 (主柜的出入金执行状态未知, 待人工干预处理) */
} eOesFundTrsfStatusT;


/* ===================================================================
 * 枚举类型定义 (5. 投资人/账户/权限相关)
 * =================================================================== */

/**
 * 账户类别定义
 * 资金账户类别与证券账户类别定义相同
 */
typedef enum _eOesAcctType {
    OES_ACCT_TYPE_NORMAL                    = 0,        /**< 普通账户 */
    OES_ACCT_TYPE_CREDIT                    = 1,        /**< 信用账户 */
    OES_ACCT_TYPE_OPTION                    = 2,        /**< 衍生品账户 */
    __OES_ACCT_TYPE_MAX                                 /**< 账户类别最大值 */
} eOesAcctTypeT;


/**
 * 资金类型定义
 * @see eOesAcctTypeT
 */
typedef enum _eOesCashType {
    /** 普通账户资金/现货资金 */
    OES_CASH_TYPE_SPOT                      = OES_ACCT_TYPE_NORMAL,
    /** 信用账户资金/信用资金 */
    OES_CASH_TYPE_CREDIT                    = OES_ACCT_TYPE_CREDIT,
    /** 衍生品账户资金/期权保证金 */
    OES_CASH_TYPE_OPTION                    = OES_ACCT_TYPE_OPTION,
    /** 资金类型最大值 */
    __OES_CASH_TYPE_MAX                     = __OES_ACCT_TYPE_MAX,

    /** 兼容性定义, 即将废弃 */
    OES_CASH_TYPE_CRE                       = OES_CASH_TYPE_CREDIT,
    OES_CASH_TYPE_OPT                       = OES_CASH_TYPE_OPTION,
} eOesCashTypeT;


/**
 * 客户状态/证券帐户/资金账户状态
 */
typedef enum _eOesAcctStatus {
    OES_ACCT_STATUS_NORMAL                  = 0,        /**< 正常 */
    OES_ACCT_STATUS_DISABLED                = 1,        /**< 非正常 */
    OES_ACCT_STATUS_LOCKED                  = 2         /**< 已锁定 */
} eOesAcctStatusT;


/**
 * 交易权限的枚举值定义
 */
typedef enum _eOesTradingPermission {
    OES_PERMIS_MARKET_ORDER                 = (1 << 1),     /**< 市价委托 */
    OES_PERMIS_STRUCTURED_FUND              = (1 << 2),     /**< 分级基金适当性 */
    OES_PERMIS_BOND_QUALIFIED_INVESTOR      = (1 << 3),     /**< 债券合格投资者 */
    OES_PERMIS_XXX4                         = (1 << 4),     /**< 融资行权 */

    OES_PERMIS_DELISTING                    = (1 << 5),     /**< 退市整理股票 */
    OES_PERMIS_RISK_WARNING                 = (1 << 6),     /**< 风险警示股票 */

    OES_PERMIS_SINGLE_MARKET_ETF            = (1 << 7),     /**< 单市场ETF申赎 */
    OES_PERMIS_CROSS_BORDER_ETF             = (1 << 8),     /**< 跨境ETF申赎 */
    OES_PERMIS_CROSS_MARKET_ETF             = (1 << 9),     /**< 跨市场ETF申赎 */
    OES_PERMIS_CURRENCY_ETF                 = (1 << 10),    /**< 货币基金申赎 */

    OES_PERMIS_STOCK_PLEDGE_REPURCHASE      = (1 << 11),    /**< 股票质押式回购 */
    OES_PERMIS_PLEDGE_REPURCHASE            = (1 << 12),    /**< 质押回购 */

    OES_PERMIS_GEM                          = (1 << 13),    /**< 创业板交易 */
    OES_PERMIS_SH_HK_STOCK_CONNECT          = (1 << 14),    /**< 沪港通 */
    OES_PERMIS_SZ_HK_STOCK_CONNECT          = (1 << 15),    /**< 深港通 */

    OES_PERMIS_CDR                          = (1 << 16),    /**< 存托凭证 */
    OES_PERMIS_INNOVATION                   = (1 << 17),    /**< 创新企业股票 */

    __OES_PERMIS_ALL                        = 0xFFFFFFFF    /**< 全部权限 */
} eOesTradingPermissionT;


/**
 * 交易限制的枚举值定义
 */
typedef enum _eOesTradingLimit {
    OES_LIMIT_BUY                           = (1 << 1),     /**< 禁止买入 */
    OES_LIMIT_SELL                          = (1 << 2),     /**< 禁止卖出 */
    OES_LIMIT_RECALL_DESIGNATION            = (1 << 3),     /**< 禁撤销指定 */
    OES_LIMIT_DESIGNATION                   = (1 << 4),     /**< 禁止转托管 */

    OES_LIMIT_REPO                          = (1 << 5),     /**< 禁止回购融资 */
    OES_LIMIT_REVERSE_REPO                  = (1 << 6),     /**< 禁止回购融券 */
    OES_LIMIT_SUBSCRIPTION                  = (1 << 7),     /**< 禁止普通申购 (新股认购) */
    OES_LIMIT_CREDIT_BUY                    = (1 << 8),     /**< 禁止融资买入 */
    OES_LIMIT_CREDIT_SELL                   = (1 << 9),     /**< 禁止融券卖出 */

    __OES_LIMIT_ALL                         = 0xFFFFFFFF    /**< 全部限制 */
} eOesTradingLimitT;


/**
 * 投资者适当性管理分类
 */
typedef enum _eOesQualificationClass {
    OES_QUALIFICATION_PUBLIC_INVESTOR           = 0,        /**< 公众投资者 */
    OES_QUALIFICATION_QUALIFIED_INVESTOR        = 1,        /**< 合格投资者(个人投资者) */
    OES_QUALIFICATION_QUALIFIED_INSTITUTIONAL   = 2         /**< 合格投资者(机构投资者) */
} eOesQualificationClassT;


/**
 * 投资者分类
 *
 * A类专业投资者: 满足《证券期货投资者适当性管理办法》第八条 (一)、 (二)、 (三) 点,
 *      比如证券公司、期货公司、基金管理公司、商业银行、保险公司、发行的理财产品等
 * B类专业投资者: 满足《证券期货投资者适当性管理办法》第八条 (四)、 (五) 点,
 *      可以是法人或者其他组织、自然人, 满足一定的净资产和金融资产的要求, 具有相关的投资经验
 * C类专业投资者: 满足《证券期货投资者适当性管理办法》第十一条 (一)、 (二) 点,
 *      由普通投资者主动申请转化而来, 满足一定的净资产和金融资产的要求, 具有相关的投资经验
 */
typedef enum _eOesInvestorClass {
    OES_INVESTOR_CLASS_NORMAL               = 0,        /**< 普通投资者 */
    OES_INVESTOR_CLASS_PROFESSIONAL_A       = 1,        /**< A类专业投资者 */
    OES_INVESTOR_CLASS_PROFESSIONAL_B       = 2,        /**< B类专业投资者 */
    OES_INVESTOR_CLASS_PROFESSIONAL_C       = 3         /**< C类专业投资者 */
} eOesInvestorClassT;


/**
 * 所有者类型 (内部使用)
 */
typedef enum _eOesOwnerType {
    OES_OWNER_TYPE_PERSONAL                 = 1,        /**< 个人投资者 */
    OES_OWNER_TYPE_EXCHANGE                 = 101,      /**< 交易所 */
    OES_OWNER_TYPE_MEMBER                   = 102,      /**< 会员 */
    OES_OWNER_TYPE_INSTITUTION              = 103,      /**< 机构投资者 */
    OES_OWNER_TYPE_PROPRIETARY              = 104,      /**< 自营 */
    OES_OWNER_TYPE_MKT_MAKER                = 105,      /**< 做市商 */
    OES_OWNER_TYPE_SETTLEMENT               = 106,      /**< 结算结构 */
    __OES_OWNER_TYPE_MAX                                /**< 所有者类型的最大值 */
} eOesOwnerTypeT;


/**
 * 客户端类型定义 (内部使用)
 */
typedef enum _eOesClientType {
    OES_CLIENT_TYPE_UNDEFINED               = 0,        /**< 客户端类型-未定义 */
    OES_CLIENT_TYPE_INVESTOR                = 1,        /**< 普通投资人 */
    OES_CLIENT_TYPE_VIRTUAL                 = 2         /**< 虚拟账户 (仅开通行情, 不可交易) */
} eOesClientTypeT;


/**
 * 客户端状态定义 (内部使用)
 */
typedef enum _eOesClientStatus {
    OES_CLIENT_STATUS_UNACTIVATED           = 0,        /**< 未激活 (不加载) */
    OES_CLIENT_STATUS_ACTIVATED             = 1,        /**< 已激活 (正常加载) */
    OES_CLIENT_STATUS_PAUSE                 = 2,        /**< 已暂停 (正常加载, 不可交易) */
    OES_CLIENT_STATUS_SUSPENDED             = 3,        /**< 已挂起 (正常加载, 不可交易、不可出入金) */
    OES_CLIENT_STATUS_CANCELLED             = 4         /**< 已注销 (不加载) */
} eOesClientStatusT;
/* -------------------------           */


/* ===================================================================
 * 枚举类型定义 (6. 期权相关)
 * =================================================================== */

/**
 * 期权合约类型
 */
typedef enum _eOesOptContractType {
    OES_OPT_CONTRACT_TYPE_CALL              = 0,        /**< 认购 */
    OES_OPT_CONTRACT_TYPE_PUT               = 1,        /**< 认沽 */
    __OES_OPT_CONTRACT_TYPE_MAX                         /**< 合约类型最大值 */
} eOesOptContractTypeT;


/**
 * 权利类别
 */
typedef enum _eOesOptInvLevel {
    OES_OPT_INV_LEVEL_B                     = 0,        /**< 期权一级权利 */
    OES_OPT_INV_LEVEL_L                     = 1,        /**< 期权二级权利 */
    OES_OPT_INV_LEVEL_A                     = 2,        /**< 期权三级权利 */
    __OES_OPT_INV_LEVEL_MAX                             /**< 期权投资人级别最大值 */
} eOesOptInvLevelT;


/**
 * 期权产品开仓类型
 */
typedef enum _eOpenFlagType {
    OPEN_FLAG_TYPE_ALLOW                    = 0,        /**< 允许 */
    OPEN_FLAG_TYPE_FORBID                   = 1,        /**< 禁止 */
    __OPEN_FLAG_TYPE_MAX                                /**< 合约类型最大值 */
} eOpenFlagTypeT;


/**
 * 开仓标识
 */
typedef enum _eOesOptionOpenFlag {
    OES_OPTION_OPEN_FLAG_ALLOW              = 0,        /**< 允许开仓 */
    OES_OPTION_OPEN_FLAG_FORBID             = 1         /**< 禁止开仓 */
} eOesOptionOpenFlagT;


/**
 * 仓位影响
 */
typedef enum _eOesPositionEff {
    OES_POSITION_INVALID                    = 0,        /**< 无效值 */
    OES_POSITION_OPEN                       = 1,        /**< 开仓 */
    OES_POSITION_CLOSE                      = 2         /**< 平仓 */
} eOesPositionEffT;


/**
 * 期权行权类型
 */
typedef enum _eOesOptionType {
    OES_OPTION_TYPE_E                       = 0,        /**< 欧式 */
    OES_OPTION_TYPE_A                       = 1,        /**< 美式 */
    __OES_OPTION_TYPE_MAX                               /**< 行权类型最大值 */
} eOesOptionTypeT;
/* -------------------------           */


/* ===================================================================
 * 委托信息的结构体定义
 * =================================================================== */

/**
 * 委托信息的基础内容定义
 */
#define __OES_ORD_BASE_INFO_PKT                         \
        /** 客户委托流水号 (由客户端维护的递增流水, 用于识别重复的委托申报) */ \
        int32           clSeqNo; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8           mktId; \
        /** 订单类型 @see eOesOrdTypeShT eOesOrdTypeSzT */ \
        uint8           ordType; \
        /** 买卖类型 @see eOesBuySellTypeT */ \
        uint8           bsType; \
        /** 按64位对齐的填充域 */ \
        uint8           __ORD_BASE_INFO_filler; \
        \
        /** 证券账户 */ \
        char            invAcctId[OES_INV_ACCT_ID_MAX_LEN]; \
        /** 证券代码 */ \
        char            securityId[OES_SECURITY_ID_MAX_LEN]; \
        \
        /** 委托数量 */ \
        int32           ordQty; \
        /** 委托价格, 单位精确到元后四位, 即1元 = 10000 */ \
        int32           ordPrice; \
        /** 原始订单(待撤销的订单)的客户订单编号 */ \
        int64           origClOrdId; \
        \
        /** 用户私有信息 (由客户端自定义填充, 并在回报数据中原样返回) */ \
        union { \
            uint64      u64;                /**< uint64 类型的用户私有信息 */ \
            int64       i64;                /**< int64 类型的用户私有信息 */ \
            uint32      u32[2];             /**< uint32[2] 类型的用户私有信息 */ \
            int32       i32[2];             /**< int32[2] 类型的用户私有信息 */ \
            char        c8[8];              /**< char[8] 类型的用户私有信息 */ \
        } userInfo


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_ORD_BASE_INFO_PKT                 \
        0, 0, 0, 0, 0, \
        {0}, {0}, \
        0, 0, 0, \
        {0}
/* -------------------------           */


/**
 * 撤单请求的基础内容定义
 */
#define __OES_ORD_CANCEL_BASE_INFO_PKT                  \
        /** 客户委托流水号 (由客户端维护的递增流水, 用于识别重复的委托申报, 必填) */ \
        int32           clSeqNo; \
        /** 市场代码 (必填) @see eOesMarketIdT */ \
        uint8           mktId; \
        /** 按64位对齐的填充域 */ \
        uint8           __ORD_CANCEL_BASE_INFO_filler1[3]; \
        \
        /** 证券账户 (选填, 若不为空则校验待撤订单是否匹配) */ \
        char            invAcctId[OES_INV_ACCT_ID_MAX_LEN]; \
        /** 证券代码 (选填, 若不为空则校验待撤订单是否匹配) */ \
        char            securityId[OES_SECURITY_ID_MAX_LEN]; \
        \
        /** 原始订单(待撤销的订单)的客户委托流水号 (若使用 origClOrdId, 则不必填充该字段) */ \
        int32           origClSeqNo; \
        /** 原始订单(待撤销的订单)的客户端环境号 (小于等于0, 则使用当前会话的 clEnvId) */ \
        int8            origClEnvId; \
        /** 按64位对齐的填充域 */ \
        uint8           __ORD_CANCEL_BASE_INFO_filler2[3]; \
        /** 原始订单(待撤销的订单)的客户订单编号 (若使用 origClSeqNo, 则不必填充该字段) */ \
        int64           origClOrdId; \
        \
        /** 用户私有信息 (由客户端自定义填充, 并在回报数据中原样返回) */ \
        union { \
            uint64      u64;                /**< uint64 类型的用户私有信息 */ \
            int64       i64;                /**< int64 类型的用户私有信息 */ \
            uint32      u32[2];             /**< uint32[2] 类型的用户私有信息 */ \
            int32       i32[2];             /**< int32[2] 类型的用户私有信息 */ \
            char        c8[8];              /**< char[8] 类型的用户私有信息 */ \
        } userInfo


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_ORD_CANCEL_BASE_INFO_PKT          \
        0, 0, {0}, \
        {0}, {0}, \
        0, 0, {0}, 0, \
        {0}
/* -------------------------           */


/**
 * 附加到请求消息末尾的, 用于延迟测量的打点信息字段 (是否启用取决于构建配置)
 */
#define __OES_ORD_REQ_LATENCY_FIELDS_IMPL               \
        /** 委托请求的客户端原始发送时间 (OES内部使用, 由API在发送时自动填充) */ \
        STimeval32T     __ordReqOrigSendTime


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_ORD_REQ_LATENCY_FIELDS_IMPL       \
        {0, 0}
/* -------------------------           */


/**
 * 附加到回报消息末尾的, 用于延迟测量的打点信息字段 (是否启用取决于构建配置)
 */
#define __OES_ORD_CNFM_LATENCY_FIELDS_IMPL              \
        /** 委托请求的初始接收时间 */ \
        STimeval32T     __ordReqOrigRecvTime; \
        /** 委托请求的入队时间 */ \
        STimeval32T     __ordReqCollectedTime; \
        /** 委托请求的实际处理开始时间 */ \
        STimeval32T     __ordReqActualDealTime; \
        /** 委托请求的处理完成时间 */ \
        STimeval32T     __ordReqProcessedTime; \
        \
        /** 委托确认的开始采集时间 */ \
        STimeval32T     __ordCnfmOrigRecvTime; \
        /** 委托确认的采集完成时间 */ \
        STimeval32T     __ordCnfmCollectedTime; \
        /** 委托确认的实际处理开始时间 */ \
        STimeval32T     __ordCnfmActualDealTime; \
        /** 委托确认的处理完成时间 */ \
        STimeval32T     __ordCnfmProcessedTime; \
        \
        /** 初始报盘时间 */ \
        STimeval32T     __ordDeclareTime; \
        /** 报盘完成时间 */ \
        STimeval32T     __ordDeclareDoneTime; \
        \
        /** 消息推送时间 (写入推送缓存以后, 实际网络发送之前) */ \
        STimeval32T     __pushingTime


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_ORD_CNFM_LATENCY_FIELDS_IMPL      \
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
        {0, 0}, {0, 0}, \
        {0, 0}
/* -------------------------           */


#if defined (_OES_EXPORT_LATENCY_STATS)

/**
 * 委托请求中的时间戳字段定义 (用于记录打点信息以统计延迟)
 */
#   define  __OES_ORD_REQ_LATENCY_FIELDS                \
            ; \
            __OES_ORD_REQ_LATENCY_FIELDS_IMPL


/**
 * 委托回报中的时间戳字段定义 (用于记录打点信息以统计延迟)
 */
#   define  __OES_ORD_CNFM_LATENCY_FIELDS               \
            ; \
            __OES_ORD_CNFM_LATENCY_FIELDS_IMPL


/* 结构体的初始化值定义 */
#   define  __NULLOBJ_OES_ORD_REQ_LATENCY_FIELDS        \
            , \
            __NULLOBJ_OES_ORD_REQ_LATENCY_FIELDS_IMPL

#   define  __NULLOBJ_OES_ORD_CNFM_LATENCY_FIELDS       \
            , \
            __NULLOBJ_OES_ORD_CNFM_LATENCY_FIELDS_IMPL

#else

#   define  __OES_ORD_REQ_LATENCY_FIELDS
#   define  __OES_ORD_CNFM_LATENCY_FIELDS

#   define  __NULLOBJ_OES_ORD_REQ_LATENCY_FIELDS
#   define  __NULLOBJ_OES_ORD_CNFM_LATENCY_FIELDS

#endif  /* _OES_EXPORT_LATENCY_STATS */
/* -------------------------           */


/**
 * 委托确认基础信息的内容定义
 */
#define __OES_ORD_CNFM_BASE_INFO_PKT                    \
        __OES_ORD_BASE_INFO_PKT \
        __OES_ORD_REQ_LATENCY_FIELDS; \
        \
        /** 客户订单编号 (在OES内具有唯一性的内部委托编号) */ \
        int64           clOrdId; \
        /** 客户端编号 */ \
        int16           clientId; \
        /** 客户端环境号 */ \
        int8            clEnvId; \
        /** 原始订单(待撤销的订单)的客户端环境号 (仅适用于撤单委托) */ \
        int8            origClEnvId; \
        /** 原始订单(待撤销的订单)的客户委托流水号 (仅适用于撤单委托) */ \
        int32           origClSeqNo; \
        \
        /** 委托日期 (格式为 YYYYMMDD, 形如 20160830) */ \
        int32           ordDate; \
        /** 委托时间 (格式为 HHMMSSsss, 形如 141205000) */ \
        int32           ordTime; \
        /** 委托确认时间 (格式为 HHMMSSsss, 形如 141206000) */ \
        int32           ordCnfmTime; \
        \
        /** 订单当前状态 @see eOesOrdStatusT */ \
        uint8           ordStatus; \
        /** 委托确认状态 (交易所返回的回报状态，仅供参考)  @see eOesOrdStatusT */ \
        uint8           ordCnfmSts; \
        /** 证券类型 @see eOesSecurityTypeT */ \
        uint8           securityType; \
        /** 证券子类型 @see eOesSubSecurityTypeT */ \
        uint8           subSecurityType; \
        \
        /** 平台号 (OES内部使用) @see eOesPlatformIdT */ \
        uint8           __platformId; \
        /** 交易网关组序号 (OES内部使用) */ \
        uint8           __tgwGrpNo; \
        /** 已报盘标志 (OES内部使用) */ \
        uint8           __declareFlag; \
        /** 按64位对齐的填充域 */ \
        uint8           __ORD_CNFM_BASE_INFO_filler; \
        /** 交易所订单编号 (深交所的订单编号是16位的非数字字符串) */ \
        char            exchOrdId[OES_EXCH_ORDER_ID_MAX_LEN]; \
        \
        /** 委托当前冻结的交易金额 */ \
        int64           frzAmt; \
        /** 委托当前冻结的利息 */ \
        int64           frzInterest; \
        /** 委托当前冻结的交易费用 */ \
        int64           frzFee; \
        /** 委托累计已发生的交易金额 */ \
        int64           cumAmt; \
        /** 委托累计已发生的利息 */ \
        int64           cumInterest; \
        /** 委托累计已发生的交易费用 */ \
        int64           cumFee; \
        \
        /** 累计执行数量 (累计成交数量) */ \
        int32           cumQty; \
        /** 已撤单数量 */ \
        int32           canceledQty; \
        \
        /** 订单/撤单拒绝原因 */ \
        int32           ordRejReason; \
        /** 交易所错误码 */ \
        int32           exchErrCode; \
        /** PBU代码 (席位号) */ \
        int32           pbuId; \
        /** 营业部代码 */ \
        int32           branchId; \
        /** 回报记录号 (OES内部使用) */ \
        int32           __rowNum; \
        /** OIW委托编号 (OES内部使用) */ \
        uint32          __recNum \
        \
        __OES_ORD_CNFM_LATENCY_FIELDS


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_ORD_CNFM_BASE_INFO_PKT            \
        __NULLOBJ_OES_ORD_BASE_INFO_PKT \
        __NULLOBJ_OES_ORD_REQ_LATENCY_FIELDS, \
        0, 0, 0, 0, 0, \
        0, 0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, 0, {0}, \
        0, 0, 0, 0, 0, 0, \
        0, 0, \
        0, 0, 0, 0, 0, 0 \
        __NULLOBJ_OES_ORD_CNFM_LATENCY_FIELDS
/* -------------------------           */


/**
 * 委托请求的结构体定义
 */
typedef struct _OesOrdReq {
    __OES_ORD_BASE_INFO_PKT
    __OES_ORD_REQ_LATENCY_FIELDS;
} OesOrdReqT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_ORD_REQ                             \
        __NULLOBJ_OES_ORD_BASE_INFO_PKT \
        __NULLOBJ_OES_ORD_REQ_LATENCY_FIELDS
/* -------------------------           */


/**
 * 撤单请求的结构体定义
 */
typedef struct _OesOrdCancelReq {
    __OES_ORD_CANCEL_BASE_INFO_PKT
    __OES_ORD_REQ_LATENCY_FIELDS;
} OesOrdCancelReqT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_ORD_CANCEL_REQ                      \
        __NULLOBJ_OES_ORD_CANCEL_BASE_INFO_PKT \
        __NULLOBJ_OES_ORD_REQ_LATENCY_FIELDS
/* -------------------------           */


/**
 * 委托拒绝(OES业务拒绝)的结构体定义
 */
typedef struct _OesOrdReject {
    __OES_ORD_BASE_INFO_PKT
    __OES_ORD_REQ_LATENCY_FIELDS;

    /** 原始订单(待撤销的订单)的客户委托流水号 (仅适用于撤单请求) */
    int32               origClSeqNo;
    /** 原始订单(待撤销的订单)的客户端环境号 (仅适用于撤单请求) */
    int8                origClEnvId;
    /** 客户端环境号 */
    int8                clEnvId;
    /** 客户端编号 */
    int16               clientId;

    /** 委托日期 (格式为 YYYYMMDD, 形如 20160830) */
    int32               ordDate;
    /** 委托时间 (格式为 HHMMSSsss, 形如 141205000) */
    int32               ordTime;

    /** 订单拒绝原因 */
    int32               ordRejReason;
    /** 按64位对齐的填充域 */
    int32               __filler;
} OesOrdRejectT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_ORD_REJECT                          \
        __NULLOBJ_OES_ORD_BASE_INFO_PKT \
        __NULLOBJ_OES_ORD_REQ_LATENCY_FIELDS, \
        0, 0, 0, 0, \
        0, 0, \
        0, 0
/* -------------------------           */


/**
 * 委托确认的结构体定义
 */
typedef struct _OesOrdCnfm {
    __OES_ORD_CNFM_BASE_INFO_PKT;
} OesOrdCnfmT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_ORD_CNFM                            \
        __NULLOBJ_OES_ORD_CNFM_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 成交基础信息的结构体定义
 * =================================================================== */

/**
 * 成交基础信息的内容定义
 */
#define __OES_TRD_BASE_INFO_PKT                         \
        /** 交易所成交编号 (以下的6个字段是成交信息的联合索引字段) */ \
        int64           exchTrdNum; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8           mktId; \
        /** 买卖类型 (取值范围: 买/卖, 申购/赎回(仅深圳)) @see eOesBuySellTypeT */ \
        uint8           trdSide; \
        /** 平台号 (OES内部使用) @see eOesPlatformIdT */ \
        uint8           __platformId; \
        /** 成交类型 (OES内部使用) @see eOesEtfTrdCnfmTypeT */ \
        uint8           __trdCnfmType; \
        /** ETF成交回报顺序号 (OES内部使用), 为区分ETF成交记录而设置 (以订单为单位) */ \
        uint32          __etfTrdCnfmSeq; \
        \
        /** 股东账户代码 */ \
        char            invAcctId[OES_INV_ACCT_ID_MAX_LEN]; \
        /** 产品代码 */ \
        char            securityId[OES_SECURITY_ID_MAX_LEN]; \
        \
        /** 成交日期 (格式为 YYYYMMDD, 形如 20160830) */ \
        int32           trdDate; \
        /** 成交时间 (格式为 HHMMSSsss, 形如 141205000) */ \
        int32           trdTime; \
        /** 成交数量 */ \
        int32           trdQty; \
        /** 成交价格 (单位精确到元后四位, 即: 1元=10000) */ \
        int32           trdPrice; \
        /** 成交金额 (单位精确到元后四位, 即: 1元=10000) */ \
        int64           trdAmt; \
        \
        /** 客户订单编号 */ \
        int64           clOrdId; \
        /** 累计执行数量 */ \
        int32           cumQty; \
        /** 回报记录号 (OES内部使用) */ \
        int32           __rowNum; \
        \
        /** 交易网关组序号 (OES内部使用) */ \
        uint8           __tgwGrpNo; \
        /** 按64位对齐填充域 */ \
        uint8           __TRD_BASE_INFO_filler[3]; \
        /** 原始委托数量 */ \
        int32           origOrdQty; \
        \
        /** PBU代码 (席位号) */ \
        int32           pbuId; \
        /** 营业部代码 */ \
        int32           branchId


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_TRD_BASE_INFO_PKT                 \
        0, 0, 0, 0, 0, 0, \
        {0}, {0}, \
        0, 0, 0, 0, 0, \
        0, 0, 0, \
        0, {0}, 0, \
        0, 0
/* -------------------------           */


/**
 * 附加的用于延迟测量的打点信息字段, 是否启用取决于构建配置
 */
#define __OES_TRD_BASE_LATENCY_FIELDS                   \
        /** 成交确认的开始采集时间 */ \
        STimeval32T     __trdCnfmOrigRecvTime; \
        /** 成交确认的采集完成时间 */ \
        STimeval32T     __trdCnfmCollectedTime; \
        /** 成交确认的实际处理开始时间 */ \
        STimeval32T     __trdCnfmActualDealTime; \
        /** 成交确认的处理完成时间 */ \
        STimeval32T     __trdCnfmProcessedTime; \
        \
        /** 消息推送时间 (写入推送缓存以后, 实际网络发送之前) */ \
        STimeval32T     __pushingTime


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_TRD_BASE_LATENCY_FIELDS           \
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
        {0, 0}
/* -------------------------           */


#if defined (_OES_EXPORT_LATENCY_STATS)

/**
 * 成交回报中的时间戳字段定义 (用于记录打点信息以统计延迟)
 */
#   define  __OES_TRD_CNFM_LATENCY_FIELDS               \
            ; \
            __OES_TRD_BASE_LATENCY_FIELDS


/* 结构体的初始化值定义 */
#   define  __NULLOBJ_OES_TRD_CNFM_LATENCY_FIELDS       \
            , \
            __NULLOBJ_OES_TRD_BASE_LATENCY_FIELDS

#else

#   define  __OES_TRD_CNFM_LATENCY_FIELDS
#   define  __NULLOBJ_OES_TRD_CNFM_LATENCY_FIELDS

#endif  /* _OES_EXPORT_LATENCY_STATS */
/* -------------------------           */


/**
 * 成交回报信息的内容定义
 */
#define __OES_TRD_CNFM_BASE_INFO_PKT                    \
        __OES_TRD_BASE_INFO_PKT; \
        \
        /** 客户委托流水号 */ \
        int32           clSeqNo; \
        /** 客户端编号 */ \
        int16           clientId; \
        /** 客户端环境号 */ \
        int8            clEnvId; \
        /** 证券子类别 (为保持兼容而位置凌乱, 后续会做调整) @see eOesSubSecurityTypeT */ \
        uint8           subSecurityType; \
        \
        /** 订单当前状态 @see eOesOrdStatusT */ \
        uint8           ordStatus; \
        /** 订单类型 @see eOesOrdTypeShT eOesOrdTypeSzT */ \
        uint8           ordType; \
        /** 买卖类型 @see eOesBuySellTypeT */ \
        uint8           ordBuySellType; \
        /** 证券类型 @see eOesSecurityTypeT */ \
        uint8           securityType; \
        /** 原始委托价格, 单位精确到元后四位, 即1元 = 10000 */ \
        int32           origOrdPrice; \
        \
        /** 累计成交金额 */ \
        int64           cumAmt; \
        /** 累计成交利息 */ \
        int64           cumInterest; \
        /** 累计交易费用 */ \
        int64           cumFee; \
        \
        /** 用户私有信息 (由客户端自定义填充, 并在回报数据中原样返回) */ \
        union { \
            uint64      u64;                /**< uint64 类型的用户私有信息 */ \
            int64       i64;                /**< int64 类型的用户私有信息 */ \
            uint32      u32[2];             /**< uint32[2] 类型的用户私有信息 */ \
            int32       i32[2];             /**< int32[2] 类型的用户私有信息 */ \
            char        c8[8];              /**< char[8] 类型的用户私有信息 */ \
        } userInfo \
        \
        __OES_TRD_CNFM_LATENCY_FIELDS


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_TRD_CNFM_BASE_INFO_PKT            \
        __NULLOBJ_OES_TRD_BASE_INFO_PKT, \
        0, 0, 0, 0, \
        0, 0, 0, 0, 0, \
        0, 0, 0, \
        {0} \
        __NULLOBJ_OES_TRD_CNFM_LATENCY_FIELDS
/* -------------------------           */


/**
 * 成交基础信息的结构体定义
 */
typedef struct _OesTrdBaseInfo {
    __OES_TRD_BASE_INFO_PKT;
} OesTrdBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_TRD_BASE_INFO                       \
        __NULLOBJ_OES_TRD_BASE_INFO_PKT
/* -------------------------           */


/**
 * 成交回报结构体定义
 */
typedef struct _OesTrdCnfm {
    __OES_TRD_CNFM_BASE_INFO_PKT;
} OesTrdCnfmT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_TRD_CNFM                            \
        __NULLOBJ_OES_TRD_CNFM_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 新股配号、中签记录信息 (OesLotWinningBaseInfo) 定义
 * =================================================================== */

/**
 * 新股配号、中签记录信息的内容定义
 */
#define __OES_LOT_WINNING_BASE_INFO_PKT                 \
        /** 证券账户 */ \
        char            invAcctId[OES_INV_ACCT_ID_MAX_LEN]; \
        /** 配号代码/中签代码 */ \
        char            securityId[OES_SECURITY_ID_MAX_LEN]; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8           mktId; \
        /** 记录类型 @see eOesLotTypeT */ \
        uint8           lotType; \
        /**
         * 失败原因, 当且仅当 lotType 为 OES_LOT_TYPE_FAILED 时此字段有效
         * @see eOesLotRejReasonT
         */ \
        uint8           rejReason; \
        /** 按64位对齐填充域 */ \
        int8            __LOT_WINNING_BASE_INFO_filler; \
        /** 配号日期/中签日期 (格式为 YYYYMMDD, 形如 20160830) */ \
        int32           lotDate; \
        \
        /** 证券名称 */ \
        char            securityName[OES_SECURITY_NAME_MAX_LEN]; \
        \
        /** 配号首个号码。当为中签记录时此字段固定为0 */ \
        int64           assignNum; \
        /** 配号成功数量/中签股数 */ \
        int32           lotQty; \
        /** 最终发行价, 单位精确到元后四位, 即1元 = 10000。当为配号记录时此字段值固定为0 */ \
        int32           lotPrice; \
        /** 中签金额, 单位精确到元后四位, 即1元 = 10000。当为配号记录时此字段值固定为0 */ \
        int64           lotAmt


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_LOT_WINNING_BASE_INFO_PKT         \
        {0}, {0}, \
        0, 0, 0, 0, 0, \
        {0}, \
        0, 0, 0, 0
/* -------------------------           */


/**
 * 新股配号、中签记录信息定义
 */
typedef struct _OesLotWinningBaseInfo {
    __OES_LOT_WINNING_BASE_INFO_PKT;
} OesLotWinningBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_LOT_WINNING_BASE_INFO               \
        __NULLOBJ_OES_LOT_WINNING_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 出入金信息的结构体定义
 * =================================================================== */

/**
 * 出入金委托基础信息的内容定义
 */
#define __OES_FUND_TRSF_BASE_INFO_PKT                   \
        /** 客户委托流水号 (由客户端维护的递增流水) */ \
        int32           clSeqNo; \
        /** 划转方向 @see eOesFundTrsfDirectT */ \
        uint8           direct; \
        /** 出入金转账类型 @see eOesFundTrsfTypeT*/ \
        uint8           isAllotOnly; \
        /** 按64位对齐填充域 */ \
        uint8           __FUND_TRSF_BASE_filler[2]; \
        \
        /** 资金账户代码 */ \
        char            cashAcctId[OES_CASH_ACCT_ID_MAX_LEN]; \
        /** 交易密码 */ \
        char            trdPasswd[OES_PWD_MAX_LEN]; \
        /**
         * 转账密码(转账方向为转入(银行转证券), 此密码为银行密码.
         * 转账方向为转出(证券转银行), 此密码为资金密码
         */ \
        char            trsfPasswd[OES_PWD_MAX_LEN]; \
        \
        /** 发生金额 (都是正数), 单位精确到元后四位, 即1元 = 10000 */ \
        int64           occurAmt; \
        \
        /** 用户私有信息 (由客户端自定义填充, 并在回报数据中原样返回) */ \
        union { \
            uint64      u64;                /**< uint64 类型的用户私有信息 */ \
            int64       i64;                /**< int64 类型的用户私有信息 */ \
            uint32      u32[2];             /**< uint32[2] 类型的用户私有信息 */ \
            int32       i32[2];             /**< int32[2] 类型的用户私有信息 */ \
            char        c8[8];              /**< char[8] 类型的用户私有信息 */ \
        } userInfo


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_FUND_TRSF_BASE_INFO_PKT           \
        0, 0, 0, {0}, \
        {0}, {0}, {0}, \
        0, {0}
/* -------------------------           */


/**
 * 出入金委托的基础信息结构体定义
 */
typedef struct _OesFundTrsfBaseInfo {
    __OES_FUND_TRSF_BASE_INFO_PKT;
} OesFundTrsfBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_FUND_TRSF_BASE_INFO                 \
        __NULLOBJ_OES_FUND_TRSF_BASE_INFO_PKT
/* -------------------------           */


/**
 * 出入金请求定义
 */
typedef struct _OesFundTrsfReq {
    __OES_FUND_TRSF_BASE_INFO_PKT;
} OesFundTrsfReqT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_FUND_TRSF_REQ                       \
        __NULLOBJ_OES_FUND_TRSF_BASE_INFO_PKT
/* -------------------------           */


/**
 * 出入金拒绝的回报结构定义 (因风控检查未通过而被OES拒绝)
 */
typedef struct _OesFundTrsfReject {
    __OES_FUND_TRSF_BASE_INFO_PKT;

    /** 委托日期 (格式为 YYYYMMDD, 形如 20160830) */
    int32               ordDate;
    /** 委托时间 (格式为 HHMMSSsss, 形如 141205000) */
    int32               ordTime;

    /** 客户端编号 */
    int16               clientId;
    /** 客户端环境号 */
    int8                clEnvId;
    /** 64位对齐的填充域 */
    int8                __filler;
    /** 错误码 */
    int32               rejReason;

    /** 错误信息 */
    char                errorInfo[OES_MAX_ERROR_INFO_LEN];
} OesFundTrsfRejectT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_FUND_TRSF_REJECT                    \
        __NULLOBJ_OES_FUND_TRSF_BASE_INFO_PKT, \
        0, 0, \
        0, 0, 0, 0, {0}
/* -------------------------           */


/**
 * 出入金委托执行状态回报的结构体定义
 */
typedef struct _OesFundTrsfReport {
    /** 客户委托流水号 (由客户端维护的递增流水) */
    int32               clSeqNo;
    /** 客户端编号 */
    int16               clientId;
    /** 客户端环境号 */
    int8                clEnvId;
    /** 划转方向 @see eOesFundTrsfDirectT */
    uint8               direct;
    /** 资金账户代码 */
    char                cashAcctId[OES_CASH_ACCT_ID_MAX_LEN];

    /** 发生金额 (都是正数), 单位精确到元后四位, 即1元 = 10000 */
    int64               occurAmt;
    /** 用户私有信息 (由客户端自定义填充, 并在回报数据中原样返回) */
    union {
        uint64          u64;                /**< uint64 类型的用户私有信息 */
        int64           i64;                /**< int64 类型的用户私有信息 */
        uint32          u32[2];             /**< uint32[2] 类型的用户私有信息 */
        int32           i32[2];             /**< int32[2] 类型的用户私有信息 */
        char            c8[8];              /**< char[8] 类型的用户私有信息 */
    } userInfo;

    /** OES出入金委托编号 (在OES内具有唯一性的内部出入金委托编号) */
    int32               fundTrsfId;
    /** 柜台出入金委托编号 */
    int32               counterEntrustNo;

    /** 出入金委托日期 (格式为 YYYYMMDD, 形如 20160830) */
    int32               operDate;
    /** 出入金委托时间 (格式为 HHMMSSsss, 形如 141205000) */
    int32               operTime;
    /** 上报柜台时间 (格式为 HHMMSSsss, 形如 141205000) */
    int32               dclrTime;
    /** 柜台执行结果采集时间 (格式为 HHMMSSsss, 形如 141205000) */
    int32               doneTime;

    /** 出入金转账类型 @see eOesFundTrsfTypeT*/
    uint8               isAllotOnly;
    /** 出入金委托执行状态 @see eOesFundTrsfStatusT */
    uint8               trsfStatus;
    /** 是否有转账到主柜 */
    uint8               __hasCounterTransfered;
    /** 按64位对齐填充域 */
    uint8               __filler;

    /** 错误原因 */
    int32               rejReason;
    /** 主柜错误码 */
    int32               counterErrCode;
    /** 按64位对齐填充域 */
    uint32              __filler2;
    /** 资金调拨流水号 */
    char                allotSerialNo[OES_MAX_ALLOT_SERIALNO_LEN];
    /** 错误信息 */
    char                errorInfo[OES_MAX_ERROR_INFO_LEN];
} OesFundTrsfReportT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_FUND_TRSF_REPORT                    \
        0, 0, 0, 0, {0}, \
        0, {0}, \
        0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, {0}, {0}
/* -------------------------           */


/* ===================================================================
 * 证券发行信息的结构体定义
 * =================================================================== */

/**
 * 证券发行基础信息的内容定义
 */
#define __OES_ISSUE_BASE_INFO_PKT                       \
        /** 证券发行代码 */ \
        char                securityId[OES_SECURITY_ID_MAX_LEN]; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8               mktId; \
        \
        /** 证券类型 @see eOesSecurityTypeT */ \
        uint8               securityType; \
        /** 证券子类型 @see eOesSubSecurityTypeT */ \
        uint8               subSecurityType; \
        /** 是否允许撤单 */ \
        uint8               isCancelAble; \
        /** 是否允许重复认购 */ \
        uint8               isReApplyAble; \
        /** 按64位对齐填充域 */ \
        uint8               __ISSUE_BASE_filler[3]; \
        \
        /** 证券名称 */ \
        char                securityName[OES_SECURITY_NAME_MAX_LEN]; \
        /** 证券代码 (正股代码) */ \
        char                underlyingSecurityId[OES_SECURITY_ID_MAX_LEN]; \
        \
        /** 发行起始日 */ \
        int32               startDate; \
        /** 发行结束日 */ \
        int32               endDate; \
        /** 总发行量 */ \
        int64               issueQty; \
        \
        /** 委托份数单位 */ \
        int32               qtyUnit; \
        /** 委托最大份数 */ \
        int32               ordMaxQty; \
        /** 委托最小份数 */ \
        int32               ordMinQty; \
        \
        /** 发行价格 */ \
        int32               issuePrice; \
        /** 申购价格区间上限 */ \
        int32               ceilPrice; \
        /** 申购价格区间下限 */ \
        int32               floorPrice



#define __NULLOBJ_OES_ISSUE_BASE_INFO_PKT               \
        {0}, 0, \
        0, 0, 0, 0, {0}, \
        {0}, {0}, \
        0, 0, 0, \
        0, 0, 0, \
        0, 0, 0
/* -------------------------           */


/**
 * 证券发行基础信息的结构体定义
 */
typedef struct _OesIssueBaseInfo {
    __OES_ISSUE_BASE_INFO_PKT;
} OesIssueBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_ISSUE_BASE_INFO                     \
        __NULLOBJ_OES_ISSUE_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 现货产品基础信息的结构体定义
 * =================================================================== */

/**
 * OES竞价的限价参数配置
 */
typedef struct _OesPriceLimit {
    /** 上涨限价, 单位精确到元后四位, 即1元 = 10000 */
    int32               ceilPrice;
    /** 下跌限价, 单位精确到元后四位, 即1元 = 10000 */
    int32               floorPrice;
} OesPriceLimitT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_PRICE_LIMIT                         \
        0, 0
/* -------------------------           */


/**
 * 现货产品基础信息的内容定义
 */
#define __OES_STOCK_BASE_INFO_PKT                       \
        /** 产品代码 */ \
        char                securityId[OES_SECURITY_ID_MAX_LEN]; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8               mktId; \
        \
        /** 证券类型 @see eOesSecurityTypeT */ \
        uint8               securityType; \
        /** 证券子类型 @see eOesSubSecurityTypeT */ \
        uint8               subSecurityType; \
        /** 产品级别 @see eOesSecurityLevelT */ \
        uint8               securityLevel; \
        /** 产品风险等级 @see eOesSecurityRiskLevelT */ \
        uint8               securityRiskLevel; \
        /** 币种 @see eOesCurrTypeT */ \
        uint8               currType; \
        /** 投资者适当性管理分类 @see eOesQualificationClassT */ \
        uint8               qualificationClass; \
        /** 是否支持当日回转交易 0: 不支持; 其他: 支持 */ \
        uint8               isDayTrading; \
        \
        /** 连续停牌标识 @see eOesSecuritySuspFlagT */ \
        uint8               suspFlag; \
        /** 临时停牌标识 (TRUE 已停牌, FALSE 未停牌) */ \
        uint8               temporarySuspFlag; \
        /** 填充字符  满足64位对齐 */ \
        uint8               __filter[6]; \
        \
        /** 买入单位 */ \
        int32               buyQtyUnit; \
        /** 卖出单位 */ \
        int32               sellQtyUnit; \
        /** 单笔买委托最大份数 */ \
        int32               buyOrdMaxQty; \
        /** 单笔买委托最小份数 */ \
        int32               buyOrdMinQty; \
        /** 单笔卖委托最大份数 */ \
        int32               sellOrdMaxQty; \
        /** 单笔卖委托最小份数 */ \
        int32               sellOrdMinQty; \
        \
        /** 最小价格变动单位, 单位精确到元后四位, 即1元 = 10000 */ \
        int32               priceUnit; \
        /** 昨日收盘价，单位精确到元后四位，即1元 = 10000 */ \
        int32               prevClose; \
        /** 面值, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               parPrice; \
        /** 债券的每百元应计利息额, 单位精确到元后八位 */ \
        int64               bondInterest; \
        \
        /** 逆回购期限 */ \
        int32               repoExpirationDays; \
        /** 占款天数 */ \
        int32               cashHoldDays; \
        \
        /** 竞价限价参数表, 数组下标为当前时段标志 @see eOesTrdSessTypeT */ \
        OesPriceLimitT      priceLimit[__OES_TRD_SESS_TYPE_MAX]; \
        \
        /** 产品名称 */ \
        char                securityName[OES_SECURITY_NAME_MAX_LEN]; \
        /** ETF申购赎代码 */ \
        char                fundId[OES_SECURITY_ID_MAX_LEN]


#define __NULLOBJ_OES_STOCK_BASE_INFO_PKT               \
        {0}, 0, \
        0, 0, 0, 0, 0, 0, 0, \
        0, 0, {0}, \
        0, 0, 0, 0, 0, 0, \
        0, 0, 0, 0, \
        0, 0, \
        {{NULLOBJ_OES_PRICE_LIMIT}}, \
        {0}, {0}
/* -------------------------           */


/**
 * 现货产品基础信息的结构体定义
 */
typedef struct _OesStockBaseInfo {
    __OES_STOCK_BASE_INFO_PKT;
} OesStockBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_STOCK_BASE_INFO                     \
        __NULLOBJ_OES_STOCK_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * Etf申赎产品基础信息的结构体定义
 * =================================================================== */

/**
 * Etf申赎产品基础信息定义
 */
#define __OES_ETF_BASE_INFO_PKT                         \
        /** Etf申赎代码 */ \
        char                fundId[OES_SECURITY_ID_MAX_LEN]; \
        /** Etf买卖代码 */ \
        char                securityId[OES_SECURITY_ID_MAX_LEN]; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8               mktId; \
        \
        /** 证券类型 @see eOesSecurityTypeT */ \
        uint8               securityType; \
        /** 证券子类型 @see eOesSubSecurityTypeT */ \
        uint8               subSecurityType; \
        /** 是否需要发布IOPV  1: 是; 0: 否 */ \
        uint8               isPublishIOPV; \
        \
        /** 交易所/基金公司的允许申购标志  1: 是; 0: 否 */ \
        uint8               isCreationAble; \
        /** 交易所/基金公司的允许赎回标志  1: 是; 0: 否 */ \
        uint8               isRedemptionAble; \
        /** 券商管理端的禁止交易标志  1: 是; 0: 否  */ \
        uint8               isDisabled; \
        /** 按64位对齐填充域 */ \
        uint8               __ETF_BASE_filler; \
        \
        /** 成分证券数目 */ \
        int32               componentCnt; \
        /** 每个篮子 (最小申购、赎回单位) 对应的ETF份数, 即申购赎回单位 */ \
        int32               creRdmUnit; \
        /** 最大现金替代比例, 精确到0.00001(=0.001%) */ \
        int32               maxCashRatio; \
        /** 前一日基金的单位净值 */ \
        int32               nav; \
        \
        /** 前一日最小申赎单位净值 */ \
        int64               navPerCU; \
        /** 红利金额 */ \
        int64               dividendPerCU; \
        \
        /** 当前交易日, 格式YYYYMMDD */ \
        int32               tradingDay; \
        /** 前一交易日, 格式YYYYMMDD */ \
        int32               preTradingDay; \
        /** 每个篮子的预估现金差额 */ \
        int64               estiCashCmpoent; \
        /** 前一日现金差额 */ \
        int64               cashCmpoent; \
        /** 当日申购限额 */ \
        int64               creationLimit; \
        /** 当日赎回限额 */ \
        int64               redemLimit; \
        /** 单个账户净申购总额限制 */ \
        int64               netCreationLimit; \
        /** 单个账户净赎回总额限制 */ \
        int64               netRedemLimit


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_ETF_BASE_INFO_PKT                 \
        {0}, {0}, 0, \
        0, 0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, 0, \
        0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, 0
/* -------------------------           */


/**
 * ETF申赎产品基础信息的结构体定义
 */
typedef struct _OesEtfBaseInfo {
    __OES_ETF_BASE_INFO_PKT;
} OesEtfBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_ETF_BASE_INFO                       \
        __NULLOBJ_OES_ETF_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * Etf成分股基础信息的结构体定义
 * =================================================================== */

/**
 * Etf成分股基础信息定义
 */
#define __OES_ETF_COMPONENT_BASE_INFO_PKT               \
        /** 成分股所属ETF的基金代码 */ \
        char                fundId[OES_SECURITY_ID_MAX_LEN]; \
        /** Etf成分股代码 */ \
        char                securityId[OES_SECURITY_ID_MAX_LEN]; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8               mktId; \
        /** 现金替代标识 @see eOesEtfSubFlagT */ \
        uint8               subFlag; \
        /** 证券类型 @see eOesSecurityTypeT */ \
        uint8               securityType; \
        /** 证券子类型 @see eOesSubSecurityTypeT */ \
        uint8               subSecurityType; \
        \
        /** 昨日收盘价格 */ \
        int32               prevClose; \
        /** 成分证券数量 */ \
        int32               qty; \
        /** 溢价比例 */ \
        int32               premiumRate; \
        \
        /** 申购替代金额 */ \
        int64               creationSubCash; \
        /** 赎回替代金额 */ \
        int64               redemptionCashSub


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_ETF_COMPONENT_BASE_INFO_PKT       \
        {0}, {0}, \
        0, 0, 0, 0, \
        0, 0, 0, \
        0, 0
/* -------------------------           */


/**
 * Etf成分股基础信息的结构体定义
 */
typedef struct _OesEtfComponentBaseInfo {
    __OES_ETF_COMPONENT_BASE_INFO_PKT;
} OesEtfComponentBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_ETF_COMPONENT_BASE_INFO             \
        __NULLOBJ_OES_ETF_COMPONENT_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 期权产品基础信息的结构体定义
 * =================================================================== */

/**
 * 期权产品基础信息的内容定义
 */
#define __OES_OPTION_BASE_INFO_PKT          \
        /** 期权合约代码 */ \
        char                securityId[OES_SECURITY_ID_MAX_LEN]; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8               mktId; \
        \
        /** 证券类型 @see eOesSecurityTypeT */ \
        uint8               securityType; \
        /** 证券子类型 @see eOesSubSecurityTypeT */ \
        uint8               subSecurityType; \
        /** 合约类型 @see eOesOptContractTypeT */ \
        uint8               contractType; \
        /** 开仓标识 @see eOesOptionOpenFlagT */ \
        uint8               openFlag; \
        /** 行权类型 @see eOesOptionTypeT */ \
        uint8               optionType; \
        /** 连续停牌标识 @see eOesSecuritySuspFlagT */ \
        uint8               suspFlag; \
        /** 临时停牌标识 (TRUE 已停牌, FALSE 未停牌) */ \
        uint8               temporarySuspFlag; \
        \
        /** 经过除权除息调整后的合约单位 */ \
        int32               contractMultiplierUnit; \
        /** 经过除权除息调整后的期权行权价, 单位精确到元后四位, 即1元 = 10000 */ \
        int32               excPrice; \
        \
        /** 期权首个交易日, 格式为YYYYMMDD */ \
        int32               startDate; \
        /** 期权最后交易/行权日, 格式为YYYYMMDD */ \
        int32               endDate; \
        /** 期权行权日, 格式为YYYYMMDD */ \
        int32               execDate; \
        /** 期权到期日, 格式为YYYYMMDD */ \
        int32               expDate; \
        \
        /** 合约前收盘价, 单位精确到元后四位, 即1元 = 10000 */ \
        int32               closePrice; \
        /** 合约前结算价, 单位精确到元后四位, 即1元 = 10000 */ \
        int32               settlPrice; \
        /** 一手等于几张合约 */ \
        int32               roundLot; \
        /** 最小报价单位 */ \
        int32               tickSize; \
        /** 涨停价, 单位精确到元后四位, 即1元 = 10000 */ \
        int32               ceilPrice; \
        /** 跌停价, 单位精确到元后四位, 即1元 = 10000 */ \
        int32               floorPrice; \
        \
        /** 当日持有一张合约所需保证金, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               marginUnit; \
        /** 未平仓合约张数 */ \
        int64               leavesQty; \
        \
        /** 单笔限价申报的申报张数上限 */ \
        int32               lmtOrdMaxQty; \
        /** 单笔限价申报的申报张数下限 */ \
        int32               lmtOrdMinQty; \
        /** 单笔市价申报的申报张数上限 */ \
        int32               mktOrdMaxQty; \
        /** 单笔市价申报的申报张数下限 */ \
        int32               mktOrdMinQty; \
        \
        /* 标的证券代码 */ \
        char                underlyingSecurityId[OES_SECURITY_ID_MAX_LEN]; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8               underlyingMktId; \
        /** 证券类型 @see eOesSecurityTypeT */ \
        uint8               underlyingSecuType; \
        /** 临近到期日期  1: 临近到期日; 0: 非临近到期日 */ \
        uint8               closeEndDateFlag; \
        /** 按64位对齐填充域 */ \
        uint8               __OPTION_BASE_filler1; \
        /** 标的前收盘价, 单位精确到元后四位, 即1元 = 10000 */ \
        int32               underlyingClosePrice; \
        \
        /** 期权合约名称 */ \
        char                securityName[OES_SECURITY_NAME_MAX_LEN]


#define __NULLOBJ_OES_OPTION_BASE_INFO_PKT              \
        {0}, 0, \
        0, 0, 0, 0, 0, 0, 0, \
        0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, 0, 0, 0, \
        0, 0, \
        0, 0, 0, 0, \
        {0}, 0, 0, 0, 0, 0, \
        {0}
/* -------------------------           */


/**
 * 期权产品基础信息的结构体定义
 */
typedef struct _OesOptionBaseInfo {
    __OES_OPTION_BASE_INFO_PKT;
} OesOptionBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_OPTION_BASE_INFO                    \
        __NULLOBJ_OES_OPTION_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 客户资金信息结构体定义
 * =================================================================== */

/**
 * 客户资金信息内容定义
 */
#define __OES_CASH_ASSET_BASE_INFO_PKT                  \
        /** 资金账户代码 */ \
        char                cashAcctId[OES_CASH_ACCT_ID_MAX_LEN]; \
        /** 客户代码 */ \
        char                custId[OES_CUST_ID_MAX_LEN]; \
        \
        /** 币种 @see eOesCurrTypeT */ \
        uint8               currType; \
        /** 资金帐户类别(冗余自资金账户) @see eOesCashTypeT */ \
        uint8               cashType; \
        /** 资金帐户状态(冗余自资金账户) @deprecated @see eOesAcctStatusT */ \
        uint8               cashAcctStatus; \
        /** 是否禁止出入金 (仅供API查询使用) */ \
        uint8               isFundTrsfDisabled; \
        /** 按64位对齐的填充域 */ \
        uint8               __CASH_ASSET_BASE_filler[4]; \
        \
        /** 期初余额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               beginningBal; \
        /** 期初可用余额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               beginningAvailableBal; \
        /** 期初可取余额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               beginningDrawableBal; \
        \
        /** 不可用资金余额(既不可交易又不可提取), 单位精确到元后四位, 即1元 = 10000 */ \
        int64               disableBal; \
        /** 当前冲正金额(红冲蓝补的资金净额), 取值可以为负数(表示资金调出), 单位精确到元后四位(即1元 = 10000) */ \
        int64               reversalAmt; \
        /** 手动冻结资金, 取值在0和当前资产之间, 单位精确到元后四位(即1元 = 10000) */ \
        int64               manualFrzAmt; \
        \
        /** 日中累计存入资金金额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               totalDepositAmt; \
        /** 日中累计提取资金金额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               totalWithdrawAmt; \
        /** 当前提取冻结资金金额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               withdrawFrzAmt; \
        \
        /** 日中累计卖获得资金金额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               totalSellAmt; \
        /** 日中累计 买/申购/逆回购 使用资金金额，单位精确到元后四位，即1元 = 10000 */ \
        int64               totalBuyAmt; \
        /** 当前交易冻结金额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               buyFrzAmt; \
        \
        /** 日中累计交易费用金额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               totalFeeAmt; \
        /** 当前冻结交易费用金额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               feeFrzAmt; \
        \
        /** 当前维持的保证金金额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               marginAmt; \
        /** 当前冻结的保证金金额, 单位精确到元后四位, 即1元 = 10000 */ \
        int64               marginFrzAmt


#define __NULLOBJ_OES_CASH_ASSET_BASE_INFO_PKT          \
        {0}, {0}, \
        0, 0, 0, 0, {0}, \
        0, 0, 0, \
        0, 0, 0, \
        0, 0, 0, \
        0, 0, 0, \
        0, 0, \
        0, 0
/* -------------------------           */


/**
 * 客户资金基础信息结构体定义
 */
typedef struct _OesCashAssetBaseInfo {
    __OES_CASH_ASSET_BASE_INFO_PKT;
} OesCashAssetBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_CASH_ASSET_BASE_INFO                \
        __NULLOBJ_OES_CASH_ASSET_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 客户基础信息的结构体定义
 * =================================================================== */

/**
 * 客户基础信息的内容定义
 */
#define __OES_CUST_BASE_INFO_PKT                        \
        /** 客户代码 */ \
        char                custId[OES_CUST_ID_MAX_LEN]; \
        \
        /** 客户类型 */ \
        uint8               custType; \
        /** 客户状态 0 正常, 非0 不正常 */ \
        uint8               status; \
        /** OES风险等级 @see eOesSecurityRiskLevelT */ \
        uint8               riskLevel; \
        /** 客户原始风险等级 */ \
        uint8               originRiskLevel; \
        /** 机构标志 (TRUE 机构投资者, FALSE 个人投资者) */ \
        uint8               institutionFlag; \
        /** 投资者分类 @see eOesInvestorClassT */ \
        uint8               investorClass; \
        /** 按64位对齐填充域 */ \
        uint8               __CUST_BASE_filler1[2]; \
        \
        /** 营业部代码 */ \
        int32               branchId; \
        /** 按64位对齐填充域 */ \
        uint32              __CUST_BASE_filler2


#define __NULLOBJ_OES_CUST_BASE_INFO_PKT                \
        {0}, \
        0, 0, 0, 0, 0, 0, {0}, \
        0, 0
/* -------------------------           */


/**
 * 客户基础信息的结构体定义
 */
typedef struct _OesCustBaseInfo {
    __OES_CUST_BASE_INFO_PKT;
} OesCustBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_CUST_BASE_INFO                      \
        __NULLOBJ_OES_CUST_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 证券账户基础信息的结构体定义
 * =================================================================== */

/**
 * 证券账户基础信息的内容定义
 */
#define __OES_INV_ACCT_BASE_INFO_PKT                    \
        /** 股东账户代码 */ \
        char                invAcctId[OES_INV_ACCT_ID_MAX_LEN]; \
        /** 市场 @see eOesMarketIdT */ \
        uint8               mktId; \
        \
        /** 账户类型 @see eOesAcctTypeT */ \
        uint8               acctType; \
        /** 账户状态 @see eOesAcctStatusT */ \
        uint8               status; \
        /** 股东账户的所有者类型 @see eOesOwnerTypeT */ \
        uint8               ownerType; \
        /** 期权投资者级别 @see eOesOptInvLevelT */ \
        uint8               optInvLevel; \
        /** 是否禁止交易 (仅供API查询使用) */ \
        uint8               isTradeDisabled; \
        /** 按64位对齐填充域 */ \
        uint8               __INV_ACCT_BASE_filler[2]; \
        \
        /** 证券账户权限限制 @see eOesTradingLimitT */ \
        uint64              limits; \
        /** 股东权限/客户权限 @see eOesTradingPermissionT */ \
        uint64              permissions; \
        \
        /** 席位号 */ \
        int32               pbuId; \
        /** 新股认购限额 */ \
        int32               subscriptionQuota


#define __NULLOBJ_OES_INV_ACCT_BASE_INFO_PKT            \
        {0}, 0, \
        0, 0, 0, 0, 0, {0}, \
        0, 0, \
        0, 0
/* -------------------------           */


/**
 * 证券账户基础信息的结构体定义
 */
typedef struct _OesInvAcctBaseInfo {
    __OES_INV_ACCT_BASE_INFO_PKT;
} OesInvAcctBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_INV_ACCT_BASE_INFO                  \
        __NULLOBJ_OES_INV_ACCT_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 股票持仓信息的结构体定义
 * =================================================================== */

/**
 * 股票持仓基础信息的内容定义
 */
#define __OES_STK_HOLDING_BASE_INFO_PKT                 \
        /** 账户代码 */ \
        char                invAcctId[OES_INV_ACCT_ID_MAX_LEN]; \
        /** 产品代码 */ \
        char                securityId[OES_SECURITY_ID_MAX_LEN]; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8               mktId; \
        /** 证券类型 @see eOesSecurityTypeT */ \
        uint8               securityType; \
        /** 证券子类型 @see eOesSubSecurityTypeT */ \
        uint8               subSecurityType; \
        /** 按64位对齐的填充域 */ \
        uint8               __HOLD_BASE_filler[5]; \
        \
        /** 日初持仓 */ \
        int64               originalHld; \
        /** 日初总持仓成本 (日初持仓成本价 = 日初总持仓成本 / 日初持仓) */ \
        int64               originalCostAmt; \
        \
        /** 日中累计买入持仓 */ \
        int64               totalBuyHld; \
        /** 日中累计卖出持仓 */ \
        int64               totalSellHld; \
        /** 当前卖出冻结持仓 */ \
        int64               sellFrzHld; \
        /** 手动冻结持仓 */ \
        int64               manualFrzHld; \
        \
        /** 日中累计买入金额 */ \
        int64               totalBuyAmt; \
        /** 日中累计卖出金额 */ \
        int64               totalSellAmt; \
        /** 日中累计买入费用 */ \
        int64               totalBuyFee; \
        /** 日中累计卖出费用 */ \
        int64               totalSellFee; \
        \
        /**
         * 日中累计转换获得持仓, ETF申赎业务使用
         * - 成分股持仓场景, 转换获得指赎回时获得的成分股持仓;
         * - ETF证券持仓场景, 转换获得指申购时获得的ETF证券股持仓;
         */ \
        int64               totalTrsfInHld; \
        /**
         * 日中累计转换付出持仓, ETF申赎业务使用
         * - 成分股持仓场景, 转换付出指申购时使用的成分股持仓;
         * - ETF证券持仓场景, 转换付出指赎回时使用的ETF证券股持仓;
         */ \
        int64               totalTrsfOutHld; \
        /** 当前转换付出冻结持仓 */ \
        int64               trsfOutFrzHld; \
        \
        /** 当前已锁定持仓 */ \
        int64               lockHld; \
        /** 当前锁定冻结持仓数量 */ \
        int64               lockFrzHld; \
        /** 当前解锁定冻结持仓数量 */ \
        int64               unlockFrzHld; \
        \
        /** 当前备兑冻结的现货持仓数量 */ \
        int64               coveredFrzHld; \
        /** 当前已备兑使用的现货持仓数量 */ \
        int64               coveredHld


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_STK_HOLDING_BASE_INFO_PKT         \
        {0}, {0}, \
        0, 0, 0, {0}, \
        0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, \
        0, 0, 0, \
        0, 0
/* -------------------------           */


/**
 * 股票持仓基础信息的结构体定义
 */
typedef struct _OesStkHoldingBaseInfo {
    __OES_STK_HOLDING_BASE_INFO_PKT;
} OesStkHoldingBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_STK_HOLDING_BASE_INFO               \
        __NULLOBJ_OES_STK_HOLDING_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 期权持仓信息的结构体定义
 * =================================================================== */

/**
 * 期权持仓基础信息的内容定义
 */
#define __OES_OPT_HOLDING_BASE_INFO_PKT                 \
        /** 账户代码 */ \
        char                invAcctId[OES_INV_ACCT_ID_MAX_LEN]; \
        /** 产品代码 */ \
        char                securityId[OES_SECURITY_ID_MAX_LEN]; \
        /** 市场代码 @see eOesMarketIdT */ \
        uint8               mktId; \
        /** 按64位对齐的填充域 */ \
        uint8               __HOLD_BASE_filler[7]; \
        \
        /** 权利仓 */ \
        int64               hldA; \
        /** 义务仓 */ \
        int64               hldB; \
        /** 当日备兑仓 */ \
        int64               hldC; \
        /** 非当日备兑仓 */ \
        int64               hldC2; \
        /** 权利仓冻结 */ \
        int64               hldRA; \
        /** 义务仓冻结 */ \
        int64               hldRB; \
        /** 备兑仓冻结 */ \
        int64               hldRC


/* 结构体的初始化值定义 */
#define __NULLOBJ_OES_OPT_HOLDING_BASE_INFO_PKT         \
        {0}, {0}, 0, {0}, \
        0, 0, 0, \
        0, 0, 0, 0
/* -------------------------           */


/**
 * 期权持仓基础信息的结构体定义
 */
typedef struct _OesOptHoldingBaseInfo {
    __OES_OPT_HOLDING_BASE_INFO_PKT;
} OesOptHoldingBaseInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_OPT_HOLDING_BASE_INFO               \
        __NULLOBJ_OES_OPT_HOLDING_BASE_INFO_PKT
/* -------------------------           */


/* ===================================================================
 * 市场状态信息的结构体定义
 * =================================================================== */

/**
 * 市场状态信息的结构体定义
 */
typedef struct _OesMarketStateInfo {
    uint8               exchId;             /**< 交易所代码 @see eOesExchangeIdT */
    uint8               platformId;         /**< 交易平台类型 @see eOesPlatformIdT */
    uint8               mktId;              /**< 市场代码 @see eOesMarketIdT */
    uint8               mktState;           /**< 市场状态 @see eOesMarketStatusT */
    uint8               __filler[4];        /**< 按64位对齐的填充域 */
} OesMarketStateInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_MARKET_STATE_INFO                   \
        0, 0, 0, 0, {0}
/* -------------------------           */


/* ===================================================================
 * 交易权限和交易限制的结构体定义
 * =================================================================== */

/**
 * 交易权限的元数据定义 (用于权限配置)
 */
typedef struct _OesTradingPermissionEntry {
    uint8               permissionNo;       /**< 权限号 */
    uint8               isDisabled;         /**< 失效标志 */
    char                counterAlias;       /**< 默认的主柜权限别名 (对应于主柜导出的权限代码) */
    uint8               __filler[5];        /**< 按64位对齐填充域 */
    uint64              permissionCode;     /**< 权限代码 (内部使用, 权限号对应的位域值) */

    /** 适用的市场 @see eOesMarketIdT */
    uint8               applicableMarkets[__OES_MKT_ID_MAX_ALIGNED8];

    /** 权限说明 (中文名称 UTF-8) */
    char                permissionMemo[OES_MAX_PERMISSION_MEMO_LEN];
} OesTradingPermissionEntryT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_TRADING_PERMISSION_ENTRY            \
        0, 0, 0, {0}, 0, \
        {0}, {0}
/* -------------------------           */


/* ===================================================================
 * 委托来源信息定义
 * =================================================================== */

/**
 * 委托请求/出入金请求的输入源信息 (内部使用)
 */
typedef struct _OesInputSourceInfo {
    /** 客户端的IP地址 */
    char                sourceIp[OES_MAX_IP_LEN];
    /** 客户端的MAC地址 */
    char                sourceMac[OES_MAX_MAC_LEN];
    /** 客户端的登录类型 */
    uint8               sourceType;
    /** 按64位对齐的填充域 */
    uint8               __filler[3];
    /** 客户端的设备序列号 */
    char                sourceDriverId[OES_MAX_DRIVER_ID_LEN];
} OesInputSourceInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_OES_INPUT_SOURCE_INFO                   \
        {0}, {0}, 0, {0}, {0}
/* -------------------------           */


#ifdef __cplusplus
}
#endif

#endif  /* _OES_BASE_MODEL_H */
