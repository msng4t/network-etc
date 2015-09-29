#ifndef _CFILTER_H_INCLUDE_
#define _CFILTER_H_INCLUDE_
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "Define.h"
#include "Queue.h"
#include "Any.h"
#include "IoHandler.h"


#define NNEXT_FILTER(apoFilter)   ((CFilter*)((unsigned char*)apoFilter->moQueue.next - offsetof(CFilter, moQueue)))

MINA_NAMESPACE_START
class CNextFilter;
class CSession;
class CFilter
{
public:
    virtual ~CFilter(void){}
    virtual void SessionOpen(SessionType aoSession, CNextFilter *apoFilter);
    virtual void SessionClose(SessionType aoSession, CNextFilter *apoFilter);
    virtual void SessionIdle(SessionType aoSession, uint32_t auStatus, CNextFilter *apoFilter);
    virtual void MessageRecived(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter);
    virtual void MessageFinished(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter);
    virtual void FilterClose(SessionType aoSession, CNextFilter *apoFilter);
    virtual void FilterWrite(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter);
};
struct CNextFilter: public CQueueNode
{
    char macName[32];
    CFilter *mpoFilter;
    CNextFilter(const char *apcName, CFilter* apoFilter);
    void SessionOpen(SessionType aoSession);
    void SessionClose(SessionType aoSession);
    void SessionIdle(SessionType aoSession, uint32_t auStatus);
    void MessageRecived(SessionType aoSession, CAny& aoMessage);
    void MessageFinished(SessionType aoSession, CAny& aoMessage);
    void FilterClose(SessionType aoSession);
    void FilterWrite(SessionType aoSession, CAny& aoMessage);

};
MINA_NAMESPACE_END
#endif
