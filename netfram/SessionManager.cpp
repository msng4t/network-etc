#include "SessionManager.h"
#include "IoConnector.h"

MINA_NAMESPACE_START

size_t CNextSession::muCount = 0;

CSessionManager::CSessionManager(void)
    : muUNameCount (0)
{

}

bool CSessionManager::Insert( CSession*& apoSession )
{
    uint64_t liSessionID;
    std::string& loGroupName = apoSession->GetService()->GetName();

    liSessionID = apoSession->GetSessionID();
    TSmartLock<CLock> loSmartLock(this->moLock);
    if ( loGroupName.length() == 0)
    {
        char lacName[10] = {0};
        snprintf(lacName, sizeof(lacName), "Default%d", this->muUNameCount++);
        this->moGroups[lacName].insert(liSessionID);
    }
    else
    {
        this->moGroups[loGroupName].insert(liSessionID);
    }
    this->moIdenties[apoSession->GetRemote()->GetID()] = liSessionID;
    this->moSessionIDs[liSessionID] = apoSession;
    return true;
}

bool CSessionManager::Find(uint64_t au64SessionID)
{
    IDSessionMapType::iterator loIter;
    loIter = this->moSessionIDs.find(au64SessionID);
    if (loIter != this->moSessionIDs.end())
    {
        return true;
    }
    return false;
}

bool CSessionManager::Find(CSession*& apoSession, uint64_t au64SessionID)
{
    IDSessionMapType::iterator loIter;
    loIter = this->moSessionIDs.find(au64SessionID);
    if (loIter != this->moSessionIDs.end())
    {
        apoSession = loIter->second;
        return true;
    }
    apoSession = NULL;
    return false;
}


bool CSessionManager::Erase(uint64_t au64SessionID)
{
    CSession* lpoSession = NULL;
    IDSessionMapType::iterator loIter;
    GroupMapType::iterator loGroupIter;
    GroupSetType::iterator loSetIter;

    TSmartLock<CLock> loSmartLock(this->moLock);
    loIter = this->moSessionIDs.find(au64SessionID);
    if (loIter == this->moSessionIDs.end())
    {
        return false;
    }
    lpoSession = loIter->second;
    this->moSessionIDs.erase(loIter);
    this->moIdenties.erase(lpoSession->GetRemote()->GetID());

    loGroupIter = this->moGroups.find(lpoSession->GetService()->GetName());
    if (loGroupIter != this->moGroups.end())
    {
        loGroupIter->second.erase(lpoSession->GetSessionID());
    }
    return true;

}

bool CSessionManager::Erase(CSession*& apoSession)
{
    return this->Erase(apoSession->GetSessionID());
}

size_t CSessionManager::GetSize(void)
{
    return this->moIdenties.size();
}

size_t CSessionManager::GetSize(std::string aoGroup)
{
    GroupMapType::iterator loIter = this->moGroups.find(aoGroup);
    if (loIter != this->moGroups.end())
    {
        return loIter->second.size();
    }
    return 0;
}


void CSessionManager::Clear(void)
{
    this->moGroups.clear();
    this->moIdenties.clear();
    this->moSessionIDs.clear();
}

void CSessionManager::Clear(CIoService* apoService)
{
    this->Clear(apoService->GetName());
}

void CSessionManager::Clear(std::string& aoGroup)
{
    GroupMapType::iterator loIter;
    GroupSetType::iterator loSetIter;
    CSession* lpoSession;

    loIter = this->moGroups.find(aoGroup);
    TSmartLock<CLock> loSmartLock(this->moLock);
    if (loIter != this->moGroups.end())
    {
        if (loIter->second.size() == 0 )
            return;
        for (loSetIter = loIter->second.begin(); loSetIter != loIter->second.end();)
        {
            lpoSession = this->moSessionIDs[*loSetIter];
            this->moIdenties.erase(lpoSession->GetRemote()->GetID());
            this->moSessionIDs.erase(*loSetIter);
        }
    }
    this->moGroups.erase(loIter);
}

void CSessionManager::EraseClosed(void)
{
    for (GroupMapType::iterator loIter = this->moGroups.begin();
            loIter != this->moGroups.end();
            loIter++)
    {
        this->EraseClosed(loIter);
    }
}

void CSessionManager::EraseClosed(std::string& aoGroup)
{
    GroupMapType::iterator loIter = this->moGroups.find(aoGroup);
    if (loIter != this->moGroups.end())
    {
        this->EraseClosed(loIter);
    }
}
void CSessionManager::EraseClosed(GroupMapType::iterator& aoIter)
{
    GroupSetType::iterator loIter;
    CSession* lpoSession;
    if (aoIter->second.size() == 0 )
        return;
    TSmartLock<CLock> loSmartLock(this->moLock);
    for (loIter = aoIter->second.begin();
            loIter != aoIter->second.end();)
    {
        lpoSession = this->moSessionIDs[*loIter];
        if (lpoSession->Closed())
        {
            this->moIdenties.erase(lpoSession->GetRemote()->GetID());
            this->moSessionIDs.erase(*loIter);
            aoIter->second.erase(loIter);
        }
        else
        {
            loIter++;
        }
    }
}


bool CSessionManager::Find(CSession*& apoSession, char const* apcIP, uint16_t au16Port, int aiFamily)
{
    IDIDMapType::iterator loIter;
    CSockAddr loSockAddr;

    loSockAddr.Set(apcIP, au16Port, aiFamily);
    loIter = this->moIdenties.find(loSockAddr.GetID());
    if (loIter != this->moIdenties.end())
    {
        apoSession = this->moSessionIDs[loIter->second];
        return true;
    }
    else
    {
        return false;
    }
}

template <typename Type>
bool CSessionManager::FindIf(Type const& aoPred, CSession*& apoSession)
{
    return aoPred(this->moSessionIDs.begin(), this->moSessionIDs.end(), apoSession);

}

template <>
bool CSessionManager::FindIf<CNextSession>(CNextSession const& aoPred, CSession*& apoSession)
{
    if (this->moSessionIDs.size() == 0)
    {
        apoSession = NULL;
        return false;
    }
    return aoPred(this->moSessionIDs.size(),
            this->moSessionIDs.begin(),
            this->moSessionIDs.end(),
            apoSession);

}

template <typename Type>
bool CSessionManager::FindIf(Type const& aoPredFunc, SessionVecType& aoSessionVec)
{
    bool lbRet = false;
    for (IDSessionMapType::iterator loIter = this->moSessionIDs.begin();
            loIter != this->moSessionIDs.end();
            loIter++)
    {
        if (aoPredFunc(&loIter->second))
        {
            lbRet = true;
            aoSessionVec.push_back(loIter->second);
        }
    }
    return lbRet;
}

void CSessionManager::Expect(std::string aoGroup)
{
    ExpectType::iterator loIter = this->moExpecting.find(aoGroup);
    if (loIter != this->moExpecting.end())
    {
        loIter->second++;
    }
    else
    {
        this->moExpecting[aoGroup] = 1;
    }
}

bool CSessionManager::CheckOne(std::string aoGroup)
{
    ExpectType::iterator loIter = this->moExpecting.find(aoGroup);
    if (loIter != this->moExpecting.end())
    {
        loIter->second--;
        if (loIter->second > 1)
        {
            return true;
        }
        else if (loIter->second == 0)
        {
            this->moExpecting.erase(loIter);
            return true;
        }
    }
    return false;

}
MINA_NAMESPACE_END
