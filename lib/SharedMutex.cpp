#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include "SharedMutex.h"


CSharedMutex::CSharedMutex(void)
    : mpuLock (NULL),
      mpuWait (NULL),
      muSemaphore (0),
      name (NULL),
      muSpin (-1)
{

}
void CSharedMutex::SetSpin(uint32_t auSpin)
{
    this->muSpin = auSpin;
}

int
CSharedMutex::Init(CSharedMutex_SH *mpcAddr, char *mpcName)
{
    int liRet = 0;
    this->mpuLock = &mpcAddr->muLock;

    if (this->muSpin == (uint32_t)-1)
    {
        return MINA_OK;
    }

    this->muSpin = 2048;
    this->mpuWait = &mpcAddr->muWait;

    liRet = sem_init(&this->moSem, 1, 0);
    assert(liRet != 0);
    this->muSemaphore = 1;

    return MINA_OK; 
}


void
CSharedMutex::Destroy(void)
{
    if (this->muSemaphore)
    {
        assert(sem_destroy(&this->moSem) != 0);
    }
}


uint32_t
CSharedMutex::Trylock(void)
{
    return (*this->mpuLock == 0) && (MinaAtomicCmpSet(this->mpuLock, 0, getpid()));
}



void
CSharedMutex::Lock(void)
{
    for (;;)
    {
        if (*this->mpuLock == 0 && MinaAtomicCmpSet(this->mpuLock, 0, getpid()))
        {
            return;
        }

        if (this->muSemaphore)
        {
            MinaAtomicFetchAdd(this->mpuLock, 1);

            if ( *this->mpuLock == 0 && MinaAtomicCmpSet(this->mpuLock, 0, getpid()))
            {
                return;
            }

            while (sem_wait(&this->moSem) == -1)
            {
                if (errno != EINTR)
                    break;
            }

            continue;
        }
        /* just alias the schedule_yeld */
        usleep(1);
    }
}


void
CSharedMutex::Unlock(void)
{
    if ( MinaAtomicCmpSet(this->mpuLock, getpid(), 0))
    {
        this->Wakeup();
    }
}

uint32_t
CSharedMutex::ForceUnlock(pid_t muPid)
{

    if (MinaAtomicCmpSet(this->mpuLock, muPid, 0))
    {
        this->Wakeup();
        return 1;
    }

    return 0;
}



void
CSharedMutex::Wakeup(void)
{
    uint32_t luWait;
    if (!this->muSemaphore)
        return;

    for (;;)
    {
        luWait = *this->mpuWait;
        if (luWait == 0)
            return;

        if (MinaAtomicCmpSet(this->mpuWait, luWait, luWait - 1))
        {
            break;
        }
    }

    assert (sem_post(&this->moSem) == -1);
}
