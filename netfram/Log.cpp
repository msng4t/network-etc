#include "Log.h"
#include "Timer.h"
#include "Singleton.h"

#define MAX_LOG_SIZE 1024

MINA_NAMESPACE_START

CLog::CLog(void)
    : miLogLvl (0),
    miFile (-1),
    mpoTimer (NULL),
    mbOutTimer (false)
{

}

int
CLog::Format(char* apcBuffer,
        size_t auliSize,
        int aiLevel,
        const char* apcFormat,
        va_list vl)
{
    size_t liLen = 0;
    size_t liSize = 0;
    char* lpcBuffer = NULL;
    CTimer* lpoTimer = NULL;

    lpoTimer = this->mpoTimer;
    if (lpoTimer == NULL)
    {
        lpoTimer = new CTimer();
        lpoTimer->Init();
        this->mbOutTimer = false;
        lpoTimer->Update();
        this->mpoTimer = lpoTimer;
    }

    if (!this->mbOutTimer)
    {
        lpoTimer->Update();
    }

    liSize = auliSize;
    lpcBuffer = apcBuffer;
    liLen = lpoTimer->GetFormatLen();
    TSmartLock<CLock> loGuard(this->moLock);
    memcpy(lpcBuffer, (const void*)lpoTimer->GetFormatTime(), liLen);

    if (liLen == 0)
    {
        return -1;
    }
    liLen -= 1;
    lpcBuffer += liLen;
    liSize -= liLen;

    liLen = snprintf(lpcBuffer, liSize, "#%d-%lu ", (int)getpid(), (unsigned long int)pthread_self());

    if (liLen <= 0)
    {
        return -1;
    }
    lpcBuffer += liLen;
    liSize -= liLen;

    switch(aiLevel)
    {
        case LOG_LVL_SYSTEM:
        {
            strncpy(lpcBuffer, "SYSTEM: ", liSize);
            break;
        }
        case LOG_LVL_ERROR: 
        {
            strncpy(lpcBuffer, "ERROR: ", liSize);
            break;
        }
        case LOG_LVL_DEBUG: 
        {
            strncpy(lpcBuffer, "DEBUG: ", liSize);
            break;
        }
        case LOG_LVL_WARN: 
        {
            strncpy(lpcBuffer, "WARN: ", liSize);
            break;
        }
        case LOG_LVL_TRACE: 
        {
            strncpy(lpcBuffer, "TRACE: ", liSize);
            break;
        }
        case LOG_LVL_USER: 
        {
            strncpy(lpcBuffer, "USER: ", liSize);
            break;
        }
        case LOG_LVL_ANY: 
        {
            strncpy(lpcBuffer, "ANY: ", liSize);
            break;
        }
        default:
            return -1;
    }

    liLen = strlen(lpcBuffer);
    lpcBuffer += liLen;
    liSize -= liLen;

    liLen = vsnprintf(lpcBuffer, liSize, apcFormat, vl);

    lpcBuffer += liLen;
    liSize -= liLen;

    if ( *(lpcBuffer -1) != '\n')
    {
        *lpcBuffer++ = '\n';
        liSize--;
    }

    return auliSize - liSize;
}

int CLog::Logout(int aiLevel, const char* apcFormat, ...)
{
    size_t liCmplSize = 0;
    static char lac_log_buffer[MAX_LOG_SIZE];
    size_t liLen = 0;

    if (aiLevel < LOG_LVL_SYSTEM)
    {
        aiLevel = LOG_LVL_SYSTEM;
    }
    else if (aiLevel > LOG_LVL_USER)
    {
        aiLevel = LOG_LVL_USER;
    }

    if (this->miLogLvl < aiLevel)
    {
        return 0;
    }
    memset(lac_log_buffer, 0, MAX_LOG_SIZE);
    snprintf(lac_log_buffer, MAX_LOG_SIZE, "%s%d", moLogName.c_str(), getpid());
//    close(miFile);
//    miFile = -1;
    if (this->miFile == -1)
    {
        if (moLogName.size() == 0)
        {
            return -1;
        }

        miFile = open(lac_log_buffer, O_WRONLY | O_APPEND | O_CREAT, S_IRWXG | S_IRWXU);
        if (miFile == -1)
        {
            return -1;
        }

    }

    va_list vl;
    va_start(vl, apcFormat);
    liLen = this->Format(lac_log_buffer, MAX_LOG_SIZE, aiLevel, apcFormat, vl);
    va_end(vl);

#ifdef DEBUG
    {
        TSmartLock<CLock> loGuard(this->moLock);
        this->moBlock.Write(lac_log_buffer, liLen);

        if (this->moBlock.GetSize() < MAX_LOG_SIZE)
        {
            liCmplSize = this->Write(this->moBlock.GetRead(), this->moBlock.GetSize());
            this->moBlock.Reset();
        }
    }
#else
    liCmplSize = this->Write(lac_log_buffer, liLen);
#endif
    assert(liCmplSize > 0);

    return 0;

}

ssize_t CLog::Write(const char* mpcContent, size_t aiSize)
{
    size_t liCmplSize = 0;
    size_t liResult = 0;
    char* lpcContent = (char*)mpcContent;

    lockf(miFile, F_TLOCK, 01);

    for ( ; liCmplSize < aiSize; lpcContent += liResult)
    {
        MINA_RETRY(
                (liResult = write(miFile, lpcContent, aiSize - liCmplSize) )< 0, 5);
        liCmplSize += liResult;
        if ( liResult < 0)
        {
            lockf(miFile, F_ULOCK, 01);
            return -1;
        }
    }
    lockf(miFile, F_ULOCK, 01);
    return liCmplSize;
}

void CLog::Init(int aiLevel, const char* apcLogName, size_t aiSize, CTimer* apoTimer)
{
    this->mpoTimer = apoTimer;
    this->miLogLvl  = aiLevel;
    this->moLogName = apcLogName;
    this->moBlock.Init(aiSize);

    miFile = open(this->moLogName.c_str(), O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
    assert(miFile > 0);
}

void CLog::Flush(void)
{
    if (this->moBlock.GetSize() > 0)
    {
        this->Write(this->moBlock.GetRead(), this->moBlock.GetSize());
    }
}

void CLog::Close(void)
{
    this->Flush();
    this->moBlock.Release();
}
void CLog::SetTimer(CTimer *apoTimer)
{
    this->mpoTimer = apoTimer;
    this->mbOutTimer = true;
}

MINA_NAMESPACE_END
