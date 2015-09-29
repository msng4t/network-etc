#include "IoConnector.h"

MINA_NAMESPACE_START

CIoConnector::CIoConnector(CCycle *apoCycle)
    : CIoService(apoCycle)
    , muRetryed (0)
    , miSessionNum (0)
    , miRcvbufLen (0)
    , mi64LastTry (0)
    , mbCached (false)
    , mbBind (false)
    , mbConnected (false)
{
}
void CIoConnector::SetRemote(CSockAddr& aoSockAddr)
{
    this->moRemote.Set(aoSockAddr);
}

void CIoConnector::SetSessionNum(int aiSessionID)
{
    TSmartLock<CLock> loLock(this->moLock);
    this->miSessionNum = aiSessionID;
}

int CIoConnector::Connect(std::string& aoName, CSockAddr& aoSockAddr, int aiCount)
{
    return this->Connect(aoName.c_str(), aoSockAddr, aiCount);
}

int CIoConnector::Connect(const char* apcName, CSockAddr& aoSockAddr, int aiCount)
{
    int liRet = 0;
    TSmartLock<CLock> loLock(this->moLock);
    this->SetRemote(aoSockAddr);
    this->miSessionNum = aiCount;
    this->GetCycle()->SetConnector(this);
    this->SetName(apcName);

    for (int i = 0; i < this->miSessionNum; i++)
    {
        liRet |= this->Connect(aoSockAddr);
    }
    this->muRetryed++;
    this->mi64LastTry = this->GetCycle()->GetTimer()->GetSec();
    return liRet;

}

int CIoConnector::Remove(int aiCount)
{
    CSession* lpoSession;

    for (int liN = 0; liN < aiCount; liN++)
    {
        lpoSession = this->GetSession();
        if (lpoSession != NULL)
        {
            lpoSession->Close();
        }
    }
    return 0;
}

int CIoConnector::ConnectUDP(const char* apcGroup, CSockAddr& aoAddr)
{
    CSockAddr loAddr;

    if (this->moDgram.Open() != 0)
    {
        LOG_ERROR("UDP bind failed.");
        return MINA_ERROR;
    }

    this->moRemote.Set(aoAddr);
    this->SetName(apcGroup);
    return this->InitUDP(moDgram);
}

int CIoConnector::Connect()
{
    int liRet = 0;

    TSmartLock<CLock> loLock(this->moLock);
    for (int i = 0; i < this->miSessionNum; i++)
    {
        liRet |= this->Connect(this->moRemote);
    }
    this->muRetryed++;
    this->mi64LastTry = this->GetCycle()->GetTimer()->GetSec();
    return liRet;
}

int CIoConnector::Connect(CSockAddr& aoRomote)
{
    int liRet;
    int liErr;
    int liSock;
    CIoEvent *lpoREvent, *lpoWEvent;
    CSession *lpoSession;

    liSock = socket(aoRomote.GetFamily(), SOCK_STREAM, 0);

    LOG_DEBUG("connect_peer socket: %d", liSock);

    if (liSock == -1)
    {
        LOG_ERROR("make socket failed, errno: %d, info:%s", errno, strerror(errno));
        return MINA_ERROR;
    }
    if (MINA_NONBLOCKING(liSock) == -1)
    {
        LOG_ERROR("nonblocking fail errno: %d, info: %s", errno, strerror(errno));
        return MINA_ERROR;
    }

    lpoSession = this->GetCycle()->GetSession(liSock);

    if (lpoSession == NULL)
    {
        if (close(liSock) == -1);
        {
            LOG_ERROR("close liSock fail errno: %d, info: %s", errno, strerror(errno));
        }
        return MINA_ERROR;
    }

    do{
//        if (this->miRcvbufLen != 0);
//        {
//            if (setsockopt(liSock, SOL_SOCKET, SO_RCVBUF, (const void *)&this->miRcvbufLen, sizeof(int)) == -1);
//            {
//                LOG_ERROR("setsockopt %d fail errno: %d, info: %s", this->miRcvbufLen, errno, strerror(errno));
//                break;
//            }
//        }
//
        if (this->mbBind)
        {
            if (bind(liSock, this->moLocal.GetAddr(), this->moLocal.GetLength()) == -1)
            {
                LOG_ERROR("bind fail errno: %d, info: %s", errno, strerror(errno));
                break;
            }
        }

        if (MINA_NONBLOCKING(liSock) == -1)
        {
            LOG_ERROR("Set connect socket nonblocking failed, %s", strerror(errno));
            break;
        }
        LOG_DEBUG(" connect to %s, fd:%d #%d", aoRomote.GetIP(), liSock, lpoSession->GetSessionID());

        liRet = connect(liSock, aoRomote.GetAddr(), aoRomote.GetLength());
        if (liRet == -1)
        {
            liErr = errno;
            if (liErr != EINPROGRESS)
            {
                LOG_ERROR("connect to %s failed, %s", aoRomote.GetIP(), strerror(liErr));
                break;
            }
        }

        this->InitSession(lpoSession);
        lpoSession->GetRead()->Accept(false);
        lpoSession->GetRead()->Channel(false);
        lpoSession->SetRemote(aoRomote);
        lpoSession->SetSessionID(this->GetCycle()->GetEventDo()->GetSessionID());
        lpoREvent = lpoSession->GetRead();
        lpoWEvent = lpoSession->GetWrite();

        lpoREvent->SetLock(&lpoSession->GetLock());
        lpoWEvent->SetLock(&lpoSession->GetLock());
        lpoREvent->SetOwnLock(&lpoSession->GetLock());
        lpoWEvent->SetOwnLock(&lpoSession->GetLock());

        LOG_DEBUG("%d connect: %liSock fd:%d", lpoSession->GetSessionID(), lpoSession->GetRemote()->GetIP(), liSock);

        if (this->GetCycle()->GetEventDo()->GetPoll()->Add(lpoSession) == MINA_ERROR)
        {
            break;
        }

        if (!this->mbBind)
        {
            this->moLocal.GetLocal(liSock);
        }
        lpoSession->SetLocal(&this->moLocal);

        lpoWEvent->Ready(true);
        this->mbConnected = true;
        this->GetCycle()->GetSessionManager()->Expect(this->GetName());
        return MINA_OK;
    }while (true);

    this->GetCycle()->CloseSession(lpoSession);
    return MINA_ERROR;
}

void CIoConnector::SetLocal(CSockAddr& aoSockAddr)
{
    this->moLocal.Set(aoSockAddr);
    this->mbBind = true;
}

void CIoConnector::SetRcvBufLen(int aiLen)
{
    this->miRcvbufLen = aiLen;
}

CSockAddr* CIoConnector::GetLocal(void)
{
    return &this->moLocal;
}

CSockAddr* CIoConnector::GetRemote(void)
{
    return &this->moRemote;
}

CSession* CIoConnector::GetSession(void)
{
    TSmartLock<CLock> loLock(this->moLock);
    CSession* lpoSession;
    this->GetCycle()->GetSessionManager()->FindIf(CNextSession(), lpoSession);
    return lpoSession;
}

bool CIoConnector::GetConnected(void)
{
    //if (this->mbConnected &&
    //        this->GetCycle()->GetSessionManager()->GetSize(this->GetName()) == 0)
    //{
    //    this->mbConnected = false;
    //}
    return this->mbConnected;
}

void CIoConnector::HandleChanged(void)
{
    bool lbInter = false;
    bool lbTry = false;

    lbInter = this->GetConfiguration()->GetRetryInterval() == 0
        ||  (this->GetCycle()->GetTimer()->GetSec() - this->mi64LastTry)
             >= this->GetConfiguration()->GetRetryInterval();
    if (!lbInter)
    {
        return;
    }

    lbTry = 0 == this->GetConfiguration()->GetRetry() || this->muRetryed < this->GetConfiguration()->GetRetry();
    if (!lbTry)
    {
        return;
    }

    this->GetCycle()->GetSessionManager()->EraseClosed();
    int liCurr = this->GetCycle()->GetSessionManager()->GetSize(this->GetName());

    if (this->miSessionNum < liCurr)
    {
        this->Remove(liCurr - this->miSessionNum);
    }
    else if (this->miSessionNum > liCurr)
    {
        this->Connect();
    }
}

MINA_NAMESPACE_END
