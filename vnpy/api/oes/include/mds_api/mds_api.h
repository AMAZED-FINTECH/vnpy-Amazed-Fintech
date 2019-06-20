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
 * @file    mds_api.h
 *
 * MDS（行情数据服务系统）接口库
 *
 * @version 0.8.13      2016/10/24
 * @version 0.12        2016/11/30
 *          - 重构错误号定义，使错误号从1000开始
 *          - 增加 MdsApi_GetErrorMsg 和 MdsApi_GetErrorMsg2 方法
 *          - 在登录报文中增加协议版本号信息, 并在登录时校验协议版本号的兼容性
 * @version 0.12.1      2016/12/21
 *          - 查询接口重命名
 *              - MdsApi_QryMktDataSnapshot -> MdsApi_QueryMktDataSnapshot
 *              - MdsApi_QrySecurityStatus -> MdsApi_QuerySecurityStatus
 *              - MdsApi_QryTrdSessionStatus -> MdsApi_QueryTrdSessionStatus
 *          - 删除 MdsApi_IsBusinessError 接口，查询接口不再返回小于 -1000 的错误
 *          - 查询接口返回值变更:
 *              - 无数据返回 NEG(ENOENT)
 *              - 查询被服务器拒绝返回 NEG(EINVAL)，具体错误信息通过日志打印
 *          - 微调mds_api日志打印格式
 * @version 0.12.3      2017/1/8
 *          - 精简API依赖的头文件，并最小化API发布包中的头文件数量
 *          - 重命名 protocol_parser/errors/mds_protocol_errors.h -> errors/mds_errors.h
 *          - 添加辅助实现实时行情订阅功能的接口
 *              - MdsApi_ResubscribeMarketData
 *              - MdsHelper_ClearSubscribeRequestEntries
 *              - MdsHelper_AddSubscribeRequestEntry
 * @version 0.12.6.3    2017/03/24
 *          - 增加mds_api.h中的头文件引用，api使用者不在需要显式引用sutil库的头文件
 * @version 0.12.8      2017/04/20
 *          - 重命名 MdsApi_SubscribeMarketData -> MdsApi_SyncSubscribeOnLogon
 *          - 重命名 MdsApi_ResubscribeMarketData -> MdsApi_SubscribeMarketData
 *          - 添加辅助的行情订阅接口
 *              - MdsApi_SubscribeByString
 *              - MdsApi_SubscribeByStringAndPrefixes
 *              - MdsHelper_SetSubscribeRequestMode
 *              - MdsHelper_SetSubscribeRequestTickType
 *              - MdsHelper_SetSubscribeRequestDataTypes
 * @version 0.15.3.3    2017/08/20
 *          - 新增接口 '重置线程级别的日志记录器名称 (MdsApi_ResetThreadLoggerName)'，
 *            以支持为当前线程设置独立的日志文件
 * @version 0.15.4      2017/08/21
 *          - 新增接口 '获取API的发行版本号 (MdsApi_GetApiVersion)'
 * @version 0.15.4.1    2017/09/25
 *          - 服务器端删除了对JSON等通信协议等支持, 改为只支持二进制协议
 * @version 0.15.5      2017/11/11
 *          - 调整接口 MdsApi_InitAll, 以支持同时初始化多个组播通道
 * @version 0.15.5.1    2017/11/17
 *          - 增加 MdsApi_IsValidTcpChannel、MdsApi_IsValidUdpChannel 等接口, 用于判断通道是否已经连接且有效
 * @version 0.15.5.2    2018/01/29
 *          - 增加 MdsApi_GetLastRecvTime、MdsApi_GetLastSendTime 接口，用于获取通道最新发送/接受消息的时间
 *          - 登录失败时, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 * @version 0.15.5.3    2018/01/24
 *          - 调整接口 MdsApi_InitAll, 增加一个函数参数 (pUdpTickOrderAddrKey)，以支持分别订阅逐笔成交和逐笔委托的行情组播
 * @version 0.15.5.9    2018/05/05
 *          - 增加可以处理压缩过的消息的 WaitOnMsg 接口:
 *              - MdsApi_WaitOnMsgCompressible
 *              - MdsApi_WaitOnTcpChannelGroupCompressible
 *              - 与不带 Compressible 后缀的接口相比, 带 Compressible 后缀的接口会自动检测和处理压缩过的消息,
 *                但也会因此带来微小的性能损失。如果对延迟不是极端敏感的话, 可以直接使用带 Compressible 后缀的接
 *                口, 以方便兼容不同的模式。
 * @version 0.15.5.12   2018/06/21
 *          - 增加 MdsApi_HasMoreCachedData 接口, 用于返回已经接收到但尚未被回调函数处理的缓存数据长度
 * @version 0.15.5.16   2018/08/23
 *          - 重构 SubscribeByString 接口
 *              - 支持订阅所有产品的行情或不订阅任何产品的行情
 *              - 支持通过 .SH 或 .SZ 后缀为证券代码指定其所属的交易所
 *              - 添加 MdsHelper_SetTickTypeOnSubscribeByString 接口, 以设置SubscribeByString默认使用的数据模式 (TickType)
 *              - 增量订阅时, 允许不指定 dataType (小于0) 而继承之前订阅的数据类型
 * @version 0.15.6      2018/03/21
 *          - 增加设置当前线程登录用户名/登录密码的接口
 *              - MdsApi_SetThreadUsername
 *              - MdsApi_SetThreadPassword
 *          - 增加返回最近一次API调用失败的错误号的接口
 *              - MdsApi_GetLastError
 *              - MdsApi_SetLastError
 * @since   2016/03/02
 */


#ifndef _MDS_API_H
#define _MDS_API_H


#include    <mds_global/mds_mkt_packets.h>
#include    <mds_api/errors/mds_errors.h>
#include    <sutil/net/spk_general_client_define.h>
#include    <sutil/platform/spk_platforms.h>

/* 为了方便外部使用而额外引入的头文件 *
#include    <sutil/time/spk_times.h>
#include    <sutil/logger/spk_log.h>
*/


#ifdef __cplusplus
extern "C" {
#endif


/* ===================================================================
 * 常量定义
 * =================================================================== */

/** 默认的主配置区段名称 */
#define MDSAPI_CFG_DEFAULT_SECTION              "mds_client"
/** 默认的日志配置区段名称 */
#define MDSAPI_CFG_DEFAULT_SECTION_LOGGER       "log"
/** 默认的TCP行情订阅服务配置项名称 */
#define MDSAPI_CFG_DEFAULT_KEY_TCP_ADDR         "tcpServer"
/** 默认的行情查询服务配置项名称 */
#define MDSAPI_CFG_DEFAULT_KEY_QRY_ADDR         "qryServer"

/** UDP行情订阅服务配置项名称 (L1快照/指数/期权) */
#define MDSAPI_CFG_DEFAULT_KEY_UDP_ADDR_L1      "udpServer.L1"
/** UDP行情订阅服务配置项名称 (L2快照) */
#define MDSAPI_CFG_DEFAULT_KEY_UDP_ADDR_L2      "udpServer.L2"

/** UDP行情订阅服务配置项名称 (L2逐笔成交) */
#define MDSAPI_CFG_DEFAULT_KEY_UDP_ADDR_TICK_TRADE          \
        "udpServer.TickTrade"
/** UDP行情订阅服务配置项名称 (L2逐笔委托) */
#define MDSAPI_CFG_DEFAULT_KEY_UDP_ADDR_TICK_ORDER          \
        "udpServer.TickOrder"

/** 默认的UDP行情订阅服务配置项名称 */
#define MDSAPI_CFG_DEFAULT_KEY_UDP_ADDR                     \
        MDSAPI_CFG_DEFAULT_KEY_UDP_ADDR_L1

/** 默认的心跳间隔 (30 秒) */
#define MDSAPI_DEFAULT_HEARTBEAT_INTERVAL                   \
        GENERAL_CLI_DEFAULT_HEARTBEAT_INTERVAL
/** 默认的UDP连接的心跳间隔 (30 秒) */
#define MDSAPI_DEFAULT_UDP_HEARTBEAT_INTERVAL               \
        GENERAL_CLI_DEFAULT_UDP_HEARTBEAT_INTERVAL

/** 默认的证券代码列表等字符串分隔符 */
#define MDSAPI_DEFAULT_STRING_DELIM                         \
        ",;| \t\r\n"
/* -------------------------           */


/* ===================================================================
 * 常量定义, 枚举类型定义
 * =================================================================== */

/**
 * 通道类型定义
 */
typedef enum _eMdsApiChannelType {
    MDSAPI_CHANNEL_TYPE_TCP         = 1,        /**< TCP行情订阅通道 */
    MDSAPI_CHANNEL_TYPE_UDP         = 2,        /**< UDP行情组播通道 */
    MDSAPI_CHANNEL_TYPE_QUERY       = 3         /**< 行情查询通道 */
} eMdsApiChannelTypeT;
/* -------------------------           */


/* ===================================================================
 * 结构体定义
 * =================================================================== */

/**
 * 客户端会话信息（连接通道信息）定义
 */
typedef SGeneralClientChannelT      MdsApiSessionInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDSAPI_SESSION_INFO             \
        NULLOBJ_GENERAL_CLIENT_CHANNEL
/* -------------------------           */


/**
 * 客户端会话的连接通道组定义（多个连接通道的集合）
 */
typedef SGeneralClientChannelGroupT MdsApiChannelGroupT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDSAPI_CHANNEL_GROUP            \
        NULLOBJ_GENERAL_CLIENT_CHANNEL_GROUP
/* -------------------------           */


/**
 * Socket URI地址信息
 */
typedef SGeneralClientAddrInfoT     MdsApiAddrInfoT;


/* 结构体初始化值定义 */
#define NULLOBJ_MDSAPI_ADDR_INFO                \
        NULLOBJ_GENERAL_CLIENT_ADDR_INFO
/* -------------------------           */


/**
 * 远程主机配置信息
 */
typedef SGeneralClientRemoteCfgT    MdsApiRemoteCfgT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDSAPI_REMOTE_CFG               \
        NULLOBJ_GENERAL_CLIENT_REMOTE_CFG
/* -------------------------           */


/**
 * MDS行情订阅信息配置
 */
typedef MdsMktDataRequestReqBufT    MdsApiSubscribeInfoT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDSAPI_SUBSCRIBE_INFO           \
        NULLOBJ_MDS_MKT_DATA_REQUEST_REQ_BUF
/* -------------------------           */


/**
 * 完整的MDS客户端配置信息
 */
typedef struct _MdsApiClientCfg {
    MdsApiRemoteCfgT        tcpChannelCfg;      /**< TCP行情订阅服务配置 */
    MdsApiRemoteCfgT        qryChannelCfg;      /**< 行情查询服务配置 */

    MdsApiRemoteCfgT        udpL1ChannelCfg;    /**< L1快照行情组播服务配置 */
    MdsApiRemoteCfgT        udpL2ChannelCfg;    /**< L2快照行情组播服务配置 */
    MdsApiRemoteCfgT        udpTradeChannelCfg; /**< L2逐笔成交组播服务配置 */
    MdsApiRemoteCfgT        udpOrderChannelCfg; /**< L2逐笔委托组播服务配置 */

    MdsApiSubscribeInfoT    subscribeInfo;      /**< TCP行情的行情订阅信息 */
} MdsApiClientCfgT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDSAPI_CLIENT_CFG               \
        {NULLOBJ_MDSAPI_REMOTE_CFG}, \
        {NULLOBJ_MDSAPI_REMOTE_CFG}, \
        {NULLOBJ_MDSAPI_REMOTE_CFG}, \
        {NULLOBJ_MDSAPI_REMOTE_CFG}, \
        {NULLOBJ_MDSAPI_REMOTE_CFG}, \
        {NULLOBJ_MDSAPI_REMOTE_CFG}, \
        {NULLOBJ_MDSAPI_SUBSCRIBE_INFO}
/* -------------------------           */


/**
 * MDS客户端运行时环境
 */
typedef struct _MdsApiClientEnv {
    MdsApiSessionInfoT      tcpChannel;         /**< TCP行情订阅通道的会话信息 */
    MdsApiSessionInfoT      qryChannel;         /**< 行情查询通道的会话信息 */

    MdsApiSessionInfoT      udpL1Channel;       /**< L1快照行情组播通道的会话信息 */
    MdsApiSessionInfoT      udpL2Channel;       /**< L2快照行情组播通道的会话信息 */
    MdsApiSessionInfoT      udpTradeChannel;    /**< L2逐笔成交组播通道的会话信息 */
    MdsApiSessionInfoT      udpOrderChannel;    /**< L2逐笔委托组播通道的会话信息 */

    MdsApiChannelGroupT     udpChannelGroup;    /**< 用于管理多个行情组播会话的通道组信息 */
} MdsApiClientEnvT;


/* 结构体的初始化值定义 */
#define NULLOBJ_MDSAPI_CLIENT_ENV               \
        {NULLOBJ_MDSAPI_SESSION_INFO}, \
        {NULLOBJ_MDSAPI_SESSION_INFO}, \
        {NULLOBJ_MDSAPI_SESSION_INFO}, \
        {NULLOBJ_MDSAPI_SESSION_INFO}, \
        {NULLOBJ_MDSAPI_SESSION_INFO}, \
        {NULLOBJ_MDSAPI_SESSION_INFO}, \
        {NULLOBJ_MDSAPI_CHANNEL_GROUP}
/* -------------------------           */


/* ===================================================================
 * 回调函数的函数原型定义
 * =================================================================== */

/**
 * 对接收到的消息进行消息处理的回调函数的函数原型定义
 *
 * @param   pSessionInfo        会话信息
 * @param   pMsgHead            消息头
 * @param   pMsgBody            消息体数据
 * @param   pCallbackParams     外部传入的参数
 * @return  大于等于0，成功；小于0，失败（错误号）
 *
 * @see     eMdsMsgTypeT
 */
typedef int32   (*F_MDSAPI_ONMSG_T) (
                MdsApiSessionInfoT *pSessionInfo,
                SMsgHeadT *pMsgHead,
                void *pMsgBody,
                void *pCallbackParams);
/* -------------------------           */


/* ===================================================================
 * 基于TCP协议的行情服务接口函数声明
 * =================================================================== */

/*
 * 连接并登录到行情服务器
 *
 * @param[out]  pOutSessionInfo 输出会话信息
 * @param       channelType     通道类型
 * @param       pUri            URI地址 (e.g. tcp://127.0.0.1:5100)
 * @param       pUsername       用户名
 * @param       pPassword       密码
 * @param       heartBtInt      心跳间隔,单位为秒
 * @param       pSocketOptions  套接字参数 (为空, 则会使用默认参数)
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ECONNREFUSED    连接失败
 * @exception   ETIMEDOUT       连接超时
 * @exception   EACCES          用户名或密码错误
 * @exception   EMLINK          连接数量超过限制
 * @exception   ESRCH           登录节点非主节点
 * @exception   EFAULT          其他业务错误
 */
BOOL    MdsApi_Logon(
                MdsApiSessionInfoT *pOutSessionInfo,
                eMdsApiChannelTypeT channelType,
                const char *pUri,
                const char *pUsername,
                const char *pPassword,
                int32 heartBtInt,
                const SSocketOptionConfigT *pSocketOptions);

/*
 * 连接并登录到行情服务集群 (基于复制集的高可用集群)
 *
 * - 待解析的地址列表可是以空格、逗号或分号分割的地址列表字符串
 *   - e.g. "tcp://127.0.0.1:5100, tcp://192.168.0.11:5100"
 * - 同时也可以在每个地址之前，为其指定对应的主机编号
 *   - e.g. "2 tcp://192.168.0.12:5100, 1 tcp://192.168.0.11:5100,
 *          3 tcp://192.168.0.13:5100"
 *
 * @param[out]  pOutSessionInfo 输出会话信息
 * @param       channelType     通道类型
 * @param       pUriList        主机地址列表 (以空格、逗号或分号分割的地址列表字符串)
 * @param       pUsername       用户名
 * @param       pPassword       密码
 * @param       heartBtInt      心跳间隔,单位为秒
 * @param       pSocketOptions  套接字参数 (为空, 则会使用默认参数)
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ECONNREFUSED    连接失败
 * @exception   ETIMEDOUT       连接超时
 * @exception   EACCES          用户名或密码错误
 * @exception   EMLINK          连接数量超过限制
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   ESRCH           登录节点非主节点
 * @exception   EFAULT          其他业务错误
 */
BOOL    MdsApi_LogonReplicaSet(
                MdsApiSessionInfoT *pOutSessionInfo,
                eMdsApiChannelTypeT channelType,
                const char *pUriList,
                const char *pUsername,
                const char *pPassword,
                int32 heartBtInt,
                const SSocketOptionConfigT *pSocketOptions);

/*
 * 连接并登录到行情服务集群 (基于对等节点的行情集群)
 *
 * - 待解析的地址列表可是以空格、逗号或分号分割的地址列表字符串
 *   - e.g. "tcp://127.0.0.1:5100, tcp://192.168.0.11:5100"
 * - 同时也可以在每个地址之前，为其指定对应的主机编号
 *   - e.g. "2 tcp://192.168.0.12:5100, 1 tcp://192.168.0.11:5100,
 *          3 tcp://192.168.0.13:5100"
 *
 * @param[out]  pOutSessionInfo 输出会话信息
 * @param       channelType     通道类型
 * @param       pUriList        主机地址列表 (以空格、逗号或分号分割的地址列表字符串)
 * @param       pUsername       用户名
 * @param       pPassword       密码
 * @param       heartBtInt      心跳间隔,单位为秒
 * @param       pSocketOptions  套接字参数 (为空, 则会使用默认参数)
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ECONNREFUSED    连接失败
 * @exception   ETIMEDOUT       连接超时
 * @exception   EACCES          用户名或密码错误
 * @exception   EMLINK          连接数量超过限制
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   ESRCH           登录节点非主节点
 * @exception   EFAULT          其他业务错误
 */
BOOL    MdsApi_LogonPeerNodes(
                MdsApiSessionInfoT *pOutSessionInfo,
                eMdsApiChannelTypeT channelType,
                const char *pUriList,
                const char *pUsername,
                const char *pPassword,
                int32 heartBtInt,
                const SSocketOptionConfigT *pSocketOptions);

/*
 * 发送证券行情订阅消息，并同步等待应答消息的返回 (仅适用于连接建立后的初次订阅)
 *
 * @param   pTcpChannel         会话信息
 * @param   pMktDataRequestReq  行情订阅请求信息
 * @param   pEntries            产品订阅列表
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_SyncSubscribeOnLogon(
                MdsApiSessionInfoT *pSessionInfo,
                const MdsMktDataRequestReqT *pMktDataRequestReq,
                const MdsMktDataRequestEntryT *pEntries);

/*
 * 以异步的方式发送证券行情实时订阅请求，以重新订阅、追加订阅或删除订阅行情数据
 *
 * @param   pTcpChannel         会话信息
 * @param   pMktDataRequestReq  行情订阅请求信息
 * @param   pEntries            产品订阅列表
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_SubscribeMarketData(
                MdsApiSessionInfoT *pSessionInfo,
                const MdsMktDataRequestReqT *pMktDataRequestReq,
                const MdsMktDataRequestEntryT *pEntries);

/*
 * 直接根据字符串形式的证券代码列表订阅行情信息
 *
 * @param   pTcpChannel         会话信息
 * @param   pSecurityListStr    证券代码列表字符串
 *                              - 证券代码支持以 .SH 或 .SZ 为后缀来指定其所属的交易所
 *                              - 空字符串 "", 表示不订阅任何产品的行情
 *                              - 空指针 NULL, 表示订阅所有产品的行情
 * @param   pDelim              证券代码列表的分隔符 (e.g. ",;| \t")
 *                              - 如果为空, 则使用默认的分隔符:
 *                                ',' 或 ';' 或 '|' 或 ' ' 或 '\t'
 * @param   exchangeId          证券代码所属的交易所代码 (如果证券代码没有 .SH 或 .SZ 后缀的话)
 * @param   securityType        证券类型 (股票(基金、债券)/指数/期权)
 * @param   subMode             订阅模式 (重新订阅/追加订阅/删除订阅)
 * @param   dataTypes           订阅的数据种类 (e.g. MDS_SUB_DATA_TYPE_L1_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_BEST_ORDERS
 *                              | MDS_SUB_DATA_TYPE_L2_TRADE)
 *                              @see eMdsSubscribeDataTypeT
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_SubscribeByString(
                MdsApiSessionInfoT *pTcpChannel,
                const char *pSecurityListStr,
                const char *pDelim,
                eMdsExchangeIdT exchangeId,
                eMdsSecurityTypeT securityType,
                eMdsSubscribeModeT subMode,
                int32 dataTypes);

/*
 * 根据字符串指针数组形式的证券代码列表订阅行情信息
 *
 * @param   pTcpChannel         会话信息
 * @param   ppSecurityArray     证券代码列表的指针数组
 *                              - 证券代码支持以 .SH 或 .SZ 为后缀来指定其所属的交易所
 *                              - 空数组 (指针非空, 但 securityCount == 0), 表示不订阅任何产品的行情
 *                              - 空指针 NULL, 表示订阅所有产品的行情
 * @param   securityCount       证券代码数量
 * @param   exchangeId          证券代码所属的交易所代码 (如果证券代码没有 .SH 或 .SZ 后缀的话)
 * @param   securityType        证券类型 (股票(基金、债券)/指数/期权)
 * @param   subMode             订阅模式 (重新订阅/追加订阅/删除订阅)
 * @param   dataTypes           订阅的数据种类 (e.g. MDS_SUB_DATA_TYPE_L1_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_BEST_ORDERS
 *                              | MDS_SUB_DATA_TYPE_L2_TRADE)
 *                              @see eMdsSubscribeDataTypeT
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_SubscribeByString2(
                MdsApiSessionInfoT *pTcpChannel,
                const char *ppSecurityArray[],
                int32 securityCount,
                eMdsExchangeIdT exchangeId,
                eMdsSecurityTypeT securityType,
                eMdsSubscribeModeT subMode,
                int32 dataTypes);

/*
 * 直接根据字符串形式的证券代码列表和证券代码前缀列表订阅行情信息
 *
 * @param   pTcpChannel         会话信息
 * @param   pSecurityListStr    证券代码列表字符串
 *                              - 证券代码支持以 .SH 或 .SZ 为后缀来指定其所属的交易所
 *                              - 空字符串 "", 表示不订阅任何产品的行情
 *                              - 空指针 NULL, 表示订阅所有产品的行情
 * @param   pDelim              证券代码列表的分隔符 (e.g. ",;| \t")
 *                              - 如果为空, 则使用默认的分隔符:
 *                                ',' 或 ';' 或 '|' 或 ' ' 或 '\t'
 * @param   pSseCodePrefixes    以逗号或空格分隔的上海证券代码前缀列表, e.g.
 *                              - "6, 300, 301" 将匹配证券代码列表中所有以 '6' 或 '300'
 *                                或 '301' 起始的证券代码
 *                              - 若为NULL或空字符串, 则不会匹配任何证券代码
 *                              - 上海证券代码前缀参考:
 *                                  - "009, 01, 02, "               //国债
 *                                  - "10, 11, 12, 13, 18, 19, "    //债券 (企业债、可转债等)
 *                                  - "20, "                        //债券 (回购)
 *                                  - "5, "                         //基金
 *                                  - "6, "                         //A股
 *                                  - "000"                         //指数
 * @param   pSzseCodePrefixes   以逗号或空格分隔的深圳证券代码前缀列表
 *                              - 若为NULL或空字符串, 则不会匹配任何证券代码
 *                              - 证券代码前缀可以和上海相同, 此时匹配的证券代码会同时对上海
 *                                和深圳两个市场进行订阅
 *                              - 深圳证券代码前缀参考:
 *                                  - "00, "                        //股票
 *                                  - "10, 11, 12, 13, "            //债券
 *                                  - "15, 16, 17, 18, "            //基金
 *                                  - "30, "                        //创业板
 *                                  - "39"                          //指数
 * @param   securityType        证券类型 (股票(基金、债券)/指数/期权)
 * @param   subMode             订阅模式 (重新订阅/追加订阅/删除订阅)
 * @param   dataTypes           订阅的数据种类 (e.g. MDS_SUB_DATA_TYPE_L1_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_BEST_ORDERS
 *                              | MDS_SUB_DATA_TYPE_L2_TRADE)
 *                              @see eMdsSubscribeDataTypeT
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_SubscribeByStringAndPrefixes(
                MdsApiSessionInfoT *pTcpChannel,
                const char *pSecurityListStr,
                const char *pDelim,
                const char *pSseCodePrefixes,
                const char *pSzseCodePrefixes,
                eMdsSecurityTypeT securityType,
                eMdsSubscribeModeT subMode,
                int32 dataTypes);

/*
 * 根据字符串指针数组形式的证券代码列表以及证券代码前缀列表订阅行情信息
 *
 * @param   pTcpChannel         会话信息
 * @param   ppSecurityArray     证券代码列表的指针数组
 *                              - 证券代码支持以 .SH 或 .SZ 为后缀来指定其所属的交易所
 *                              - 空字符串 "", 表示不订阅任何产品的行情
 *                              - 空指针 NULL, 表示订阅所有产品的行情
 * @param   securityCount       证券代码数量
 * @param   pSseCodePrefixes    以逗号或空格分隔的上海证券代码前缀列表, e.g.
 *                              - "6, 300, 301" 将匹配证券代码列表中所有以 '6' 或 '300'
 *                                或 '301' 起始的证券代码
 *                              - 若为NULL或空字符串, 则不会匹配任何证券代码
 *                              - 上海证券代码前缀参考:
 *                                  - "009, 01, 02, "               //国债
 *                                  - "10, 11, 12, 13, 18, 19, "    //债券 (企业债、可转债等)
 *                                  - "20, "                        //债券 (回购)
 *                                  - "5, "                         //基金
 *                                  - "6, "                         //A股
 *                                  - "000"                         //指数
 * @param   pSzseCodePrefixes   以逗号或空格分隔的深圳证券代码前缀列表
 *                              - 若为NULL或空字符串, 则不会匹配任何证券代码
 *                              - 证券代码前缀可以和上海相同, 此时匹配的证券代码会同时对上海
 *                                和深圳两个市场进行订阅
 *                              - 深圳证券代码前缀参考:
 *                                  - "00, "                        //股票
 *                                  - "10, 11, 12, 13, "            //债券
 *                                  - "15, 16, 17, 18, "            //基金
 *                                  - "30, "                        //创业板
 *                                  - "39"                          //指数
 * @param   securityType        证券类型 (股票(基金、债券)/指数/期权)
 * @param   subMode             订阅模式 (重新订阅/追加订阅/删除订阅)
 * @param   dataTypes           订阅的数据种类 (e.g. MDS_SUB_DATA_TYPE_L1_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_BEST_ORDERS
 *                              | MDS_SUB_DATA_TYPE_L2_TRADE)
 *                              @see eMdsSubscribeDataTypeT
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_SubscribeByStringAndPrefixes2(
                MdsApiSessionInfoT *pTcpChannel,
                const char *ppSecurityArray[],
                int32 securityCount,
                const char *pSseCodePrefixes,
                const char *pSzseCodePrefixes,
                eMdsSecurityTypeT securityType,
                eMdsSubscribeModeT subMode,
                int32 dataTypes);

/*
 * 发送心跳消息
 *
 * @param   pSessionInfo        会话信息
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_SendHeartbeat(
                MdsApiSessionInfoT *pSessionInfo);

/*
 * 发送测试请求消息
 *
 * @param   pSessionInfo        会话信息
 * @param   pTestReqId          测试请求标识符 (C32, 可以为空)
 * @param   testReqIdSize       测试请求标识符的长度
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_SendTestRequest(
                MdsApiSessionInfoT *pSessionInfo,
                const char *pTestReqId,
                int32 testReqIdSize);

/*
 * 发送注销消息
 *
 * @param   pSessionInfo        会话信息
 * @param   isDestory           发送注销消息后是否立即释放相关资源
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_Logout(
                MdsApiSessionInfoT *pSessionInfo,
                BOOL isDestory);

/*
 * 直接断开与服务器的连接并释放会话数据
 *
 * @param   pSessionInfo        会话信息
 */
void    MdsApi_Destory(
                MdsApiSessionInfoT *pSessionInfo);

/*
 * 阻塞接收MDS行情消息，并调用回调函数进行消息处理
 * 阻塞等待直到完整的消息接收完成或超时
 *
 * @param   pTcpChannel         会话信息
 * @param   timeoutMs           超时时间(毫秒)
 * @param   pOnMsgCallback      进行消息处理的回调函数
 * @param   pCallbackParams     回调函数的参数
 * @return  大于等于0, 成功 (返回回调函数的返回值);
 *          小于0, 失败 (负的错误号)
 *
 * @exception   NEG(ETIMEDOUT)  超时
 * @exception   NEG(EPIPE)      连接已破裂
 * @exception   Others          由read()系统调用或回调函数pOnMsgCallback返回的错误
 */
int32   MdsApi_WaitOnMsg(
                MdsApiSessionInfoT *pTcpChannel,
                int32 timeoutMs,
                F_MDSAPI_ONMSG_T pOnMsgCallback,
                void *pCallbackParams);

/*
 * 阻塞接收MDS行情消息，并调用回调函数进行消息处理 (可以处理压缩过的消息)
 *
 * - 与不带 Compressible 后缀的接口相比, 带 Compressible 后缀的接口会自动检测和处理压缩过的
 *   消息, 但也会因此带来微小的性能损失。
 * - 如果对延迟不是极端敏感的话, 可以直接使用带 Compressible 后缀的接口, 以方便兼容不同的模式。
 *
 * @param   pTcpChannel         会话信息
 * @param   timeoutMs           超时时间(毫秒)
 * @param   pOnMsgCallback      进行消息处理的回调函数
 * @param   pCallbackParams     回调函数的参数
 * @return  大于等于0, 成功 (返回回调函数的返回值);
 *          小于0, 失败 (负的错误号)
 *
 * @exception   NEG(ETIMEDOUT)  超时
 * @exception   NEG(EPIPE)      连接已破裂
 * @exception   Others          由read()系统调用或回调函数pOnMsgCallback返回的错误
 */
int32   MdsApi_WaitOnMsgCompressible(
                MdsApiSessionInfoT *pTcpChannel,
                int32 timeoutMs,
                F_MDSAPI_ONMSG_T pOnMsgCallback,
                void *pCallbackParams);
/* -------------------------           */


/* ===================================================================
 * 查询接口函数声明
 * =================================================================== */

/*
 * 获取API的发行版本号
 *
 * @return  API的发行版本号 (如: "0.15.3")
 */
const char *
        MdsApi_GetApiVersion();

/*
 * 查询证券行情快照
 *
 * @param       pQryChannel     会话信息
 * @param       exchangeId      交易所代码
 * @param       securityType    证券类型
 * @param       instrId         产品代码
 * @param[out]  pRspBuf         用于输出查询结果的应答数据缓存
 * @retval      =0              查询成功
 * @retval      <0              查询失败 (负的错误号)
 * @exception   NEG(ENOENT)     未检索到待查询的数据
 *
 * @see         eMdsExchangeIdT
 * @see         eMdsSecurityTypeT
 */
int32   MdsApi_QueryMktDataSnapshot(
                MdsApiSessionInfoT *pQryChannel,
                eMdsExchangeIdT exchangeId,
                eMdsSecurityTypeT securityType,
                int32 instrId,
                MdsMktDataSnapshotT *pRspBuf);

/*
 * 查询(深圳)证券实时状态
 *
 * @param       pQryChannel     会话信息
 * @param       exchangeId      交易所代码
 * @param       securityType    证券类型
 * @param       instrId         产品代码
 * @param[out]  pRspBuf         用于输出查询结果的应答数据缓存
 * @retval      =0              查询成功
 * @retval      <0              查询失败 (负的错误号)
 * @exception   NEG(ENOENT)     未检索到待查询的数据
 *
 * @see         eMdsExchangeIdT
 * @see         eMdsSecurityTypeT
 */
int32   MdsApi_QuerySecurityStatus(
                MdsApiSessionInfoT *pQryChannel,
                eMdsExchangeIdT exchangeId,
                eMdsSecurityTypeT securityType,
                int32 instrId,
                MdsSecurityStatusMsgT *pRspBuf);

/*
 * 查询(上证)市场状态
 *
 * @param       pQryChannel     会话信息
 * @param       exchangeId      交易所代码
 * @param       securityType    证券类型
 * @param       instrId         产品代码
 * @param[out]  pRspBuf         用于输出查询结果的应答数据缓存
 * @retval      =0              查询成功
 * @retval      <0              查询失败 (负的错误号)
 * @exception   NEG(ENOENT)     未检索到待查询的数据
 *
 * @see         eMdsExchangeIdT
 * @see         eMdsSecurityTypeT
 */
int32   MdsApi_QueryTrdSessionStatus(
                MdsApiSessionInfoT *pQryChannel,
                eMdsExchangeIdT exchangeId,
                eMdsSecurityTypeT securityType,
                MdsTradingSessionStatusMsgT *pRspBuf);
/* -------------------------           */


/* ===================================================================
 * 基于UDP协议的行情服务接口函数声明
 * =================================================================== */

/*
 * 建立与行情服务器的UDP(组播)连接
 *
 * @param[out]  pOutSessionInfo 输出会话信息
 * @param       pUri            URI地址 (e.g. udp-mcast://239.1.150.100:5300)
 * @param       pSocketOptions  套接字参数 (为空, 则会使用默认参数)
 * @return      TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_ConnectToUdpMcast(
                MdsApiSessionInfoT *pOutSessionInfo,
                const char *pUri,
                const SSocketOptionConfigT *pSocketOptions);

/*
 * 阻塞接收MDS-UDP行情消息，并调用回调函数进行消息处理
 * 阻塞等待直到完整的消息接收完成或超时
 *
 * @param       pUdpChannel     会话信息
 * @param       timeoutMs       超时时间(毫秒)
 * @param       pOnMsgCallback  进行消息处理的回调函数
 * @param       pCallbackParams 回调函数的参数
 * @return      大于等于0, 成功 (返回回调函数的返回值);
 *              小于0, 失败 (负的错误号)
 *
 * @exception   NEG(ETIMEDOUT)  超时
 * @exception   NEG(EPIPE)      连接已破裂
 * @exception   Others          由read()系统调用或回调函数pOnMsgCallback返回的错误
 */
int32   MdsApi_WaitOnUdpMsg(
                MdsApiSessionInfoT *pUdpChannel,
                int32 timeoutMs,
                F_MDSAPI_ONMSG_T pOnMsgCallback,
                void *pCallbackParams);
/* -------------------------           */


/* ===================================================================
 * 完整的行情客户端环境的统一初始化和析构接口声明
 * =================================================================== */

/*
 * 完整的初始化行情客户端环境
 *
 * @param[out]  pOutCliEnv      输出行情客户端环境信息
 * @param       pCfgFile        配置文件路径
 * @param       pLoggerSection  日志记录器的配置区段名称
 * @param       pCfgSection     行情客户端配置区段名称
 * @param       pTcpAddrKey     TCP行情订阅地址的配置项关键字 (为空或配置项未设置则不连接)
 * @param       pQryAddrKey     行情查询服务地址的配置项关键字 (为空或配置项未设置则不连接)
 * @param       pUdpL1AddrKey   L1快照行情组播地址的配置项关键字 (为空或配置项未设置则不连接)
 * @param       pUdpL2AddrKey   L2快照行情组播地址的配置项关键字 (为空或配置项未设置则不连接)
 * @param       pUdpTickTradeAddrKey
 *                              L2逐笔成交组播地址的配置项关键字 (为空或配置项未设置则不连接)
 * @param       pUdpTickOrderAddrKey
 *                              L2逐笔委托组播地址的配置项关键字 (为空或配置项未设置则不连接)
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ECONNREFUSED    连接失败
 * @exception   ETIMEDOUT       连接超时
 * @exception   EACCES          用户名或密码错误
 * @exception   EMLINK          连接数量超过限制
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   ESRCH           登录节点非主节点
 * @exception   EFAULT          其他业务错误
 *
 * @see         MdsApi_InitAllByConvention
 */
BOOL    MdsApi_InitAll(
                MdsApiClientEnvT *pOutCliEnv,
                const char *pCfgFile,
                const char *pLoggerSection,
                const char *pCfgSection,
                const char *pTcpAddrKey,
                const char *pQryAddrKey,
                const char *pUdpL1AddrKey,
                const char *pUdpL2AddrKey,
                const char *pUdpTickTradeAddrKey,
                const char *pUdpTickOrderAddrKey);

/*
 * 按照默认的配置名称, 完整的初始化行情客户端环境
 *
 * @param[out]  pOutCliEnv      输出行情客户端环境信息
 * @param       pCfgFile        配置文件路径
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ECONNREFUSED    连接失败
 * @exception   ETIMEDOUT       连接超时
 * @exception   EACCES          用户名或密码错误
 * @exception   EMLINK          连接数量超过限制
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   ESRCH           登录节点非主节点
 * @exception   EFAULT          其他业务错误
 *
 * @see         MdsApi_InitAll
 */
BOOL    MdsApi_InitAllByConvention(
                MdsApiClientEnvT *pOutCliEnv,
                const char *pCfgFile);

/*
 * 注销并关闭所有的行情客户端会话
 *
 * @param[out]  pCliEnv         行情客户端环境信息
 * @param       isDestory       是否立即释放相关资源
 * @return      TRUE 成功; FALSE 失败
 */
void    MdsApi_LogoutAll(MdsApiClientEnvT *pCliEnv, BOOL isDestory);

/*
 * 直接断开与服务器的连接并释放会话数据
 *
 * @param[out]  pCliEnv         行情客户端环境信息
 */
void    MdsApi_DestoryAll(
                MdsApiClientEnvT *pCliEnv);
/* -------------------------           */


/* ===================================================================
 * 基于配置文件的辅助接口函数声明
 * =================================================================== */

/*
 * 初始化日志记录器
 *
 * @param       pCfgFile        配置文件路径
 * @param       pLoggerSection  日志记录器的配置区段名称(为空则使用默认值)
 * @return      TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_InitLogger(
                const char *pCfgFile,
                const char *pLoggerSection);

/*
 * 重置线程级别的日志记录器名称
 *
 * @param   pLogSystemName  线程级别的日志系统名称
 * @return  TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_ResetThreadLoggerName(
                const char *pLogSystemName);

/*
 * 初始化基于TCP协议的行情订阅通道 (包括完整的配置解析、连接建立、登录和行情订阅过程)
 *
 * @param[out]  pTcpChannel     TCP行情订阅通道的会话信息
 * @param       pCfgFile        配置文件路径
 * @param       pCfgSection     行情客户端配置区段名称(为空则使用默认值)
 * @param       pAddrKey        地址列表的配置项关键字(为空则使用默认值)
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ECONNREFUSED    连接失败
 * @exception   ETIMEDOUT       连接超时
 * @exception   EACCES          用户名或密码错误
 * @exception   EMLINK          连接数量超过限制
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   ESRCH           登录节点非主节点
 * @exception   EFAULT          其他业务错误
 */
BOOL    MdsApi_InitTcpChannel(
                MdsApiSessionInfoT *pTcpChannel,
                const char *pCfgFile,
                const char *pCfgSection,
                const char *pAddrKey);

/*
 * 初始化基于TCP协议的行情订阅通道 (包括完整的连接建立、登录和行情订阅过程)
 *
 * @param[out]  pTcpChannel     TCP行情订阅通道的会话信息
 * @param       pRemoteCfg      远程主机配置信息
 * @param       pSubscribeInfo  行情订阅配置信息
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ECONNREFUSED    连接失败
 * @exception   ETIMEDOUT       连接超时
 * @exception   EACCES          用户名或密码错误
 * @exception   EMLINK          连接数量超过限制
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   ESRCH           登录节点非主节点
 * @exception   EFAULT          其他业务错误
 */
BOOL    MdsApi_InitTcpChannel2(
                MdsApiSessionInfoT *pTcpChannel,
                MdsApiRemoteCfgT *pRemoteCfg,
                MdsApiSubscribeInfoT *pSubscribeInfo);

/*
 * 初始化行情查询通道 (包括完整的配置解析、连接建立和登录过程)
 *
 * @param[out]  pQryChannel     行情查询通道的会话信息
 * @param       pCfgFile        配置文件路径
 * @param       pCfgSection     行情客户端配置区段名称(为空则使用默认值)
 * @param       pAddrKey        地址列表的配置项关键字(为空则使用默认值)
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ECONNREFUSED    连接失败
 * @exception   ETIMEDOUT       连接超时
 * @exception   EACCES          用户名或密码错误
 * @exception   EMLINK          连接数量超过限制
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   ESRCH           登录节点非主节点
 * @exception   EFAULT          其他业务错误
 */
BOOL    MdsApi_InitQryChannel(
                MdsApiSessionInfoT *pQryChannel,
                const char *pCfgFile,
                const char *pCfgSection,
                const char *pAddrKey);

/*
 * 初始化行情查询通道 (包括完整的连接建立和登录过程)
 *
 * @param[out]  pQryChannel     行情查询通道的会话信息
 * @param       pRemoteCfg      远程主机配置信息
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ECONNREFUSED    连接失败
 * @exception   ETIMEDOUT       连接超时
 * @exception   EACCES          用户名或密码错误
 * @exception   EMLINK          连接数量超过限制
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   ESRCH           登录节点非主节点
 * @exception   EFAULT          其他业务错误
 */
BOOL    MdsApi_InitQryChannel2(
                MdsApiSessionInfoT *pQryChannel,
                MdsApiRemoteCfgT *pRemoteCfg);

/*
 * 初始化基于UDP协议的行情订阅通道 (包括完整的配置解析和连接建立过程)
 *
 * @param[out]  pUdpChannel     UDP行情订阅通道的会话信息
 * @param       pCfgFile        配置文件路径
 * @param       pCfgSection     行情客户端配置区段名称(为空则使用默认值)
 * @param       pAddrKey        地址列表的配置项关键字(为空则使用默认值)
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   Others          由系统调用返回的错误
 */
BOOL    MdsApi_InitUdpChannel(
                MdsApiSessionInfoT *pUdpChannel,
                const char *pCfgFile,
                const char *pCfgSection,
                const char *pAddrKey);

/*
 * 初始化基于UDP协议的行情订阅通道 (包括完整的连接建立过程)
 *
 * @param[out]  pUdpChannel     UDP行情订阅通道的会话信息
 * @param       pRemoteCfg      远程主机配置信息
 * @retval      TRUE            成功
 * @retval      FALSE           失败。此时 errno 将被设置, 可以通过 errno/SPK_GET_ERRNO() 获取到具体失败原因
 *
 * @exception   EINVAL          配置异常或传入参数非法
 * @exception   ENOENT          地址列表中没有找到有效的节点配置
 * @exception   Others          由系统调用返回的错误
 */
BOOL    MdsApi_InitUdpChannel2(
                MdsApiSessionInfoT *pUdpChannel,
                MdsApiRemoteCfgT *pRemoteCfg);

/*
 * 解析服务器地址列表字符串
 *
 * - 待解析的地址列表可是以空格、逗号或分号分割的地址列表字符串
 *   - e.g. "tcp://127.0.0.1:5100, tcp://192.168.0.11:5100"
 * - 同时也可以在每个地址之前，为其指定对应的主机编号
 *   - e.g. "2 tcp://192.168.0.12:5100, 1 tcp://192.168.0.11:5100,
 *          3 tcp://192.168.0.13:5100"
 *
 * @param       pUriList        主机地址列表 (以空格、逗号或分号分割的地址列表字符串)
 * @param[out]  pOutAddrList    用于输出解析后的地址信息的地址信息数组
 * @param       addrListLength  地址信息列表的数组长度
 * @return      大于等于0, 解析得到的地址数量; 小于0, 解析失败
 */
int32   MdsApi_ParseAddrListString(
                const char *pUriList,
                MdsApiAddrInfoT *pOutAddrList,
                int32 addrListLength);

/*
 * 解析客户端配置文件
 *
 * @param       pCfgFile        配置文件路径
 * @param       pSection        配置区段名称
 * @param       pAddrKey        地址列表的配置项关键字
 * @param[out]  pOutRemoteCfg   输出远程主机配置信息
 * @param[out]  pOutSubscribeInfo   输出行情订阅信息
 * @return      TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_ParseConfigFromFile(
                const char *pCfgFile,
                const char *pSection,
                const char *pAddrKey,
                MdsApiRemoteCfgT *pOutRemoteCfg,
                MdsApiSubscribeInfoT *pOutSubscribeInfo);

/*
 * 按照默认的配置名称, 从配置文件中解析所有配置信息
 *
 * @param       pCfgFile        配置文件路径
 * @param[out]  pOutApiCfg      输出远程主机配置信息
 * @return      TRUE 成功; FALSE 失败
 */
BOOL    MdsApi_ParseAllConfig(
                const char *pCfgFile,
                MdsApiClientCfgT *pOutApiCfg);
/* -------------------------           */


/* ===================================================================
 * 用于同时接收多个连接通道数据的通道组接口函数
 * =================================================================== */

/*
 * 初始化(重置)通道组信息 (用于同时接收多个连接通道的数据)
 *
 * @param[in,out]   pChannelGroup   通道组信息
 * @return          TRUE, 成功; FALSE 失败
 */
BOOL    MdsApi_InitChannelGroup(
                MdsApiChannelGroupT *pChannelGroup);

/*
 * 销毁通道组信息 (同时关闭通道组下的所有连接)
 *
 * @param       pChannelGroup   通道组信息
 * @return      TRUE, 成功; FALSE 失败
 */
BOOL    MdsApi_DestoryChannelGroup(
                MdsApiChannelGroupT *pChannelGroup);

/*
 * 将连接信息添加到通道组
 *
 * @note        在检测通道组下各连接的I/O事件时，是从后向前进行扫描的，所以建议将更
 *              活跃的连接添加到其它连接的后面
 *
 * @param       pChannelGroup   通道组信息
 * @param       pChannel        连接信息
 * @return      TRUE, 成功; FALSE 失败
 */
BOOL    MdsApi_AddToChannelGroup(
                MdsApiChannelGroupT *pChannelGroup,
                MdsApiSessionInfoT *pChannel);

/*
 * 从通道组中删除连接
 *
 * @param       pChannelGroup   通道组信息
 * @param       pTargetChannel  待删除连接的连接信息
 * @return      TRUE, 成功; FALSE 失败
 */
BOOL    MdsApi_DeleteFromChannelGroup(
                MdsApiChannelGroupT *pChannelGroup,
                MdsApiSessionInfoT *pTargetChanne);

/*
 * 获取通道组中指定下标的连接信息
 *
 * @param       pChannelGroup   通道组信息
 * @param       index           下标位置
 * @return      连接信息
 */
MdsApiSessionInfoT *
        MdsApi_GetFromChannelGroup(
                MdsApiChannelGroupT *pChannelGroup,
                int32 index);

/*
 * 获取通道组中与指定Socket描述符相匹配的连接信息
 *
 * @param       pChannelGroup   通道组信息
 * @param       socketFd        Socket描述符
 * @return      连接信息
 */
MdsApiSessionInfoT *
        MdsApi_GetFromChannelGroupBySocket(
                MdsApiChannelGroupT *pChannelGroup,
                SPK_SOCKET socketFd);

/*
 * 遍历通道组下的所有通道信息并执行回调函数
 *
 * @param       pChannelGroup   通道组信息
 * @param       fnCallback      待执行的回调函数
 * @param       pParams         回调函数的参数
 * @return      0, 成功; 小于 0, errno 取负
 */
int32   MdsApi_ForeachInChannelGroup(
                MdsApiChannelGroupT *pChannelGroup,
                int32 (*fnCallback)(MdsApiSessionInfoT *pChannel, void *pParams),
                void *pParams);

/*
 * 检测TCP通道组下各连接通道的读事件，并调用回调函数进行消息处理
 *
 * @param       pChannelGroup   通道组信息
 * @param       timeoutMs       超时时间(毫秒)
 * @param       pOnMsgCallback  进行消息处理的回调函数
 * @param       pCallbackParams 回调函数的参数
 * @param[out]  ppFailed        返回处理失败时所对应的连接信息
 *                              - 允许为空, 为空则忽略该参数
 *                              - 当因为超时(一定时间内没有数据)而返回时, 该参数的值为空
 * @return      大于等于0, 成功 (返回回调函数的返回值);
 *              小于0, 失败 (负的错误号)
 *
 * @exception   NEG(ETIMEDOUT)  超时
 * @exception   NEG(EPIPE)      连接已破裂
 * @exception   Others          由select/read系统调用或回调函数返回的错误
 */
int32   MdsApi_WaitOnTcpChannelGroup(
                MdsApiChannelGroupT *pChannelGroup,
                int32 timeoutMs,
                F_MDSAPI_ONMSG_T pOnMsgCallback,
                void *pCallbackParams,
                MdsApiSessionInfoT **ppFailed);

/*
 * 检测TCP通道组下各连接通道的读事件，并调用回调函数进行消息处理 (可以处理压缩过的消息)
 *
 * - 与不带 Compressible 后缀的接口相比, 带 Compressible 后缀的接口会自动检测和处理压缩过的
 *   消息, 但也会因此带来微小的性能损失。
 * - 如果对延迟不是极端敏感的话, 可以直接使用带 Compressible 后缀的接口, 以方便兼容不同的模式。
 *
 * @param       pChannelGroup   通道组信息
 * @param       timeoutMs       超时时间(毫秒)
 * @param       pOnMsgCallback  进行消息处理的回调函数
 * @param       pCallbackParams 回调函数的参数
 * @param[out]  ppFailed        返回处理失败时所对应的连接信息
 *                              - 允许为空, 为空则忽略该参数
 *                              - 当因为超时(一定时间内没有数据)而返回时, 该参数的值为空
 * @return      大于等于0, 成功 (返回回调函数的返回值);
 *              小于0, 失败 (负的错误号)
 *
 * @exception   NEG(ETIMEDOUT)  超时
 * @exception   NEG(EPIPE)      连接已破裂
 * @exception   Others          由select/read系统调用或回调函数返回的错误
 */
int32   MdsApi_WaitOnTcpChannelGroupCompressible(
                MdsApiChannelGroupT *pChannelGroup,
                int32 timeoutMs,
                F_MDSAPI_ONMSG_T pOnMsgCallback,
                void *pCallbackParams,
                MdsApiSessionInfoT **ppFailed);

/*
 * 检测UDP通道组下各连接通道的读事件，并调用回调函数进行消息处理
 *
 * @param       pChannelGroup   通道组信息
 * @param       timeoutMs       超时时间(毫秒)
 * @param       pOnMsgCallback  进行消息处理的回调函数
 * @param       pCallbackParams 回调函数的参数
 * @param[out]  ppFailed        返回处理失败时所对应的连接信息
 *                              - 允许为空, 为空则忽略该参数
 *                              - 当因为超时(一定时间内没有数据)而返回时, 该参数的值为空
 * @return      大于等于0, 成功 (返回回调函数的返回值);
 *              小于0, 失败 (负的错误号)
 *
 * @exception   NEG(ETIMEDOUT)  超时
 * @exception   NEG(EPIPE)      连接已破裂
 * @exception   Others          由select/read系统调用或回调函数返回的错误
 */
int32   MdsApi_WaitOnUdpChannelGroup(
                MdsApiChannelGroupT *pChannelGroup,
                int32 timeoutMs,
                F_MDSAPI_ONMSG_T pOnMsgCallback,
                void *pCallbackParams,
                MdsApiSessionInfoT **ppFailed);
/* -------------------------           */


/* ===================================================================
 * 错误处理等辅助函数
 * =================================================================== */

/*
 * 设置当前线程登录MDS时使用的登录用户名
 * 不设置或者参数为空的话, 登录时会尝试使用配置文件中的配置
 *
 * @param   pUsername           登录用户名
 */
void    MdsApi_SetThreadUsername(
                const char *pUsername);

/*
 * 返回当前线程登录MDS时使用的登录用户名
 *
 * @return  登录用户名
 */
const char *
        MdsApi_GetThreadUsername();

/*
 * 设置当前线程登录MDS时使用的登录密码
 * 不设置或者参数为空的话, 登录时会尝试使用配置文件中的配置
 *
 * @param   pPassword           登录密码
 *                              - 支持通过密码前缀指定密码类型, 如 md5:PASSWORD, txt:PASSWORD
 */
void    MdsApi_SetThreadPassword(
                const char *pPassword);

/*
 * 获取通道最新接受消息时间
 *
 * @param   pSessionInfo        会话信息
 * @return  通道最新接受消息时间(单位: 秒)
 */
int64    MdsApi_GetLastRecvTime(
                const MdsApiSessionInfoT *pSessionInfo);

/*
 * 获取通道最新发送消息时间
 *
 * @param   pSessionInfo        会话信息
 * @return  通道最新发送消息时间(单位: 秒)
 */
int64    MdsApi_GetLastSendTime(
                const MdsApiSessionInfoT *pSessionInfo);

/*
 * 返回行情订阅通道是否还有更多已接收但尚未回调处理完成的数据
 *
 * @param   pTcpChannel         会话信息
 * @return  已接收到但尚未处理完成的数据长度
 * @note    当在回调函数里面调用时, 该接口返回的数据长度是包含本次回调数据的
 */
int32   MdsApi_HasMoreCachedData(
                const MdsApiSessionInfoT *pTcpChannel);

/*
 * 返回基于TCP协议的行情订阅通道是否已经连接且有效
 *
 * @param   pTcpChannel         会话信息
 * @return  TRUE 已连接; FALSE 未连接或连接无效
 */
BOOL    MdsApi_IsValidTcpChannel(
                const MdsApiSessionInfoT *pTcpChannel);

/*
 * 返回行情查询通道是否已经连接且有效
 *
 * @param   pQryChannel         会话信息
 * @return  TRUE 已连接; FALSE 未连接或连接无效
 */
BOOL    MdsApi_IsValidQryChannel(
                const MdsApiSessionInfoT *pQryChannel);

/*
 * 返回基于UDP协议的行情组播通道是否已经连接且有效
 *
 * @param   pUdpChannel         会话信息
 * @return  TRUE 已连接; FALSE 未连接或连接无效
 */
BOOL    MdsApi_IsValidUdpChannel(
                const MdsApiSessionInfoT *pUdpChannel);

/*
 * 返回通道组是否已经连接
 *
 * @param   pChannelGroup       通道组信息
 * @return  TRUE 已连接; FALSE 未连接或连接无效
 */
BOOL    MdsApi_IsValidChannelGroup(
                const MdsApiChannelGroupT *pChannelGroup);

/*
 * 返回当前线程最近一次API调用失败的错误号
 *
 * @return  错误号
 */
int32   MdsApi_GetLastError();

/*
 * 设置当前线程的API错误号
 *
 * @param   errCode             错误号
 */
void    MdsApi_SetLastError(
                int32 errCode);

/*
 * 返回错误号对应的错误信息
 *
 * @param   errCode             错误号
 * @return  错误号对应的错误信息
 */
const char*
        MdsApi_GetErrorMsg(
                int32 errCode);

/*
 * 返回消息头中的状态码所对应的错误信息
 *
 * @param   status              状态码
 * @param   detailStatus        明细状态码
 * @return  状态码所对应的错误信息
 */
const char *
        MdsApi_GetErrorMsg2(
                uint8 status,
                uint8 detailStatus);
/* -------------------------           */


/* ===================================================================
 * 辅助的订阅请求信息维护函数
 * =================================================================== */

/*
 * 设置SubscribeByString接口默认使用的数据模式 (TickType)
 *
 * @param   tickType            数据模式 (TickType) @see eMdsSubscribedTickTypeT
 */
void    MdsHelper_SetTickTypeOnSubscribeByString(
                eMdsSubscribedTickTypeT tickType);

/*
 * 清空订阅信息中的产品列表
 *
 * @param   pSubscribeInfo      订阅信息
 */
void    MdsHelper_ClearSubscribeRequestEntries(
                MdsApiSubscribeInfoT *pSubscribeInfo);

/*
 * 设置订阅模式
 *
 * @param   pSubscribeInfo      订阅信息
 * @param   subMode             订阅模式 @see eMdsSubscribeModeT
 */
void    MdsHelper_SetSubscribeRequestMode(
                MdsApiSubscribeInfoT *pSubscribeInfo,
                eMdsSubscribeModeT subMode);

/*
 * 设置数据模式 (TickType)
 *
 * @param   pSubscribeInfo      订阅信息
 * @param   tickType            数据模式 (TickType) @see eMdsSubscribedTickTypeT
 */
void    MdsHelper_SetSubscribeRequestTickType(
                MdsApiSubscribeInfoT *pSubscribeInfo,
                eMdsSubscribedTickTypeT tickType);

/*
 * 设置订阅的数据种类
 *
 * @param   pSubscribeInfo      订阅信息
 * @param   dataTypes           订阅的数据种类 (e.g. MDS_SUB_DATA_TYPE_L1_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_SNAPSHOT
 *                              | MDS_SUB_DATA_TYPE_L2_BEST_ORDERS
 *                              | MDS_SUB_DATA_TYPE_L2_TRADE)
 *                              @see eMdsSubscribeDataTypeT
 */
void    MdsHelper_SetSubscribeRequestDataTypes(
                MdsApiSubscribeInfoT *pSubscribeInfo,
                int32 dataTypes);

/*
 * 设置指定市场和证券类型的订阅标志
 *
 * @param   pSubscribeInfo      订阅信息
 * @param   exchangeId          交易所代码  @see eMdsExchangeIdT
 *                              - MDS_EXCH_SSE, 上交所
 *                              - MDS_EXCH_SZSE, 深交所
 * @param   securityType        证券类型  @see eMdsSecurityTypeT
 *                              - MDS_SECURITY_TYPE_STOCK, 股票（含债券、基金等现货产品）
 *                              - MDS_SECURITY_TYPE_INDEX, 指数
 *                              - MDS_SECURITY_TYPE_OPTION, 期权（衍生品）
 * @param   subFlag             订阅标志 @see eMdsMktSubscribeFlagT
 *                              -  0: (Default) 根据订阅列表订阅产品行情
 *                              -  1: (All) 订阅该市场和证券类型下的所有产品行情
 *                              -  2: (Disable) 禁用该市场下的所有产品行情
 */
void    MdsHelper_SetSubscribeRequestSubFlag(
                MdsApiSubscribeInfoT *pSubscribeInfo,
                eMdsExchangeIdT exchangeId,
                eMdsSecurityTypeT securityType,
                eMdsMktSubscribeFlagT subFlag);

/*
 * 添加待订阅产品到订阅信息中
 *
 * @param   pSubscribeInfo      订阅信息
 * @param   exchangeId          交易所代码  @see eMdsExchangeIdT
 *                              - MDS_EXCH_SSE, 上交所
 *                              - MDS_EXCH_SZSE, 深交所
 * @param   securityType        证券类型  @see eMdsSecurityTypeT
 *                              - MDS_SECURITY_TYPE_STOCK, 股票（含债券、基金等现货产品）
 *                              - MDS_SECURITY_TYPE_INDEX, 指数
 *                              - MDS_SECURITY_TYPE_OPTION, 期权（衍生品）
 * @param   securityId          证券代码 (转换为整型数值的证券代码)
 * @return  大于等于0, 成功 (返回已添加到订阅列表中的产品数量);
 *          小于0, 失败 (负的错误号)
 */
int32   MdsHelper_AddSubscribeRequestEntry(
                MdsApiSubscribeInfoT *pSubscribeInfo,
                eMdsExchangeIdT exchangeId,
                eMdsSecurityTypeT securityType,
                int32 securityId);
/* -------------------------           */


#ifdef __cplusplus
}
#endif

#endif  /* _MDS_API_H */
