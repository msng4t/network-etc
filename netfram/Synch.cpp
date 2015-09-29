#include <errno.h>
#include <assert.h>
#include <string.h>
#include "Synch.h"

MINA_NAMESPACE_START

CLock::CLock(void)
{
    int liErr = pthread_mutex_init(&this->moMutex, NULL);
    assert(liErr == 0);
}
CLock::~CLock(void)
{
    int liErr = pthread_mutex_destroy(&this->moMutex);
    assert(liErr == 0);
}

int CLock::Lock()
{
    int liErr;
    liErr = pthread_mutex_lock(&this->moMutex);
    assert(liErr == 0);
    return MINA_OK;
}

int CLock::Trylock(void)
{
    int liErr = pthread_mutex_trylock(&this->moMutex);
    if (liErr == EBUSY)
    {
        return MINA_AGAIN;
    }

    assert(liErr == 0);
    return MINA_OK;
}
int CLock::Unlock(void)
{
    int liErr;
    liErr = pthread_mutex_unlock(&this->moMutex);
    assert(liErr == 0);
    return MINA_OK;
}

CLock::operator pthread_mutex_t* (void)
{
    return &this->moMutex;
}

CRWLock::CRWLock(void)
{
    assert(pthread_rwlock_init(&this->moRWLock, NULL) == 0);
}

CRWLock::~CRWLock(void)
{
    assert(pthread_rwlock_destroy(&this->moRWLock) == 0);
}

int CRWLock::RLock(void)
{
    assert(pthread_rwlock_rdlock(&this->moRWLock) == 0);
    return MINA_OK;
}

int CRWLock::WLock(void)
{
    assert(pthread_rwlock_wrlock(&this->moRWLock) == 0);
    return MINA_OK;
}

int CRWLock::TryRlock(void)
{
    int liErr = pthread_rwlock_tryrdlock(&this->moRWLock);
    if (liErr == EBUSY)
    {
        return MINA_AGAIN;
    }
    assert(liErr == 0);
    return MINA_OK;
}

int CRWLock::TryWlock(void)
{
    int liErr = pthread_rwlock_trywrlock(&this->moRWLock);
    if (liErr == EBUSY)
    {
        return MINA_AGAIN;
    }
    assert(liErr == 0);
    return MINA_OK;
}

int CRWLock::Unlock(void)
{
    assert(pthread_rwlock_unlock(&this->moRWLock) == 0);
    return 0;
}

CRWLock::operator pthread_rwlock_t* (void)
{
    return &this->moRWLock;
}

CSpinLock::CSpinLock(void)
{
    int liErr = pthread_spin_init(&this->moSpin, 0);
    assert(liErr == 0);
}

CSpinLock::~CSpinLock(void)
{
    int liErr = pthread_spin_destroy(&this->moSpin);
    assert (liErr == 0);
}

int CSpinLock::Lock(void)
{
    int liErr = pthread_spin_lock(&this->moSpin);
    assert(liErr == 0);
    return MINA_OK;
}

int CSpinLock::Trylock(void)
{
    int liErr = pthread_spin_trylock(&this->moSpin);
    if (liErr == EBUSY)
    {
        return MINA_AGAIN;
    }
    assert(liErr == 0);
    return MINA_OK;
}

int CSpinLock::Unlock(void)
{
    int liErr = pthread_spin_unlock(&this->moSpin);
    assert(liErr == 0);
    return MINA_OK;
}



CCondition::CCondition(void)
{
    int liErr = pthread_cond_init(&this->moCond, NULL);
    assert(liErr == 0);
}
CCondition::~CCondition(void)
{
    int liErr = pthread_cond_destroy(&this->moCond);
    assert(liErr == 0);
}

int CCondition::Wait( CLock& aoLock, int64_t const& aiTimeout)
{
    int liErr;
    if (aiTimeout < 0 || aiTimeout == INT64_MAX)
    {
        liErr = pthread_cond_wait(&this->moCond, (pthread_mutex_t *)aoLock);
        assert(liErr == 0);
    }
    else
    {
        struct timespec ls_ts;
        int64_t liNSecs;
        liErr = clock_gettime(CLOCK_REALTIME, &ls_ts);
        assert (liErr == 0);
        liNSecs = ls_ts.tv_sec * MINA_TIME_SEC + ls_ts.tv_nsec * MINA_TIME_NSEC + aiTimeout;
        ls_ts.tv_sec = liNSecs / MINA_TIME_SEC;
        ls_ts.tv_nsec = liNSecs / MINA_TIME_NSEC;
        liErr = pthread_cond_timedwait(&this->moCond, (pthread_mutex_t *)aoLock, &ls_ts);
        assert(liErr == 0);
    }

    return MINA_OK;
}

int CCondition::Signal(void)
{
    int liErr = pthread_cond_signal(&this->moCond);
    assert(liErr == 0);
    return MINA_OK;
}
int CCondition::Broadcast(void)
{
    int liErr = pthread_cond_broadcast(&this->moCond);
    assert(liErr == 0);
    return MINA_OK;

}

MINA_NAMESPACE_END
