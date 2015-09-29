#ifndef _IO_EPOLL_INCLUDE_
#define _IO_EPOLL_INCLUDE_
#include <sys/epoll.h>
#include "Session.h"
#include "Define.h"
#include "IoEvent.h"
#include "Log.h"
#include "Singleton.h"
#include "Timer.h"
//#include "EventDo.h"


MINA_NAMESPACE_START

class CPoll
{
public:
    CPoll(void);
    int Add(CIoEvent *apoEvent, int aiEvent, uint32_t auFlags);
    int Del(CIoEvent *apoEvent, int aiEvent, uint32_t auFlags);

    int Enable(CIoEvent *apoEvent, int aiEvent, uint32_t auFlags);
    int Disable(CIoEvent *apoEvent, int aiEvent, uint32_t auFlags);

    int Add(CSession *apoSession);
    int Del(CSession *apoSession, uint32_t auFlags);

    int ProcessChanges(void);
    int Poll(CTimer* apoTimer, uint32_t auFlags);

    int Init(size_t auMaxEvent, size_t auHint, uint32_t auMaxDelay);
    void Done(void);
private:
    int miEpfd;
    struct epoll_event *mpoEventList;
    size_t muNevents;
    uint32_t muMaxDelay;
};

MINA_NAMESPACE_END
#endif
