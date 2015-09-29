#ifndef _CONFIGURATION_INCLUDE_
#define _CONFIGURATION_INCLUDE_
#include <inttypes.h>
#include <string>
#include "TimeValue.h"
#include "Define.h"

MINA_NAMESPACE_START

class CConfiguration
{
public:
    CConfiguration(void);

    void SetMaxListener( size_t auCount );

    size_t GetMaxListener(void) const;

    void SetMaxSession( size_t auCount );

    size_t GetMaxSession(void) const;

    size_t GetReadBufferSize(void) const;

    uint32_t GetMaxDelay( void );

    void SetMaxDelay( uint32_t auMaxDelay );

    void SetReadBufferSize( size_t auBufferSize );

    size_t GetWriteBufferSize(void) const;

    void SetWriteBufferSize( size_t auBufferSize );

    int  GetReadIdle(void) const;

    void SetReadIdle( int const& aoTime );

    int  GetWriteIdle(void) const;

    void SetWriteIdle( int const& aoTime );

    int  GetWriteTimeout(void) const;

    void SetWriteTimeout( int const& aoTime );

    void SetAutoSuspendWrite( bool abIsAutoSuspend );

    bool IsAutoSuspendWrite(void) const;

    void SetDirectWrite( bool abIsDirectWrite);

    bool IsDirectWrite(void) const;

    void SetAutoResumeRead( bool abIsAutoResume );

    void SetSessionPBlock( bool abSameBlock );

    bool GetSessionPBlock( void );

    bool IsAutoResumeRead(void) const;

    void SetMaxHint( size_t auCount );

    size_t GetMaxHint(void) const;

    void SetProcessors( size_t auPoolSize );

    size_t GetProcessors(void) const;

    size_t GetThreadStackSize(void ) const;

    void SetThreadStackSize(size_t auStackSize);

private:
    size_t muMaxListener;

    size_t muMaxSession;

    size_t muReadBufferSize;

    size_t muWriteBufferSize;

    int miReadIdle;

    int miWritIdle;

    int miWriteTimeout;

    bool mbAutoSuspendWrite;

    bool mbAutoResumeRead;

    bool mbDirectWrite;

    bool mbSessionPBlock;

    size_t muMaxHint;

    size_t muProcessors;

    size_t muStackSize;

    uint32_t muMaxDelay;
};

class CServiceConf
{
public:
    CServiceConf(void);

    size_t GetReadBufferSize(void) const;

    void SetReadBufferSize( size_t auBufferSize );

    size_t GetWriteBufferSize(void) const;

    void SetRetry(uint32_t aiTimes);

    uint32_t GetRetry( void );

    void SetRetryInterval(uint32_t aiInterval);

    uint32_t GetRetryInterval( void );

    void SetWriteBufferSize( size_t auBufferSize );

    int  GetReadIdle(void) const;

    void SetReadIdle( int const& aoTime );

    int  GetWriteIdle(void) const;

    void SetWriteIdle( int const& aoTime );

    int  GetWriteTimeout(void) const;

    void SetWriteTimeout( int const& aoTime );

    void SetAutoSuspendWrite( bool abIsAutoSuspend );

    bool IsAutoSuspendWrite(void) const;

    void SetAutoResumeRead( bool abIsAutoResume );

    bool IsAutoResumeRead(void) const;

    void SetDirectWrite( bool abIsDirectWrite);

    bool IsDirectWrite(void) const;

    void SetSessionPBlock( bool abSameBlock );

    bool GetSessionPBlock( void );

    void operator=(CConfiguration& aoConfiguration);

    void Set(CConfiguration& aoConfiguration);
private:
    size_t muReadBufferSize;

    size_t muWriteBufferSize;

    int miReadIdle;

    int miWritIdle;

    int miWriteTimeout;

    bool mbAutoSuspendWrite;
    bool mbAutoSuspendSet;

    bool mbAutoResumeRead;
    bool mbAutoResumeSet;

    bool mbDirectWrite;
    bool mbDirectSet;

    bool mbSessionPBlock;
    bool mbSessionPBlockSet;

    uint32_t            muRetry;

    uint32_t            muRetryInterval;

    CConfiguration* mpoGlobal;
};

MINA_NAMESPACE_END
#endif
