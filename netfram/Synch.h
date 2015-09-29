#ifndef _mina_THREAD_H_INCLUDE_
#define _mina_THREAD_H_INCLUDE_

#include <pthread.h>
#include <inttypes.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "Define.h"

MINA_NAMESPACE_START

class CLock
{
    friend class CCondition;

public:
    CLock(void);
    ~CLock(void);

    int Lock();
    int Trylock(void);
    int Unlock(void);

protected:
    operator pthread_mutex_t* (void);

private:
    CLock( CLock const& );
    CLock& operator = ( CLock const& );

private:
    pthread_mutex_t moMutex;
};

class CRWLock
{
    friend class CCondition;

public:
    CRWLock(void);
    ~CRWLock(void);

    int RLock(void);
    int WLock(void);
    int TryRlock(void);
    int TryWlock(void);
    int Unlock(void);

protected:
    operator pthread_rwlock_t* (void);

private:
    CRWLock( CRWLock const& );
    CRWLock& operator = ( CRWLock const& );

private:
    pthread_rwlock_t moRWLock;
};

class CSpinLock
{
public:
    CSpinLock(void);
    ~CSpinLock(void);
    int Lock(void);
    int Trylock(void);
    int Unlock(void);

private:
    CSpinLock( CSpinLock const& );
    CSpinLock& operator = ( CSpinLock const& );

private:
    pthread_spinlock_t moSpin;
};

class CFileLock
{
public:
    CFileLock( const char* apcName );
    ~CFileLock(void);

private:
    CFileLock( CFileLock const& );
    CFileLock operator = ( CFileLock const& );
    int Lock(void);
    int Trylock(void);
    int Unlock(void);

private:
    int miHandle;
};


template<typename LockType> class TSmartLock
{
public:
    TSmartLock( LockType& aoLock )
        : moLock(aoLock)
        , mbIsOwner(false)
    {
        this->Lock();
    }
    int Lock(void)
    {
        if (this->moLock.Lock() == 0)
        {
            this->mbIsOwner = true;
            return 0;
        }
        else
        {
            this->mbIsOwner = false;
            return -1;
        }
    }
    int TryLock(void)
    {
        if (this->moLock.TryLock() == 0)
        {
            this->mbIsOwner = true;
            return 0;
        }
        else
        {
            this->mbIsOwner = false;
            return -1;
        }
    }
    
    int Unlock(void)
    {
        if (this->mbIsOwner)
        {
            this->mbIsOwner = false;
            return this->moLock.Unlock();
        }
        else
        {
            return -1;
        }
    }
    
    ~TSmartLock(void)
    {
        this->Unlock();
    }

private:
    LockType& moLock;

    bool mbIsOwner;
};

class CCondition
{
public:
    CCondition(void);
    ~CCondition(void);
    /* NSEC */
    int Wait( CLock& aoLock, int64_t const& aiTimeout = INT64_MAX );
    int Signal(void);
    int Broadcast(void);

private:
    CCondition( CCondition const& );
    CCondition operator = ( CCondition const& );

private:
    pthread_cond_t moCond;
};

class CSemaphore
{
public:
    CSemaphore( int aiInitValue = 0 );
    ~CSemaphore(void);
    int Wait( int64_t const& aiTimeout = INT64_MAX);
    int Post(void);
    int GetValue(void);

private:
    CSemaphore( CSemaphore const& );
    CSemaphore operator = ( CSemaphore const& );

private:
    sem_t moSema;
};

MINA_NAMESPACE_END
#endif
