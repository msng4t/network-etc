#ifndef _MINA_SESSION_H_INCLUDE_
#define _MINA_SESSION_H_INCLUDE_
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "IoEvent.h"
#include "Define.h"
#include "SockAddr.h"
#include "Queue.h"
#include "IoHandler.h"
#include "FilterChain.h"



MINA_NAMESPACE_START

class CFilterChain;
class CCycle;
class CIoService;
enum session_status
{
    MINA_READ_IDLE,
    MINA_WRITE_IDLE
};

class CListen
{
public:
    void Init(const char* apcIp, uint16_t auPort, int aiType = AF_INET, int aiBacklog = 6, uint64_t auTimeout = 60);
    int Open(void);
    void Init(CSockAddr& aoAddr, int aiBacklog = 6, uint64_t auTimeout = 60);
    CSockAddr& GetSockAddr(void);
    int GetType(void);
    int GetBacklog(void);
    CSession *GetSession(void);
    void SetSession(CSession *apoSession);
    int GetFd(void);
private:
    int miFd;
    CSockAddr moLocal;
    uint64_t muAccTimeout;
    int miType;
    int miBacklog;
    CSession *mpoSession;

};

class CSession
{
public:
    CSession(void);
    void Close(void);
    void Reset(void);
    CIoEvent* GetRead(void);
    CIoEvent* GetWrite(void);
    void* GetData(void);
    void SetData(void* apvData);
    volatile uint64_t& GetLock(void) volatile ;
    void SetLock(uint32_t apuLock);
    void SetRead(CIoEvent *apoEvent);
    void SetWrite(CIoEvent *apoEvent);
    int GetFd();
    void SetFileDesc(int aiFd);
    CSockAddr *GetRemote(void);
    void SetRemote(CSockAddr& aoAddr);
    CSockAddr *GetDest(void);
    void SetDest(CSockAddr& aoAddr);
    CSockAddr *GetLocal(void);
    void SetLocal(CSockAddr *apoAddr);
    CListen* GetListen();
    void SetListen(CListen *apoListen);
    void SetSessionID(uint32_t auSessionID);
    uint64_t GetSessionID(void);
    GENGETSET(Destoyed);
    GENGETSET(Idle);
    GENGETSET(Closed);
    GENGETSET(ReadActive);
    GENGETSET(UDP);
    ssize_t Recv(char *buf, size_t size);
    ssize_t Send(char *buf, size_t size);
    CIoHandler* GetHandler(void);
    void SetHandler(CIoHandler* apoHandler);
    void SetFilterChain(CFilterChain* apoFilterChain);
    CFilterChain* GetFilterChain(void);
    void SetCycle( CCycle* apoCycle);
    CCycle* GetCycle( void );
    void SetService( CIoService* apoService );
    CIoService* GetService( void );
    void Write(CAny& aoMessage);
private:
    void *mpoData;
    CIoEvent *mpoRead;
    CIoEvent *mpoWrite;
    int miFd;
    CListen *mpoListen;
    off_t muSent;
    CSockAddr moRemote;
    CSockAddr moDest;
    CSockAddr *mpoLocal;
    volatile uint64_t muSessionID;
    bool mbDestoyed;
    bool mbIdle;
    bool mbClosed;
    bool mbReadActive;
    bool mbUDP;
    volatile uint64_t muLock;
    CIoHandler *mpoHandler;
    CFilterChain *mpoFilterChain;
    CCycle*      mpoCycle;
    CIoService*  mpoService;
};


MINA_NAMESPACE_END

#endif
