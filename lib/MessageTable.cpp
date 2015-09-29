#include "MessageTable.h"

MINA_NAMESPACE_START


CMessageNode::CMessageNode(void)
    : mu64InSessionID(0),
      mu64OutSessionID(0),
      mu64InMessageID(0),
      mu64OutMessageID(0)
{
    this->muTimeSec = Instance<CTimer>()->GetSec();
}
CMessageNode::CMessageNode(uint64_t au64InSessionID,
        uint64_t au64OutSessionID,
        uint64_t au64InMessageID,
        uint64_t au64OutMessageID,
        std::string const& aoRemark)
    : mu64InSessionID(au64InSessionID),
      mu64OutSessionID(au64OutSessionID),
      mu64InMessageID(au64OutMessageID),
      mu64OutMessageID(au64OutSessionID),
      moRemark(aoRemark)
{
    this->muTimeSec = Instance<CTimer>()->GetSec();
}

uint64_t CMessageNode::CMessageNode::GetInSessionID(void)
{
    return this->mu64InSessionID;
}

uint64_t CMessageNode::CMessageNode::GetOutSessionID(void)
{
    return this->mu64OutSessionID;
}

uint64_t CMessageNode::CMessageNode::GetInMessageID(void)
{
    return this->mu64InMessageID;
}

uint64_t CMessageNode::CMessageNode::GetOutMessageID(void)
{
    return this->mu64OutMessageID;
}

std::string& CMessageNode::CMessageNode::GetRemark(void)
{
    return this->moRemark;
}

time_t& CMessageNode::GetTimeSec(void)
{
    return this->muTimeSec;
}


MINA_NAMESPACE_END
