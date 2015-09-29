#ifndef _MINA_CYCLE_H_INCLUDE_
#define _MINA_CYCLE_H_INCLUDE_

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
#include "Configuration.h"
#include "Poll.h"
#include "EventDo.h"
#define ACCEPTORSIZE 6

MINA_NAMESPACE_START
class CPoll;
class CIoConnector;
class CIoAcceptor;
class CCycle
{
public:
    CCycle(void);
    ~CCycle();
    int Init(void);
    int InitEvent(void);
    uint32_t GetSessionN(void);
    uint32_t GetFreeSessionN(void);
    CSession* GetSession(int sock);
    CSession* GetSessions(void);
    CSession* GetFreeSessions(void);
    CIoEvent* GetReads(void);
    CIoEvent* GetWrites(void);
    void FreeSession(CSession *apoSession);
    void CloseSession(CSession *apoSession);
    CConfiguration* GetConfiguration(void);
    CTimer* GetTimer(void);

    CEventDo* GetEventDo(void);
    CEventPosted* GetPostDo(void);
    void SetAcceptor(CIoAcceptor *apoAcceptor);
    CIoAcceptor* GetAcceptor( int aiNum );
    MessageTableType* GetMessageTable(void);
    void SetMessageTable(MessageTableType* apoTable);
    CSessionManager* GetSessionManager(void);
    void SetSessionManager(CSessionManager* apoManager);
    void SetConnector(CIoConnector* apoConnector);
    void ConnectAll(void);
    void Wakeup(void);
private:
    CSession *mpoFreeSessions;

    uint32_t muFreeCount;
    uint32_t muMaxFd;


    CConfiguration moConfiguration;
    CSession *mpoSessions;
    CIoEvent *mpoREventPtrs;
    CIoEvent *mpoWEventPtrs;
    CTimer moTimer;
    CEventDo moEventDo;
    CEventPosted moPostDo;

    CSockPair moPair;
    int miCurAcceptors;
    int miCurConnectors;
    int miCurMaxAcceptors;
    int miCurMaxConnectors;
    CIoAcceptor *mapoAcceptors[ACCEPTORSIZE];
    CIoConnector* mapoConnectors[ACCEPTORSIZE];
    CIoAcceptor **mppoAcceptors;
    CIoConnector **mppoConnectors;


    MessageTableType *mpoMessageTable;
    CSessionManager* mpoSessionManager;
};

CCycle* GetCycle(void);

MINA_NAMESPACE_END

#endif
