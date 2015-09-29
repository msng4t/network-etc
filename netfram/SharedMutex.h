#ifndef _CMINA_SHMTX_H_INCLUDE_
#define _CMINA_SHMTX_H_INCLUDE_
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "Define.h"


class CSharedMutex_SH
{
public:
    volatile uint64_t muLock;
    volatile uint64_t muWait;
};

class CSharedMutex
{
public:
    CSharedMutex(void);
    int Init(CSharedMutex_SH *mpcAddr, char *mpcName);
    void Destroy(void);
    uint32_t Trylock(void);
    void Lock(void);
    void Unlock(void);
    uint32_t ForceUnlock(pid_t muPid);
    void SetSpin(uint32_t auSpin);
private:
    void Wakeup(void);
    volatile uint64_t *mpuLock;
    volatile uint64_t *mpuWait;
    uint32_t muSemaphore;
    sem_t moSem;
    char *name;
    uint32_t muSpin;
};

inline uint64_t MinaAtomicCmpSet(volatile uint64_t *au64Lock, uint64_t au64Old, uint64_t au64Set);
inline uint64_t MinaAtomicFetchAdd(volatile uint64_t *au64Lock, uint64_t au64Add);

#define ATOMIC_TRYLOCK(au64Lock)  (*(au64Lock) == 0 && MinaAtomicCmpSet(au64Lock, 0, 1))
#define ATOMIC_UNLOCK(au64Lock)   (*(au64Lock) = 0)


inline uint64_t 
MinaAtomicCmpSet(volatile uint64_t *au64Lock, uint64_t au64Old, uint64_t au64Set)
{
    if (*au64Lock == au64Old)
    {
        *au64Lock = au64Set;
        return 1;
    }

    return 0;
}

inline uint64_t
MinaAtomicFetchAdd(volatile uint64_t *au64Lock, uint64_t au64Add)
{
    uint32_t au64Old;

    au64Old = *au64Lock;
    *au64Lock += au64Add;
    
    return au64Old;
}


#endif
