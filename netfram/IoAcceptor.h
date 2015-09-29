#ifndef _IO_ACCEPTOR_INCLUDE_
#define _IO_ACCEPTOR_INCLUDE_
#include <stdint.h>
#include <assert.h>
#include "Define.h"
#include "SockAddr.h"
#include "IoService.h"
#include "Session.h"
#include "SockDgram.h"

MINA_NAMESPACE_START

class CIoAcceptor: public CIoService
{
public:
    CIoAcceptor(CCycle *apoCycle);
    ~CIoAcceptor(void);
    int Bind(const char* apcGroup, CSockAddr& aoAddr);
    int Bind(const char* apcGroup, const char* apcIP, uint16_t au16Port);
    int Bind(std::string aoGroup, CSockAddr& aoAddr);
    int Bind(std::string aoGroup, const char* apcIP, uint16_t au16Port);
    int Bind(CSockAddr& aoAddr);
    int Bind(const char* apcIP, uint16_t au16Port);

    int Join(std::string& aoGroup, CSockAddr& aoAddr);
    int Join(std::string& aoGroup, const char* apcIP, uint16_t au16Port);
    int Join(const char* apcGroup, CSockAddr& aoGroup);
    int Join(const char* apcGroup, const char* apcIP, uint16_t au16Port);
    int Join(CSockAddr& aoGroup);
    int Join(const char* apcIP, uint16_t au16Port);

    int BindUDP(std::string& aoGroup, CSockAddr& aoAddr);
    int BindUDP(std::string& aoGroup, const char* apcIP, uint16_t au16Port);
    int BindUDP(const char* apcGroup, CSockAddr& aoAddr);
    int BindUDP(const char* apcGroup, const char* apcIP, uint16_t au16Port);
    int BindUDP(CSockAddr& aoAddr);
    int BindUDP(const char* apcIP, uint16_t au16Port);

    CListen* GetListen(void);
private:
    CListen moListen;
    CSockDgram moDgram;


};

MINA_NAMESPACE_END
#endif
