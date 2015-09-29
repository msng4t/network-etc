#ifndef _MINA_EVENTPOSTED_H_INCLUDE_
#define _MINA_EVENTPOSTED_H_INCLUDE_

#include "Define.h"
#include "Session.h"
#include "Queue.h"
#include "Poll.h"
#include "Configuration.h"
#include "Timer.h"
#include "Thread.h"
#include "SockPair.h"
#include "MessageTable.h"
#include "SessionManager.h"
#include "IoAcceptor.h"
#include "Configuration.h"
#include "Poll.h"
#include "EventDo.h"

MINA_NAMESPACE_START
class CCycle;
class CEventPosted
{
public:
    CEventPosted();
    void ProcessAccept( void );
    int ProcessPosted(void);
    CLock& GetLock(void);
    void SetCycle( CCycle* apoCycle );
    bool HasPosted( void );
    void Post(CIoEvent *apoEvent);
    void PostAccept(CIoEvent *apoEvent);
    void LockedDeletePosted(CIoEvent* apoEvent);
    void PostEvent(CIoEvent *apoEvent, uint32_t auFlag);
private:
    CLock moLock;
    CQueueNode moHead;
    CQueueNode moAccept;
    CCycle   *mpoCycle;
};

MINA_NAMESPACE_END
#endif
