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
 * @file    mds_mkt_packets.h
 *
 * 订单执行系统订单接收模块通讯报文定义
 *
 * @version 0.8.12      2016/09/13
 * @version 0.12        2016/11/30
 *          - 在登录报文中增加协议版本号信息, 并在登录时校验协议版本号的兼容性
 * @version 0.12.8      2017/03/07
 *          - 重新定义消息类型的取值
 *          - 将证券行情全幅消息拆分为'Level1市场行情快照'、'指数行情快照'、'期权行情快照'三个消息:
 *              - MDS_MSGTYPE_MARKET_DATA_SNAPSHOT_FULL_REFRESH
 *              - MDS_MSGTYPE_INDEX_SNAPSHOT_FULL_REFRESH
 *              - MDS_MSGTYPE_OPTION_SNAPSHOT_FULL_REFRESH
 *          - 调整订阅类型 (TickType) 的枚举类型定义和取值
 *              - 重定义 MDS_TICK_TYPE_LATEST_ONLY -> MDS_TICK_TYPE_LATEST_SIMPLIFIED
 *                  - 只订阅最新的行情快照数据, 并忽略和跳过已经过时的数据 (推送的数据量最小, 不会重复发送最新快照)
 *              - 重定义 MDS_TICK_TYPE_ALL_TICK -> MDS_TICK_TYPE_ALL_INCREMENTS
 *                  - 订阅所有时点的行情快照数据 (需要注意的是, 在该模式下将会额外发送Level2行情快照的增量更新消息)
 *              - 新增 MDS_TICK_TYPE_LATEST_TIMELY
 *                  - 只订阅最新的行情快照数据, 并立即发送最新数据 (会更及时的获取到交易活跃的最新行情, 但会重复发送最新快照)
 *          - 为了方便使用, 扩大每个订阅请求中能同时指定的产品数量至: 4000
 *          - 重新定义了行情订阅消息
 *              - 订阅模式 (@see eMdsSubscribeModeT)
 *              - 市场和证券类型订阅标志 (@see eMdsMktSubscribeFlagT)
 *              - 数据模式 (@see eMdsSubscribedTickTypeT)
 *              - 数据种类 (@see eMdsSubscribeDataTypeT)
 *              - 开始时间 (-1: 从头开始获取, 0: 从最新位置开始获取实时行情, >0: 从指定的起始时间开始获取)
 *              - 在推送实时行情数据之前, 是否需要推送已订阅产品的初始的行情快照
 *          - 调整了行情订阅的应答消息, 将返回各市场的实际订阅结果, 即实际已订阅的产品数量
 * @version 0.15.5.1    2017/11/15
 *          - 增加数值类型的协议版本号 MDS_APPL_VER_VALUE, 以方便比对版本
 * @version 0.15.5.9    2018/05/03
 *          - 增加仅内部使用的消息类型 MDS_MSGTYPE_COMPRESSED_PACKETS (压缩的数据包)
 * @version 0.15.5.10   2018/05/24
 *          - 行情订阅条件和订阅配置中增加 '待订阅的内部频道号', 该字段仅内部使用, 前端不需要关心和处理 (兼容之前版本)
 *          - 行情订阅条件和订阅配置中增加 '逐笔数据的过期时间类型 tickExpireType' (兼容之前版本)
 *          - 协议版本号升级 => 0.15.5.10 (兼容之前版本)
 * @version 0.15.5.11   2018/06/01
 *          - 协议版本号升级 => 0.15.5.11。协议本身与之前的版本不兼容, 但将通过如下方式兼容之前的API:
 *            - 如果使用的是旧版本的API, 那么服务器端将不再推送增量更新消息 (只推送全量快照), 以此来保持兼容
 *            - 如果需要使用增量更新消息的话, 就需要更新到最新版本的API, 否则不需要更新API
 * @version 0.15.5.16   2018/08/23
 *          - 增加可订阅的数据种类 (DataType), 以支持单独订阅指数行情和期权行情
 *              - MDS_SUB_DATA_TYPE_INDEX_SNAPSHOT, 与L1_SNAPSHOT的区别在于, INDEX_SNAPSHOT可以单独订阅指数行情
 *              - MDS_SUB_DATA_TYPE_OPTION_SNAPSHOT, 与L1_SNAPSHOT的区别在于, OPTION_SNAPSHOT可以单独订阅期权行情
 * @version 0.15.6      2018/05/18
 *          - 调整行情订阅消息中可订阅的数据种类 (DataType) 的枚举类型定义和取值
 *              - 新增 MDS_SUB_DATA_TYPE_DEFAULT (默认数据类型)
 * @since   2016/01/03
 */


#ifndef _MDS_MKT_PACKETS_H
#define _MDS_MKT_PACKETS_H


#include    <mds_global/mds_base_model.h>
#include    <mds_global/mds_qry_packets.h>
#include    <sutil/net/spk_global_packet.h>


#ifdef __cplusplus
extern "C" {
#endif


/* ===================================================================
 * 协议版本号定义
 * =================================================================== */

/** 当前采用的协议版本号 */
#define MDS_APPL_VER_ID                         "0.15.7.4"

/**
 * 当前采用的协议版本号数值
 * - 版本号数值的格式为 10 位整型数值, 形如: 1AABBCCDDX, 其中:
 *   - AA 为主版本号
 *   - BB 为副版本号
 *   - CC 为发布号
 *   - DD 为构建号
 *   - X  0, 表示不带时间戳的正常版本; 1, 表示带时间戳的延迟测量版本
 */
#define MDS_APPL_VER_VALUE                      (1001507041)

/** 兼容的最低协议版本号 */
#define MDS_MIN_APPL_VER_ID                     "0.15.5"
/* -------------------------           */


/* ===================================================================
 * 消息代码及报文中的枚举类型定义
 * =================================================================== */

/**
 * 通信消息的消息类型定义
 */
typedef enum _eMdsMsgType {
    /*
     * 会话类消息
     */
    /** 心跳消息 (1/0x01) */
    MDS_MSGTYPE_HEARTBEAT                           = 1,
    /** 测试请求消息 (2/0x02) */
    MDS_MSGTYPE_TEST_REQUEST                        = 2,
    /** 登录消息 (3/0x03) */
    MDS_MSGTYPE_LOGON                               = 3,
    /** 注销消息 (4/0x04) */
    MDS_MSGTYPE_LOGOUT                              = 4,
    /** 证券行情订阅消息 (5/0x05) */
    MDS_MSGTYPE_MARKET_DATA_REQUEST                 = 5,
    /** 压缩的数据包 (6/0x06, 内部使用) */
    MDS_MSGTYPE_COMPRESSED_PACKETS                  = 6,
    /** 最大的会话消息类型 */
    __MDS_MSGTYPE_SESSION_MAX,

    /*
     * Level1 行情消息
     */
    /** Level1 市场行情快照 (10/0x0A) */
    MDS_MSGTYPE_MARKET_DATA_SNAPSHOT_FULL_REFRESH   = 10,
    /** Level1/Level2 指数行情快照 (11/0x0B) */
    MDS_MSGTYPE_INDEX_SNAPSHOT_FULL_REFRESH         = 11,
    /** Level1/Level2 期权行情快照 (12/0x0C) */
    MDS_MSGTYPE_OPTION_SNAPSHOT_FULL_REFRESH        = 12,

    /** 市场状态消息 (13/0x0D, 仅上海) */
    MDS_MSGTYPE_TRADING_SESSION_STATUS              = 13,
    /** 证券状态消息 (14/0x0E, 仅深圳) */
    MDS_MSGTYPE_SECURITY_STATUS                     = 14,
    /** 最大的Level-1行情消息类型 */
    __MDS_MSGTYPE_L1_MAX,

    /*
     * Level2 行情消息
     */
    /** Level2 市场行情快照 (20/0x14) */
    MDS_MSGTYPE_L2_MARKET_DATA_SNAPSHOT             = 20,
    /** Level2 委托队列快照 (买一/卖一前五十笔) (21/0x15) */
    MDS_MSGTYPE_L2_BEST_ORDERS_SNAPSHOT             = 21,

    /** Level2 逐笔成交行情 (22/0x16) */
    MDS_MSGTYPE_L2_TRADE                            = 22,
    /** Level2 逐笔委托行情 (23/0x17, 仅深圳) */
    MDS_MSGTYPE_L2_ORDER                            = 23,

    /** Level2 快照行情的增量更新消息 (24/0x18, 仅上海) */
    MDS_MSGTYPE_L2_MARKET_DATA_INCREMENTAL          = 24,
    /** Level2 委托队列快照的增量更新消息 (25/0x19, 仅上海) */
    MDS_MSGTYPE_L2_BEST_ORDERS_INCREMENTAL          = 25,

    /** Level2 市场总览消息 (26/0x1A, 仅上海) */
    MDS_MSGTYPE_L2_MARKET_OVERVIEW                  = 26,
    /** Level2 虚拟集合竞价消息 (27/0x1B, 仅上海) */
    MDS_MSGTYPE_L2_VIRTUAL_AUCTION_PRICE            = 27,
    /** 最大的Level-2行情消息类型 */
    __MDS_MSGTYPE_L2_MAX,

    /*
     * 查询类消息
     */
    /** 查询证券行情 (80/0x50) */
    MDS_MSGTYPE_QRY_MARKET_DATA_SNAPSHOT            = 80,
    /** 查询(深圳)证券状态 (81/0x51) */
    MDS_MSGTYPE_QRY_SECURITY_STATUS                 = 81,
    /** 查询(上证)市场状态 (82/0x52) */
    MDS_MSGTYPE_QRY_TRADING_SESSION_STATUS          = 82,
    /** 最大的查询消息类型 */
    __MDS_MSGTYPE_QRY_MAX

} eMdsMsgTypeT;
/* -------------------------           */


/**
 * 订阅模式 (SubMode) 定义
 * - 0: (Set) 重新订阅, 设置为订阅列表中的股票
 * - 1: (Append) 追加订阅, 增加订阅列表中的股票
 * - 2: (Delete) 删除订阅, 删除订阅列表中的股票
 */
typedef enum _eMdsSubscribeMode {
    /** 重新订阅, 设置为订阅列表中的股票 */
    MDS_SUB_MODE_SET                            = 0,

    /** 追加订阅, 增加订阅列表中的股票 */
    MDS_SUB_MODE_APPEND                         = 1,

    /** 删除订阅, 删除订阅列表中的股票 */
    MDS_SUB_MODE_DELETE                         = 2,

    __MAX_MDS_SUB_MODE
} eMdsSubscribeModeT;
/* -------------------------           */


/**
 * 市场-产品类型订阅标志 (SubFlag) 定义
 * -  0: (Default) 根据订阅列表订阅产品行情
 * -  1: (All) 订阅该市场和证券类型下的所有产品行情 (为兼容之前的版本, 也可以赋值为 -1)
 * -  2: (Disable) 禁用该市场下的所有产品行情
 */
typedef enum _eMdsMktSubscribeFlag {
    /** 根据订阅列表订阅产品行情 */
    MDS_MKT_SUB_FLAG_DEFAULT                    = 0,

    /** 订阅该市场和证券类型下的所有产品行情 */
    MDS_MKT_SUB_FLAG_ALL                        = 1,

    /** 禁用该市场下的所有产品行情 */
    MDS_MKT_SUB_FLAG_DISABLE                    = 2,

    __MAX_MDS_MKT_SUB_FLAG
} eMdsMktSubscribeFlagT;
/* -------------------------           */


/**
 * 数据模式 (TickType) 定义 (仅对快照行情生效)
 * 订阅最新的行情快照还是所有时点的行情快照
 * -  0: (LatestSimplified) 只订阅最新的行情快照数据, 并忽略和跳过已经过时的数据
 *       - 该模式推送的数据量最小, 没有重复数据, 也不会重复发送最新快照
 *       - 该模式在时延和带宽方面都相对优秀, 如果没有特殊需求, 推荐使用该模式即可
 * -  1: (LatestTimely) 只订阅最新的行情快照数据, 并立即发送最新数据
 *       - 只要有行情更新事件, 便立即推送该产品的最新行情, 但也会因此重复发送多次相同的最新行情
 *       - 如果某些产品的交易很活跃, 而客户端处理又比较耗时的话, 那么该模式可能会更及时的获取到
 *         这些产品的最新行情
 *       - 此外, 因为与 AllIncrements 模式下的数据一一对应, 可以方便与增量更新消息进行比对测试
 *       - 通常情况下, 推荐使用 LatestSimplified 模式即可
 * -  2: (AllIncrements) 订阅所有时点的行情快照数据 (包括Level2增量更新消息)
 *       - 该模式会推送所有时点的行情数据, 包括Level2行情快照的增量更新消息
 *       - 如果需要获取全量的行情明细, 或者需要直接使用Level2的增量更新消息, 可以使用该模式
 */
typedef enum _eMdsSubscribedTickType {
    /** 只订阅最新的行情快照数据, 并忽略和跳过已经过时的数据 (推送的数据量最小, 不会重复发送最新快照) */
    MDS_TICK_TYPE_LATEST_SIMPLIFIED             = 0,

    /** 只订阅最新的行情快照数据, 并立即发送最新数据 (会更及时的获取到交易活跃的最新行情, 但会重复发送最新快照) */
    MDS_TICK_TYPE_LATEST_TIMELY                 = 1,

    /** 订阅所有时点的行情快照数据 (包括Level2增量更新消息) */
    MDS_TICK_TYPE_ALL_INCREMENTS                = 2,

    __MAX_MDS_TICK_TYPE
} eMdsSubscribedTickTypeT;
/* -------------------------           */


/**
 * 逐笔数据的过期时间定义 (仅对逐笔数据生效)
 */
typedef enum _eMdsSubscribedTickExpireType {
    /** 不过期 */
    MDS_TICK_EXPIRE_TYPE_NONE                   = 0,

    /** 立即过期 (1秒, 如果逐笔数据时间落后于最新快照时间1秒, 则视为过期并跳过该逐笔数据) */
    MDS_TICK_EXPIRE_TYPE_IMMEDIATE              = 1,

    /** 及时过期 (3秒, 如果逐笔数据时间落后于最新快照时间3秒, 则视为过期并跳过该逐笔数据) */
    MDS_TICK_EXPIRE_TYPE_TIMELY                 = 2,

    /** 超时过期 (30秒, 如果逐笔数据时间落后于最新快照时间30秒, 则视为过期并跳过该逐笔数据) */
    MDS_TICK_EXPIRE_TYPE_TIMEOUT                = 3,

    __MAX_MDS_TICK_EXPIRE_TYPE
} eMdsSubscribedTickExpireTypeT;
/* -------------------------           */


/**
 * 可订阅的数据种类 (DataType) 定义
 * - 0:      默认数据种类 (所有)
 * - 0x0001: L1快照/指数/期权
 * - 0x0002: L2快照
 * - 0x0004: L2委托队列
 * - 0x0008: L2逐笔成交
 * - 0x0010: L2逐笔委托 (深圳)
 * - 0x0020: L2虚拟集合竞价 (上海)
 * - 0x0040: L2市场总览 (上海)
 * - 0x0100: 市场状态 (上海)
 * - 0x0200: 证券实时状态 (深圳)
 * - 0x0400: 指数行情 (与0x0001的区别在于, 0x0400可以单独订阅指数行情)
 * - 0x0800: 期权行情 (与0x0001的区别在于, 0x0800可以单独订阅期权行情)
 * - 0xFFFF: 所有数据种类
 */
typedef enum _eMdsSubscribeDataType {
    /** 默认数据种类 (所有种类) */
    MDS_SUB_DATA_TYPE_DEFAULT                   = 0,

    /** L1快照/指数/期权 (L1快照行情 + 指数行情 + 期权行情) */
    MDS_SUB_DATA_TYPE_L1_SNAPSHOT               = 0x0001,

    /** L2快照 */
    MDS_SUB_DATA_TYPE_L2_SNAPSHOT               = 0x0002,

    /** L2委托队列 */
    MDS_SUB_DATA_TYPE_L2_BEST_ORDERS            = 0x0004,

    /** L2逐笔成交 */
    MDS_SUB_DATA_TYPE_L2_TRADE                  = 0x0008,

    /** L2逐笔委托 (*深圳, 0x10/16) */
    MDS_SUB_DATA_TYPE_L2_ORDER                  = 0x0010,

    /** L2虚拟集合竞价 (*上海, 0x20/32) */
    MDS_SUB_DATA_TYPE_L2_VIRTUAL_AUCTION_PRICE  = 0x0020,

    /** L2市场总览 (*上海, 0x40/64) */
    MDS_SUB_DATA_TYPE_L2_MARKET_OVERVIEW        = 0x0040,

    /** 市场状态 (*上海, 0x100/256) */
    MDS_SUB_DATA_TYPE_TRADING_SESSION_STATUS    = 0x0100,

    /** 证券实时状态 (*深圳, 0x200/512) */
    MDS_SUB_DATA_TYPE_SECURITY_STATUS           = 0x0200,

    /** 指数行情 (与L1_SNAPSHOT的区别在于, INDEX_SNAPSHOT可以单独订阅指数行情) */
    MDS_SUB_DATA_TYPE_INDEX_SNAPSHOT            = 0x400,

    /** 期权行情 (与L1_SNAPSHOT的区别在于, OPTION_SNAPSHOT可以单独订阅期权行情) */
    MDS_SUB_DATA_TYPE_OPTION_SNAPSHOT           = 0x800,

    /** 空数据种类 (可用于不指定任何数量种类的情况) */
    MDS_SUB_DATA_TYPE_NONE                      = 0x8000,

    /** 所有数据种类 */
    MDS_SUB_DATA_TYPE_ALL                       = 0xFFFF,

    __MAX_MDS_SUB_DATA_TYPE                     = 0x7FFFFFFF
} eMdsSubscribeDataTypeT;
/* -------------------------           */


/**
 * 可订阅的内部数据频道定义 (供内部使用, 尚未对外开放)
 */
typedef enum _eMdsSubscribedChannelNo {
    /** 默认频道 (所有频道) */
    MDS_CHANNEL_NO_DEFAULT                      = 0,

    /** 频道1 */
    MDS_CHANNEL_NO_ONE                          = 0x01,
    /** 频道2 */
    MDS_CHANNEL_NO_TWO                          = 0x02,
    /** 频道3 */
    MDS_CHANNEL_NO_THREE                        = 0x04,
    /** 频道4 */
    MDS_CHANNEL_NO_FOUR                         = 0x08,

    /** 所有频道 */
    MDS_CHANNEL_NO_ALL                          = 0x0F,
    /** 空数据频道 (可用于不订阅任何频道的情况) */
    MDS_CHANNEL_NO_NONE                         = 0x80
} eMdsTickChannelNoT;
/* -------------------------           */


/* ===================================================================
 * 会话消息报文定义
 * =================================================================== */

/**
 * 登录请求报文
 */
typedef struct _MdsLogonReq {
    /** 加密方法 */
    int32               encryptMethod;
    /** 心跳间隔, 单位为秒 (有效范围为 [10~300], 若取值小于0则赋值为默认值30秒, 否则设置为最大/最小值) */
    int32               heartBtInt;

    /** 用户名 */
    char                username[MDS_MAX_USERNAME_LEN];
    /** 密码 */
    char                password[MDS_MAX_PASSWORD_LEN];
    /** 客户端采用的协议版本号 */
    char                applVerId[MDS_VER_ID_MAX_LEN];
} MdsLogonReqT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_LOGON_REQ                   \
        0, 0, {0}, {0}, {0}
/* -------------------------           */


/**
 * 登录请求的应答报文
 */
typedef struct _MdsLogonRsp {
    /** 加密方法 */
    int32               encryptMethod;
    /** 心跳间隔, 单位为秒 */
    int32               heartBtInt;

    /** 用户名 */
    char                username[MDS_MAX_USERNAME_LEN];
    /** 服务器端采用的协议版本号 */
    char                applVerId[MDS_VER_ID_MAX_LEN];
    /** 服务器端兼容的最低协议版本号 */
    char                minVerId[MDS_VER_ID_MAX_LEN];

    uint8               hostNum;                /**< 服务端主机编号 */
    uint8               isLeader;               /**< 是否是'主节点' */
    uint8               leaderHostNum;          /**< '主节点'的主机编号 */
    uint8               __filler[5];            /**< 按64位对齐的填充域 */
} MdsLogonRspT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_LOGON_RSP                   \
        0, 0, {0}, {0}, {0}, \
        0, 0, 0, {0}
/* -------------------------           */


/**
 * 行情订阅请求的订阅产品条目
 */
typedef struct _MdsMktDataRequestEntry {
    uint8               exchId;                 /**< 交易所代码 @see eMdsExchangeIdT */
    uint8               securityType;           /**< 证券类型 @see eMdsSecurityTypeT */
    uint8               __filler[2];            /**< 按64位对齐的填充域 */
    int32               instrId;                /**< 产品代码 */
} MdsMktDataRequestEntryT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_DATA_REQUEST_ENTRY      \
        0, 0, {0}, 0
/* -------------------------           */


/**
 * 行情订阅请求报文
 *
 * - 对于可同时订阅产品数量有如下限制:
 *   - 每个订阅请求中最多能同时指定 4000 只产品, 可以通过追加订阅的方式订阅更多数量的产品
 *   - 对于沪/深两市的现货产品没有总订阅数量的限制, 可以订阅任意数量的产品
 *   - 对于沪/深两市的期权产品, 限制对每个市场最多允许同时订阅 2000 只期权产品
 *
 * @see MdsMktDataRequestEntryT
 */
typedef struct _MdsMktDataRequestReq {
    /**
     * 订阅模式
     * - 0: (Set) 重新订阅, 设置为订阅列表中的股票
     * - 1: (Append) 追加订阅, 增加订阅列表中的股票
     * - 2: (Delete) 删除订阅, 删除订阅列表中的股票
     *
     * @see eMdsSubscribeModeT
     */
    uint8               subMode;

    /**
     * 数据模式, 订阅最新的行情快照还是所有时点的数据
     * -  0: (LatestSimplified) 只订阅最新的行情快照数据, 并忽略和跳过已经过时的数据
     *       - 该模式推送的数据量最小, 没有重复数据, 也不会重复发送最新快照
     *       - 该模式在时延和带宽方面都相对优秀, 如果没有特殊需求, 推荐使用该模式即可
     * -  1: (LatestTimely) 只订阅最新的行情快照数据, 并立即发送最新数据
     *       - 只要有行情更新事件, 便立即推送该产品的最新行情, 但也会因此重复发送多次相同的最新行情
     *       - 如果某些产品的交易很活跃, 而客户端处理又比较耗时的话, 那么该模式可能会更及时的获取到
     *         这些产品的最新行情
     *       - 此外, 因为与 AllIncrements 模式下的数据一一对应, 可以方便与增量更新消息进行比对测试
     *       - 通常情况下, 推荐使用 LatestSimplified 模式即可
     * -  2: (AllIncrements) 订阅所有时点的行情快照数据 (包括Level2增量更新消息)
     *       - 该模式会推送所有时点的行情数据, 包括Level2行情快照的增量更新消息
     *       - 如果需要获取全量的行情明细, 或者需要直接使用Level2的增量更新消息, 可以使用该模式
     *
     * @see eMdsSubscribedTickTypeT
     */
    uint8               tickType;

    /**
     * 上证股票(债券/基金)产品的订阅标志
     * -  0: (Default) 根据订阅列表订阅产品行情
     * -  1: (All) 订阅该市场和证券类型下的所有产品行情 (为兼容之前的版本, 也可以赋值为 -1)
     * -  2: (Disable) 禁用该市场下的所有股票/债券/基金行情
     *
     * @see eMdsMktSubscribeFlagT
     */
    int8                sseStockFlag;

    /**
     * 上证指数产品的订阅标志
     * -  0: (Default) 根据订阅列表订阅产品行情
     * -  1: (All) 订阅该市场和证券类型下的所有产品行情
     * -  2: (Disable) 禁用该市场下的所有指数行情
     *
     * @see eMdsMktSubscribeFlagT
     */
    int8                sseIndexFlag;

    /**
     * 上证期权产品的订阅标志
     * -  0: (Default) 根据订阅列表订阅产品行情
     * -  1: (All) 订阅该市场和证券类型下的所有产品行情
     * -  2: (Disable) 禁用该市场下的所有期权行情
     *
     * @see eMdsMktSubscribeFlagT
     */
    int8                sseOptionFlag;

    /**
     * 深圳股票(债券/基金)产品的订阅标志
     * -  0: (Default) 根据订阅列表订阅产品行情
     * -  1: (All) 订阅该市场和证券类型下的所有产品行情
     * -  2: (Disable) 禁用该市场下的所有股票/债券/基金行情
     *
     * @see eMdsMktSubscribeFlagT
     */
    int8                szseStockFlag;

    /**
     * 深圳指数产品的订阅标志
     * -  0: (Default) 根据订阅列表订阅产品行情
     * -  1: (All) 订阅该市场和证券类型下的所有产品行情
     * -  2: (Disable) 禁用该市场下的所有指数行情
     *
     * @see eMdsMktSubscribeFlagT
     */
    int8                szseIndexFlag;

    /**
     * 深圳期权产品的订阅标志
     * -  0: (Default) 根据订阅列表订阅产品行情
     * -  1: (All) 订阅该市场和证券类型下的所有产品行情
     * -  2: (Disable) 禁用该市场下的所有期权行情
     *
     * @see eMdsMktSubscribeFlagT
     */
    int8                szseOptionFlag;

    /**
     * 在推送实时行情数据之前, 是否需要推送已订阅产品的初始的行情快照
     * 该参数只在初始订阅时有效, 会话过程中的实时订阅将不支持该参数, 也不会再次推送初始行情快照
     * -  0: 不需要
     * -  1: 需要, 即确保客户端可以至少收到一幅已订阅产品的快照行情 (如果有的话)
     */
    uint8               isRequireInitialMktData;

    /** 待订阅的内部频道号 (供内部使用, 尚未对外开放) */
    uint8               __channelNos;

    /**
     * 逐笔数据的过期时间类型
     * -  0: 不过期
     * -  1: 立即过期 (1秒, 若落后于快照1秒则视为过期)
     * -  2: 及时过期 (3秒)
     * -  3: 超时过期 (30秒)
     *
     * @see     eMdsSubscribedTickExpireTypeT
     * @note    因为存在不可控的网络因素, 所以做不到百分百的精确过滤, 如果对数据的
     *          时效性有精确要求, 就需要在前端对数据再进行一次过滤
     */
    uint8               tickExpireType;

    /** 按64位对齐的填充域 */
    uint8               __filler;

    /**
     * 订阅的数据种类
     * - 0:      默认数据种类 (所有)
     * - 0x0001: L1快照/指数/期权
     * - 0x0002: L2快照
     * - 0x0004: L2委托队列
     * - 0x0008: L2逐笔成交
     * - 0x0010: L2逐笔委托 (深圳)
     * - 0x0020: L2虚拟集合竞价 (上海)
     * - 0x0040: L2市场总览 (上海)
     * - 0x0100: 市场状态 (上海)
     * - 0x0200: 证券实时状态 (深圳)
     * - 0x0400: 指数行情 (与0x0001的区别在于, 0x0400可以单独订阅指数行情)
     * - 0x0800: 期权行情 (与0x0001的区别在于, 0x0800可以单独订阅期权行情)
     * - 0xFFFF: 所有数据
     *
     * @see eMdsSubscribeDataTypeT
     */
    int32               dataTypes;

    /**
     * 请求订阅的行情数据的起始时间 (格式为: HHMMSS 或 HHMMSSsss)
     * - -1: 从头开始获取
     * -  0: 从最新位置开始获取实时行情
     * - >0: 从指定的起始时间开始获取 (HHMMSS / HHMMSSsss)
     * - 对于应答数据, 若为 0 则表示当前没有比起始时间更加新的行情数据
     */
    int32               beginTime;

    /**
     * 本次订阅的产品数量 (订阅列表中的产品数量)
     * - 该字段表示后续报文为subSecurityCnt个订阅产品条目结构体, 通过这样的方式可以实现同
     *   时订阅多只产品的行情快照
     * - 每个订阅请求中最多能同时指定 4000 只产品, 可以通过追加订阅的方式订阅更多数量的产品
     * - 订阅产品总数量的限制如下:
     *   - 对于沪/深两市的现货产品没有订阅数量限制, 可以订阅任意数量的产品
     *   - 对于沪/深两市的期权产品, 限制对每个市场最多允许同时订阅 2000 只期权产品
     *
     * @see MdsMktDataRequestEntryT
     */
    int32               subSecurityCnt;

    /*
     * 后续报文为 subSecurityCnt 个订阅产品条目结构体
     * @see MdsMktDataRequestEntryT
     */

} MdsMktDataRequestReqT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_DATA_REQUEST_REQ        \
        0, 0, \
        0, 0, 0, 0, 0, 0, \
        0, 0, 0, 0, \
        0, 0, 0
/* -------------------------           */


/**
 * 完整的行情订阅请求报文缓存
 */
typedef struct _MdsMktDataRequestReqBuf {
    /** 行情订阅请求 */
    MdsMktDataRequestReqT           mktDataRequestReq;

    /* 订阅产品列表 */
    MdsMktDataRequestEntryT         entries[MDS_MAX_SECURITY_CNT_PER_SUBSCRIBE];
} MdsMktDataRequestReqBufT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_DATA_REQUEST_REQ_BUF    \
        {NULLOBJ_MDS_MKT_DATA_REQUEST_REQ}, \
        {{NULLOBJ_MDS_MKT_DATA_REQUEST_ENTRY}}
/* -------------------------           */


/**
 * 行情订阅请求的应答报文
 */
typedef struct _MdsMktDataRequestRsp {
    /**
     * 订阅模式
     * - 0: (Set) 重新订阅, 设置为订阅列表中的股票
     * - 1: (Append) 追加订阅, 增加订阅列表中的股票
     * - 2: (Delete) 删除订阅, 删除订阅列表中的股票
     *
     * @see eMdsSubscribeModeT
     */
    uint8               subMode;

    /**
     * 数据模式, 订阅最新的行情快照还是所有时点的数据
     * -  0: (LatestSimplified) 只订阅最新的行情快照数据, 并忽略和跳过已经过时的数据
     *       - 该模式推送的数据量最小, 没有重复数据, 也不会重复发送最新快照
     *       - 该模式在时延和带宽方面都相对优秀, 如果没有特殊需求, 推荐使用该模式即可
     * -  1: (LatestTimely) 只订阅最新的行情快照数据, 并立即发送最新数据
     *       - 只要有行情更新事件, 便立即推送该产品的最新行情, 但也会因此重复发送多次相同的最新行情
     *       - 如果某些产品的交易很活跃, 而客户端处理又比较耗时的话, 那么该模式可能会更及时的获取到
     *         这些产品的最新行情
     *       - 此外, 因为与 AllIncrements 模式下的数据一一对应, 可以方便与增量更新消息进行比对测试
     *       - 通常情况下, 推荐使用 LatestSimplified 模式即可
     * -  2: (AllIncrements) 订阅所有时点的行情快照数据 (包括Level2增量更新消息)
     *       - 该模式会推送所有时点的行情数据, 包括Level2行情快照的增量更新消息
     *       - 如果需要获取全量的行情明细, 或者需要直接使用Level2的增量更新消息, 可以使用该模式
     *
     * @see eMdsSubscribedTickTypeT
     */
    uint8               tickType;

    /**
     * 在推送实时行情数据之前, 是否需要推送已订阅产品的初始的行情快照
     * 该参数只在初始订阅时有效, 会话过程中的实时订阅将不支持该参数, 也不会再次推送初始行情快照
     */
    uint8               isRequireInitialMktData;

    /** 订阅的内部频道号 (供内部使用, 尚未对外开放) */
    uint8               __channelNos;

    /**
     * 逐笔数据的过期时间类型
     * -  0: 不过期
     * -  1: 立即过期 (1秒, 若落后于快照1秒则视为过期)
     * -  2: 及时过期 (3秒)
     * -  3: 超时过期 (30秒)
     *
     * @see     eMdsSubscribedTickExpireTypeT
     * @note    因为存在不可控的网络因素, 所以做不到百分百的精确过滤, 如果对数据的
     *          时效性有精确要求, 就需要在前端对数据再进行一次过滤
     */
    uint8               tickExpireType;

    /** 按64位对齐的填充域 */
    uint8               __filler[3];

    /**
     * 订阅的数据种类
     * - 0:      默认数据种类 (所有)
     * - 0x0001: L1快照/指数/期权
     * - 0x0002: L2快照
     * - 0x0004: L2委托队列
     * - 0x0008: L2逐笔成交
     * - 0x0010: L2逐笔委托 (深圳)
     * - 0x0020: L2虚拟集合竞价 (上海)
     * - 0x0040: L2市场总览 (上海)
     * - 0x0100: 市场状态 (上海)
     * - 0x0200: 证券实时状态 (深圳)
     * - 0x0400: 指数行情 (与0x0001的区别在于, 0x0400可以单独订阅指数行情)
     * - 0x0800: 期权行情 (与0x0001的区别在于, 0x0800可以单独订阅期权行情)
     * - 0xFFFF: 所有数据
     *
     * @see eMdsSubscribeDataTypeT
     */
    int32               dataTypes;

    /**
     * 请求订阅的行情数据的起始时间 (格式为: HHMMSS 或 HHMMSSsss)
     * - -1: 从头开始获取
     * -  0: 从最新位置开始获取实时行情
     * - >0: 从指定的起始时间开始获取 (HHMMSS / HHMMSSsss)
     * - 对于应答数据, 若为 0 则表示当前没有比起始时间更加新的行情数据
     */
    int32               beginTime;

    /**
     * 上证股票(债券/基金)产品的订阅结果 (实际已订阅的产品数量)
     * - -1: 订阅了所有产品;
     * -  0: 未订阅或已禁用;
     * - >0: 已订阅的产品数量(按照订阅列表成功订阅的产品数量)
     */
    int32               sseStockSubscribed;

    /**
     * 上证指数产品的订阅结果 (实际已订阅的产品数量)
     * - -1: 订阅了所有产品;
     * -  0: 未订阅或已禁用;
     * - >0: 已订阅的产品数量(按照订阅列表成功订阅的产品数量)
     */
    int32               sseIndexSubscribed;

    /**
     * 上证期权产品的订阅结果 (实际已订阅的产品数量)
     * - -1: 订阅了所有产品;
     * -  0: 未订阅或已禁用;
     * - >0: 已订阅的产品数量(按照订阅列表成功订阅的产品数量)
     */
    int32               sseOptionSubscribed;

    /**
     * 深圳股票(债券/基金)产品的订阅结果 (实际已订阅的产品数量)
     * - -1: 订阅了所有产品;
     * -  0: 未订阅或已禁用;
     * - >0: 已订阅的产品数量(按照订阅列表成功订阅的产品数量)
     */
    int32               szseStockSubscribed;

    /**
     * 深圳指数产品的订阅结果 (实际已订阅的产品数量)
     * - -1: 订阅了所有产品;
     * -  0: 未订阅或已禁用;
     * - >0: 已订阅的产品数量(按照订阅列表成功订阅的产品数量)
     */
    int32               szseIndexSubscribed;

    /**
     * 深圳期权产品的订阅结果 (实际已订阅的产品数量)
     * - -1: 订阅了所有产品;
     * -  0: 未订阅或已禁用;
     * - >0: 已订阅的产品数量(按照订阅列表成功订阅的产品数量)
     */
    int32               szseOptionSubscribed;

} MdsMktDataRequestRspT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_DATA_REQUEST_RSP        \
        0, 0, 0, 0, 0, {0}, \
        0, 0, \
        0, 0, 0, 0, 0, 0
/* -------------------------           */


/**
 * 测试请求报文
 */
typedef struct _MdsTestRequestReq {
    /** 测试请求标识符 */
    char                testReqId[MDS_MAX_TEST_REQ_ID_LEN];

    /** 发送时间 (timeval结构或形如'YYYYMMDD-HH:mm:SS.sss'的字符串) */
    char                sendTime[MDS_MAX_SENDING_TIME_LEN];

    /** 按64位对齐的填充域 */
    char                __filler[2];
} MdsTestRequestReqT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_TEST_REQUEST_REQ            \
        {0}, {0}, {0}
/* -------------------------           */


/**
 * 测试请求的应答报文
 */
typedef struct _MdsTestRequestRsp {
    /** 测试请求标识符 */
    char                testReqId[MDS_MAX_TEST_REQ_ID_LEN];

    /** 测试请求的原始发送时间 (timeval结构或形如'YYYYMMDD-HH:mm:SS.sss'的字符串) */
    char                origSendTime[MDS_MAX_SENDING_TIME_LEN];

    /** 按64位对齐的填充域 */
    char                __filler1[2];

    /** 测试请求应答的发送时间 (timeval结构或形如'YYYYMMDD-HH:mm:SS.sss'的字符串) */
    char                respTime[MDS_MAX_SENDING_TIME_LEN];

    /** 按64位对齐的填充域 */
    char                __filler2[2];

#ifdef  _MDS_ENABLE_LATENCY_STATS
    /** 消息实际接收时间 (开始解码等处理之前的时间) */
    STimeval32T         __recvTime;
    /** 消息采集处理完成时间 */
    STimeval32T         __collectedTime;
    /** 消息推送时间 (写入推送缓存以后, 实际网络发送之前) */
    STimeval32T         __pushingTime;
#endif

} MdsTestRequestRspT;


/* 结构体初始化值的尾部填充字段定义 */
#ifdef  _MDS_ENABLE_LATENCY_STATS
# define    __NULLOBJ_MDS_TEST_REQUEST_RSP_TAILER       \
            , {0, 0}, {0, 0}, {0, 0}
#else
# define    __NULLOBJ_MDS_TEST_REQUEST_RSP_TAILER
#endif


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_TEST_REQUEST_RSP                    \
        {0}, {0}, {0}, {0}, {0} \
        __NULLOBJ_MDS_TEST_REQUEST_RSP_TAILER
/* -------------------------           */


/* ===================================================================
 * 汇总的请求/应答消息定义
 * =================================================================== */

/**
 * 汇总的请求消息的消息体定义
 */
typedef union _MdsMktReqMsgBody {
    /** 完整的行情订阅请求报文缓存 */
    MdsMktDataRequestReqBufT        wholeMktDataReqBuf;
    /** 行情订阅请求报文 */
    MdsMktDataRequestReqT           mktDataRequestReq;
    /** 测试请求报文 */
    MdsTestRequestReqT              testRequestReq;
    /** 登录请求报文 */
    MdsLogonReqT                    logonReq;

    /** 证券行情查询请求 */
    MdsQryMktDataSnapshotReqT       qryMktDataSnapshotReq;
    /** (深圳)证券实时状态查询请求 */
    MdsQrySecurityStatusReqT        qrySecurityStatusReq;
    /** (上证)市场状态查询请求 */
    MdsQryTrdSessionStatusReqT      qryTrdSessionStatusReq;
} MdsMktReqMsgBodyT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_REQ_MSG_BODY            \
        {NULLOBJ_MDS_MKT_DATA_REQUEST_REQ_BUF}
/* -------------------------           */


/**
 * 汇总的应答消息的消息体定义
 */
typedef union _MdsMktRspMsgBody {
    /** 行情订阅请求的应答报文 */
    MdsMktDataRequestRspT           mktDataRequestRsp;
    /** 测试请求的应答报文 */
    MdsTestRequestRspT              testRequestRsp;
    /** 登录请求的应答报文 */
    MdsLogonRspT                    logonRsp;

    /** 证券行情全幅消息 */
    MdsMktDataSnapshotT             mktDataSnapshot;
    /** Level2 逐笔成交行情 */
    MdsL2TradeT                     trade;
    /** Level2 逐笔委托行情 */
    MdsL2OrderT                     order;
    /** Level2 逐笔数据丢失消息 @depricated 已废弃 */
    MdsL2TickLostT                  tickLost;

    /** 市场状态消息 */
    MdsTradingSessionStatusMsgT     trdSessionStatus;
    /** 证券实时状态消息 */
    MdsSecurityStatusMsgT           securityStatus;
} MdsMktRspMsgBodyT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_MKT_RSP_MSG_BODY            \
        {NULLOBJ_MDS_MKT_DATA_REQUEST_RSP}
/* -------------------------           */


/* ===================================================================
 * 组播消息的报文头定义
 * =================================================================== */

/**
 * 组播消息的报文头
 * 组播报文的组成结构为:
 * - 报文头
 * - 报文体
 *   - 消息头 1 + 消息体 1
 *   - 消息头 2 + 消息体 2
 *   - 消息头 3 + 消息体 3
 *   - ...
 *   - 目前一个组播报文中最多包含3条行情消息
 */
typedef struct _MdsUdpPktHead {
    int32               msgCnt;                 /**< 报文中包含的消息数量 */
    int32               pktSiz;                 /**< 报文体数据大小 */
    uint64              pktSeq;                 /**< 报文顺序号 */
} MdsUdpPktHeadT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDS_UDP_PKT_HEAD                \
        0, 0, 0
/* -------------------------           */


#ifdef __cplusplus
}
#endif

#endif  /* _MDS_MKT_PACKETS_H */
