#ifndef _CMINA_GROUP_MANAGER_H_INCLUDE_
#define _CMINA_GROUP_MANAGER_H_INCLUDE_
#include <sys/socket.h>
#include <sys/types.h>
#include "Define.h"
#include "Log.h"
#include "Singleton.h"
#include <vector>
#include <set>
#include <map>
#include "SocketOpt.h"
#include <inttypes.h>
#include "SockAddr.h"
#include "Session.h"
#include "IoService.h"

MINA_NAMESPACE_START
typedef std::set<uint64_t> GroupSetType;
typedef std::map<std::string, GroupSetType > GroupMapType;
typedef std::pair<std::string, GroupSetType > GroupPair;
typedef std::map<uint64_t, CSession*> IDSessionMapType;
typedef std::vector<uint64_t> SessionVecType;
typedef std::map<uint64_t, uint64_t> IDIDMapType;
typedef std::pair<uint64_t, uint64_t> IDIDPairType;
typedef std::map<std::string, int> ExpectType;

class CNextSession;
class CSessionManager
{
public:
    CSessionManager(void);
    bool Insert(CSession*& apoSession);
    bool Find(uint64_t au64SessionID);
    bool Find(CSession*& apoSession, uint64_t au64SessionID);
    bool Find(CSession*& apoSession, char const* apcIP, uint16_t au16Port, int aiFamily = AF_INET);
    template <typename Type>
    bool FindIf(CSession*& apoSession, Type const& aoPred);
    template <typename Type>
    bool FindIf(CSession*& apoSession, char* const apcGroup, Type const& aoPred);
    template <typename Type>
    bool FindIf(CSession*& apoSession, std::string& aoGroup, Type const& aoPred);
    template <typename Type>
    bool FindIf(SessionVecType& aoSessionVec, Type const& aoPredFunc);
    template <typename Type>
    bool FindIf(SessionVecType& aoSessionVec, std::string& aoGroup, Type const& aoPredFunc);
    template <typename Type>
    bool FindIf(SessionVecType& aoSessionVec, char* const apcGroup, Type const& aoPredFunc);
    template <typename Type>
    bool FindIf(Type const& aoPred, CSession*& apoSession);
    template <typename Type>
    bool FindIf(Type const& aoPredFunc, SessionVecType& aoSessionVec);

    bool Erase(uint64_t au64SessionID);
    bool Erase(CSession*& apoSession);
    size_t GetSize(void);
    size_t GetSize(CIoService* apoService);
    size_t GetSize(std::string aoGroup);
    void Clear(void);
    void Clear(CIoService* apoService);
    void Clear(std::string& aoGroup);
    void EraseClosed(GroupMapType::iterator& aoIter);
    void EraseClosed(void);
    void EraseClosed(CIoService* apoService);
    void EraseClosed(std::string& aoGroup);
    void Expect(std::string aoGroup);
    bool CheckOne(std::string aoGroup);
private:
    IDIDMapType moIdenties;
    IDSessionMapType moSessionIDs;

    ExpectType moExpecting;
    GroupMapType moGroups;
    CLock moLock;
    uint8_t muUNameCount;
};

template <>
bool CSessionManager::FindIf<CNextSession>(CNextSession const&, CSession*&);

class CNextSession
{
public:
    bool operator() (size_t auSize,
            IDSessionMapType::iterator aoFirst,
            IDSessionMapType::iterator aoLast,
            CSession*& apoSession) const
    {
        IDSessionMapType::iterator loFirst;
        IDSessionMapType::iterator loTemp;
        loFirst = aoFirst;

        if (loFirst == aoLast)
        {
            apoSession = NULL;
            return false;
        }
        this->muCount = this->muCount % auSize;
        for (size_t i = 0; i < this->muCount; i++)
        {
            if (loFirst == aoLast)
            {
                loFirst = aoFirst;
            }
            loFirst++;
        }
        this->muCount++;
        loTemp = loFirst;
        while (loFirst == aoLast || loFirst->second->Closed() )
        {
            if (loFirst == aoLast)
            {
                loFirst = aoFirst;
            }
            else
            {
                loFirst++;
                this->muCount++;
            }

            if (loFirst == loTemp)
            {
                apoSession = NULL;
                return false;
            }
        }
        apoSession = loFirst->second;
        return true;
    }
private:
    static size_t muCount;
};

MINA_NAMESPACE_END
#endif
