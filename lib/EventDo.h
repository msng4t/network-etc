#ifndef _MINA_EVENT_H_INCLUDE_
#define _MINA_EVENT_H_INCLUDE_

#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/epoll.h>
#include "Define.h"
#include "IoAcceptor.h"
#include "IoEvent.h"
#include "Synch.h"
#include "Poll.h"
#include "Session.h"
#include "Singleton.h"
#include "Log.h"
#include "Timer.h"
#include "SocketOpt.h"
#include "OsAction.h"
#include "FilterChain.h"
#include "SessionManager.h"
#include "Thread.h"
#include "EventPosted.h"

#define MINA_INVALID_INDEX 0xd0d0d0d0
#define connectionss 65535
#define MINA_UPDATE_TIME           1
#define MINA_POST_EVENTS           2
#define MINA_POST_THREAD_EVENTS    4
MINA_NAMESPACE_START

class CCycle;
class CIoAcceptor;
class CEventDo
{
public:
    void Accept(CIoEvent *apoEvent);
    int Init( CCycle* apoCycle );
    bool LockPost(void);
    int TrylockAccept(void);
    int ForceUnlockAccept(pid_t auPid);
    bool UnlockPost(void);
    void Process(CThreadManager* apoProcessor);
    CEventDo(void);
    uint64_t GetSessionID(void);
    int Read(CIoEvent *apoEvent);
    int Check(CIoEvent *apoEvent);
    int Write(CIoEvent *apoEvent, bool abPost = true);
    CPoll* GetPoll(void);
    int EnableAccept(void);

private:
    int Bind( void );
    int DisableAccept(void);
    void CloseAccepted(CSession *apoSession);
    volatile uint64_t mu64SessionID;
    CPoll moPoll;
    CCycle* mpoCycle;
};





#define MINA_UPDATE_TIME           1
#define MINA_POST_EVENTS           2
#define MINA_POST_THREAD_EVENTS    4




MINA_NAMESPACE_END
#endif
