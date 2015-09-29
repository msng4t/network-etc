#ifndef _CMINA_LOG_H_INCLUDE_
#define _CMINA_LOG_H_INCLUDE_
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <string>
#include "Define.h"
#include "Memblock.h"
#include "Timer.h"
#include "Synch.h"
#include "Singleton.h"


MINA_NAMESPACE_START

#define LOG_SYSTEM(...)    Instance<CLog>()->Logout(LOG_LVL_SYSTEM, __VA_ARGS__)
#define LOG_ERROR(...)     Instance<CLog>()->Logout(LOG_LVL_ERROR, __VA_ARGS__)
#define LOG_WARN(...)      Instance<CLog>()->Logout(LOG_LVL_WARN, __VA_ARGS__)
#define LOG_DEBUG(...)     Instance<CLog>()->Logout(LOG_LVL_DEBUG, __VA_ARGS__)
#define LOG_TRACE(...)     Instance<CLog>()->Logout(LOG_LVL_TRACE, __VA_ARGS__)
#define LOG_TRACES()        Instance<CLog>()->Logout(LOG_LVL_TRACE, "%s:%d%s", __FILE__, __LINE__, __func__)
#define LOG_ANY(...)       Instance<CLog>()->Logout(LOG_LVL_DEBUG, __VA_ARGS__)
#define LOG_USER(...)      Instance<CLog>()->Logout(LOG_LVL_USER, __VA_ARGS__)
#define LOG_FLUSH()        Instance<CLog>()->Flush();
/*
#define LOG_SYSTEM(...) printf( __VA_ARGS__)
#define LOG_ERROR(...) printf( __VA_ARGS__)
#define LOG_WARN(...) printf( __VA_ARGS__)
#define LOG_DEBUG(...) printf( __VA_ARGS__)
#define LOG_TRACE(...) printf( __VA_ARGS__)
#define LOG_ANY(...) printf( __VA_ARGS__)
#define LOG_USER(...) printf( __VA_ARGS__)
*/



enum ELLOGTYPE
{
    LOG_LVL_SYSTEM  = 0,
    LOG_LVL_ERROR   = 1,
    LOG_LVL_WARN    = 2,
    LOG_LVL_DEBUG   = 3,
    LOG_LVL_TRACE   = 4,
    LOG_LVL_ANY     = 5,
    LOG_LVL_USER    =6
};

class CLog
{
private:
    int miLogLvl;
    int miFile;
    std::string moLogName;
    CLock moLock;
    CMemblock moBlock;
    CTimer* mpoTimer;
    bool mbOutTimer;
public:
    CLog(void);
    void SetTimer(CTimer *apoTimer);
    int Format(char* apcBuffer, size_t auSize, int aiLevel, const char* apcformat, va_list vl);
    int Logout(int aiLevel, const char* apcFormat, ...);
    void Init(int aiLevel, const char* apcLogName, size_t aiSize, CTimer* apoTimer = NULL);
    ssize_t Write(const char* mpcContent, size_t aiSize);
    void Flush(void);
    void Close(void);

};
MINA_NAMESPACE_END
#endif
