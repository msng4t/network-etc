#include <assert.h>
#include "Configuration.h"
#include "Singleton.h"

MINA_NAMESPACE_START


CConfiguration::CConfiguration(void)
    : muMaxListener (0)
    , muMaxSession (0)
    , muReadBufferSize (0)
    , muWriteBufferSize (0)
    , miReadIdle (0)
    , miWritIdle (0)
    , miWriteTimeout (0)
    , mbAutoSuspendWrite (false)
    , mbAutoResumeRead (false)
    , mbDirectWrite (true)
    , mbSessionPBlock (false)
    , muMaxHint (5)
    , muProcessors (1)
    , muStackSize (1024)
    , muMaxDelay (600)
{
}

void CConfiguration::SetMaxListener( size_t auCount )
{
    this->muMaxListener= auCount;
}

size_t CConfiguration::GetMaxListener(void) const
{
    return this->muMaxListener;
}

void CConfiguration::SetMaxSession( size_t auCount )
{
    this->muMaxSession = auCount;
}

size_t CConfiguration::GetMaxSession(void) const
{
    return this->muMaxSession;
}

size_t CConfiguration::GetReadBufferSize(void) const
{
    return this->muReadBufferSize;
}

void CConfiguration::SetReadBufferSize( size_t auBufferSize )
{
    this->muReadBufferSize = auBufferSize;
}

size_t CConfiguration::GetWriteBufferSize(void) const
{
    return this->muWriteBufferSize;
}

void CConfiguration::SetWriteBufferSize( size_t auBufferSize )
{
    this->muWriteBufferSize = auBufferSize;
}

int  CConfiguration::GetReadIdle(void) const
{
    return this->miReadIdle;
}

void CConfiguration::SetReadIdle( int const& aoTime )
{
    this->miReadIdle = aoTime;
}

int  CConfiguration::GetWriteIdle(void) const
{
    return this->miWritIdle;
}

void CConfiguration::SetWriteIdle( int const& aoTime )
{
    this->miWritIdle = aoTime;
}

int CConfiguration::GetWriteTimeout(void) const
{
    return this->miWriteTimeout;
}

void CConfiguration::SetWriteTimeout( int const& aoTime )
{
    this->miWriteTimeout = aoTime;
}

//processor configuration
void CConfiguration::SetAutoSuspendWrite( bool abIsAutoSuspend )
{
    this->mbAutoSuspendWrite = abIsAutoSuspend;
}

bool CConfiguration::IsAutoSuspendWrite(void) const
{
    return this->mbAutoSuspendWrite;
}

void CConfiguration::SetDirectWrite( bool abIsDirectWrite)
{
    this->mbDirectWrite = abIsDirectWrite;
}

bool CConfiguration::IsDirectWrite(void) const
{
    return this->mbDirectWrite;
}
void CConfiguration::SetSessionPBlock( bool abSameBlock )
{
    mbSessionPBlock = abSameBlock;
}

bool CConfiguration::GetSessionPBlock( void )
{
    return this->mbSessionPBlock;
}

void CConfiguration::SetAutoResumeRead( bool abIsAutoResume )
{
    this->mbAutoResumeRead = abIsAutoResume;
}

bool CConfiguration::IsAutoResumeRead(void) const
{
    return this->mbAutoResumeRead;
}

void CConfiguration::SetMaxHint( size_t auCount )
{
    this->muMaxHint = auCount;
}

size_t CConfiguration::GetMaxHint(void) const
{
    return this->muMaxHint;
}

void CConfiguration::SetProcessors( size_t auPoolSize )
{
    this->muProcessors = auPoolSize;
}

size_t CConfiguration::GetProcessors(void) const
{
    return this->muProcessors;
}

size_t CConfiguration::GetThreadStackSize(void ) const
{
    return this->muStackSize;
}

void CConfiguration::SetThreadStackSize(size_t auStackSize)
{
    this->muStackSize = auStackSize;
}

uint32_t CConfiguration::GetMaxDelay( void )
{
    return this->muMaxDelay;
}

void CConfiguration::SetMaxDelay( uint32_t auMaxDelay )
{
    this->muMaxDelay = auMaxDelay;
}

CServiceConf::CServiceConf(void)
    : muReadBufferSize (0)
    , muWriteBufferSize (0)
    , miReadIdle (0)
    , miWritIdle (0)
    , miWriteTimeout (0)
    , mbAutoSuspendWrite (false)
    , mbAutoSuspendSet (false)
    , mbAutoResumeRead (false)
    , mbAutoResumeSet (false)
    , mbSessionPBlock (false)
    , mbSessionPBlockSet (false)
    , muRetry (10)
    , muRetryInterval (5)
{
  this->mpoGlobal = Instance<CConfiguration>();
}

size_t CServiceConf::GetReadBufferSize(void) const
{
    if (this->muReadBufferSize != 0)
    {
        return this->muReadBufferSize;
    }
        return this->mpoGlobal->GetReadBufferSize();
}

void CServiceConf::SetReadBufferSize( size_t auBufferSize )
{
    this->muReadBufferSize = auBufferSize;
}

size_t CServiceConf::GetWriteBufferSize(void) const
{
    if (this->muWriteBufferSize != 0)
    {
        return this->muWriteBufferSize;
    }
        return this->mpoGlobal->GetWriteBufferSize();
}

void CServiceConf::SetWriteBufferSize( size_t auBufferSize )
{
    this->muWriteBufferSize = auBufferSize;
}

int CServiceConf::GetReadIdle(void) const
{
    if (this->miReadIdle != 0)
    {
        return this->miReadIdle;
    }
        return this->mpoGlobal->GetReadIdle();
}

void CServiceConf::SetReadIdle( int const& aoTime )
{
    this->miReadIdle = aoTime;
}

int CServiceConf::GetWriteIdle(void) const
{
    if (this->miWritIdle != 0)
    {
        return this->miWritIdle;
    }
        return this->mpoGlobal->GetWriteIdle();
}

void CServiceConf::SetWriteIdle( int const& aoTime )
{
    this->miWritIdle = aoTime;
}

int CServiceConf::GetWriteTimeout(void) const
{
    if (this->miWriteTimeout != 0)
    {
        return this->miWriteTimeout;
    }
        return this->mpoGlobal->GetWriteTimeout();
}

void CServiceConf::SetWriteTimeout( int const& aoTime )
{
    this->miWriteTimeout = aoTime;
}

void CServiceConf::SetAutoSuspendWrite( bool abIsAutoSuspend )
{
    this->mbAutoSuspendSet = true;
    this->mbAutoSuspendWrite = abIsAutoSuspend;
}

bool CServiceConf::IsAutoSuspendWrite(void) const
{
    if (this->mbAutoSuspendSet)
    {
        return this->mbAutoSuspendWrite;
    }
        return this->mpoGlobal->IsAutoSuspendWrite();
}

void CServiceConf::SetDirectWrite( bool abIsDirectWrite)
{
    this->mbDirectSet = true;
    this->mbDirectWrite = abIsDirectWrite;
}

bool CServiceConf::IsDirectWrite(void) const
{
    if (this->mbDirectSet)
    {
        return this->mbDirectWrite;
    }
        return this->mpoGlobal->IsDirectWrite();
}


void CServiceConf::SetAutoResumeRead( bool abIsAutoResume )
{
    this->mbAutoResumeSet = true;
    this->mbAutoResumeRead = abIsAutoResume;
}

bool CServiceConf::IsAutoResumeRead(void) const
{
    if (this->mbAutoResumeSet)
    {
        return this->mbAutoResumeRead;
    }
    return this->mpoGlobal->IsAutoResumeRead();
}

void CServiceConf::operator=(CConfiguration& aoConfiguration)
{
    this->Set(aoConfiguration);
}

void CServiceConf::Set(CConfiguration& aoConfiguration)
{
    this->mpoGlobal =  &aoConfiguration;
}

void CServiceConf::SetRetry(uint32_t auTimes)
{
    this->muRetry = auTimes;
}

uint32_t CServiceConf::GetRetry( void )
{
    return this->muRetry;
}

void CServiceConf::SetRetryInterval(uint32_t auInterval)
{
    this->muRetryInterval = auInterval;
}

uint32_t CServiceConf::GetRetryInterval( void )
{
    return this->muRetryInterval;
}

void CServiceConf::SetSessionPBlock( bool abSameBlock )
{
    mbSessionPBlock = abSameBlock;
    mbSessionPBlockSet = true;
}

bool CServiceConf::GetSessionPBlock( void )
{
    if (mbSessionPBlockSet)
    {
        return this->mbSessionPBlock;
    }
    return mpoGlobal->GetSessionPBlock();
}
MINA_NAMESPACE_END
