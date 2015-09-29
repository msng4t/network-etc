#ifndef _IO_SERVICE_INCLUDE_
#define _IO_SERVICE_INCLUDE_
#include <stdint.h>
#include <assert.h>
#include <map>
#include "Define.h"
#include "IoHandler.h"
#include "FilterChain.h"
#include "Configuration.h"
#include "SockDgram.h"


MINA_NAMESPACE_START
class CCycle;
class CSession;
class CIoService
{
public:
    CIoService(CCycle *apoCycle);
    virtual ~CIoService(void);
    void SetHandler(CIoHandler* apoHanlder);
    CIoHandler* GetHandler(void);
    void SetFilterChain(CFilterChain *apoFilterChain);
    CFilterChain* GetFilterChain(void);
    void InitSession(CSession *apoSession);
    void SetName( std::string& aoName );
    void SetName( const char* apcName );
    std::string& GetName( void );
    CServiceConf* GetConfiguration( void );
    CCycle* GetCycle( void );
    int InitUDP( CSockDgram& aoDgram);
private:
    CIoHandler *mpoHandler;
    CFilterChain moFilterChain;
    std::map<uint64_t, CSession*> moSessions;
    CCycle* mpoCycle;
    std::string moName;
    CServiceConf moConfigure;


};

MINA_NAMESPACE_END
#endif
