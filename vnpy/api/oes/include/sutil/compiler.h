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
 * @file    compiler.h
 *
 * 公共类型定义
 *
 * @version $Id$
 * @since   2011/07/07
 */


#ifndef _SPK_COMPILER_H
#define _SPK_COMPILER_H


#include    <sutil/types.h>
#include    <sutil/stdc.h>

#include    <stdio.h>
#include    <stdlib.h>
#include    <stddef.h>
#include    <limits.h>
#include    <errno.h>
#include    <assert.h>


/*
 * 平台特定的头文件
 */
#if ! (defined (__WINDOWS__) || defined (__MINGW__))
#   include <sys/select.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


/* ===================================================================
 * 针对GCC的扩展属性定义
 * =================================================================== */

#if defined(__GNUC__) && (__GNUC__ >= 3)
#   undef   __inline
#   define  __inline                    inline __attribute__ ((always_inline))

#   ifndef  likely
#       define  likely(x)               __builtin_expect (!!(x), 1)
#   endif

#   ifndef  unlikely
#       define  unlikely(x)             __builtin_expect (!!(x), 0)
#   endif

#   ifndef  __attribute_align
#       define  __attribute_align(x)    __attribute__ ((aligned (x)))
#   endif

#   ifndef  __attribute_deprecated
#       define  __attribute_deprecated  __attribute__ ((deprecated))
#   endif

#   ifndef  __attribute_unused
#       define  __attribute_unused      __attribute__ ((unused))
#   endif

#else
#   ifndef  __attribute__
#       define  __attribute__(A)
#   endif

#   ifndef  __inline
#       define  __inline                inline
#   endif

#   ifndef  likely
#       define  likely(x)               (x)
#   endif

#   ifndef  unlikely
#       define  unlikely(x)             (x)
#   endif

#   ifndef  __attribute_align
#       define  __attribute_align(x)
#   endif

#   ifndef  __attribute_deprecated
#       define  __attribute_deprecated
#   endif

#   ifndef  __attribute_unused
#       define  __attribute_unused
#   endif

#endif
/* -------------------------           */


/*
 * define __thread for thread local storage
 */
#ifndef __thread
#   if defined (__WINDOWS__)
#       define  __thread                __declspec(thread)
#   elif defined (__MINGW32__) && ! (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#       define  __thread                __declspec(thread)
#   else
#       define  __thread                __thread
#   endif
#endif
/* -------------------------           */


/*
 * for inline error in g++ and gtest
 * 为了解决编写和编译单元测试代码时的, 待测试函数的可见性而设
 */
#if defined(__cplusplus) || defined(__TEST) || defined(__DEBUG) \
        || defined(__MINGW__) || defined(__CYGWIN__) || defined(__WINDOWS__)
#  define   SPK_INTERNAL_INLINE
#else
#  define   SPK_INTERNAL_INLINE         inline __attribute__ ((always_inline))
#endif
/* -------------------------           */


/*
 * 为了对内部函数进行测试而定义的常量
 */
#if defined(__TEST)
#   define  SPK_TESTABLE_STATIC
#else
#   define  SPK_TESTABLE_STATIC         static
#endif
/* -------------------------           */


/* ===================================================================
 * 常用的数值处理宏定义
 * =================================================================== */

/*
 * 返回x对应的负数
 */
#ifndef NEG
#   define  NEG(x)                      ((x) > 0 ? -(x) : (x))
#endif

#undef  SPK_NEG
#define SPK_NEG(x)                      ((x) > 0 ? -(x) : (x))
/* -------------------------           */


/* ===================================================================
 * 结构体处理相关的宏函数定义
 * =================================================================== */

#ifndef offsetof
#   define  offsetof(TYPE, MEMBER)  ( (int32) &((TYPE *) 0)->MEMBER )
#endif

#ifndef container_of
#   define  container_of(PTR, TYPE, MEMBER)     \
            ( (TYPE *) ((char *) (PTR) - offsetof(TYPE, MEMBER)) )
#endif


/*
 * 返回以SIZE / BASESIZE 的最大倍数
 */
#undef  SPK_CEIL_TIMES
#define SPK_CEIL_TIMES(SIZE, BASESIZE)          \
        ( ((SIZE) + (BASESIZE) - 1) / (BASESIZE) )

/*
 * 返回BASESIZE的最大整数倍的SIZE
 */
#undef  SPK_ADJUST_SIZE
#define SPK_ADJUST_SIZE(SIZE, BASESIZE)         \
        ( SPK_CEIL_TIMES((SIZE), (BASESIZE)) * (BASESIZE) )
/* -------------------------           */

/*
 * 返回以SIZE / BASESIZE 4舍5入后的倍数
 */
#undef  SPK_ROUND_TIMES
#define SPK_ROUND_TIMES(SIZE, BASESIZE)         \
        ( ((SIZE) + ((BASESIZE) >> 1)) / (BASESIZE) )

/*
 * 返回BASESIZE 4舍5入后的整倍数 SIZE
 */
#undef  SPK_ROUND_SIZE
#define SPK_ROUND_SIZE(SIZE, BASESIZE)          \
        ( SPK_ROUND_TIMES((SIZE), (BASESIZE)) * (BASESIZE) )
/* -------------------------           */


#undef  SPK_ALIGN_SIZE
#undef  SPK_ALIGN_SIZE4
#undef  SPK_ALIGN_SIZE8
#undef  SPK_ALIGN_SIZE16
#undef  SPK_ALIGN_SIZE32
#undef  SPK_ALIGN_SIZE256
#undef  SPK_ALIGN_SIZE1024

/*
 * 返回以BASESIZE为单位对齐的SIZE (BASESIZE 需为2的n次方)
 */
#define SPK_ALIGN_SIZE(SIZE, BASESIZE)          \
        ( ((SIZE) + (BASESIZE - 1)) & ~(BASESIZE - 1) )

/* 返回按4字节对齐的SIZE */
#define SPK_ALIGN_SIZE4(SIZE)           SPK_ALIGN_SIZE((SIZE), 4)

/* 返回按8字节对齐的SIZE */
#define SPK_ALIGN_SIZE8(SIZE)           SPK_ALIGN_SIZE((SIZE), 8)

/* 返回按16字节对齐的SIZE */
#define SPK_ALIGN_SIZE16(SIZE)          SPK_ALIGN_SIZE((SIZE), 16)

/* 返回按32字节对齐的SIZE */
#define SPK_ALIGN_SIZE32(SIZE)          SPK_ALIGN_SIZE((SIZE), 32)

/* 返回按256字节对齐的SIZE */
#define SPK_ALIGN_SIZE256(SIZE)         SPK_ALIGN_SIZE((SIZE), 256)

/* 返回按1024字节对齐的SIZE */
#define SPK_ALIGN_SIZE1024(SIZE)        SPK_ALIGN_SIZE((SIZE), 1024)
/* -------------------------           */


/* ===================================================================
 * sched_yield 及 sleep 定义
 * =================================================================== */

/*
 * 让出CPU, 同时进行最小粒度的Sleep
 */
#undef  SPK_SLEEP_YIELD
#if defined (_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L)
#   include <time.h>

#   define  SPK_SLEEP_YIELD()           \
    do { \
        struct timespec __SLEEP_YIELD_ts = {0, 1}; \
        nanosleep(&__SLEEP_YIELD_ts, (struct timespec *) NULL); \
    } while (0)
#elif defined (__WINDOWS__) || defined (__MINGW__)
#   define  SPK_SLEEP_YIELD()           \
            Sleep(0)
#else
#   define  SPK_SLEEP_YIELD()           \
    do { \
        struct timeval  __SLEEP_YIELD_tv = {0, 1}; \
        select(0, (fd_set *) NULL, (fd_set *) NULL, (fd_set *) NULL, \
                &__SLEEP_YIELD_tv); \
    } while (0)
#endif
/* -------------------------           */


/*
 * 让出CPU, 无需进行Sleep
 */
#undef  SPK_SCHED_YIELD
#if defined (__LINUX__)
#   include <sched.h>
#   define  SPK_SCHED_YIELD()           sched_yield()
#elif defined (__WINDOWS__) || defined (__MINGW__)
#   define  SPK_SCHED_YIELD()           YieldProcessor()
#else
#   define  SPK_SCHED_YIELD()           SPK_SLEEP_YIELD()
#endif
/* -------------------------           */


/*
 * 睡眠指定的毫秒数
 */
#undef  SPK_SLEEP_MS
#if defined (_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L)
#   include <time.h>

#   define  SPK_SLEEP_MS(MS)            \
    do { \
        struct timespec __SLEEP_MS_ts = {0, 0}; \
        long            __SLEEP_MS_msec = (MS); \
        if (__SLEEP_MS_msec < 1000) { \
            __SLEEP_MS_ts.tv_nsec = __SLEEP_MS_msec * 1000000; \
        } else { \
            __SLEEP_MS_ts.tv_sec = (__SLEEP_MS_msec / 1000); \
            __SLEEP_MS_ts.tv_nsec = (__SLEEP_MS_msec % 1000) * 1000000; \
        } \
        nanosleep(&__SLEEP_MS_ts, (struct timespec *) NULL); \
    } while (0)
#elif defined (__WINDOWS__) || defined (__MINGW__)
#   define  SPK_SLEEP_MS(MS)            \
            Sleep((MS))
#else
#   define  SPK_SLEEP_MS(MS)            \
    do { \
        struct timeval  __SLEEP_MS_tv = {0, 0}; \
        long            __SLEEP_MS_msec = (MS); \
        if (__SLEEP_MS_msec < 1000) { \
            __SLEEP_MS_tv.tv_usec = __SLEEP_MS_msec * 1000; \
        } else { \
            __SLEEP_MS_tv.tv_sec = (__SLEEP_MS_msec / 1000); \
            __SLEEP_MS_tv.tv_usec = (__SLEEP_MS_msec % 1000) * 1000; \
        } \
        select(0, (fd_set *) NULL, (fd_set *) NULL, (fd_set *) NULL, \
                &__SLEEP_MS_tv); \
    } while (0)
#endif
/* -------------------------           */


/*
 * 睡眠指定的微秒数
 */
#undef  SPK_SLEEP_US
#if defined (_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L)
#   include <time.h>

#   define  SPK_SLEEP_US(US)            \
    do { \
        struct timespec __SLEEP_US_ts = {0, 0}; \
        long            __SLEEP_US_msec = (US); \
        if (__SLEEP_US_msec < 1000000) { \
            __SLEEP_US_ts.tv_nsec = __SLEEP_US_msec * 1000; \
        } else { \
            __SLEEP_US_ts.tv_sec = (__SLEEP_US_msec / 1000000); \
            __SLEEP_US_ts.tv_nsec = (__SLEEP_US_msec % 1000000) * 1000; \
        } \
        nanosleep(&__SLEEP_US_ts, (struct timespec *) NULL); \
    } while (0)
#elif defined (__WINDOWS__) || defined (__MINGW__)
#   define  SPK_SLEEP_US(US)            \
    do { \
        long            __SLEEP_US_msec = (US); \
        Sleep(__SLEEP_US_msec > 1000 ? __SLEEP_US_msec / 1000 : 1); \
    } while (0)
#else
#   define  SPK_SLEEP_US(US)            \
    do { \
        struct timeval  __SLEEP_US_tv = {0, 0}; \
        long            __SLEEP_US_msec = (US); \
        if (__SLEEP_US_msec < 1000000) { \
            __SLEEP_US_tv.tv_usec = __SLEEP_US_msec; \
        } else { \
            __SLEEP_US_tv.tv_sec = (__SLEEP_US_msec / 1000000); \
            __SLEEP_US_tv.tv_usec = (__SLEEP_US_msec % 1000000); \
        } \
        select(0, (fd_set *) NULL, (fd_set *) NULL, (fd_set *) NULL, \
                &__SLEEP_US_tv); \
    } while (0)
#endif
/* -------------------------           */


/*
 * 睡眠指定的纳秒数 (最大超过1秒)
 */
#undef  SPK_SLEEP_NS
#if defined (_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L)
#   include <time.h>

#   define  SPK_SLEEP_NS(NS)            \
    do { \
        struct timespec __SLEEP_NS_ts = {0, (NS) % 1000000000}; \
        nanosleep(&__SLEEP_NS_ts, (struct timespec *) NULL); \
    } while (0)
#elif defined (__WINDOWS__) || defined (__MINGW__)
#   define  SPK_SLEEP_NS(NS)            \
    do { \
        long            __SLEEP_NS_msec = (NS); \
        Sleep(__SLEEP_NS_msec > 1000000 ? __SLEEP_NS_msec / 1000000 \
                : __SLEEP_NS_msec > 1000 ? 1 : 0); \
    } while (0)
#else
#   define  SPK_SLEEP_NS(NS)            \
    do { \
        struct timeval  __SLEEP_NS_tv = {0, ((NS) % 1000000000) / 1000}; \
        select(0, (fd_set *) NULL, (fd_set *) NULL, (fd_set *) NULL, \
                &__SLEEP_NS_tv); \
    } while (0)
#endif
/* -------------------------           */


/*
 * 定时并睡眠指定的毫秒数 (粗略的定时)
 */
#undef  SPK_TIMED_SLEEP
#if defined (_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L)
#   include <time.h>

#   define  SPK_TIMED_SLEEP(MS)         \
    do { \
        long            __TIMED_SLEEP_msec = (MS); \
        struct timespec __TIMED_SLEEP_tsReq, __TIMED_SLEEP_tsRem; \
        struct timespec *__TIMED_SLEEP_pTimerRep = &__TIMED_SLEEP_tsReq, \
                        *__TIMED_SLEEP_pTimerRem = &__TIMED_SLEEP_tsRem, \
                        *__TIMED_SLEEP_pTimerTmp; \
        if (__TIMED_SLEEP_msec > 999) { \
            __TIMED_SLEEP_tsReq.tv_sec = (__TIMED_SLEEP_msec / 1000); \
            __TIMED_SLEEP_tsReq.tv_nsec = \
                    (__TIMED_SLEEP_msec % 1000) * 1000000; \
        } else { \
            __TIMED_SLEEP_tsReq.tv_sec = 0; \
            __TIMED_SLEEP_tsReq.tv_nsec = __TIMED_SLEEP_msec * 1000000; \
        } \
        while (nanosleep(__TIMED_SLEEP_pTimerRep, __TIMED_SLEEP_pTimerRem) \
                && errno == EINTR) { \
            __TIMED_SLEEP_pTimerTmp = __TIMED_SLEEP_pTimerRep; \
            __TIMED_SLEEP_pTimerRep = __TIMED_SLEEP_pTimerRem; \
            __TIMED_SLEEP_pTimerRem = __TIMED_SLEEP_pTimerTmp; \
        } \
    } while (0)
#else
#   define  SPK_TIMED_SLEEP(MS)         SPK_SLEEP_MS(MS)
#endif
/* -------------------------           */


/*
 * 定时并睡眠, 按指定的起始时间计算需要睡眠的毫秒数
 */
#undef  SPK_TIMED_SLEEP_SINCE
#if ( defined (_XOPEN_SOURCE) && (_XOPEN_SOURCE >= 600) ) \
        || ( defined (_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L) )
#   include <time.h>

#   define  SPK_TIMED_SLEEP_SINCE(MS, P_SINCE_TV)       \
    do { \
        struct timespec __TIMED_SINCE_abstime; \
        long            __TIMED_SINCE_msec = (MS); \
        if (__TIMED_SINCE_msec > 999) { \
            __TIMED_SINCE_abstime.tv_sec = (P_SINCE_TV)->tv_sec \
                    + (__TIMED_SINCE_msec / 1000); \
            __TIMED_SINCE_abstime.tv_nsec = (P_SINCE_TV)->tv_usec * 1000 \
                    + (__TIMED_SINCE_msec % 1000) * 1000000; \
        } else { \
            __TIMED_SINCE_abstime.tv_sec = (P_SINCE_TV)->tv_sec; \
            __TIMED_SINCE_abstime.tv_nsec = (P_SINCE_TV)->tv_usec * 1000 \
                    + __TIMED_SINCE_msec * 1000000; \
        } \
        if (__TIMED_SINCE_abstime.tv_nsec >= 1000000000LL) { \
            __TIMED_SINCE_abstime.tv_nsec -= 1000000000LL; \
            __TIMED_SINCE_abstime.tv_sec += 1; \
        } \
        while (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, \
                &__TIMED_SINCE_abstime, (struct timespec *) NULL) == EINTR); \
    } while (0)
#else
#   include <time.h>

#   define  SPK_TIMED_SLEEP_SINCE(MS, P_SINCE_TV)       \
    do { \
        struct timeval  __TIMED_SINCE_now; \
        long            __TIMED_SINCE_diff; \
        long            __TIMED_SINCE_msec = (MS); \
        __spk_gettimeofday(&__TIMED_SINCE_now, (struct timezone *) NULL); \
        __TIMED_SINCE_diff = \
                (__TIMED_SINCE_now.tv_sec - (P_SINCE_TV)->tv_sec) * 1000 \
                + (__TIMED_SINCE_now.tv_usec - (P_SINCE_TV)->tv_usec) / 1000; \
        if (__TIMED_SINCE_diff < __TIMED_SINCE_msec) { \
            __TIMED_SINCE_msec -= __TIMED_SINCE_diff; \
            SPK_TIMED_SLEEP(__TIMED_SINCE_msec); \
        } \
    } while (0)
#endif
/* -------------------------           */


/* ===================================================================
 * 编译屏障（compiler barrier）、内存屏障（memory barrier）等汇编语句定义
 * =================================================================== */

/*
 * COMPILER_BARRIER
 */
static __inline void __SPK_compiler_barrier(void) {
#if ! defined (__WINDOWS__)
    __asm__ __volatile__("": : :"memory");
#elif defined (_MSC_VER) && _MSC_VER >= 1400
    MemoryBarrier();
#endif
}

#ifndef SPK_COMPILER_BARRIER
#   define SPK_COMPILER_BARRIER()       __SPK_compiler_barrier()
#endif
/* -------------------------           */


/*
 * cpu_relax
 */
#ifndef __SPK_CPU_RELAX
# if defined (__x86_64__) && ! defined (__WINDOWS__)
    /* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
    static __inline void __SPK_rep_nop(void) {
        __asm__ __volatile__("rep;nop": : :"memory");
    }
#   define  __SPK_CPU_RELAX()           __SPK_rep_nop()

# else
    /*#   warning "Not defined __x86_64__"*/
#   define  __SPK_CPU_RELAX()           __SPK_compiler_barrier()

# endif
#endif
/* -------------------------           */


/*
 * SPK_ACCESS_ONCE
 */
#ifndef SPK_ACCESS_ONCE
# if ! defined (__WINDOWS__)
#   define SPK_ACCESS_ONCE(x)           (*(volatile typeof(x) *) &(x))

# elif defined (_MSC_VER) && _MSC_VER >= 1600
#   define SPK_ACCESS_ONCE(x)           (*(volatile decltype(x) *) &(x))

# else
#   define SPK_ACCESS_ONCE(x)           (*(volatile decltype(x) *) &(x))

# endif
#endif
/* -------------------------           */


#ifdef __cplusplus
}
#endif

#endif  /* _SPK_COMPILER_H */
