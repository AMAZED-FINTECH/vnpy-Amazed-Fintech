/*
 * Copyright 2016 the original author or authors.
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
 * @file    mds_base_model.h
 *
 * MDS系统的基础领域模型(数据结构)定义
 *
 * - 行情数据的字段命名风格说明:
 *   - 以大写字母开头的字段, 都是直接来源于交易所的数据;
 *   - 以小写字母开头的字段, 是额外增加的或者经过转换的数据;
 *   - 以双下划线开头的字段, 是系统的内部字段
 *
 * @version 0.8.12      2016/09/13
 * @version 0.12.8      2017/03/07
 *          - 删除独立的 MdsMktDataSnapshotEntryMetaT 结构体,
 *            将字段整合到 MdsMktDataSnapshotHeadT 中, 并删除原 meta.priceUnit (价格单位) 字段
 *          - 重命名 MdsMktDataSnapshotEntryT -> MdsL1SnapshotBodyT
 *          - 重命名 MdsIndexDataSnapshotEntryT -> MdsIndexSnapshotBodyT
 *          - 重命名 MdsStockDataSnapshotEntryT -> MdsStockSnapshotBodyT
 *          - 重命名 MdsStockSnapshotBodyT.PreCloseIOPV -> NAV
 *          - 重定义 MdsStockSnapshotBodyT.PriceLevel[5] -> BidLevels[5] + OfferLevels[5]
 *              - PriceLevel\[(\w+)\].BuyPrice -> BidLevels[\1].Price
 *              - PriceLevel\[(\w+)\].BuyVolume -> BidLevels[\1].OrderQty
 *              - PriceLevel\[(\w+)\].SellPrice -> OfferLevels[\1].Price
 *              - PriceLevel\[(\w+)\].SellVolume -> OfferLevels[\1].OrderQty
 *          - 重命名 MdsStockSnapshotBodyT.TradeVolume -> TotalVolumeTraded
 *          - 重命名 MdsIndexSnapshotBodyT.TradeVolume -> TotalVolumeTraded
 *          - 删除 MdsStockSnapshotBodyT.MDStreamID, MdsIndexSnapshotBodyT.MDStreamID 字段
 *          - 为'市场状态'和'证券实时状态'消息添加 __exchSendingTime, __mdsRecvTime 字段,
 *            以方便比对实际的行情延时
 * @version 0.12.8.2    2017/05/16
 *          - 添加 '逐笔数据丢失消息(MdsL2TickLostT)', 以通知下游系统逐笔数据(逐笔成交/逐笔
 *            委托)发生了数据丢失, 并且无法重建, 将放弃这些丢失的逐笔数据
 *          - 调整证券实时状态结构体中延迟打点时间戳字段的字段位置, 将这些字段调整到结构体的末尾,
 *            以方便客户端兼容处理
 * @version 0.12.9.7    2017/07/12
 *          - 不再对外发布逐笔丢失消息(MDS_MSGTYPE_L2_TICK_LOST), 该消息已废弃
 * @version 0.15.4.1    2017/09/07
 *          - 默认开启用于统计延时的打点信息, 并将打点信息的时间戳类型改为 timeval32 (STimeval32T) 类型,
 *            同时将行情推送的打点信息, 从消息头迁移到消息体中 (__pushingTime)
 * @version 0.15.5.10   2018/05/20
 *          - 行情结构体中增加内部频道号字段 (兼容之前版本), 该字段仅内部使用, 前端不需要关心和处理
 * @version 0.15.5.11   2018/06/01
 *          - 扩大Level2增量更新消息中最大已变更价位列表和委托明细的最大数量, 修复因为支持
 *            同时变更的价位数量不足, 导致部分巨幅波动场景下会丢失价位信息的BUG
 * @since   2016/02/11
 */


#ifndef _MDS_BASE_MODEL_H
#define _MDS_BASE_MODEL_H


#include    <sutil/types.h>


#ifdef __cplusplus
extern "C" {
#endif


/* ===================================================================
 * 预编译宏处理
 * =================================================================== */

/* 默认开启用于统计延时的打点信息 */
#if ! defined (_MDS_DISABLE_LATENCY_STATS) \
        && ! defined (_MDS_ENABLE_LATENCY_STATS)
#   define  _MDS_ENABLE_LATENCY_STATS           1
#elif defined (_MDS_DISABLE_LATENCY_STATS) \
        && defined (_MDS_ENABLE_LATENCY_STATS)
#   undef   _MDS_ENABLE_LATENCY_STATS
#endif
/* -------------------------           */


/* ===================================================================
 * 常量定义 (宏定义)
 * =================================================================== */

/** 每次的行情订阅请求中可以同时指定的最大订阅产品数量 (可以通过追加订阅的方式订阅更多的产品) */
#define MDS_MAX_SECURITY_CNT_PER_SUBSCRIBE      (4000)
/** 对于沪/深两市的期权产品, 限制对每个市场最多允许同时订阅 2000 只产品 */
#define MDS_MAX_OPTION_CNT_TOTAL_SUBSCRIBED     (2000)

/** 用户名最大长度 */
#define MDS_MAX_USERNAME_LEN                    (40)
/** 密码最大长度 */
#define MDS_MAX_PASSWORD_LEN                    (40)
/** 协议版本号的最大长度 */
#define MDS_VER_ID_MAX_LEN                      (32)
/** 测试请求标识符的最大长度 */
#define MDS_MAX_TEST_REQ_ID_LEN                 (32)

/** 产品代码长度(C6/C8) */
#define MDS_MAX_INSTR_CODE_LEN                  (9)
/** 实际的股票产品代码长度 */
#define MDS_REAL_STOCK_CODE_LEN                 (6)
/** 实际的期权产品代码长度 */
#define MDS_REAL_OPTION_CODE_LEN                (8)

/** 发送时间字段(YYYYMMDD-HH:mm:SS.sss (*C21))的最大长度 */
#define MDS_MAX_SENDING_TIME_LEN                (22)
/** 发送时间字段(YYYYMMDD-HH:mm:SS.sss (*C21))的实际有效数据长度 */
#define MDS_REAL_SENDING_TIME_LEN               (21)

/** 交易日期字段(YYYYMMDD (*C8))的最大长度 */
#define MDS_MAX_TRADE_DATE_LEN                  (9)
/** 交易日期字段(YYYYMMDD (*C8))的实际有效数据长度 */
#define MDS_REAL_TRADE_DATE_LEN                 (8)

/** 最新更新时间字段(HHMMSSss (*C8))的最大长度 */
#define MDS_MAX_UPDATE_TIME_LEN                 (9)
/** 最新更新时间字段(HHMMSSss (*C8))的实际有效数据长度 */
#define MDS_REAL_UPDATE_TIME_LEN                (8)

/** 全市场行情状态字段(*C8)的最大长度 */
#define MDS_MAX_TRADING_SESSION_ID_LEN          (9)
/** 全市场行情状态字段(*C8)的实际有效数据长度 */
#define MDS_REAL_TRADING_SESSION_ID_LEN         (8)

/** 产品实时阶段及标志(C8/C4)的最大长度 */
#define MDS_MAX_TRADING_PHASE_CODE_LEN          (9)
/** 产品实时阶段及标志(C8/C4)的实际有效数据长度 */
#define MDS_REAL_TRADING_PHASE_CODE_LEN         (8)

/** 证券状态字段(深圳证券实时状态消息 C8)的最大长度 */
#define MDS_MAX_FINANCIAL_STATUS_LEN            (9)
/** 证券状态字段(深圳证券实时状态消息 C8)的实际有效数据长度 */
#define MDS_REAL_FINANCIAL_STATUS_LEN           (8)

/** 证券业务开关的最大数量(深圳证券实时状态消息) */
#define MDS_MAX_SECURITY_SWITCH_CNT             (32)

/** 统一的价格单位 */
#define MDS_UNIFIED_PRICE_UNIT                  (10000)
/** 统一的金额单位 */
#define MDS_UNIFIED_MONEY_UNIT                  (10000)

/** 总成交金额的金额单位 (上证的总成交金额精度原本为2位, 但在此统一整合为4位精度) */
#define MDS_TOTAL_VALUE_TRADED_UNIT             MDS_UNIFIED_MONEY_UNIT
/** 指数的价格单位 */
#define MDS_INDEX_PRICE_UNIT                    MDS_UNIFIED_PRICE_UNIT
/** 股票的价格单位 (上证的股票价格精度原本为3位, 但在此统一整合为4位精度) */
#define MDS_STOCK_PRICE_UNIT                    MDS_UNIFIED_PRICE_UNIT
/** 期权的价格单位 */
#define MDS_OPTION_PRICE_UNIT                   MDS_UNIFIED_PRICE_UNIT

/** 股票代码的最大范围 */
#define MDS_MAX_STOCK_ID_SCOPE                  (1000000)
/** 期权代码的最大范围 */
#define MDS_MAX_OPTION_ID_SCOPE                 (100000000)
/* -------------------------           */


/* ===================================================================
 * Level2 相关的常量定义
 * =================================================================== */

/** Level2增量更新的价位列表最大数量 */
#define MDS_MAX_L2_PRICE_LEVEL_INCREMENTS       (40)

/** Level2披露的买一／卖一委托明细最大数量 */
#define MDS_MAX_L2_DISCLOSE_ORDERS_CNT          (50)

/** Level2增量更新的委托明细最大数量 */
#define MDS_MAX_L2_DISCLOSE_ORDERS_INCREMENTS   (152)
/* -------------------------           */


/* ===================================================================
 * 常量定义 (枚举类型定义)
 * =================================================================== */

/**
 * 交易所代码
 */
typedef enum _eMdsExchangeId {
    MDS_EXCH_SSE                        = 1,    /**< 交易所-上交所 */
    MDS_EXCH_SZSE                       = 2,    /**< 交易所-深交所 */
    __MAX_MDS_EXCH,

    __MAX_MDS_EXCH_ALIGNED4             = 4,    /**< 交易所代码最大值 (按4字节对齐的大小) */
    __MAX_MDS_EXCH_ALIGNED8             = 8     /**< 交易所代码最大值 (按8字节对齐的大小) */
} eMdsExchangeIdT;


/**
 * 消息来源
 */
typedef enum _eMdsMsgSource {
    MDS_MSGSRC_UNDEFINED                = 0,    /**< 消息来源-未定义 */
    MDS_MSGSRC_EZEI_TCP                 = 1,    /**< 消息来源-EzEI(TCP) */
    MDS_MSGSRC_EZEI_UDP                 = 2,    /**< 消息来源-EzEI(UDP) */

    MDS_MSGSRC_VDE_LEVEL2               = 3,    /**< 消息来源-SSE-VDE-LEVEL2 */
    MDS_MSGSRC_VDE_LEVEL1               = 4,    /**< 消息来源-SSE-VDE-LEVEL1-FAST */

    MDS_MSGSRC_SZSE_MDGW_BINARY         = 5,    /**< 消息来源-SZSE-MDGW-Binary(TCP) */
    MDS_MSGSRC_SZSE_MDGW_STEP           = 6,    /**< 消息来源-SZSE-MDGW-STEP(TCP) */

    MDS_MSGSRC_MDS_TCP                  = 7,    /**< 消息来源-MDS(TCP) */
    MDS_MSGSRC_MDS_UDP                  = 8,    /**< 消息来源-MDS(UDP) */

    MDS_MSGSRC_FILE_MKTDT               = 9,    /**< 消息来源-文件(mktdt) */

    MDS_MSGSRC_SSE_MDGW_BINARY          = 10,   /**< 消息来源-SSE-MDGW-Binary(TCP) */
    MDS_MSGSRC_SSE_MDGW_STEP            = 11,   /**< 消息来源-SSE-MDGW-Binary(TCP) */
    __MAX_MDS_MSGSRC,

    /** 消息来源-SZSE-MDGW-Binary @depricated 已过时, 请使用 MDS_MSGSRC_SZSE_MDGW_BINARY */
    MDS_MSGSRC_MDGW_BINARY              = MDS_MSGSRC_SZSE_MDGW_BINARY,
    /** 消息来源-SZSE-MDGW-STEP @depricated 已过时, 请使用 MDS_MSGSRC_SZSE_MDGW_STEP */
    MDS_MSGSRC_MDGW_STEP                = MDS_MSGSRC_SZSE_MDGW_STEP
} eMdsMsgSourceT;


/**
 * 证券类型
 */
typedef enum _eMdsSecurityType {
    MDS_SECURITY_TYPE_STOCK             = 1,    /**< 股票(基金/债券) */
    MDS_SECURITY_TYPE_INDEX             = 2,    /**< 指数 */
    MDS_SECURITY_TYPE_OPTION            = 3,    /**< 期权 */
    __MAX_MDS_SECURITY_TYPE
} eMdsSecurityTypeT;


/**
 * 行情数据类型
 *
 * 上交所接口协议中的原始取值 (C5):
 *  - MD001 指数行情数据
 *  - MD002 股票(A、B股)行情数据
 *  - MD003 债券行情数据
 *  - MD004 基金行情数据
 *  - M0301 期权交易行情
 *
 * 深交所接口协议中的原始取值:
 *  - 010   现货(股票,基金,债券等)集中竞价交易快照行情
 *  - 020   质押式回购交易快照行情
 *  - 030   债券分销快照行情 (* 属于综合业务, 本系统不处理)
 *  - 040   期权集中竞价交易快照行情 (==> M0301)
 *  - 060   以收盘价交易的盘后定价交易快照行情 (* 属于综合业务, 本系统不处理)
 *  - 061   以成交量加权平均价交易的盘后定价交易快照行情 (* 属于综合业务, 本系统不处理)
 *  - 900   指数快照行情 (==> MD001)
 *  - 910   成交量统计指标快照行情
 */
typedef enum _eMdsMdStreamType {
    MDS_MD_STREAM_TYPE_INDEX            = 1,    /**< 指数 */
    MDS_MD_STREAM_TYPE_STOCK            = 2,    /**< 上交所股票 */
    MDS_MD_STREAM_TYPE_BOND             = 3,    /**< 上交所债券(Level1) */
    MDS_MD_STREAM_TYPE_FUND             = 4,    /**< 上交所基金(Level1) */

    MDS_MD_STREAM_TYPE_SZSE_STOCK       = 5,    /**< 深交所现货(股票,基金,债券等)集中竞价交易快照 */
    MDS_MD_STREAM_TYPE_SZSE_REPO        = 6,    /**< 深交所质押式回购交易快照 */
    MDS_MD_STREAM_TYPE_SZSE_TRADE_STATS = 7,    /**< 深交所成交量统计指标快照 */

    MDS_MD_STREAM_TYPE_OPTION           = 8,    /**< 期权 */
    MDS_MD_STREAM_TYPE_OTHERS           = 9,    /**< 其它市场总揽/虚拟集合竞价等快照消息 */
    __MAX_MDS_MD_STREAM_TYPE
} eMdsMdStreamTypeT;


/**
 * 行情数据级别 (Level1 / Level2)
 */
typedef enum _eMdsMdLevel {
    MDS_MD_LEVEL_0                      = 0,    /**< 未设置 */
    MDS_MD_LEVEL_1                      = 1,    /**< Level-1 行情 */
    MDS_MD_LEVEL_2                      = 2,    /**< Level-2 行情 */
    __MAX_MDS_MD_LEVEL
} eMdsMdLevelT;


/**
 * Level2增量更新消息的价位运算 (1=Add, 2=Update, 3=Delete)
 */
typedef enum _eMdsL2PriceLevelOperator {
    MDS_L2_PX_OPERATOR_ADD              = 1,    /**< L2价位运算 - Add */
    MDS_L2_PX_OPERATOR_UPDATE           = 2,    /**< L2价位运算 - Update */
    MDS_L2_PX_OPERATOR_DELETE           = 3,    /**< L2价位运算 - Delete */
    __MAX_MDS_L2_PX_OPERATOR
} eMdsL2PriceLevelOperatorT;


/**
 * Level2虚拟集合竞价的虚拟未匹配量的方向
 */
typedef enum _eMdsL2VirtualAuctionSide {
    MDS_L2_VIRTUAL_AUCTION_SIDE_NONE    = 0,    /**< 无未匹配量, 买卖两边的未匹配量都为0 */
    MDS_L2_VIRTUAL_AUCTION_SIDE_BID     = 1,    /**< 买方有未匹配量, 卖方未匹配量为0 */
    MDS_L2_VIRTUAL_AUCTION_SIDE_OFFER   = 2     /**< 卖方有未匹配量, 买方未匹配量为0 */
} eMdsL2VirtualAuctionSideT;


/**
 * Level2逐笔成交的成交类别
 * - 仅适用于深交所 ('4'=撤销, 'F'=成交)
 * - 对于上证, 将固定取值为 'F'(成交)
 */
typedef enum _eMdsL2TradeExecType {
    MDS_L2_TRADE_EXECTYPE_CANCELED      = '4',  /**< L2执行类型 - 已撤销 */
    MDS_L2_TRADE_EXECTYPE_TRADE         = 'F',  /**< L2执行类型 - 已成交 */
} eMdsL2TradeExecTypeT;


/**
 * Level2逐笔成交的内外盘标志
 * - 仅适用于上证 ('B'=外盘,主动买, 'S'=内盘,主动卖, 'N'=未知)
 * - 对于深交所, 将固定取值为 'N'(未知)
 */
typedef enum _eMdsL2TradeBSFlag {
    MDS_L2_TRADE_BSFLAG_BUY             = 'B',  /**< L2内外盘标志 - 外盘,主动买 */
    MDS_L2_TRADE_BSFLAG_SELL            = 'S',  /**< L2内外盘标志 - 内盘,主动卖 */
    MDS_L2_TRADE_BSFLAG_UNKNOWN         = 'N',  /**< L2内外盘标志 - 未知 */
} eMdsL2TradeBSFlagT;


/**
 * Level2逐笔委托的买卖方向 ('1'=买 '2'=卖 'G'=借入 'F'=出借)
 */
typedef enum _eMdsL2OrderSideT {
    MDS_L2_ORDER_SIDE_BUY               = '1',  /**< L2买卖方向 - 买 */
    MDS_L2_ORDER_SIDE_SELL              = '2',  /**< L2买卖方向 - 卖 */
    MDS_L2_ORDER_SIDE_BORROW            = 'G',  /**< L2买卖方向 - 借入 */
    MDS_L2_ORDER_SIDE_LEND              = 'F'   /**< L2买卖方向 - 出借 */
} eMdsL2OrderSideT;


/**
 * Level2逐笔委托的订单类型 ('1'=市价 '2'=限价 'U'=本方最优)
 */
typedef enum _eMdsL2OrderType {
    MDS_L2_ORDER_TYPE_MKT               = '1',  /**< L2订单类型 - 市价 */
    MDS_L2_ORDER_TYPE_LMT               = '2',  /**< L2订单类型 - 限价 */
    MDS_L2_ORDER_TYPE_SAMEPARTY_BEST    = 'U'   /**< L2订单类型 - 本方最优 */
} eMdsL2OrderTypeT;


/**
 * 客户端类型定义 (内部使用)
 */
typedef enum _eMdsClientType {
    MDS_CLIENT_TYPE_UNDEFINED           = 0,    /**< 客户端类型-未定义 */
    MDS_CLIENT_TYPE_INVESTOR            = 1,    /**< 普通投资人 */
    MDS_CLIENT_TYPE_VIRTUAL             = 2     /**< 虚拟账户 (仅开通行情, 不可交易) */
} eMdsClientTypeT;


/**
 * 客户端状态定义 (内部使用)
 */
typedef enum _eMdsClientStatus {
    MDS_CLIENT_STATUS_UNACTIVATED       = 0,    /**< 未激活 (不加载) */
    MDS_CLIENT_STATUS_ACTIVATED         = 1,    /**< 已激活 (正常加载) */
    MDS_CLIENT_STATUS_PAUSE             = 2,    /**< 已暂停 (正常加载, 不可交易) */
    MDS_CLIENT_STATUS_SUSPENDED         = 3,    /**< 已挂起 (正常加载, 不可交易、不可出入金) */
    MDS_CLIENT_STATUS_CANCELLED         = 4     /**< 已注销 (不加载) */
} eMdsClientStatusT;
/* -------------------------           */


/* ===================================================================
 * Level1 行情消息定义
 * =================================================================== */

/**
 * 市场状态消息(MsgType=h)定义 (仅适用于上海市场, 深圳市场没有该行情)
 */
typedef struct _MdsTradingSessionStatusMsg {
    uint8               exchId;                 /**< 交易所代码(沪/深) @see eMdsExchangeIdT */
    uint8               securityType;           /**< 证券类型(股票/期权) @see eMdsSecurityTypeT */
    int8                __isRepeated;           /**< 是否是重复的行情 (供内部使用, 小于0 表示数据倒流) */
    uint8               __filler1;              /**< 按64位对齐的填充域 */

    int32               tradeDate;              /**< 交易日期 (YYYYMMDD, 通过拆解SendingTime得到, 并非官方数据) */
    int32               updateTime;             /**< 行情时间 (HHMMSSsss, 交易所时间, 通过拆解SendingTime得到, 并非官方数据) */
    int32               __exchSendingTime;      /**< 交易所发送时间 (HHMMSSsss) */
    int32               __mdsRecvTime;          /**< MDS接收到时间 (HHMMSSsss) */

    int32               TotNoRelatedSym;        /**< 最大产品数目 (包括指数) */

    /**
     * 全市场行情状态 (*C8)
     * 该字段为 8 位字符串,左起每位表示特定的含义,无定义则填空格。
     * 第 1 位: ‘S’表示全市场启动期间(开市前), ‘T’表示全市场处于交易期间 (含中间休市), ‘E’表示全市场处于闭市期间。
     * 第 2 位: ‘1’表示开盘集合竞价结束标志, 未结束取‘0’。
     * 第 3 位: ‘1’表示市场行情闭市标志, 未闭市取‘0’。
     */
    char                TradingSessionID[MDS_MAX_TRADING_SESSION_ID_LEN];

    uint8               __filler3[3];           /**< 按64位对齐的填充域 */
    uint32              __dataVersion;          /**< 行情数据的更新版本号 (当__isRepeated!=0时, 该值仅作为参考值) */
    uint64              __origTickSeq;          /**< 对应的原始行情的序列号(供内部使用) */

#ifdef  _MDS_ENABLE_LATENCY_STATS
    /** 消息原始接收时间 (从网络接收到数据的最初时间) */
    STimeval32T         __origNetTime;
    /** 消息实际接收时间 (开始解码等处理之前的时间) */
    STimeval32T         __recvTime;
    /** 消息采集处理完成时间 */
    STimeval32T         __collectedTime;
    /** 消息加工处理完成时间 */
    STimeval32T         __processedTime;
    /** 消息推送时间 (写入推送缓存以后, 实际网络发送之前) */
    STimeval32T         __pushingTime;
#endif

} MdsTradingSessionStatusMsgT;


/* 结构体初始化值的尾部填充字段定义 */
#ifdef  _MDS_ENABLE_LATENCY_STATS
# define    __NULLOBJ_MDS_TRADING_SESSION_STATUS_MSG_TAILER             \
            , {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
#else
# define    __NULLOBJ_MDS_TRADING_SESSION_STATUS_MSG_TAILER
#endif


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_TRADING_SESSION_STATUS_MSG                          \
        NULLOBJ_MDS_TRADING_SESSION_STATUS_MSG2(0)

/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_TRADING_SESSION_STATUS_MSG2(EXCH_ID)                \
        (EXCH_ID), 0, 0, 0, \
        0, 0, 0, 0, 0, \
        {0}, \
        {0}, 0, 0 \
        __NULLOBJ_MDS_TRADING_SESSION_STATUS_MSG_TAILER
/* -------------------------           */


/**
 * 证券实时状态定义 (仅适用于深圳市场, 上海市场没有该行情)
 */
typedef struct _MdsSecurityStatusMsg {
    uint8               exchId;                 /**< 交易所代码(沪/深) @see eMdsExchangeIdT */
    uint8               securityType;           /**< 证券类型(股票/期权) @see eMdsSecurityTypeT */
    int8                __isRepeated;           /**< 是否是重复的行情 (供内部使用, 小于0 表示数据倒流) */
    uint8               __filler1;              /**< 按64位对齐的填充域 */

    int32               tradeDate;              /**< 交易日期 (YYYYMMDD, 通过拆解数据生成时间OrigTime得到) */
    int32               updateTime;             /**< 行情时间 (HHMMSSsss, 交易所时间, 通过拆解数据生成时间OrigTime得到) */
    int32               __exchSendingTime;      /**< 交易所发送时间 (HHMMSSsss, 目前获取不到深交所的发送时间, 固定为 0) */
    int32               __mdsRecvTime;          /**< MDS接收到时间 (HHMMSSsss) */

    int32               instrId;                /**< 产品代码 */

    /** 产品代码 C6 / C8 (如: '000001' 等) */
    char                SecurityID[MDS_MAX_INSTR_CODE_LEN];

    /**
     * 证券状态 (C8)
     * A=上市公司早间披露提示
     * B=上市公司午间披露提示
     */
    char                FinancialStatus[MDS_MAX_FINANCIAL_STATUS_LEN];

    uint8               __filler2;              /**< 按64位对齐的填充域 */
    uint8               __channelNo;            /**< 内部频道号 (供内部使用, 取值范围{1,2,4,8}) */
    uint32              __dataVersion;          /**< 行情数据的更新版本号 (当__isRepeated!=0时, 该值仅作为参考值) */
    uint64              __origTickSeq;          /**< 对应的原始行情的序列号(供内部使用) */

    int32               NoSwitch;               /**< 开关个数 */
    int32               __filler4;              /**< 按64位对齐的填充域 */

    /**
     * 证券业务开关列表
     * 业务开关列表为定长数组, 数组的下标位置对应于各个业务开关, 业务开关说明如下:
     *  -  1: 融资买入, 适用于融资标的证券
     *  -  2: 融券卖出, 适用于融券标的证券
     *  -  3: 申购, 适用于 ETF/LOF 等开放式基金, 对于黄金 ETF 是指现金申购
     *  -  4: 赎回, 适用于 ETF/LOF 等开放式基金, 对于黄金 ETF 是指现金赎回开关
     *  -  5: 认购, 适用于网上发行认购代码
     *  -  6: 转股, 适用于处于转股回售期的可转债
     *  -  7: 回售, 适用于处于转股回售期的可转债
     *  -  8: 行权, 适用于处于行权期的权证或期权
     *  - 10: 买开仓, 适用于期权等衍生品
     *  - 11: 卖开仓, 适用于期权等衍生品
     *  - 12: 黄金ETF实物申购, 适用于黄金 ETF
     *  - 13: 黄金ETF实物赎回, 适用于黄金 ETF
     *  - 14: 预受要约, 适用于处于要约收购期的股票
     *  - 15: 解除要约, 适用于处于要约收购期的股票
     *  - 18: 转股撤单, 适用于处于转股回售期的可转债
     *  - 19: 回售撤单, 适用于处于转股回售期的可转债
     *  - 20: 质押, 适用于质押式回购可质押入库证券
     *  - 21: 解押, 适用于质押式回购可质押入库证券
     *  - 22: 表决权, 适用于优先股
     *  - 23: 股票质押式回购, 适用于可开展股票质押式回购业务的证券
     *  - 24: 实时分拆, 适用于分级基金
     *  - 25: 实时合并, 适用于分级基金
     *  - 26: 备兑开仓, 适用于期权等衍生品
     *  - 27: 做市商报价, 适用于期权等支持做市商报价的证券
     */
    struct {
        /** 业务开关的使能标志 (0 未启用, 1 启用) */
        uint8           switchFlag;

        /** 开关状态 (0 关闭, 1 开启) */
        uint8           switchStatus;
    } switches[MDS_MAX_SECURITY_SWITCH_CNT];

#ifdef  _MDS_ENABLE_LATENCY_STATS
    /** 消息原始接收时间 (从网络接收到数据的最初时间) */
    STimeval32T         __origNetTime;
    /** 消息实际接收时间 (开始解码等处理之前的时间) */
    STimeval32T         __recvTime;
    /** 消息采集处理完成时间 */
    STimeval32T         __collectedTime;
    /** 消息加工处理完成时间 */
    STimeval32T         __processedTime;
    /** 消息推送时间 (写入推送缓存以后, 实际网络发送之前) */
    STimeval32T         __pushingTime;
#endif

} MdsSecurityStatusMsgT;


/* 结构体初始化值的尾部填充字段定义 */
#ifdef  _MDS_ENABLE_LATENCY_STATS
# define    __NULLOBJ_MDS_SECURITY_STATUS_MSG_TAILER                    \
            , {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
#else
# define    __NULLOBJ_MDS_SECURITY_STATUS_MSG_TAILER
#endif


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_SECURITY_STATUS_MSG                                 \
        0, 0, 0, 0, \
        0, 0, 0, 0, 0, \
        {0}, {0}, \
        0, 0, 0, 0, \
        0, 0, \
        { \
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
            {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
        } \
        __NULLOBJ_MDS_SECURITY_STATUS_MSG_TAILER
/* -------------------------           */


/* ===================================================================
 * Level1 快照行情定义
 * =================================================================== */

/**
 * 价位信息定义
 */
typedef struct _MdsPriceLevelEntry {
    int32               Price;                  /**< 委托价格 */
    int32               NumberOfOrders;         /**< 价位总委托笔数 (Level1不揭示该值, 固定为0) */
    int64               OrderQty;               /**< 委托数量 */
} MdsPriceLevelEntryT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_PRICE_LEVEL_ENTRY                                   \
        0, 0, 0
/* -------------------------           */


/**
 * Level1/Level2 快照行情(证券行情全幅消息)的消息头定义
 */
typedef struct _MdsMktDataSnapshotHead {
    uint8               exchId;                 /**< 交易所代码(沪/深) @see eMdsExchangeIdT */
    uint8               securityType;           /**< 证券类型(股票/期权) @see eMdsSecurityTypeT */
    int8                __isRepeated;           /**< 是否是重复的行情 (内部使用, 小于0表示数据倒流) */
    uint8               __origMdSource;         /**< 原始行情数据来源 @see eMdsMsgSourceT */

    int32               tradeDate;              /**< 交易日期 (YYYYMMDD, 8位整型数值) */
    int32               updateTime;             /**< 行情时间 (HHMMSSsss, 交易所时间, 只有上海L1可能会通过拆解SendingTime得到) */

    int32               instrId;                /**< 产品代码 */
    int16               bodyLength;             /**< 实际数据长度 */
    uint8               mdStreamType;           /**< 行情数据类型 @see eMdsMdStreamTypeT */
    uint8               __channelNo;            /**< 内部频道号 (供内部使用, 取值范围{1,2,4,8}) */
    uint32              __dataVersion;          /**< 行情数据的更新版本号 */
    uint64              __origTickSeq;          /**< 对应的原始行情的序列号(供内部使用) */

#ifdef  _MDS_ENABLE_LATENCY_STATS
    /** 消息原始接收时间 (从网络接收到数据的最初时间) */
    STimeval32T         __origNetTime;
    /** 消息实际接收时间 (开始解码等处理之前的时间) */
    STimeval32T         __recvTime;
    /** 消息采集处理完成时间 */
    STimeval32T         __collectedTime;
    /** 消息加工处理完成时间 */
    STimeval32T         __processedTime;
    /** 消息推送时间 (写入推送缓存以后, 实际网络发送之前) */
    STimeval32T         __pushingTime;
#endif

} MdsMktDataSnapshotHeadT;


/* 结构体初始化值的尾部填充字段定义 */
#ifdef  _MDS_ENABLE_LATENCY_STATS
# define    __NULLOBJ_MDS_MKT_DATA_SNAPSHOT_HEAD_TAILER                 \
            , {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
#else
# define    __NULLOBJ_MDS_MKT_DATA_SNAPSHOT_HEAD_TAILER
#endif


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_DATA_SNAPSHOT_HEAD                              \
        NULLOBJ_MDS_MKT_DATA_SNAPSHOT_HEAD3(0, 0, 0)

/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_DATA_SNAPSHOT_HEAD2(EXCH_ID, MD_SOURCE)         \
        NULLOBJ_MDS_MKT_DATA_SNAPSHOT_HEAD3(EXCH_ID, 0, (MD_SOURCE))

/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_DATA_SNAPSHOT_HEAD3(EXCH_ID, SECURITY_TYPE,     \
                MD_SOURCE)                                              \
        (EXCH_ID), (SECURITY_TYPE), 0, (MD_SOURCE), \
        0, 0, \
        0, 0, 0, 0, 0, 0 \
        __NULLOBJ_MDS_MKT_DATA_SNAPSHOT_HEAD_TAILER
/* -------------------------           */


/**
 * Level1/Level2 指数快照行情定义
 */
typedef struct _MdsIndexSnapshotBody {
    /** 产品代码 C6 / C8 (如: '000001' 等) */
    char                SecurityID[MDS_MAX_INSTR_CODE_LEN];
    /**
     * 产品实时阶段及标志 C8
     * @see MdsStockSnapshotBodyT.TradingPhaseCode
     */
    char                TradingPhaseCode[MDS_MAX_TRADING_PHASE_CODE_LEN];
    char                __filler[6];            /**< 按64位对齐的填充域 */

    uint64              NumTrades;              /**< 成交笔数 (仅适用于深圳, 上海指数行情中没有成交笔数信息) */
    uint64              TotalVolumeTraded;      /**< 成交总量 (@note 上海指数的成交量单位是手, 深圳指数的成交量单位是股) */
    int64               TotalValueTraded;       /**< 成交总金额 (金额单位精确到元后四位, 即: 1元=10000) */

    int64               PrevCloseIdx;           /**< 昨日收盘指数 (价格单位精确到元后四位, 即: 1元=10000) */
    int64               OpenIdx;                /**< 今开盘指数 (价格单位精确到元后四位, 即: 1元=10000) */
    int64               HighIdx;                /**< 最高指数 */
    int64               LowIdx;                 /**< 最低指数 */
    int64               LastIdx;                /**< 最新指数 */
    int64               CloseIdx;               /**< 今收盘指数 */

    int32               StockNum;               /**< 统计量指标样本个数 (用于深圳成交量统计指标) */
    int32               __filler1;              /**< 按64位对齐的填充域 */
} MdsIndexSnapshotBodyT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_INDEX_SNAPSHOT_BODY                                 \
        {0}, {0}, {0}, \
        0, 0, 0, \
        0, 0, 0, 0, 0, 0, \
        0, 0
/* -------------------------           */


/**
 * Level1 股票快照行情定义
 * 股票(A、B股)、债券、基金、期权
 *
 * 关于集合竞价期间的虚拟集合竞价行情 (上海L1、深圳L1):
 * - 集合竞价期间的虚拟成交价通过买卖盘档位揭示, 其中买一和卖一都揭示虚拟成交价格和成交数量,
 *   买二或卖二揭示虚拟成交价位上的买剩余量或卖剩余量
 */
typedef struct _MdsStockSnapshotBody {
    /** 产品代码 C6 / C8 (如: '600000' 等) */
    char                SecurityID[MDS_MAX_INSTR_CODE_LEN];

    /**
     * 产品实时阶段及标志 C8 / C4
     *
     * 上交所股票 (C8):
     *  -# 第 1 位:
     *      - ‘S’表示启动 (开市前) 时段，‘C’表示集合竞价时段，‘T’表示连续交易时段
     *      - ‘B’表示休市时段，‘E’表示闭市时段，‘P’表示产品停牌
     *      - ‘M’表示可恢复交易的熔断时段 (盘中集合竞价)，‘N’表示不可恢复交易的熔断时段 (暂停交易至闭市)
     *      - ‘D’表示开盘集合竞价阶段结束到连续竞价阶段开始之前的时段 (如有的话)
     *      - ‘U’表示收盘集合竞价时段。
     *  -# 第 2 位:
     *      - ‘0’表示此产品不可正常交易 (在产品进入停牌、熔断(暂停交易至闭市)状态时值为‘0’)，
     *      - ‘1’表示此产品可正常交易 (在产品进入开盘集合竞价、连续交易、收盘集合竞价、熔断(盘中集合竞价)状态时值为‘1’)，
     *      - 无意义填空格。
     *      - 闭市后保持该产品闭市前的是否可正常交易状态。
     *  -# 第 3 位:
     *      - ‘0’表示未上市, ‘1’表示已上市。
     *  -# 第 4 位:
     *      - ‘0’表示此产品在当前时段不接受进行新订单申报,
     *      - ‘1’表示此产品在当前时段可接受进行新订单申报。
     *      - 无意义填空格。
     *      - 仅在交易时段有效，在非交易时段无效。
     *
     * 上交所期权 (C4):
     *  -# 第 1 位:
     *      - ‘S’表示启动(开市前)时段, ‘C’表示集合竞价时段, ‘T’表示连续交易时段,
     *      - ‘B’表示休市时段, ‘E’表示闭市时段, ‘V’表示波动性中断, ‘P’ 表示临时停牌, ‘U’收盘集合竞价。
     *      - ‘M’表示可恢复交易的熔断(盘中集合竞价), ‘N’表示不可恢复交易的熔断(暂停交易至闭市)
     *  -# 第 2 位:
     *      - ‘0’表示未连续停牌, ‘1’表示连续停牌。(预留,暂填空格)
     *  -# 第 3 位:
     *      - ‘0’表示不限制开仓, ‘1’表示限制备兑开仓, ‘2’表示卖出开仓, ‘3’表示限制卖出开仓、备兑开仓,
     *      - ‘4’表示限制买入开仓, ‘5’表示限制买入开 仓、备兑开仓,‘6’表示限制买入开仓、卖出开仓,
     *      - ‘7’表示限制买入开仓、卖出开仓、备兑开仓
     *  -# 第 4 位:
     *      - ‘0’表示此产品在当前时段不接受进行新订单申报, ‘1’ 表示此产品在当前时段可接受进行新订单申报。
     *
     * 深交所 (C8):
     *  -# 第 0 位:
     *      - S=启动(开市前) O=开盘集合竞价 T=连续竞价
     *      - B=休市 C=收盘集合竞价 E=已闭市 H=临时停牌
     *      - A=盘后交易 V=波动性中断
     *  -# 第 1 位:
     *      - 0=正常状态
     *      - 1=全天停牌
     */
    char                TradingPhaseCode[MDS_MAX_TRADING_PHASE_CODE_LEN];
    char                __filler[6];            /**< 按64位对齐的填充域 */

    uint64              NumTrades;              /**< 成交笔数 */
    uint64              TotalVolumeTraded;      /**< 成交总量 */
    int64               TotalValueTraded;       /**< 成交总金额 (金额单位精确到元后四位, 即: 1元=10000) */

    int32               PrevClosePx;            /**< 昨日收盘价/期权合约昨日结算价 (价格单位精确到元后四位, 即: 1元=10000) */
    int32               OpenPx;                 /**< 今开盘价 (价格单位精确到元后四位, 即: 1元=10000) */
    int32               HighPx;                 /**< 最高价 */
    int32               LowPx;                  /**< 最低价 */
    int32               TradePx;                /**< 成交价 */
    int32               ClosePx;                /**< 今收盘价/期权收盘价 (仅上海, 深圳行情没有单独的收盘价) */

    int32               IOPV;                   /**< 基金份额参考净值/ETF申赎的单位参考净值 (适用于基金) */
    int32               NAV;                    /**< 基金 T-1 日净值 (适用于基金) */
    uint64              TotalLongPosition;      /**< 合约总持仓量 (适用于期权) */

    /** 五档买盘价位信息 */
    MdsPriceLevelEntryT BidLevels[5];

    /** 五档卖盘价位信息 */
    MdsPriceLevelEntryT OfferLevels[5];

} MdsStockSnapshotBodyT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_STOCK_SNAPSHOT_BODY                                 \
        {0}, {0}, {0}, \
        0, 0, 0, \
        0, 0, 0, 0, 0, 0, \
        0, 0, 0, \
        { \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY} \
        }, \
        { \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY} \
        }
/* -------------------------           */


/**
 * Level1 证券行情全幅消息的完整消息体定义
 */
typedef union _MdsL1SnapshotBody {
    MdsStockSnapshotBodyT           stock;      /**< 股票、债券、基金行情数据 */
    MdsStockSnapshotBodyT           option;     /**< 期权行情数据 */
    MdsIndexSnapshotBodyT           index;      /**< 指数行情数据 */
} MdsL1SnapshotBodyT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_DATA_SNAPSHOT_BODY                              \
        {NULLOBJ_MDS_STOCK_SNAPSHOT_BODY}
/* -------------------------           */


/* ===================================================================
 * Level2 快照行情消息定义
 * =================================================================== */

/**
 * Level2 快照行情定义
 * 股票(A、B股)、债券、基金
 *
 * 关于集合竞价期间的虚拟集合竞价行情 (上海L2、深圳L2):
 * - 深圳L2集合竞价期间的虚拟成交价通过买卖盘档位揭示, 其中买一和卖一都揭示虚拟成交价格和成交数量,
 *   买二或卖二揭示虚拟成交价位上的买剩余量或卖剩余量
 * - 上海L2的虚拟集合竞价行情通过单独虚拟集合竞价快照消息(MdsL2VirtualAuctionPriceT)发布
 */
typedef struct _MdsL2StockSnapshotBody {
    /** 产品代码 C6 / C8 (如: '600000' 等) */
    char                SecurityID[MDS_MAX_INSTR_CODE_LEN];

    /**
     * 产品实时阶段及标志 C8 / C4
     *
     * 上交所股票 (C8):
     *  -# 第 1 位:
     *      - ‘S’表示启动 (开市前) 时段, ‘C’表示集合竞价时段, ‘T’表示连续交易时段
     *      - ‘B’表示休市时段, ‘E’表示闭市时段, ‘P’表示产品停牌
     *      - ‘M’表示可恢复交易的熔断时段 (盘中集合竞价), ‘N’表示不可恢复交易的熔断时段 (暂停交易至闭市)
     *      - ‘D’表示开盘集合竞价阶段结束到连续竞价阶段开始之前的时段 (如有) 。
     *  -# 第 2 位:
     *      - ‘0’表示此产品不可正常交易,
     *      - ‘1’表示此产品可正常交易,
     *      - 无意义填空格。
     *  -# 第 3 位:
     *      - ‘0’表示未上市, ‘1’表示已上市。
     *  -# 第 4 位:
     *      - ‘0’表示此产品在当前时段不接受进行新订单申报,
     *      - ‘1’ 表示此产品在当前时段可接受进行新订单申报。
     *      - 无意义填空格。
     *
     * 上交所期权 (C4):
     *  -# 第 1 位:
     *      - ‘S’表示启动(开市前)时段, ‘C’表示集合竞价时段, ‘T’表示连续交易时段,
     *      - ‘B’表示休市时段, ‘E’表示闭市时段, ‘V’表示波动性中断, ‘P’ 表示临时停牌, ‘U’收盘集合竞价。
     *      - ‘M’表示可恢复 交易的熔断(盘中集合竞价), ‘N’表示不可恢复交易的熔断(暂停交易至闭市)
     *  -# 第 2 位:
     *      - ‘0’表示未连续停牌, ‘1’表示连续停牌。(预留,暂填空格)
     *  -# 第 3 位:
     *      - ‘0’表示不限制开仓, ‘1’表示限制备兑开仓, ‘2’表示卖出开仓, ‘3’表示限制卖出开仓、备兑开仓,
     *      - ‘4’表示限制买入开仓, ‘5’表示限制买入开 仓、备兑开仓,‘6’表示限制买入开仓、卖出开仓,
     *      - ‘7’表示限制买入开仓、卖出开仓、备兑开仓
     *  -# 第 4 位:
     *      - ‘0’表示此产品在当前时段不接受进行新订单申报, ‘1’ 表示此产品在当前时段可接受进行新订单申报。
     *
     * 深交所 (C8):
     *  -# 第 0 位:
     *      - S=启动(开市前) O=开盘集合竞价 T=连续竞价
     *      - B=休市 C=收盘集合竞价 E=已闭市 H=临时停牌
     *      - A=盘后交易 V=波动性中断
     *  -# 第 1 位:
     *      - 0=正常状态
     *      - 1=全天停牌
     */
    char                TradingPhaseCode[MDS_MAX_TRADING_PHASE_CODE_LEN];
    char                __filler[6];            /**< 按64位对齐的填充域 */

    uint64              NumTrades;              /**< 成交笔数 */
    uint64              TotalVolumeTraded;      /**< 成交总量 */
    int64               TotalValueTraded;       /**< 成交总金额 (金额单位精确到元后四位, 即: 1元=10000) */

    int32               PrevClosePx;            /**< 昨日收盘价/期权合约昨日结算价 (价格单位精确到元后四位, 即: 1元=10000) */
    int32               OpenPx;                 /**< 今开盘价 (价格单位精确到元后四位, 即: 1元=10000) */
    int32               HighPx;                 /**< 最高价 */
    int32               LowPx;                  /**< 最低价 */
    int32               TradePx;                /**< 成交价 */
    int32               ClosePx;                /**< 今收盘价/期权收盘价 (仅上海, 深圳行情没有单独的收盘价) */

    int32               IOPV;                   /**< 基金份额参考净值/ETF申赎的单位参考净值 (适用于基金) */
    int32               NAV;                    /**< 基金 T-1 日净值 (适用于基金) */
    uint64              TotalLongPosition;      /**< 合约总持仓量 (适用于期权) */

    int64               TotalBidQty;            /**< 委托买入总量 */
    int64               TotalOfferQty;          /**< 委托卖出总量 */
    int32               WeightedAvgBidPx;       /**< 加权平均委买价格 */
    int32               WeightedAvgOfferPx;     /**< 加权平均委卖价格 */
    int32               BidPriceLevel;          /**< 买方委托价位数 (实际的委托价位总数, 仅上海) */
    int32               OfferPriceLevel;        /**< 卖方委托价位数 (实际的委托价位总数, 仅上海) */

    /** 十档买盘价位信息 */
    MdsPriceLevelEntryT BidLevels[10];

    /** 十档卖盘价位信息 */
    MdsPriceLevelEntryT OfferLevels[10];

} MdsL2StockSnapshotBodyT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_STOCK_SNAPSHOT_BODY                              \
        {0}, {0}, {0}, \
        0, 0, 0, \
        0, 0, 0, 0, 0, 0, \
        0, 0, 0, \
        0, 0, 0, 0, 0, 0, \
        { \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY} \
        }, \
        { \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY}, \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY} \
        }
/* -------------------------           */


/**
 * Level2 快照行情的增量更新消息定义 (增量更新消息仅适用于上海L2)
 * 股票(A、B股)、债券、基金
 */
typedef struct _MdsL2StockSnapshotIncremental {
    uint64              NumTrades;              /**< 成交笔数 */
    uint64              TotalVolumeTraded;      /**< 成交总量 */
    int64               TotalValueTraded;       /**< 成交总金额 (金额单位精确到元后四位, 即: 1元=10000) */

    int32               OpenPx;                 /**< 今开盘价 (价格单位精确到元后四位, 即: 1元=10000) */
    int32               TradePx;                /**< 成交价 */
    int32               ClosePx;                /**< 今收盘价/期权收盘价 (仅上海, 深圳行情没有单独的收盘价) */
    int32               IOPV;                   /**< 基金份额参考净值/ETF申赎的单位参考净值 (适用于基金) */

    int64               TotalBidQty;            /**< 委托买入总量 */
    int64               TotalOfferQty;          /**< 委托卖出总量 */
    int32               WeightedAvgBidPx;       /**< 加权平均委买价格 */
    int32               WeightedAvgOfferPx;     /**< 加权平均委卖价格 */
    int32               BidPriceLevel;          /**< 买方委托价位数 (实际的委托价位总数, 仅上海) */
    int32               OfferPriceLevel;        /**< 卖方委托价位数 (实际的委托价位总数, 仅上海) */

    /** 最优申买价 */
    int32               BestBidPrice;
    /** 增量更新消息中是否已经包含了最优申买价位 */
    uint8               HasContainedBestBidLevel;
    /** 买盘价位数量 (不包括已删除且大于最优申买价的价位) */
    uint8               NoBidLevel;
    /** 按64位对齐的填充域 */
    uint8               __filler1[2];

    /** 最优申卖价 */
    int32               BestOfferPrice;
    /** 增量更新消息中是否已经包含了最优申买价位 */
    uint8               HasContainedBestOfferLevel;
    /** 卖盘价位数量 (不包括已删除且小于最优申卖价的价位) */
    uint8               NoOfferLevel;
    /** 按64位对齐的填充域 */
    uint8               __filler2[2];

    /**
     * (发生变更的价位列表中) 各个价位的运算操作
     * - 1 = Add
     * - 2 = Update
     * - 3 = Delete
     * @see eMdsL2PriceLevelOperatorT
     */
    uint8               PriceLevelOperator[MDS_MAX_L2_PRICE_LEVEL_INCREMENTS];

    /** 发生变更的价位列表 (该字段为变长数组, 实际元素数量为: NoBidLevel + NoOfferLevel) */
    MdsPriceLevelEntryT PriceLevels[MDS_MAX_L2_PRICE_LEVEL_INCREMENTS];

} MdsL2StockSnapshotIncrementalT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_STOCK_SNAPSHOT_INCREMENTAL_BODY                  \
        0, 0, 0, \
        0, 0, 0, 0, \
        0, 0, 0, 0, 0, 0, \
        0, 0, 0, {0}, \
        0, 0, 0, {0}, \
        {0}, \
        { \
            {NULLOBJ_MDS_PRICE_LEVEL_ENTRY} \
        }
/* -------------------------           */


/**
 * Level2 委托队列信息 (买一／卖一前五十笔委托明细)
 */
typedef struct _MdsL2BestOrdersSnapshotBody {
    /** 产品代码 C6 / C8 (如: '600000' 等) */
    char                SecurityID[MDS_MAX_INSTR_CODE_LEN];
    uint8               __filler[5];            /**< 按64位对齐的填充域 */
    uint8               NoBidOrders;            /**< 买一价位的揭示委托笔数 */
    uint8               NoOfferOrders;          /**< 卖一价位的揭示委托笔数 */

    uint64              TotalVolumeTraded;      /**< 成交总量 (来自快照行情的冗余字段) */
    int32               BestBidPrice;           /**< 最优申买价 */
    int32               BestOfferPrice;         /**< 最优申卖价 */

    /** 买一价位的委托明细(前50笔) */
    int32               BidOrderQty[MDS_MAX_L2_DISCLOSE_ORDERS_CNT];

    /** 卖一价位的委托明细(前50笔) */
    int32               OfferOrderQty[MDS_MAX_L2_DISCLOSE_ORDERS_CNT];

} MdsL2BestOrdersSnapshotBodyT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_BEST_ORDERS_SNAPSHOT_BODY                        \
        {0}, {0}, \
        0, 0, \
        0, 0, 0, \
        {0}, \
        {0}
/* -------------------------           */


/**
 * Level2 委托队列的增量更新信息 (买一／卖一前五十笔委托明细, 增量更新消息仅适用于上海L2)
 */
typedef struct _MdsL2BestOrdersSnapshotIncremental {
    /** 成交总量 (来自快照行情的冗余字段) */
    uint64              TotalVolumeTraded;

    /** 最优申买价 */
    int32               BestBidPrice;
    /** 增量更新消息中是否已经包含了最优申买价位 */
    uint8               HasContainedBestBidLevel;
    /** 当前最优申买价下被连续删除掉的订单笔数 */
    uint8               ContinualDeletedBidOrders;
    /** 买盘需要更新的笔数 (不包括被连续删除掉的订单) */
    uint8               NoBidOrders;
    /** 按64位对齐的填充域 */
    uint8               __filler1;

    /** 最优申卖价 */
    int32               BestOfferPrice;
    /** 增量更新消息中是否已经包含了最优申买价位 */
    uint8               HasContainedBestOfferLevel;
    /** 当前最优申卖价下被连续删除掉的订单笔数 */
    uint8               ContinualDeletedOfferOrders;
    /** 卖盘需要更新的笔数 (不包括被连续删除掉的订单) */
    uint8               NoOfferOrders;
    /** 按64位对齐的填充域 */
    uint8               __filler2;

    /**
     * (发生变更的委托明细中) 待更新或删除的订单位置 (即需要删除和更新的上一次订单的位置)
     * - 订单位置从 0 开始
     * - 小于0, 表示删除该位置的订单
     * - 大于等于0, 表示更新该位置的订单
     * - CHAR_MAX, 表示新增
     */
    int8                OperatorEntryID[MDS_MAX_L2_DISCLOSE_ORDERS_INCREMENTS];

    /** 发生变更的委托明细 (该字段为变长数组, 实际元素数量为: NoBidOrders + NoOfferOrders) */
    int32               OrderQty[MDS_MAX_L2_DISCLOSE_ORDERS_INCREMENTS];

} MdsL2BestOrdersSnapshotIncrementalT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_BEST_ORDERS_SNAPSHOT_INCREMENTAL                 \
        0, \
        0, 0, 0, 0, \
        0, 0, 0, 0, \
        {0}, \
        {0}
/* -------------------------           */


/**
 * Level2 虚拟集合竞价快照消息定义
 */
typedef struct _MdsL2VirtualAuctionPrice {
    /** 产品代码 C6 / C8 (如: '600000' 等) */
    char                SecurityID[MDS_MAX_INSTR_CODE_LEN];

    /**
     * 虚拟未匹配量的方向
     * - 0 = 无未匹配量, 买卖两边的未匹配量都为0
     * - 1 = 买方有未匹配量, 卖方未匹配量为0
     * - 2 = 卖方有未匹配量, 买方未匹配量为0
     * @see eMdsL2VirtualAuctionSideT
     */
    uint8               LeavesSide;
    uint8               __filler[2];            /**< 按64位对齐的填充域 */

    int32               Price;                  /**< 虚拟参考价格 (价格单位精确到元后四位, 即: 1元=10000) */
    int64               VirtualAuctionQty;      /**< 虚拟匹配量 */
    int64               LeavesQty;              /**< 虚拟未匹配量 */
} MdsL2VirtualAuctionPriceT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_VIRTUAL_AUCTION_PRICE                            \
        {0}, 0, {0}, \
        0, 0, 0
/* -------------------------           */


/**
 * Level2 市场总览消息定义
 */
typedef struct _MdsL2MarketOverview {
    int32               OrigDate;               /**< 市场日期 (YYYYMMDD) */
    int32               OrigTime;               /**< 市场时间 (HHMMSSss0, 实际精度为百分之一秒(HHMMSSss)) */

    int32               __exchSendingTime;      /**< 交易所发送时间 (HHMMSS000, 实际精度为秒(HHMMSS)) */
    int32               __mdsRecvTime;          /**< MDS接收到时间 (HHMMSSsss) */
} MdsL2MarketOverviewT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_MARKET_OVERVIEW                                  \
        0, 0, \
        0, 0
/* -------------------------           */


/**
 * Level2 快照行情的完整消息体定义
 */
typedef union _MdsL2SnapshotBody {
    /** Level2 快照行情(股票、债券、基金、期权) */
    MdsL2StockSnapshotBodyT                 l2Stock;
    /** Level2 快照行情的增量更新消息 */
    MdsL2StockSnapshotIncrementalT          l2StockIncremental;

    /** Level2 委托队列(买一／卖一前五十笔委托明细) */
    MdsL2BestOrdersSnapshotBodyT            l2BestOrders;
    /** Level2 委托队列(买一／卖一前五十笔委托明细)的增量更新消息 */
    MdsL2BestOrdersSnapshotIncrementalT     l2BestOrdersIncremental;

    /** 指数行情数据 */
    MdsIndexSnapshotBodyT                   index;

    /** Level2 虚拟集合竞价 (仅上证) */
    MdsL2VirtualAuctionPriceT               l2VirtualAuctionPrice;

    /** Level2 市场总览 (仅上证) */
    MdsL2MarketOverviewT                    l2MarketOverview;
} MdsL2SnapshotBodyT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_SNAPSHOT_BODY                                    \
        {NULLOBJ_MDS_L2_STOCK_SNAPSHOT_BODY}
/* -------------------------           */


/**
 * (对外发布的) 完整的 Level1/Level2 证券行情全幅消息定义
 */
typedef struct _MdsMktDataSnapshot {
    /** 行情数据的消息头 */
    MdsMktDataSnapshotHeadT                 head;

    union {
        /** Level2 快照行情(股票、债券、基金) */
        MdsL2StockSnapshotBodyT             l2Stock;
        /** Level2 快照行情的增量更新消息 */
        MdsL2StockSnapshotIncrementalT      l2StockIncremental;

        /** Level2 委托队列(买一／卖一前五十笔委托明细) */
        MdsL2BestOrdersSnapshotBodyT        l2BestOrders;
        /** Level2 委托队列(买一／卖一前五十笔委托明细)的增量更新消息 */
        MdsL2BestOrdersSnapshotIncrementalT l2BestOrdersIncremental;

        /** Level1 股票、债券、基金行情数据 */
        MdsStockSnapshotBodyT               stock;

        /** Level1/Level2 期权行情数据 */
        MdsStockSnapshotBodyT               option;

        /** Level1/Level2 指数行情数据 */
        MdsIndexSnapshotBodyT               index;

        /** Level2 虚拟集合竞价 (仅上证) */
        MdsL2VirtualAuctionPriceT           l2VirtualAuctionPrice;

        /** Level2 市场总览 (仅上证) */
        MdsL2MarketOverviewT                l2MarketOverview;
    };
} MdsMktDataSnapshotT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_DATA_SNAPSHOT                                   \
        {NULLOBJ_MDS_MKT_DATA_SNAPSHOT_HEAD}, \
        {{NULLOBJ_MDS_L2_STOCK_SNAPSHOT_BODY}}


/* 结构体大小定义 */
#define DATASIZE_MDS_L2_STOCK_SNAPSHOT                                  \
        (sizeof(MdsMktDataSnapshotHeadT) + sizeof(MdsL2StockSnapshotBodyT))

#define DATASIZE_MDS_L2_BEST_ORDERS_SNAPSHOT                            \
        (sizeof(MdsMktDataSnapshotHeadT) + sizeof(MdsL2BestOrdersSnapshotBodyT))

#define DATASIZE_MDS_STOCK_SNAPSHOT                                     \
        (sizeof(MdsMktDataSnapshotHeadT) + sizeof(MdsStockSnapshotBodyT))

#define DATASIZE_MDS_OPTION_SNAPSHOT                                    \
        (sizeof(MdsMktDataSnapshotHeadT) + sizeof(MdsStockSnapshotBodyT))

#define DATASIZE_MDS_INDEX_SNAPSHOT                                     \
        (sizeof(MdsMktDataSnapshotHeadT) + sizeof(MdsIndexSnapshotBodyT))

#define DATASIZE_MDS_L2_VIRTUAL_AUCTION_PRICE                           \
        (sizeof(MdsMktDataSnapshotHeadT) + sizeof(MdsL2VirtualAuctionPriceT))

#define DATASIZE_MDS_L2_MARKET_OVERVIEW                                 \
        (sizeof(MdsMktDataSnapshotHeadT) + sizeof(MdsL2MarketOverviewT))
/* -------------------------           */


/* ===================================================================
 * Level2 逐笔成交/逐笔委托行情消息定义
 * =================================================================== */

/**
 * Level2 逐笔成交行情定义
 */
typedef struct _MdsL2Trade {
    uint8               exchId;                 /**< 交易所代码(沪/深) @see eMdsExchangeIdT */
    uint8               securityType;           /**< 证券类型(股票/期权) @see eMdsSecurityTypeT */
    int8                __isRepeated;           /**< 是否是重复的行情 (内部使用, 小于0表示数据倒流) */
    uint8               __filler1;              /**< 按64位对齐的填充域 */

    int32               tradeDate;              /**< 交易日期 (YYYYMMDD, 非官方数据) */
    int32               TransactTime;           /**< 成交时间 (HHMMSSsss) */

    int32               instrId;                /**< 产品代码 */
    int32               ChannelNo;              /**< 成交通道/频道代码 [0..9999] */
    int32               ApplSeqNum;             /**< 成交序号/消息记录号 (从1开始, 按频道连续) */

    /** 产品代码 C6 / C8 (如: '600000' 等) */
    char                SecurityID[MDS_MAX_INSTR_CODE_LEN];

    /**
     * 成交类别 (仅适用于深交所, '4'=撤销, 'F'=成交)
     * 对于上证, 将固定设置为 'F'(成交)
     * @see eMdsL2TradeExecTypeT
     */
    char                ExecType;

    /**
     * 内外盘标志 (仅适用于上证, 'B'=外盘,主动买, 'S'=内盘,主动卖, 'N'=未知)
     * 对于深交所, 将固定设置为 'N'(未知)
     * @see eMdsL2TradeBSFlagT
     */
    char                TradeBSFlag;

    uint8               __filler3[4];           /**< 按64位对齐的填充域 */
    uint8               __channelNo;            /**< 内部频道号 (供内部使用, 取值范围{1,2,4,8}) */
    uint64              __origTickSeq;          /**< 对应的原始行情的序列号 (内部使用) */

    int32               TradePrice;             /**< 成交价格 (价格单位精确到元后四位, 即: 1元=10000) */
    int32               TradeQty;               /**< 成交数量 (上海债券的数量单位为: 手) */
    int64               TradeMoney;             /**< 成交金额 (金额单位精确到元后四位, 即: 1元=10000) */

    int64               BidApplSeqNum;          /**< 买方订单号 (从 1 开始计数, 0 表示无对应委托) */
    int64               OfferApplSeqNum;        /**< 卖方订单号 (从 1 开始计数, 0 表示无对应委托) */

#ifdef  _MDS_ENABLE_LATENCY_STATS
    /** 消息原始接收时间 (从网络接收到数据的最初时间) */
    STimeval32T         __origNetTime;
    /** 消息实际接收时间 (开始解码等处理之前的时间) */
    STimeval32T         __recvTime;
    /** 消息采集处理完成时间 */
    STimeval32T         __collectedTime;
    /** 消息加工处理完成时间 */
    STimeval32T         __processedTime;
    /** 消息推送时间 (写入推送缓存以后, 实际网络发送之前) */
    STimeval32T         __pushingTime;
#endif

} MdsL2TradeT;


/* 结构体初始化值的尾部填充字段定义 */
#ifdef  _MDS_ENABLE_LATENCY_STATS
# define    __NULLOBJ_MDS_L2_TRADE_TAILER                               \
            , {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
#else
# define    __NULLOBJ_MDS_L2_TRADE_TAILER
#endif


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_TRADE                                            \
        0, 0, 0, 0, \
        0, 0, \
        0, 0, 0, \
        {0}, 0, 0, \
        {0}, 0, 0, \
        0, 0, 0, \
        0, 0 \
        __NULLOBJ_MDS_L2_TRADE_TAILER
/* -------------------------           */


/**
 * Level2 逐笔委托行情定义 (仅适用于深交所)
 */
typedef struct _MdsL2Order {
    uint8               exchId;                 /**< 交易所代码(沪/深) @see eMdsExchangeIdT */
    uint8               securityType;           /**< 证券类型(股票/期权) @see eMdsSecurityTypeT */
    int8                __isRepeated;           /**< 是否是重复的行情 (内部使用, 小于0表示数据倒流) */
    uint8               __filler1;              /**< 按64位对齐的填充域 */

    int32               tradeDate;              /**< 交易日期 YYYYMMDD (自然日) */
    int32               TransactTime;           /**< 委托时间 HHMMSSsss */

    int32               instrId;                /**< 产品代码 */
    int32               ChannelNo;              /**< 频道代码 [0..9999] */
    int32               ApplSeqNum;             /**< 委托序号 (从1开始, 按频道连续) */

    /** 产品代码 C6 / C8 (如: '000001' 等) */
    char                SecurityID[MDS_MAX_INSTR_CODE_LEN];

    /** 买卖方向 ('1'=买 '2'=卖 'G'=借入 'F'=出借) */
    char                Side;

    /** 订单类型 ('1'=市价 '2'=限价 'U'=本方最优) */
    char                OrderType;

    uint8               __filler3[4];           /**< 按64位对齐的填充域 */
    uint8               __channelNo;            /**< 内部频道号 (供内部使用, 取值范围{1,2,4,8}) */
    uint64              __origTickSeq;          /**< 对应的原始行情的序列号 (内部使用) */

    int32               Price;                  /**< 委托价格 (价格单位精确到元后四位, 即: 1元=10000) */
    int32               OrderQty;               /**< 委托数量 */

#ifdef  _MDS_ENABLE_LATENCY_STATS
    /** 消息原始接收时间 (从网络接收到数据的最初时间) */
    STimeval32T         __origNetTime;
    /** 消息实际接收时间 (开始解码等处理之前的时间) */
    STimeval32T         __recvTime;
    /** 消息采集处理完成时间 */
    STimeval32T         __collectedTime;
    /** 消息加工处理完成时间 */
    STimeval32T         __processedTime;
    /** 消息推送时间 (写入推送缓存以后, 实际网络发送之前) */
    STimeval32T         __pushingTime;
#endif

} MdsL2OrderT;


/* 结构体初始化值的尾部填充字段定义 */
#ifdef  _MDS_ENABLE_LATENCY_STATS
# define    __NULLOBJ_MDS_L2_ORDER_TAILER                               \
            , {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
#else
# define    __NULLOBJ_MDS_L2_ORDER_TAILER
#endif


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_ORDER                                            \
        0, 0, 0, 0, \
        0, 0, \
        0, 0, 0, \
        {0}, 0, 0, \
        {0}, 0, 0, \
        0, 0 \
        __NULLOBJ_MDS_L2_ORDER_TAILER
/* -------------------------           */


/**
 * Level2 逐笔数据丢失消息定义
 * 逐笔数据(逐笔成交/逐笔委托)发生了数据丢失, 并且无法重建, 将放弃这些丢失的逐笔数据
 * @depricated 已废弃
 */
typedef struct _MdsL2TickLost {
    uint8               exchId;                 /**< 交易所代码(沪/深) @see eMdsExchangeIdT */
    uint8               __filler3[3];           /**< 按64位对齐的填充域 */

    int32               tradeDate;              /**< 交易日期 YYYYMMDD (自然日) */
    int32               lostTime;               /**< 发生数据丢失的时间 HHMMSSsss */

    int32               channelNo;              /**< 频道代码 */
    int32               beginApplSeqNum;        /**< 已丢失逐笔数据的起始序号 */
    int32               endApplSeqNum;          /**< 已丢失逐笔数据的结束序号 */

    uint64              __origTickSeq;          /**< 对应的原始行情的序列号 (内部使用) */
} MdsL2TickLostT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_L2_TICK_LOST                                        \
        0, {0}, \
        0, 0, \
        0, 0, 0, \
        0
/* -------------------------           */


#ifdef __cplusplus
}
#endif

#endif  /* _MDS_BASE_MODEL_H */
