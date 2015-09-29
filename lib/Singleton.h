#ifndef _SINGLETON_INCLUDE_
#define _SINGLETON_INCLUDE_
#include <stdlib.h>
#include <assert.h>
#include "Define.h"
MINA_NAMESPACE_START

template <typename T>
class TSingleton
{
public:
    static T* Instance(void)
    {
        if (mpoInstance == NULL)
        {
            mpoInstance = new T();
            assert(mpoInstance != NULL);
        }
        return mpoInstance;
    }

    static void Release(void)
    {
        delete mpoInstance;
    }
private:
    static T* mpoInstance;
};

template<typename T> T* TSingleton<T>::mpoInstance = NULL;
template<typename T> inline T* Instance(void)
{
    return TSingleton<T>::Instance();
}

template<typename T> inline void Release(void)
{
    TSingleton<T>::Release();
}

MINA_NAMESPACE_END
#endif
