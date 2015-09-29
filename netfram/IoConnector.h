#ifndef _CMINA_CONNECTOR_H_INCLUDE_
#define _CMINA_CONNECTOR_H_INCLUDE_
#include <sys/socket.h>
#include <sys/types.h>
#include "Define.h"
#include "Log.h"
#include "Singleton.h"
#include <vector>
#include "SocketOpt.h"
#include <inttypes.h>
#include "SockAddr.h"
#include "Session.h"
#include "IoService.h"
#include "SessionManager.h"
#include "Cycle.h"


MINA_NAMESPACE_START

class CIoConnector: public CIoService
{
public:
    CIoConnector(CCycle *apoCycle);
    int Connect(void);
    void Close(void);
    int Connect(const char* apcName, CSockAddr& aoSockAddr, int aiCount);
    int Connect(std::string& aoName, CSockAddr& aoSockAddr, int aiCount);
    int Connect(CSockAddr& aoSockAddr);
    void SetRetry(uint32_t aiRetry);
    void SetRcvBufLen(int aiLen);
    void SetLocal(CSockAddr& aoSockAddr);
    void SetRemote(CSockAddr& aoSockAddr);
    CSockAddr* GetLocal(void);
    CSockAddr* GetRemote(void);
    CSession* GetSession(void);
    bool GetConnected(void);
    void SetSessionNum(int aiNumber);
    void HandleChanged(void);
    int Remove(int aiCount);
    int ConnectUDP(const char* apcGroup, CSockAddr& aoAddr);

private:
    CSockAddr moRemote;
    CSockAddr moLocal;
    CSockDgram moDgram;

    CLock moLock;
    uint32_t            muRetryed;
    int                 miSessionNum;
    int                 miRcvbufLen;
    int64_t             mi64LastTry;
    bool                mbCached;
    bool                mbBind;
    bool                mbConnected;

};

MINA_NAMESPACE_END
#endif
