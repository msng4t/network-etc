#ifndef _TIMEVALUE_INCLUDE_
#define _TIMEVALUE_INCLUDE_
#include <time.h>
#include <inttypes.h>
#include "Define.h"

MINA_NAMESPACE_START

#ifndef INT64_C
#   define INT64_C(n)      n##ll
#endif
/** 
 * define nano second
 */
#define TIME_NSEC       (INT64_C(1))

/**
 * define a millisecond that used nano second
 */
#define TIME_USEC       (TIME_NSEC * INT64_C(1000))

/**
 * define a microsecond that used nano second
 */
#define TIME_MSEC       (TIME_USEC * INT64_C(1000))

/**
 * define a second that used nano second
 */
#define TIME_SEC        (TIME_MSEC * INT64_C(1000))

/**
 * define a minute that used nano second
 *
 * @def TIME_MIN
 */
#define TIME_MIN        (TIME_SEC * INT64_C(60))

/**
 * define a hour that used nano second
 *
 * @def TIME_HOUR
 */
#define TIME_HOUR       (TIME_MIN * INT64_C(60))

/**
 * define a day that used nano second
 *
 * @def TIME_DAY
 */
#define TIME_DAY        (TIME_HOUR * INT64_C(24))

/**
 * define a week that used nano second
 *
 * @def TIME_WEEK
 */
#define TIME_WEEK       (TIME_DAY * INT64_C(7))



/**
 * this macro shall convert the time represented by timeval structure to the value count by nano second.
 *
 * @param[in]    tv     the timeval structure.
 *
 * @return     the time value count by nano second.
 */
#define TIME_FROM_TIMEVAL(tv)   (TIME_SEC * (tv)->tv_sec + TIME_NSEC * (tv)->tv_usec)

/**
 * This macro shall convert the time represented by nano second to the timeval structure
 *
 * @param[out]    tv   the timeval structure.
 *
 * @param[in]     t    the time value count by nano second.
 */
#define int64_tO_TIMEVAL(tv, t)  \
    do{ \
        int64_t  li_t = (t); \
        (tv)->tv_sec = li_t / TIME_SEC; \
        (tv)->tv_usec = (li_t % TIME_SEC) / TIME_USEC; \
    }while(0)


/**
 * This macro shall convert the time represented by timespec structure to the value count by nano second.
 * 
 * @param[in]    ts    the timespec structure
 *
 * @return      the time value count by nano second.
 */
#define TIME_FROM_TIMESPEC(ts)  (TIME_SEC * (ts)->tv_sec + TIME_NSEC * (ts)->tv_nsec)

/**
 * This macro shall convert the time represented by nano second to the timespec structure
 *
 * @param[out]    ts     the timespec structure.
 *
 * @param[in]     t      the time valude count by nano second.
 */
#define int64_tO_TIMESPEC(ts, t)  \
    do{ \
        int64_t  li_t = (t); \
        (ts)->tv_sec = li_t / TIME_SEC; \
        (ts)->tv_nsec = (li_t % TIME_SEC) / TIME_NSEC; \
    }while(0)


class CTimeValue
{
public:

    static const CTimeValue ZERO;
    static const CTimeValue MAXTIME;

public:

    CTimeValue();

    CTimeValue( double adTimeValue );

    CTimeValue( int64_t ai64TimeValue, int64_t ai64Unit );

    void Set( int64_t ai64TimeValue, int64_t ai64Unit );

    void Nsec( int64_t ai64Nsec );

    void Usec( int64_t ai64Usec );

    void Msec( int64_t ai64Msec );

    void Sec( int64_t ai64Sec );

    void Min( int64_t ai64Min );

    void Hour( int64_t ai64Hour );

    void Day( int64_t ai64Day );

    void Week( int64_t ai64Week );

    int64_t Nsec(void) const;

    int64_t Usec(void) const;

    int64_t Msec(void) const;

    int64_t Sec(void) const;

    int64_t Min(void) const;

    int64_t Hour(void) const;

    int64_t Day(void) const;

    int64_t Week(void) const;

    CTimeValue& operator +=( CTimeValue const& aoRhs );


    CTimeValue& operator +=(int64_t ai64Rhs );


    CTimeValue& operator =( CTimeValue const& aoRhs );


    CTimeValue& operator =(int64_t ai64Rhs );


    CTimeValue& operator -=( CTimeValue const& aoRhs );


    CTimeValue& operator -=( int64_t ai64Rhs );


    CTimeValue operator++(int);


    CTimeValue& operator++(void);


    CTimeValue operator--(int);

    CTimeValue& operator--(void);


    friend CTimeValue operator +( CTimeValue const& aoLhs, CTimeValue const& aoRhs);


    friend CTimeValue operator -( CTimeValue const& aoLhs, CTimeValue const& aoRhs);


    friend bool operator <( CTimeValue const& aoLhs, CTimeValue const& aoRhs);


    friend bool operator >( CTimeValue const& aoLhs, CTimeValue const& aoRhs);


    friend bool operator >=( CTimeValue const& aoLhs, CTimeValue const& aoRhs);


    friend bool operator <=( CTimeValue const& aoLhs, CTimeValue const& aoRhs);


    friend bool operator ==( CTimeValue const& aoLhs, CTimeValue const& aoRhs);


    friend bool operator !=( CTimeValue const& aoLhs, CTimeValue const& aoRhs);

private:
    int64_t mi64NanoSec;

};

//////////////////////////////////// inline implement /////////////////////////////////////////

inline CTimeValue operator +( CTimeValue const& aoLhs, CTimeValue const& aoRhs )
{
    CTimeValue loTmp(aoLhs);
    loTmp += aoRhs;
    return loTmp;
}

inline CTimeValue operator -( CTimeValue const& aoLhs, CTimeValue const& aoRhs)
{
    CTimeValue loTmp(aoLhs);
    loTmp -= aoRhs;
    return loTmp;
}

inline bool operator <( CTimeValue const& aoLhs, CTimeValue const& aoRhs)
{
    return aoLhs.Nsec() < aoRhs.Nsec();
}

inline bool operator >( CTimeValue const& aoLhs, CTimeValue const& aoRhs)
{
    return aoLhs.Nsec() > aoRhs.Nsec();
}

inline bool operator >=( CTimeValue const& aoLhs, CTimeValue const& aoRhs)
{
    return aoLhs.Nsec() >= aoRhs.Nsec();
}

inline bool operator <=( CTimeValue const& aoLhs, CTimeValue const& aoRhs)
{
    return aoLhs.Nsec() <= aoRhs.Nsec();
}

inline bool operator ==( CTimeValue const& aoLhs, CTimeValue const& aoRhs)
{
    return aoLhs.Nsec() == aoRhs.Nsec();
}

inline bool operator !=( CTimeValue const& aoLhs, CTimeValue const& aoRhs)
{
    return aoLhs.Nsec() != aoRhs.Nsec();
}
MINA_NAMESPACE_END

#endif
