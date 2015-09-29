#ifndef _CFILTER_CHAIN_H_INCLUDE_
#define _CFILTER_CHAIN_H_INCLUDE_
#include "Define.h"
#include "Filter.h"
#include "IoHandler.h"


MINA_NAMESPACE_START
class CFilterChain
{
public:
    CQueueNode moHead;
    CFilterChain(void);
    void AddFirst(const char *apcName, CFilter *apoFilter);
    void AddLast(const char *apcName, CFilter *apoFilter);

    void FireSessionOpen(SessionType aoSession);
    void FireSessionClose(SessionType aoSession);
    void FireSessionIdle(SessionType aoSession, uint32_t auStatus);

    void FireMessageRecived(SessionType aoSession, CAny& aoMessage);
    void FireMessageFinished(SessionType aoSession, CAny& aoMessage);
    void Response(SessionType aoSession, CAny& aoMessage);

    void FireFilterWrite(SessionType aoSession, CAny& aoMessage);
    void FireFilterClose(SessionType aoSession);

    int Init(void);
    ~CFilterChain(void);
};

class CFilterHead: public CFilter
{
    virtual void SessionOpen(SessionType aoSession, CNextFilter *apoFilter);
    virtual void SessionClose(SessionType aoSession, CNextFilter *apoFilter);
    virtual void SessionIdle(SessionType aoSession, uint32_t auStatus, CNextFilter *apoFilter);
    virtual void MessageRecived(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter);
    virtual void MessageFinished(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter);
    virtual void FilterClose(SessionType aoSession, CNextFilter *apoFilter);
    virtual void FilterWrite(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter);
};

class CFilterTail: public CFilter
{
    virtual void SessionOpen(SessionType aoSession, CNextFilter *apoFilter);
    virtual void SessionClose(SessionType aoSession, CNextFilter *apoFilter);
    virtual void SessionIdle(SessionType aoSession, uint32_t auStatus, CNextFilter *apoFilter);
    virtual void MessageRecived(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter);
    virtual void MessageFinished(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter);
    virtual void FilterClose(SessionType aoSession, CNextFilter *apoFilter);
    virtual void FilterWrite(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter);

};


MINA_NAMESPACE_END
#endif

