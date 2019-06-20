/*
 * Copyright 2009-2016 the original author or authors.
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
 * @file    spk_general_client_define.h
 *
 * 适用于一般情况下的通用客户端环境
 *
 * @version 1.0 2016/8/10
 * @since   2016/8/10
 */


#ifndef _SPK_GENERAL_CLIENT_ENV_DEFINE_H
#define _SPK_GENERAL_CLIENT_ENV_DEFINE_H


#include    <sutil/net/spk_global_packet.h>
#include    <sutil/net/spk_data_buffer_define.h>
#include    <sutil/net/spk_socket_base_define.h>


#ifdef __cplusplus
extern "C" {
#endif


/* ===================================================================
 * 常量定义
 * =================================================================== */

/** 默认的心跳间隔(秒) */
#define GENERAL_CLI_DEFAULT_HEARTBEAT_INTERVAL      (30)
/** 最小的心跳间隔(秒) */
#define GENERAL_CLI_MIN_HEARTBEAT_INTERVAL          (5)
/** 最大的心跳间隔(秒) */
#define GENERAL_CLI_MAX_HEARTBEAT_INTERVAL          (3600)

/** 默认的UDP连接的心跳间隔(秒) */
#define GENERAL_CLI_DEFAULT_UDP_HEARTBEAT_INTERVAL  (30)
/** 最大的UDP连接的心跳间隔/最大空闲时间(秒) */
#define GENERAL_CLI_MAX_UDP_ALIVE_INTERVAL          (180)
/** 默认的超时时间(毫秒) */
#define GENERAL_CLI_DEFAULT_TIMEOUT_MS              SPK_DEFAULT_SO_TIMEOUT_MS

/** 最大的单个应答消息大小 */
#define GENERAL_CLI_MAX_RSPMSG_SIZE                 (4 * 1024 * 1024)
/** 默认的TCP接收缓存大小 */
#define GENERAL_CLI_DEFAULT_TCP_RECVBUF_SIZE        (8 * 1024 * 1024)
/** 默认的UDP接收缓存大小 */
#define GENERAL_CLI_DEFAULT_UDP_RECVBUF_SIZE        (4 * 1024 * 1024)
/** 最小的接收缓存剩余可用空间大小 */
#define GENERAL_CLI_MIN_RECVBUF_SURPLUS_SIZE        (128 * 1024)
/** 默认的编解码缓存大小 */
#define GENERAL_CLI_DEFAULT_CODEC_BUF_SIZE          (512 * 1024)

/** 最大的主机编号 */
#define GENERAL_CLI_MAX_HOST_NUM                    (9)
/** 可连接的最大远程服务器数量 */
#define GENERAL_CLI_MAX_REMOTE_CNT                  (8)
/** 连接通道组的最大连接数量 */
#define GENERAL_CLI_MAX_CHANNEL_GROUP_SIZE          (64)

/** 会话信息中用于存储自定义数据的扩展空间大小 */
#define GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE        (128)

/** 客户端名称最大长度 */
#define GENERAL_CLI_MAX_NAME_LEN                    (32)
/** 客户端说明最大长度 */
#define GENERAL_CLI_MAX_DESC_LEN                    (32)
/** 密码最大长度 */
#define GENERAL_CLI_MAX_PWD_LEN                     (40)
/** 发送方/接收方代码字符串的最大长度 */
#define GENERAL_CLI_MAX_COMP_ID_LEN                 (32)
/* -------------------------           */


/* ===================================================================
 * 常量定义（枚举类型定义）
 * =================================================================== */

/**
 * 服务器集群的集群类型
 */
typedef enum _eGeneralClientClusterType {
    GENERAL_CLI_CLUSTER_UNDEFINED       = 0,    /**< 未指定 (使用默认的集群类型) */
    GENERAL_CLI_CLUSTER_REPLICA_SET     = 1,    /**< 基于复制集的高可用集群 */
    GENERAL_CLI_CLUSTER_PEER_NODES      = 2,    /**< 基于对等节点的服务器集群 */
    __MAX_GENERAL_CLI_CLUSTER_TYPE
} eGeneralClientClusterTypeT;
/* -------------------------           */


/**
 * 加密类型
 */
typedef enum _eGeneralClientEncryptType {
    GENERAL_CLI_ENCRYPT_NONE            = 0,    /**< 加密类型-无 */
    GENERAL_CLI_ENCRYPT_MD5             = 0x01, /**< 加密类型-MD5 */
    GENERAL_CLI_ENCRYPT_SHA             = 0x02, /**< 加密类型-SHA */
    GENERAL_CLI_ENCRYPT_DES             = 0x10, /**< 加密类型-DES */
    GENERAL_CLI_ENCRYPT_AES             = 0x20, /**< 加密类型-AES */
    GENERAL_CLI_ENCRYPT_RSA             = 0x40  /**< 加密类型-RSA */
} eGeneralClientEncryptTypeT;
/* -------------------------           */


/* ===================================================================
 * 结构体定义
 * =================================================================== */

/**
 * 通用的客户端会话信息（连接通道信息）定义
 */
typedef struct _SGeneralClientChannel {
    union {
        /** Socket描述符 */
        SPK_SOCKET      socketFd;
        /** 按64位对齐的填充域 */
        uint64          __socket_fd_filler;
    };

    int32               heartBtInt;             /**< 心跳间隔，单位为秒 (允许预先赋值) */
    int32               testReqInt;             /**< 测试请求间隔，单位为秒 */
    uint8               protocolType;           /**< 协议类型 (Binary, JSON等) (允许预先赋值) */
    uint8               remoteHostNum;          /**< 已连接上的对端服务器的主机编号 */
    uint8               remoteIsLeader;         /**< 对端服务器是否是'主节点' */
    uint8               leaderHostNum;          /**< '主节点'的主机编号 */
    uint8               __filler1[4];           /**< 按64位对齐填充域 */

    struct _SDataBufferVar
                        __codecBuf;             /**< 编解码缓存 */
    struct _SDataBufferVar
                        __recvBuf;              /**< 接收缓存 */
    char                *__pDataStartPoint;     /**< 数据起始位置指针 */
    void                *__customPtr;           /**< 可以由应用层自定义使用的指针变量 */
    int32               __reavedSize;           /**< 已接收到但尚未处理的数据长度 */
    int32               __customFlag;           /**< 可以由应用层自定义使用的整型变量 */
    int64               __totalInMsgSize;       /**< 累计接收到的未压缩数据大小 */
    int64               __totalCompressedSize;  /**< 累计接收到的已压缩数据大小 */
    int64               __totalDecompressSize;  /**< 解压缩后的数据总大小 */

    uint64              firstInMsgSeq;          /**< 已接收到的起始入向消息序号 */
    uint64              lastInMsgSeq;           /**< 实际已接收到的入向消息序号 (对应于登录应答消息的 lastOutMsgSeq) */
    uint64              nextInMsgSeq;           /**< 期望的入向消息序号 */
    STimevalT           lastRecvTime;           /**< 接收时间 */

    SSocketChannelInfoT channel;                /**< 连接通道信息 */
    uint64              nextOutMsgSeq;          /**< 出向消息序号 */
    uint64              lastOutMsgSeq;          /**< 实际已发送的出向消息序号 (对应于登录应答消息的 lastInMsgSeq) */
    STimevalT           lastSendTime;           /**< 发送时间 */

    /** 发送方代码 */
    char                senderCompId[GENERAL_CLI_MAX_COMP_ID_LEN];
    /** 接收方代码 */
    char                targetCompId[GENERAL_CLI_MAX_COMP_ID_LEN];

    int32               __magicNumber;          /**< 标识会话结构是否已经正确初始化过 */
    int32               __magicSize;            /**< 标识会话信息的结构体大小 */

    uint8               __channelType;          /**< 通道类型 */
    int8                __clEnvId;              /**< 客户端环境号 */
    uint8               __groupFlag;            /**< 通道组标志 */
    uint8               __protocolHints;        /**< 协议约定信息 */
    uint8               __filler[4];            /**< 按64位对齐填充域 */

    /** 保留给服务器或API内部使用的，用于存储自定义数据的扩展空间 */
    union {
        char            buf[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE];
        int8            i8[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE];
        uint8           u8[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE];
        int16           i16[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 2];
        uint16          u16[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 2];
        int32           i32[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 4];
        uint32          u32[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 4];
        int64           i64[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 8];
        uint64          u64[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 8];
        void            *ptr[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 8];
        char            __padding[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE
                                  + SPK_CACHE_LINE_SIZE];
    } __reserveData;

    /** 可以由应用层自定义使用的，用于存储自定义数据的扩展空间 */
    union {
        char            buf[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE];
        int8            i8[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE];
        uint8           u8[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE];
        int16           i16[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 2];
        uint16          u16[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 2];
        int32           i32[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 4];
        uint32          u32[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 4];
        int64           i64[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 8];
        uint64          u64[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 8];
        void            *ptr[GENERAL_CLI_MAX_SESSION_EXTDATA_SIZE / 8];
    } __extData;
} SGeneralClientChannelT;


/* 结构体的初始化值定义 */
#define NULLOBJ_GENERAL_CLIENT_CHANNEL          \
        {0}, 0, 0, \
        0, 0, 0, 0, {0}, \
        {NULLOBJ_SPK_DATA_BUFFER}, \
        {NULLOBJ_SPK_DATA_BUFFER}, \
        0, 0, 0, 0, 0, 0, 0, \
        0, 0, 0, {0, 0}, \
        {NULLOBJ_SOCKET_CHANNEL_INFO}, \
        0, 0, {0, 0}, \
        {0}, {0}, \
        0, 0, \
        0, 0, 0, 0, {0}, \
        {{0}}, {{0}}
/* -------------------------           */


/**
 * 通用的连接通道组定义（多个连接通道的集合）
 */
typedef struct _SGeneralClientChannelGroup {
    /** 连接通道数量 */
    int32               channelCount;
    /** 可以由应用层自定义使用的整型变量 */
    int32               __customFlag;
    /** 连接通道信息列表 */
    SGeneralClientChannelT
                        *channelList[GENERAL_CLI_MAX_CHANNEL_GROUP_SIZE];

    /** 最大的连接描述符 (仅供系统内部使用) */
    int32               __maxFd;
    /** 通道组标志 (仅供系统内部使用) */
    uint8               __groupFlag;
    /** 按64位对齐的填充域 */
    uint8               __filler[3];
    /** 连接描述符集合 (仅供系统内部使用) */
    fd_set              __fdSet;
} SGeneralClientChannelGroupT;


/* 结构体的初始化值定义 */
#if defined (__WINDOWS__) || (defined (__MINGW__) && ! defined (USE_GNULIB))
#define NULLOBJ_GENERAL_CLIENT_CHANNEL_GROUP    \
        0, 0, {0}, \
        0, 0, {0}, \
        {0, {0}}

#else
#define NULLOBJ_GENERAL_CLIENT_CHANNEL_GROUP    \
        0, 0, {0}, \
        0, 0, {0}, \
        {{0}}

#endif
/* -------------------------           */


/**
 * Socket URI地址信息
 */
typedef struct _SGeneralClientAddrInfo {
    /** 地址信息 */
    char                uri[SPK_MAX_URI_LEN];
    /** 接收方代码 */
    char                targetCompId[GENERAL_CLI_MAX_COMP_ID_LEN];
    /** 用户名 */
    char                username[GENERAL_CLI_MAX_NAME_LEN];
    /** 密码 */
    char                password[GENERAL_CLI_MAX_PWD_LEN];

    /** 主机编号 */
    uint8               hostNum;
    /** 按64位对齐的填充域 */
    uint8               __filler[7];
} SGeneralClientAddrInfoT;


/* 结构体初始化值定义 */
#define NULLOBJ_GENERAL_CLIENT_ADDR_INFO        \
        {0}, {0}, {0}, {0}, \
        0, {0}
/* -------------------------           */


/**
 * 远程主机配置信息
 */
typedef struct _SGeneralClientRemoteCfg {
    int32               addrCnt;                /**< 服务器地址的数量 */
    int32               heartBtInt;             /**< 心跳间隔,单位为秒 */
    uint8               clusterType;            /**< 服务器集群的集群类型 (0:对等节点, 1:复制集) */
    int8                clEnvId;                /**< 客户端环境号 */
    uint8               __filler[6];            /**< 按64位对齐的填充域 */

    /** 发送方代码 */
    char                senderCompId[GENERAL_CLI_MAX_COMP_ID_LEN];
    /** 接收方代码 */
    char                targetCompId[GENERAL_CLI_MAX_COMP_ID_LEN];
    /** 用户名 */
    char                username[GENERAL_CLI_MAX_NAME_LEN];
    /** 密码 */
    char                password[GENERAL_CLI_MAX_PWD_LEN];

    /** 服务器地址列表 */
    SGeneralClientAddrInfoT     addrList[GENERAL_CLI_MAX_REMOTE_CNT];

    /** 套接口选项配置 */
    SSocketOptionConfigT        socketOpt;
} SGeneralClientRemoteCfgT;


/* 结构体的初始化值定义 */
#define NULLOBJ_GENERAL_CLIENT_REMOTE_CFG       \
        0, 0, 0, 0, {0}, \
        {0}, {0}, {0}, {0}, \
        {{NULLOBJ_GENERAL_CLIENT_ADDR_INFO}}, \
        {NULLOBJ_SOCKET_OPTION_CONFIG}
/* -------------------------           */


/* ===================================================================
 * 辅助的宏定义
 * =================================================================== */

/**
 * 返回服务器地址所对应的接收方代码配置
 *
 * @param   P_REMOTE_CFG    主机配置信息的结构体指针 @see SGeneralClientRemoteCfgT
 * @param   ADDR_INDEX      服务器地址的顺序号
 * @return  接收方代码
 */
#define _SGeneralClient_GetTargetCompId(P_REMOTE_CFG, ADDR_INDEX)   \
        ( (P_REMOTE_CFG)->addrList[(ADDR_INDEX)].targetCompId[0] != '\0' \
                ? (P_REMOTE_CFG)->addrList[(ADDR_INDEX)].targetCompId \
                        : (P_REMOTE_CFG)->targetCompId )


/**
 * 返回服务器地址所对应的用户名配置
 *
 * @param   P_REMOTE_CFG    主机配置信息的结构体指针 @see SGeneralClientRemoteCfgT
 * @param   ADDR_INDEX      服务器地址的顺序号
 * @return  用户名
 */
#define _SGeneralClient_GetUsername(P_REMOTE_CFG, ADDR_INDEX)       \
        ( (P_REMOTE_CFG)->addrList[(ADDR_INDEX)].username[0] != '\0' \
                ? (P_REMOTE_CFG)->addrList[(ADDR_INDEX)].username \
                        : (P_REMOTE_CFG)->username )


/**
 * 返回服务器地址所对应的密码配置
 *
 * @param   P_REMOTE_CFG    主机配置信息的结构体指针 @see SGeneralClientRemoteCfgT
 * @param   ADDR_INDEX      服务器地址的顺序号
 * @return  密码
 */
#define _SGeneralClient_GetPassword(P_REMOTE_CFG, ADDR_INDEX)       \
        ( (P_REMOTE_CFG)->addrList[(ADDR_INDEX)].password[0] != '\0' \
                ? (P_REMOTE_CFG)->addrList[(ADDR_INDEX)].password \
                        : (P_REMOTE_CFG)->password )
/* -------------------------           */


#ifdef __cplusplus
}
#endif

#endif  /* _SPK_GENERAL_CLIENT_ENV_DEFINE_H */
