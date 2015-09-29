#ifndef _SOCK_ADDR_INCLUDE_
#define _SOCK_ADDR_INCLUDE_
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <assert.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/un.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string>
#include "Define.h"

MINA_NAMESPACE_START

class CSockAddr
{
public:
    CSockAddr(char const* apcIP, uint16_t au16_port);
    CSockAddr();
    int GetRemote(int aiSock);
    int GetLocal(int aiSock);
    uint32_t GetLocalIP(const char *apcDevName);
    bool IsLocal(uint32_t aiIP, bool abLoopSkip);
    int SetFamily(int ai_family);
    int GetFamily(void);
    int Set(CSockAddr& aoOthter);
    int Set(const struct sockaddr* apoAddr, size_t auLen);
    int Set(char const* apcIPaddr, uint16_t auPort, int aiFamily = AF_INET);
    int Set(char const* apcHost, char const* apcService, int aiFamily, int aiType, int aiProto);
    int Set(char const* apcIPPort);
    int SetPort(uint16_t au16_port);
    int GetPort(void);
    int SetIP(const char* apcIPaddr);
    char* GetIP(void);
    static char* GetIP(uint32_t aiIP);
    int SetWild(void);
    int GetWild(void);
    int SetLoop(void);
    int GetLoop(void);
    bool operator==(CSockAddr& aoOthter);
    bool operator!=(CSockAddr& aoOthter);
    size_t GetLength(void);
    struct sockaddr* GetAddr(void);
    uint32_t Ip2ID(const char *apcIP);
    std::string ID2String(uint64_t au64ID);
    uint64_t GetID(const char *ip, int port);
    uint64_t GetID( void );

private:
    char macIPBuffer[64];
    struct sockaddr_storage moAddr;

};
MINA_NAMESPACE_END
#endif
