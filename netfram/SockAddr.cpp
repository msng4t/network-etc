#include "SockAddr.h"

MINA_NAMESPACE_START

CSockAddr::CSockAddr(void)
{
}

CSockAddr::CSockAddr(char const* apcIP, uint16_t au16Port)
{
    this->SetIP(apcIP);
    this->SetPort(au16Port);
}
int
CSockAddr::SetFamily(int aiFamily)
{
    switch (aiFamily)
    {
        case AF_UNSPEC:
        case AF_INET:
#if defined(AF_INET6)
        case AF_INET6:
#endif
        case AF_UNIX:
            this->GetAddr()->sa_family = aiFamily;
            return 0;
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}

int
CSockAddr::GetFamily(void)
{
    int liFamily;
    switch (this->GetAddr()->sa_family)
    {
        case AF_UNSPEC:
        case AF_INET:
#if defined(AF_INET6)
        case AF_INET6:
#endif
        case AF_UNIX:
            liFamily = this->GetAddr()->sa_family;
            break;
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
    return liFamily;

}

int
CSockAddr::Set(const struct sockaddr* apoAddr, size_t auLen)
{
    if (auLen > sizeof(this->moAddr))
    {
        errno = EINVAL;
        return -1;
    }
    memcpy(this->GetAddr(), apoAddr, auLen);
    return 0;
}
int
CSockAddr::Set(CSockAddr& aoOthter)
{
    memcpy(this->GetAddr(), aoOthter.GetAddr(), aoOthter.GetLength());
    return 0;
}

int
CSockAddr::Set(char const* apcIPaddr,
        uint16_t auPort,
        int aiFamily)
{
    if (SetFamily(aiFamily) != 0)
        return -1;
    if (SetPort(auPort) != 0)
        return -1;
    if (SetIP(apcIPaddr) != 0)
        return -1;
    return 0;
}

int CSockAddr::Set(char const* apcau64ID)
{
    char lacBuffer[21];
    char *lpcPose = NULL;
    int liPort = -1;

    for (size_t liC = 0; liC < sizeof(lacBuffer); liC++)
    {
        lacBuffer[liC] = apcau64ID[liC];
        if (lacBuffer[liC] == ':')
        {
            lacBuffer[liC] = 0;
            lpcPose = &lacBuffer[liC + 1];
        }
    }

    if (lpcPose == NULL || lpcPose > lacBuffer + 16)
    {
        return -1;
    }

    liPort = atoi(lpcPose);
    return this->Set(lacBuffer, liPort);

}

int
CSockAddr::SetIP(const char* apcIP)
{
    switch (this->GetAddr()->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in* lps_sin = (struct sockaddr_in *)this->GetAddr();

            if (this->GetLength()< sizeof(*lps_sin))
            {
                return -1;
            }

            return inet_pton(
                    this->GetAddr()->sa_family,
                    apcIP,
                    &(lps_sin->sin_addr.s_addr)) == 1 ? 0 : -1;
            }
#if defined(AF_INET6)
        case AF_INET6:
        {
            struct sockaddr_in6* lps_sin6 = (struct sockaddr_in6*)this->GetAddr();

            if (this->GetLength()< sizeof(*lps_sin6))
            {
                return -1;
            }

            return inet_pton(
                    this->GetAddr()->sa_family,
                    apcIP,
                    &(lps_sin6->sin6_addr))
                == 1 ? 0 : -1;
        }
#endif
        case AF_UNIX:
        {
            struct sockaddr_un* lps_un = (struct sockaddr_un*)this->GetAddr();
//            strncpy(lps_un->sun_path, apcIP, this->GetLength() - offsetof(struct sockaddr_un, sun_path));
            strncpy(lps_un->sun_path, apcIP, sizeof(lps_un->sun_path));
            return 0;
        }
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}

char*
CSockAddr::GetIP(void)
{
    int liSet = 0;
    do{
        switch (this->GetAddr()->sa_family)
        {
            case AF_INET:
                {
                    struct sockaddr_in * lps_in = (struct sockaddr_in*)this->GetAddr();

                    if (sizeof(this->macIPBuffer)< INET_ADDRSTRLEN)
                    {
                        liSet = -1;
                        break;
                    }

                    if (NULL == inet_ntop(AF_INET, &(lps_in->sin_addr.s_addr), this->macIPBuffer, sizeof(this->macIPBuffer)))
                    {
                        liSet = -1;
                        break;
                    }
                    break;
                }
#if defined(AF_INET6)
            case AF_INET6:
                {
                    struct sockaddr_in6* lps_sin6 = (struct sockaddr_in6 *)this->GetAddr();

                    if (sizeof(this->macIPBuffer) < INET6_ADDRSTRLEN)
                    {
                        liSet = -1;
                        break;
                    }

                    if (NULL == inet_ntop(AF_INET6, &(lps_sin6->sin6_addr), this->macIPBuffer, sizeof(this->macIPBuffer)))
                    {
                        liSet = -1;
                    }
                    break;
                }
#endif
            case AF_UNIX:
                {
                    struct sockaddr_un *lps_un = (struct sockaddr_un*)this->GetAddr();
                    strncpy(this->macIPBuffer, lps_un->sun_path, sizeof(this->macIPBuffer));

                    break;
                }
            default:
                errno = EAFNOSUPPORT;
                liSet = -1;
        }
    }while(false);
    if (liSet != 0)
    {
        this->macIPBuffer[0] = '\0';
    }

    return this->macIPBuffer;
}


int
CSockAddr::Set(char const* apcHost,
        char const* apcService,
        int aiFamily,
        int aiType,
        int aiProto)
{
    size_t lu_idx;
#if defined(HAVE_GETADDRINFO)
    struct addrinfo ls_hint;
    struct* addrinfo lps_addrinfo = NULL;
    struct* addrinfo lps_it;
    int     li_ret;

    memset(&ls_hint, 0, sizeof(ls_hint));

    ls_hint->aiFamily = aiFamily;
    ls_hint->aiType = aiType;
    ls_hint->aiProto = aiProto;

    ls_ret = getaddrinfo(apcHost, apcService, &ls_hint, &lps_addrinfo);

    if (0 != li_ret)
    {
        errno = li_ret;
        return -1;
    }

    lps_it = lps_addrinof;

    for (lu_idx = 0; lu_idx < au_saslen && lps_itr != NULL; ++lu_idx)
    {
        assert(lps_itr->ai_addlen <= sizeof(struct sockaddr_storage));

        memcpy(this->GetAddr()[lu_idx], lps_it->ai_addr, lps_it->ai_addlen);

        lps_it = lps_it->ai_next;
    }

    freeaddrinfo(lps_addrinfo);

    return (int)lu_idx;
#else
    if (AF_INET == aiFamily)
    {
        struct hostent* lps_host;
        struct servent* lps_serv;
        char** lppc_str;

        if (apcHost)
        {
            lps_host = gethostbyname(apcHost);
            if (NULL == lps_host)
            {
                errno = h_errno;
                return -1;
            }
        }

        if (apcService)
        {
            switch (aiProto)
            {
                case 0:
                    lps_serv = getservbyname(apcService, NULL);
                    break;
                case IPPROTO_TCP:
                    lps_serv = getservbyname(apcService, "tcp");
                    break;
                case IPPROTO_UDP:
                    lps_serv = getservbyname(apcService, "udp");
                    break;
                default:
                    errno = ENOPROTOOPT;
                    return -1;
            }

            if (NULL == lps_serv)
            {
                errno = 0;
                return -1;
            }
        }

        if (lps_host)
        {
            lppc_str = lps_host->h_addr_list;

            for (lu_idx = 0; lu_idx < 1 && NULL != lppc_str; ++lu_idx)
            {
                struct sockaddr_in *lps_inaddr = (struct sockaddr_in *)&this->GetAddr()[lu_idx];
                assert(sizeof(struct in_addr) == lps_host->h_length);
                memcpy(&(lps_inaddr->sin_addr.s_addr), *lppc_str, lps_host->h_length);

                if (lps_serv->s_port)
                {
                    lps_inaddr->sin_port = lps_serv->s_port;
                }
            }
            return lu_idx;
        }
        else if (lps_serv)
        {
            struct sockaddr_in *lps_inaddr = (struct sockaddr_in *)&this->GetAddr()[0];
            lps_inaddr->sin_port = lps_serv->s_port;

            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        errno = EAFNOSUPPORT;
        return -1;
    }
#endif
}

int
CSockAddr::SetPort(uint16_t au16Port)
{
    switch (this->GetAddr()->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in * lps_sin = (struct sockaddr_in*)this->GetAddr();

            if (this->GetLength() < sizeof(struct sockaddr_in))
            {
                errno = EINVAL;
                return -1;
            }

            lps_sin->sin_port = htons(au16Port);

            return 0;
        }
#if defined(AF_INET6)
        case AF_INET6:
        {
            struct sockaddr_in6* lps_sin6 = (struct sockaddr_in6 *)this->GetAddr();

            if (this->GetLength() < sizeof(struct sockaddr_in6))
            {
                errno = EINVAL;
                return -1;
            }

            lps_sin6->sin6_port = htons(au16Port);

            return 0;
        }
#endif
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}

int
CSockAddr::GetPort(void)
{
    uint16_t luPort = 0;
    switch (this->GetAddr()->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in * lps_sin = (struct sockaddr_in*)this->GetAddr();

            luPort = (int)ntohs(lps_sin->sin_port);

            return luPort;
        }
#if defined(AF_INET6)
        case AF_INET6:
        {
            struct sockaddr_in6* lps_sin6 = (struct sockaddr_in6 *)this->GetAddr();

            luPort = (int)ntohs(lps_sin6->sin6_port);
            return luPort;
        }
#endif
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}

int
CSockAddr::SetWild(void)
{
    switch (this->GetAddr()->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in * lps_sin = (struct sockaddr_in*)this->GetAddr();

            if (this->GetLength() < sizeof(struct sockaddr_in))
            {
                errno = EINVAL;
                return -1;
            }

            lps_sin->sin_addr.s_addr = htonl(INADDR_ANY);

            return 0;
        }
#if defined(AF_INET6)
        case AF_INET6:
        {
            struct sockaddr_in6* lps_sin6 = (struct sockaddr_in6 *)this->GetAddr();

            if (this->GetLength() < sizeof(struct sockaddr_in6))
            {
                errno = EINVAL;
                return -1;
            }

            memcpy(&(lps_sin6->sin6_addr), &in6addr_any, sizeof(lps_sin6->sin6_addr));

            return 0;
        }
#endif
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}

int
CSockAddr::GetWild(void)
{
    switch (this->GetAddr()->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in * lps_sin = (struct sockaddr_in*)this->GetAddr();

            if (this->GetLength() < sizeof(struct sockaddr_in))
            {
                errno = EINVAL;
                return -1;
            }

            return (lps_sin->sin_addr.s_addr == htonl(INADDR_ANY)) ? 1 : 0;

            return 0;
        }
#if defined(AF_INET6)
        case AF_INET6:
        {
            struct sockaddr_in6* lps_sin6 = (struct sockaddr_in6 *)this->GetAddr();

            if (this->GetLength() < sizeof(struct sockaddr_in6))
            {
                errno = EINVAL;
                return -1;
            }

            return ( 0 == memcmp(&(lps_sin6->sin6_addr), &in6addr_any, sizeof(lps_sin6->sin6_addr))) ? 1 : 0;

            return 0;
        }
#endif
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}


int
CSockAddr::SetLoop(void)
{
    switch (this->GetAddr()->sa_family)
    {
        case AF_INET:
            return this->SetIP("127,0.0.1");
#if defined(AF_INET6)
        case AF_INET6:
        {
            struct sockaddr_in6* lps_sin6 = (struct sockaddr_in6 *)this->GetAddr();

            if (this->GetLength() < sizeof(struct sockaddr_in6))
            {
                errno = EINVAL;
                return -1;
            }

            memcpy(&(lps_sin6->sin6_addr), &in6addr_loopback, sizeof(lps_sin6->sin6_addr));

            return 0;
        }
#endif
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}

int
CSockAddr::GetLoop(void)
{
    switch (this->GetAddr()->sa_family)
    {
        case AF_INET:
        {
            struct sockaddr_in * lps_sin = (struct sockaddr_in*)this->GetAddr();

            if (this->GetLength() < sizeof(struct sockaddr_in))
            {
                errno = EINVAL;
                return -1;
            }

            return (lps_sin->sin_addr.s_addr = htonl(uint32_t(0x7F000001))) ? 1 : 0;
        }
#if defined(AF_INET6)
        case AF_INET6:
        {
            struct sockaddr_in6* lps_sin6 = (struct sockaddr_in6 *)this->GetAddr();

            if (this->GetLength() < sizeof(struct sockaddr_in6))
            {
                errno = EINVAL;
                return -1;
            }

            return (0 == memcmp(&(lps_sin6->sin6_addr), &in6addr_loopback, sizeof(lps_sin6->sin6_addr))) ? 1 : 0;
        }
#endif
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}

size_t
CSockAddr::GetLength(void)
{
    switch (this->GetAddr()->sa_family)
    {
        case AF_INET:
            return sizeof(struct sockaddr_in);

#if defined(AF_INET6)
        case AF_INET6:
            return sizeof(struct sockaddr_in6);
#endif
        case AF_UNIX:
            return sizeof(struct sockaddr_un);
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
}

struct sockaddr* CSockAddr::GetAddr(void)
{
    return reinterpret_cast<struct sockaddr *>(&this->moAddr);
}

uint32_t CSockAddr::GetLocalIP(const char *apcDevName)
{
    int             liFd;
    int             liIntrface;
    struct ifreq    laoBuf[16];
    struct ifconf   loIfc;

    if ((liFd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        return 0;
    }

    loIfc.ifc_len = sizeof(laoBuf);
    loIfc.ifc_buf = (caddr_t) laoBuf;
    if (ioctl(liFd, SIOCGIFCONF, (char *) &loIfc))
    {
        close(liFd);
        return 0;
    }

    liIntrface = loIfc.ifc_len / sizeof(struct ifreq);
    while (liIntrface-- > 0)
    {
        if(ioctl(liFd,SIOCGIFFLAGS,(char *) &laoBuf[liIntrface]))
        {
            continue;
        }
        if(laoBuf[liIntrface].ifr_flags&IFF_LOOPBACK)
        {
            continue;
        }
        if (!(laoBuf[liIntrface].ifr_flags&IFF_UP))
        {
            continue;
        }
        if (apcDevName != NULL && strcmp(apcDevName, laoBuf[liIntrface].ifr_name))
        {
            continue;
        }
        if (!(ioctl(liFd, SIOCGIFADDR, (char *) &laoBuf[liIntrface])))
        {
            close(liFd);
            return ((struct sockaddr_in *) (&laoBuf[liIntrface].ifr_addr))->sin_addr.s_addr;
        }
    }
    close(liFd);
    return 0;
}

bool CSockAddr::IsLocal(uint32_t aiIP, bool abLoopSkip)
{
    int             liFd;
    int             liIntrface;
    struct ifreq    laoBuf[16];
    struct ifconf   loIfc;

    if ((liFd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        return false;
    }

    loIfc.ifc_len = sizeof(laoBuf);
    loIfc.ifc_buf = (caddr_t) laoBuf;
    if (ioctl(liFd, SIOCGIFCONF, (char *) &loIfc))
    {
        close(liFd);
        return false;
    }

    liIntrface = loIfc.ifc_len / sizeof(struct ifreq);
    while (liIntrface-- > 0)
    {
        if(ioctl(liFd,SIOCGIFFLAGS,(char *) &laoBuf[liIntrface]))
        {
            continue;
        }
        if(abLoopSkip && laoBuf[liIntrface].ifr_flags&IFF_LOOPBACK)
        {
            continue;
        }
        if (!(laoBuf[liIntrface].ifr_flags&IFF_UP))
        {
            continue;
        }
        if (!(ioctl(liFd, SIOCGIFADDR, (char *) &laoBuf[liIntrface])))
        {
            if (((struct sockaddr_in *) (&laoBuf[liIntrface].ifr_addr))->sin_addr.s_addr
                    == aiIP)
            {
                close(liFd);
                return true;
            }
        }
    }
    close(liFd);
    return false;
}

int CSockAddr::GetLocal(int aiSock)
{
    struct sockaddr loSockAddr;
    socklen_t liSockLen = sizeof(loSockAddr);
    if (getsockname(aiSock, &loSockAddr, &liSockLen) != 0)
    {
        return MINA_ERROR;
    }
    this->Set(&loSockAddr, liSockLen);

    return MINA_OK;
}

int CSockAddr::GetRemote(int aiSock)
{
    struct sockaddr loSockAddr;
    socklen_t liSockLen = sizeof(loSockAddr);
    if (getpeername(aiSock, &loSockAddr, &liSockLen) != 0)
    {
        return MINA_ERROR;
    }
    this->Set(&loSockAddr, liSockLen);

    return MINA_OK;
}

bool CSockAddr::operator==(CSockAddr& aoOthter)
{
    return !memcpy(&this->moAddr, aoOthter.GetAddr(), aoOthter.GetLength());
}

bool CSockAddr::operator!=(CSockAddr& aoOthter)
{
    return memcpy(&this->moAddr, aoOthter.GetAddr(), aoOthter.GetLength());
}

/**
 * 10.0.100.89 => 0x5964000a
 */
uint32_t CSockAddr::Ip2ID(const char *apcIP)
{
    uint32_t luID;

    if (apcIP == NULL)
    {
        return 0;
    }
    luID = inet_addr(apcIP);
    if (luID == (uint32_t)INADDR_NONE) {
        struct hostent *lpoTent;
        if ((lpoTent = gethostbyname(apcIP)) == NULL)
        {
            return 0;
        }
        luID = ((struct in_addr *)lpoTent->h_addr)->s_addr;
    }
    return luID;
}

/**
 * uint64_t ID to IP:port
 */
std::string CSockAddr::ID2String(uint64_t au64ID)
{
    char lacIPPort[32];
    uint32_t lu32IP = (uint32_t)(au64ID & 0xffffffff);
    int liPort = (int)((au64ID >> 32 ) & 0xffff);
    unsigned char *bytes = (unsigned char *) &lu32IP;
    if (liPort > 0) {
        sprintf(lacIPPort, "%d.%d.%d.%d:%d", bytes[0], bytes[1], bytes[2], bytes[3], liPort);
    } else {
        sprintf(lacIPPort, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    }
    return lacIPPort;
}

/**
 * ip:port to uint64_t
 */
uint64_t CSockAddr::GetID(const char *ip, int aiPort)
{
    uint32_t lu32IP = 0;
    const char *p = strchr(ip, ':');
    uint64_t lu64ID = aiPort;

    if (p != NULL && p>ip) {
        int len = p-ip;
        if (len>64) len = 64;
        char tmp[128];
        strncpy(tmp, ip, len);
        tmp[len] = '\0';
        lu32IP = Ip2ID(tmp);
        aiPort = atoi(p+1);
    } else {
        lu32IP = Ip2ID(ip);
    }
    if (lu32IP == 0) {
        return 0;
    }
    lu64ID <<= 32;
    lu64ID |= lu32IP;
    return lu64ID;
}

uint64_t CSockAddr::GetID( void )
{
    uint64_t lu64ID = this->GetPort();
    lu64ID <<= 32;
    lu64ID |= (uint32_t)((struct sockaddr_in*)&this->moAddr)->sin_addr.s_addr;
    return lu64ID;
}

char *CSockAddr::GetIP(uint32_t aiIP)
{
  union ip_addr{
    uint32_t addr;
    uint8_t s[4];
  } a;
  a.addr = aiIP;
  static char s[16];
  sprintf(s, "%u.%u.%u.%u", a.s[0], a.s[1], a.s[2], a.s[3]);
  return s;
}
MINA_NAMESPACE_END
