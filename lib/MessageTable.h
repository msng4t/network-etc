#ifndef _MESSAGE_TABLE_INCLUDE_
#define _MESSAGE_TABLE_INCLUDE_
#include <map>
#include <stdint.h>
#include <string>
#include "Define.h"
#include "Timer.h"
#include "Synch.h"
#include "Singleton.h"

MINA_NAMESPACE_START

template<typename KeyType, typename ValueType>
class TMessageTable
{
public:
    typedef std::map<KeyType, ValueType> TableType;
    typedef std::pair<KeyType, ValueType> TablePair;
    TMessageTable(void);
    TMessageTable(size_t auCapacity);
    void Put(KeyType const& atKey, ValueType const& atValue);
    void Delete(KeyType const& atKey);
    bool Get(KeyType const& atKey, ValueType& atValue);
    bool Find(KeyType const& atKey, ValueType const atValue);
    template<typename RecycleFunc>
    void ClearTimeout(time_t const& aoTimeout, RecycleFunc aoRecycle);
private:
    CLock moLock;
    TableType moTable;
};

template<typename KeyType, typename ValueType>
TMessageTable<KeyType, ValueType>::TMessageTable(size_t auSize)
    :moTable(auSize)
{
}

template<typename KeyType, typename ValueType>
TMessageTable<KeyType, ValueType>::TMessageTable(void)
{
}

template<typename KeyType, typename ValueType>
void TMessageTable<KeyType, ValueType>::Put(KeyType const& atKey, ValueType const& atValue)
{
    TSmartLock<CLock> loSmartLock(this->moLock);
    this->moTable.insert(TablePair(atKey, atValue));
}

template<typename KeyType, typename ValueType>
bool TMessageTable<KeyType, ValueType>::Get(KeyType const& atKey, ValueType& atValue)
{
    TSmartLock<CLock> loSmartLock(this->moLock);
    typename TableType::iterator loIter = this->moTable.find(atKey);
    if (loIter != this->moTable.end())
    {
        atValue = loIter->second;
        return true;
    }
    else
    {
        return false;
    }
}

template<typename KeyType, typename ValueType>
bool TMessageTable<KeyType, ValueType>::Find(KeyType const& atKey, ValueType const atValue)
{
    TSmartLock<CLock> loSmartLock(this->moLock);
    typename TableType::iterator loIter = this->moTable.find(atKey);
    if (loIter != this->moTable.end())
    {
        atValue = loIter->second;
        return true;
    }
    else
    {
        return false;
    }

}
template<typename KeyType, typename ValueType>
template<typename RecycleFunc>
void TMessageTable<KeyType, ValueType>::ClearTimeout(time_t const& auTimeout, RecycleFunc aoRecycle)
{
    TSmartLock<CLock> loSmartLock(this->moLock);
    time_t luCurSec = Instance<CTimer>()->GetSec();

    for (typename TableType::iterator loIter = this->moTable.begin();
            loIter != this->moTable.end();
            loIter++)
    {
        if (luCurSec - loIter->second.Sec() > auTimeout)
        {
            aoRecycle(loIter->second);
            this->moTable.erase(loIter++);
            continue;
        }

    }
}

template<typename KeyType, typename ValueType>
void TMessageTable<KeyType, ValueType>::Delete(KeyType const& atKey)
{
    this->moTable.erase(atKey);
}

class CMessageNode
{
public:
    CMessageNode(void);
    CMessageNode(uint64_t au64InSessionID,
            uint64_t au64OutSessionID,
            uint64_t au64InMessageID,
            uint64_t au64OutMessageID,
            std::string const& aoRemark);
    uint64_t GetInSessionID(void);
    uint64_t GetOutSessionID(void);
    uint64_t GetInMessageID(void);
    uint64_t GetOutMessageID(void);
    std::string& GetRemark(void);
    time_t& GetTimeSec(void);
private:
    uint64_t mu64InSessionID;
    uint64_t mu64OutSessionID;
    uint64_t mu64InMessageID;
    uint64_t mu64OutMessageID;
    std::string moRemark;
    time_t muTimeSec;
};

typedef TMessageTable<uint64_t, CMessageNode> MessageTableType;
MINA_NAMESPACE_END

#endif
