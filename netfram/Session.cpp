#include "IoEvent.h"
#include "Session.h"
#include "Define.h"
#include "Queue.h"
#include "SocketOpt.h"
#include "Log.h"
#include "Singleton.h"
#include "Timer.h"
#include "Cycle.h"

MINA_NAMESPACE_START


CSession::CSession(void)
  : mpoData(NULL)
    , mpoRead (NULL)
    , mpoWrite (NULL)
    , miFd (-1)
    , mpoListen (NULL)
    , muSent (0)
    , mpoLocal (NULL)
    , muSessionID (0)
    , mbDestoyed (false)
    , mbIdle (false)
    , mbClosed (false)
    , mbReadActive (false)
    , mbUDP (false)
    , muLock (0)
{
}

void CSession::Close(void)
{
    if (!this->mbClosed)
    {
        this->mbClosed = true;
        this->mbReadActive = false;
    }
}

void CSession::Reset(void)
{
    mpoData = NULL;
    mpoRead = NULL;
    mpoWrite = NULL;
    miFd = 0;
    mpoListen = NULL;
    muSent = 0;
    mpoLocal = NULL;
    muSessionID = 0;
    mbDestoyed = 0;
    mbIdle = 0;
    mbClosed = 0;
    mbReadActive = 0;
    muLock = 0;

}

CSockAddr& CListen::GetSockAddr(void)
{
    return this->moLocal;
}

int CListen::GetType(void)
{
    return this->miType;
}

int CListen::GetBacklog(void)
{
    return this->miBacklog;
}

CSession *CListen::GetSession(void)
{
    return this->mpoSession;
}
void CListen::SetSession(CSession *apoSession)
{
    this->mpoSession = apoSession;
}

void CListen::Init(CSockAddr& aoAddr, int aiBacklog, uint64_t auTimeout)
{
    this->moLocal.Set(aoAddr);
    this->miFd = -1;
    this->miBacklog = aiBacklog;
    this->muAccTimeout = auTimeout;
    this->miType = SOCK_STREAM;
}

void CListen::Init(const char* apcIp, uint16_t auPort, int aiFamily, int aiBacklog, uint64_t auTimeout)
{
    CSockAddr loSock;
    loSock.Set(apcIp, auPort, aiFamily);
    this->Init(loSock, aiBacklog, auTimeout);

}
int CListen::GetFd(void)
{
    return this->miFd;
}

int
CListen::Open(void)
{
    int liAddrReuse;
    uint32_t liTimes;
    uint32_t liFails;
    int liErrno;
    int liSock;

    for (liTimes = 5; liTimes; liTimes--)
    {
        liFails = 0;
        liSock = socket(this->moLocal.GetFamily(), this->miType, 0);

        if (liSock == -1)
        {
            LOG_ERROR("Open listening socket fail(%s)", strerror(errno));
            return MINA_ERROR;
        }

        liAddrReuse = 1;
        if (setsockopt(liSock, SOL_SOCKET, SO_REUSEADDR, (char *)&liAddrReuse, sizeof(int))
                == -1)
        {
            LOG_ERROR("Set reuse socketaddr fail(%s)", strerror(errno));

            close(liSock);
            return MINA_ERROR;
        }

        if ( MINA_NONBLOCKING(liSock) == -1)
        {
            LOG_ERROR("nonblocking(%d) liFails", liSock);
            close(liSock);
            return MINA_ERROR;
        }

        if (bind(liSock, this->moLocal.GetAddr(), this->moLocal.GetLength()) == -1)
        {
            liErrno = errno;

            LOG_ERROR("Listening socket bind (%s:%d) fail(%s)",
                    this->moLocal.GetIP(), this->moLocal.GetPort(), strerror(liErrno));

            close(liSock);
            liFails =1;
            if (liErrno == EADDRINUSE )
            {
                continue;
            }
        }

        if (listen(liSock, this->miBacklog) == -1)
        {
            LOG_ERROR("Listening socket listen (%s:%d) fail(%s)",
                    this->moLocal.GetIP(), this->moLocal.GetPort(), strerror(liErrno));
            close(liSock);
            return MINA_ERROR;
        }
        this->miFd = liSock;

        LOG_ERROR("Listening to fd:%d", liSock);
        if (!liFails)
        {
            break;
        }

        LOG_ERROR("try again to bind() after 500ms");

        usleep(500 * 1000);
    }

    if (liFails)
    {
        LOG_ERROR("Listening socket bind to (%s:%d)",
                this->moLocal.GetIP(), this->moLocal.GetPort());
        return MINA_ERROR;
    }

    return MINA_OK;
}

CIoHandler* CSession::GetHandler(void)
{
    return this->mpoHandler;
}

void CSession::SetHandler(CIoHandler* apoHandler)
{
    this->mpoHandler = apoHandler;
}

CIoEvent*
CSession::GetRead(void)
{
    return this->mpoRead;
}

CIoEvent* CSession::GetWrite(void)
{
    return this->mpoWrite;
}

void* CSession::GetData(void)
{
    return this->mpoData;
}

void CSession::SetData(void* apvData)
{
    this->mpoData = apvData;
}

volatile uint64_t& CSession::GetLock(void)volatile
{
    return this->muLock;
}

void CSession::SetRead(CIoEvent *apoEvent)
{
    this->mpoRead = apoEvent;
}

void CSession::SetWrite(CIoEvent *apoEvent)
{
    this->mpoWrite= apoEvent;
}

int CSession::GetFd()
{
    return this->miFd;
}

void CSession::SetFileDesc(int aiFd)
{
    this->miFd = aiFd;
}

CSockAddr *CSession::GetDest(void)
{
    return &this->moDest;
}

CSockAddr *CSession::GetRemote(void)
{
    return &this->moRemote;
}

CSockAddr *CSession::GetLocal(void)
{
    return this->mpoLocal;
}

void CSession::SetDest(CSockAddr& aoAddr)
{
    this->moDest.Set(aoAddr);
}

void CSession::SetRemote(CSockAddr& aoAddr)
{
    this->moRemote.Set(aoAddr);
}

void CSession::SetLocal(CSockAddr *apoAddr)
{
    this->mpoLocal = apoAddr;
}

CListen* CSession::GetListen()
{
    return this->mpoListen;
}

void CSession::SetListen(CListen *apoListen)
{
    this->mpoListen = apoListen;
}

void CSession::SetSessionID(uint32_t auSessionID)
{
    this->muSessionID = auSessionID;
}

uint64_t CSession::GetSessionID(void)
{
    return this->muSessionID;
}

void CSession::SetLock(uint32_t apuLock)
{
    this->muLock = 0;
}

void CSession::SetFilterChain(CFilterChain* apoFilterChain)
{
    this->mpoFilterChain = apoFilterChain;
}

CFilterChain* CSession::GetFilterChain(void)
{
    return this->mpoFilterChain;
}

void CSession::Write(CAny& aoMessage)
{
    this->mpoFilterChain->Response(this, aoMessage);
}

void CSession::SetCycle( CCycle* apoCycle)
{
    this->mpoCycle = apoCycle;
}

CCycle* CSession::GetCycle( void )
{
    return this->mpoCycle;
}

void CSession::SetService( CIoService* apoService )
{
    this->mpoService = apoService;
}

CIoService* CSession::GetService( void )
{
    return this->mpoService;
}


MINA_NAMESPACE_END

