
#include "TimeValue.h"

MINA_NAMESPACE_START

CTimeValue const CTimeValue::ZERO(0, TIME_NSEC);
CTimeValue const CTimeValue::MAXTIME(INT64_MAX, TIME_NSEC);

CTimeValue::CTimeValue()
    : mi64NanoSec(0)
{
}

CTimeValue::CTimeValue( int64_t ai64TimeValue, int64_t ai64Unit )
    : mi64NanoSec(ai64TimeValue * ai64Unit)
{
}

CTimeValue::CTimeValue( double adTimeValue )
    : mi64NanoSec(int64_t(adTimeValue * TIME_SEC) )
{
}

void CTimeValue::Set(int64_t ai64TimeValue, int64_t ai64Unit)
{
    this->mi64NanoSec = ai64TimeValue * ai64Unit;
}

void CTimeValue::Nsec( int64_t ai64Nsec )
{
    this->Set( ai64Nsec, TIME_NSEC );
}

void CTimeValue::Usec( int64_t ai64Usec )
{
    this->Set( ai64Usec, TIME_USEC );
}

void CTimeValue::Msec( int64_t ai64Msec )
{
    this->Set( ai64Msec, TIME_MSEC );
}

void CTimeValue::Sec( int64_t ai64Sec )
{
    this->Set( ai64Sec, TIME_SEC );
}

void CTimeValue::Min( int64_t ai64Min )
{
    this->Set( ai64Min, TIME_MIN );
}

void CTimeValue::Hour( int64_t ai64Hour )
{
    this->Set( ai64Hour, TIME_HOUR );
}

void CTimeValue::Day( int64_t ai64Day )
{
    this->Set( ai64Day, TIME_DAY );
}

void CTimeValue::Week( int64_t ai64Week )
{
    this->Set( ai64Week, TIME_WEEK );
}

int64_t CTimeValue::Nsec(void) const
{
    return this->mi64NanoSec;
}

int64_t CTimeValue::Usec(void) const
{
    return this->mi64NanoSec / TIME_USEC;
}

int64_t CTimeValue::Msec(void) const
{
    return this->mi64NanoSec / TIME_MSEC;
}

int64_t CTimeValue::Sec(void) const
{
    return this->mi64NanoSec / TIME_SEC;
}

int64_t CTimeValue::Min(void) const
{
    return this->mi64NanoSec / TIME_MIN;
}

int64_t CTimeValue::Hour(void) const
{
    return this->mi64NanoSec / TIME_HOUR;
}

int64_t CTimeValue::Day(void) const
{
    return this->mi64NanoSec / TIME_DAY;
}

int64_t CTimeValue::Week(void) const
{
    return this->mi64NanoSec / TIME_WEEK;
}

CTimeValue& CTimeValue::operator +=( CTimeValue const& aoRhs )
{
    this->mi64NanoSec += aoRhs.mi64NanoSec;
    return *this;
}

CTimeValue& CTimeValue::operator +=(int64_t ai64Rhs )
{
    this->mi64NanoSec += ai64Rhs * TIME_SEC;
    return *this;
}

CTimeValue& CTimeValue::operator =( CTimeValue const& aoRhs )
{
    this->mi64NanoSec = aoRhs.mi64NanoSec;
    return *this;
}

CTimeValue& CTimeValue::operator =(int64_t ai64Rhs )
{
    this->mi64NanoSec = ai64Rhs * TIME_SEC;
    return *this;
}

CTimeValue& CTimeValue::operator -=( CTimeValue const& aoRhs )
{
    this->mi64NanoSec -= aoRhs.mi64NanoSec;
    return *this;
}

CTimeValue& CTimeValue::operator -=( int64_t ai64Rhs )
{
    this->mi64NanoSec -= ai64Rhs * TIME_SEC;
    return *this;
}

CTimeValue CTimeValue::operator++(int)
{
    CTimeValue loTmp(*this);
    
    (*this) += 1;
    
    return loTmp;
}

CTimeValue& CTimeValue::operator++(void)
{
    (*this) += 1;
    return *this;
}

CTimeValue CTimeValue::operator--(int)
{
    CTimeValue loTmp(*this);
    
    (*this) -= 1;
    
    return loTmp;
}

CTimeValue& CTimeValue::operator--(void)
{
    (*this) -= 1;
    return *this;
}

MINA_NAMESPACE_END
