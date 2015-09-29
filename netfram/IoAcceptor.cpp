#include  "IoAcceptor.h"
#include "Log.h"
#include "Singleton.h"
#include "Cycle.h"

MINA_NAMESPACE_START

CIoAcceptor::CIoAcceptor(CCycle *apoCycle)
    : CIoService(apoCycle)
{
}
CIoAcceptor::~CIoAcceptor(void)
{

}

int CIoAcceptor::Bind(const char* apcGroup, CSockAddr& aoAddr)
{
    CIoEvent *lpoREvent = NULL;
    CSession *lpoSession = NULL;

    this->moListen.Init(aoAddr);
    if (this->moListen.Open() != MINA_OK)
    {
        return MINA_ERROR;
    }

    lpoSession = this->GetCycle()->GetSession(this->moListen.GetFd());

    if (lpoSession == NULL)
    {
        return MINA_ERROR;
    }

    lpoSession->SetListen(&this->moListen);
    lpoSession->SetService(this);
    this->moListen.SetSession(lpoSession);

    lpoREvent = lpoSession->GetRead();
    lpoREvent->Accept(true);
    this->SetName(apcGroup);

    return MINA_OK;
}

int CIoAcceptor::Bind(const char* apcGroup, const char* apcIP, uint16_t au16Port)
{
    CSockAddr loAddr;
    loAddr.Set(apcIP, au16Port);
    return this->Bind(apcGroup, loAddr);
}

int CIoAcceptor::Bind(std::string aoGroup, CSockAddr& aoAddr)
{
    return this->Bind(aoGroup.c_str(), aoAddr);
}

int CIoAcceptor::Bind(std::string aoGroup, const char* apcIP, uint16_t au16Port)
{
    CSockAddr loAddr;
    loAddr.Set(apcIP, au16Port);
    return this->Bind(aoGroup.c_str(), loAddr);
}
int CIoAcceptor::Bind(CSockAddr& aoAddr)
{
    return this->Bind(aoAddr.GetIP(), aoAddr);
}

int CIoAcceptor::Bind(const char* apcIP, uint16_t au16Port)
{
    CSockAddr loAddr;
    loAddr.Set(apcIP, au16Port);
    return this->Bind(apcIP, loAddr);
}

int CIoAcceptor::BindUDP(std::string& aoGroup, CSockAddr& aoAddr)
{
    return this->BindUDP(aoGroup.c_str(), aoAddr);
}

int CIoAcceptor::BindUDP(CSockAddr& aoAddr)
{
    return this->BindUDP(aoAddr.GetIP(), aoAddr);
}

int CIoAcceptor::BindUDP(const char* apcIP, uint16_t au16Port)
{
    CSockAddr loAddr;
    loAddr.Set(apcIP, au16Port);
    return this->BindUDP(apcIP, loAddr);
}

int CIoAcceptor::BindUDP(std::string& aoGroup, const char* apcIP, uint16_t au16Port)
{
    CSockAddr loAddr;
    loAddr.Set(apcIP, au16Port);
    return this->BindUDP(aoGroup.c_str(), loAddr);

}

int CIoAcceptor::BindUDP(const char* apcGroup, const char* apcIP, uint16_t au16Port)
{
    CSockAddr loAddr;
    loAddr.Set(apcIP, au16Port);
    return this->BindUDP(apcGroup, loAddr);
}

int CIoAcceptor::BindUDP(const char* apcGroup, CSockAddr& aoAddr)
{
    CSockAddr loAddr;

    if (this->moDgram.Open(aoAddr) != 0)
    {
        LOG_ERROR("UDP bind failed.");
        return MINA_ERROR;
    }

    this->SetName(apcGroup);
    return this->InitUDP(moDgram);
}

int CIoAcceptor::Join(const char* apcGroup, CSockAddr& aoAddr)
{
    CSockAddr loAddr;

    if (this->moDgram.Join(aoAddr) != 0)
    {
        LOG_ERROR("UDP join multicast group failed.");
        return MINA_ERROR;
    }

    this->SetName(apcGroup);
    return this->InitUDP(moDgram);
}

int CIoAcceptor::Join(std::string& aoGroup, CSockAddr& aoAddr)
{
    return Join(aoGroup.c_str(), aoAddr);
}

int CIoAcceptor::Join(std::string& aoGroup, const char* apcIP, uint16_t au16Port)
{
    CSockAddr loAddr;
    loAddr.Set(apcIP, au16Port);
    return this->Join(aoGroup.c_str(), loAddr);
}

int CIoAcceptor::Join(const char* apcGroup, const char* apcIP, uint16_t au16Port)
{
    CSockAddr loAddr;
    loAddr.Set(apcIP, au16Port);
    return this->Join(apcGroup, loAddr);

}

int CIoAcceptor::Join(CSockAddr& aoGroup)
{
    return this->Join(aoGroup.GetIP(), aoGroup);
}

int CIoAcceptor::Join(const char* apcIP, uint16_t au16Port)
{
    CSockAddr loAddr;
    loAddr.Set(apcIP, au16Port);
    return this->Join(apcIP, loAddr);
}


CListen* CIoAcceptor::GetListen(void)
{
    return &this->moListen;
}

MINA_NAMESPACE_END
