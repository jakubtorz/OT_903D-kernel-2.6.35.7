




#ifndef _TIMER_H
#define _TIMER_H


#include "link.h"

#undef MSEC_PER_SEC
#define MSEC_PER_SEC                    1000
#undef USEC_PER_MSEC
#define USEC_PER_MSEC                   1000
#define USEC_PER_TU                     1024 /* microsecond */


#define MGMT_MAX_TIMEOUT_INTERVAL       ((UINT_32)0x7fffffff)


typedef VOID (*PFN_MGMT_TIMEOUT_FUNC)(UINT_32);

typedef struct _TIMER_T {
    LINK_ENTRY_T            rLinkEntry;
    OS_SYSTIME              rExpiredSysTime;
    UINT_32                 u4Data;
    BOOLEAN                 fgNeedHwAccess;
    PFN_MGMT_TIMEOUT_FUNC   pfMgmtTimeOutFunc;
#if DBG
    UINT_8                  aucDbgString[64];
#endif
} TIMER_T, *P_TIMER_T;





/* Check if time "a" is before time "b" */
#define TIME_BEFORE(a,b)        ((UINT_32)((UINT_32)(a) - (UINT_32)(b)) > 0x7fffffff)


#define TIME_AFTER(a,b)         TIME_BEFORE(b,a)


/* The macros to convert OS system time */
#if (KAL_HZ <= MSEC_PER_SEC) && !(MSEC_PER_SEC % KAL_HZ)
    #define SYSTIME_TO_MSEC(_systime)           ((_systime) * (MSEC_PER_SEC / KAL_HZ))
    #define MSEC_TO_SYSTIME(_msec)              (((_msec) + (MSEC_PER_SEC / KAL_HZ) - 1) / \
                                                 (MSEC_PER_SEC / KAL_HZ))
#elif (KAL_HZ > MSEC_PER_SEC) && !(KAL_HZ % MSEC_PER_SEC)
    #define SYSTIME_TO_MSEC(_systime)           (((_systime) + (KAL_HZ / MSEC_PER_SEC) - 1) / \
                                                 (KAL_HZ / MSEC_PER_SEC))
    #define MSEC_TO_SYSTIME(_msec)              ((_msec) * (KAL_HZ / MSEC_PER_SEC))
#else
    #define SYSTIME_TO_MSEC(_systime)           (((_systime) * MSEC_PER_SEC) / KAL_HZ)
    #define MSEC_TO_SYSTIME(_msec)              (((_msec) * KAL_HZ + MSEC_PER_SEC - 1) / \
                                                 MSEC_PER_SEC)
#endif

#define SYSTIME_TO_SEC(_systime)            ((_systime) / KAL_HZ)
#define SEC_TO_SYSTIME(_sec)                ((_sec) * KAL_HZ)


/* The macros to convert second & millisecond */
#define MSEC_TO_SEC(_msec)                  ((_msec) / MSEC_PER_SEC)
#define SEC_TO_MSEC(_sec)                   ((UINT_32)(_sec) * MSEC_PER_SEC)


/* The macros to convert millisecond & microsecond */
#define USEC_TO_MSEC(_usec)                 ((_usec) / USEC_PER_MSEC)
#define MSEC_TO_USEC(_msec)                 ((UINT_32)(_msec) * USEC_PER_MSEC)


/* The macros to convert TU & microsecond, TU & millisecond */
#define TU_TO_USEC(_tu)                     ((_tu) * USEC_PER_TU)
#define TU_TO_MSEC(_tu)                     USEC_TO_MSEC( TU_TO_USEC(_tu) )


/* The macros to convert TU & & OS system time, round up by 0.5 */
#define TU_TO_SYSTIME(_tu)                  MSEC_TO_SYSTIME( TU_TO_MSEC(_tu) )
#define SYSTIME_TO_TU(_systime)             \
    ((SYSTIME_TO_USEC(_systime) + ((USEC_PER_TU / 2) - 1)) / USEC_PER_TU)


/* The macros to convert OS system time & microsecond */
#define SYSTIME_TO_USEC(_systime)           (SYSTIME_TO_MSEC(_systime) * USEC_PER_MSEC)


/* The macro to get the current OS system time */
#define GET_CURRENT_SYSTIME(_systime_p)     {*(_systime_p) = kalGetTimeTick();}

/* The macro to copy the system time */
#define COPY_SYSTIME(_destTime, _srcTime)   (_destTime) = (_srcTime)

/* The macro to get the system time difference between t1 and t2 (t1 - t2) */
#define GET_SYSTIME_DIFFERENCE(_time1, _time2, _diffTime) \
        (_diffTime) = (_time1) - (_time2)

/* The macro to check for the expiration, if TRUE means _currentTime >= _expirationTime */
#define CHECK_FOR_EXPIRATION(_currentTime, _expirationTime) \
        ((UINT_32)(_currentTime) - (UINT_32)(_expirationTime) <= 0x7fffffffUL)

/* The macro to check for the timeout */
#define CHECK_FOR_TIMEOUT(_currentTime, _timeoutStartingTime, _timeout) \
    CHECK_FOR_EXPIRATION((_currentTime), ((_timeoutStartingTime) + (_timeout)))

/* The macro to set the expiration time with a specified timeout *//* Watch out for round up.*/
#define SET_EXPIRATION_TIME(_expirationTime, _timeout) \
        { \
            GET_CURRENT_SYSTIME(&(_expirationTime)); \
            (_expirationTime) += (OS_SYSTIME)(_timeout); \
        }

#define timerRenewTimer(adapter,tmr,interval) \
        timerStartTimer(adapter,tmr,interval,(tmr)->function,(tmr)->data)

#define timerCheckTimeout(eol) \
        ((INT_32)kalGetTimeTick() - (INT_32)(eol) >= 0)

#define MGMT_INIT_TIMER(_adapter_p, _timer, _callbackFunc) \
        timerInitTimer(_adapter_p, &(_timer), (UINT_32)(_callbackFunc))


VOID
timerInitialize (
    IN P_ADAPTER_T  prAdapter,
    IN PFN_TIMER_CALLBACK   prfnTimerHandler
    );

VOID
timerUnInitialize (
    IN P_ADAPTER_T  prAdapter
    );

VOID
timerInitTimer (
    IN P_ADAPTER_T              prAdapter,
    IN P_TIMER_T                prTimer,
    IN PFN_MGMT_TIMEOUT_FUNC    pfFunc,
    IN UINT_32                  u4Data,
    IN BOOLEAN                  fgNeedHwAccess
    );

VOID
timerStopTimer (
    IN P_ADAPTER_T              prAdapter,
    IN P_TIMER_T                prTimer
    );

VOID
timerStartTimer (
    IN P_ADAPTER_T              prAdapter,
    IN P_TIMER_T                prTimer,
    IN UINT_32                  u4TimeoutMs
    );

VOID
timerDoTimeOutCheck (
    IN P_ADAPTER_T  prAdapter
    );

#if !CFG_USE_SW_ROOT_TIMER
VOID
timerDoTimeOutCheckOnLogicReset (
    IN P_ADAPTER_T  prAdapter
    );
#endif

__KAL_INLINE__
INT_32
timerPendingTimer (
    IN P_TIMER_T prTimer
    )
{
    ASSERT(prTimer);

    return prTimer->rLinkEntry.prNext != NULL;
}

#endif /* _TIMER_H */
