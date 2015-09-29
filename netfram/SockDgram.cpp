#include "SockDgram.h"
#include "Log.h"
#include "Singleton.h"
#include "Define.h"

MINA_NAMESPACE_START
CSockDgram::CSockDgram()
    : miFd (-1)
{
}

int CSockDgram::Open( void )
{
    this->miFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->miFd == -1)
    {
        LOG_ERROR("open socket failed(%s).", strerror(errno));
        return MINA_ERROR;
    }
    return 0;
}

int CSockDgram::Open( CSockAddr& aoAddr )
{
    this->Open();
    if (this->miFd == -1)
    {
        return MINA_ERROR;
    }

    if (bind(this->miFd, aoAddr.GetAddr(), aoAddr.GetLength()) < 0)
    {
        LOG_ERROR("bind failed(%s)", strerror(errno));
        return MINA_ERROR;
    }
    this->moLocal = aoAddr;

    return MINA_OK;
}

#define FAMILY2LEVEL(family) (family == AF_INET ? IPPROTO_IP : IPPROTO_IPV6)


int CSockDgram::Join( CSockAddr& aoGroup,
    char const*                 apcIfName,
    unsigned                      auIfIndex)

{
    bool lbBinded = true;

    if (this->moLocal.GetPort() == 0)
    {
        this->moLocal = aoGroup;
        this->moLocal.SetWild();
        lbBinded = false;
    }
    else if (this->moLocal.GetPort() != aoGroup.GetPort())
    {
        return MINA_ERROR;
    }

    if (this->miFd == -1)
    {
        if (this->Open())
        {
            return MINA_ERROR;
        }
    }

    if (!lbBinded)
    {
        if (bind(this->miFd, moLocal.GetAddr(), moLocal.GetLength()) < 0)
        {
            LOG_ERROR("bind failed(%s)", strerror(errno));
            return MINA_ERROR;
        }
    }

#if defined(MCAST_JOIN_GROUP)
    struct group_req lsGrpReq;

    if (auIfIndex > 0)
    {
        lsGrpReq.gr_interface = auIfIndex;
    }
    else if (apcIfName != NULL)
    {
        if ( (lsGrpReq.gr_interface = if_nametoindex(apcIfName)) == 0)
        {
            errno = ENXIO;  /* if name not found */
            return -1;
        }
    }
    else
    {
        lsGrpReq.gr_interface = 0;
    }

    if (aoGroup.GetLength() > sizeof(lsGrpReq.gr_group))
    {
        errno = EINVAL;
        return -1;
    }

    memcpy(&lsGrpReq.gr_group, aoGroup.GetAddr(), aoGroup.GetLength());

    return (int) setsockopt(
        miFd,
        FAMILY2LEVEL(aoGroup.GetAddr()->sa_family),
        MCAST_JOIN_GROUP,
        &lsGrpReq,
        sizeof(lsGrpReq));
#else
    switch (aoGroup.GetAddr()->sa_family)
    {
        case AF_INET:
        {
            struct ip_mreq          lsMreq;
            struct ifreq            lsIfreq;

            memcpy(&lsMreq.imr_multiaddr,
                &((const struct sockaddr_in *) aoGroup.GetAddr())->sin_addr,
                sizeof(struct in_addr));

            if (auIfIndex > 0)
            {
                if (if_indextoname(auIfIndex, lsIfreq.ifr_name) == NULL)
                {
                    errno = (ENXIO);  /* i/f index not found */
                    return -1;
                }

                goto IOCTL;
            }
            else if (apcIfName != NULL)
            {
                strncpy(lsIfreq.ifr_name, apcIfName, IFNAMSIZ);
IOCTL:
                if (ioctl(miFd, SIOCGIFADDR, &lsIfreq) < 0)
                {
                    return -1;
                }

                memcpy(&lsMreq.imr_interface,
                    &((struct sockaddr_in *) &lsIfreq.ifr_addr)->sin_addr,
                    sizeof(struct in_addr));
            }
            else
            {
                lsMreq.imr_interface.s_addr = hton32(INADDR_ANY);
            }

            return(int) setsockopt( miFd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                    &lsMreq, sizeof(lsMreq));
        }

#   if defined(IPV6_JOIN_GROUP)
        case AF_INET6:
        {
            struct ipv6_mreq        lsMreq6;

            memcpy(&lsMreq6.ipv6mr_multiaddr,
                &((const struct sockaddr_in6 *) aoGroup.GetAddr())->sin6_addr,
                sizeof(struct in6_addr));

            if (auIfIndex > 0)
            {
                lsMreq6.ipv6mr_interface = auIfIndex;
            }
            else if (apcIfName != NULL)
            {
                if ((lsMreq6.ipv6mr_interface = if_nametoindex(apcIfName)) == 0)
                {
                    errno = ENXIO;
                    return -1;
                }
            }
            else
            {
                lsMreq6.ipv6mr_interface = 0;
            }

            return (int) setsockopt( miFd, IPPROTO_IPV6, IPV6_JOIN_GROUP,
                    &lsMreq6, sizeof(lsMreq6));
        }
#   endif

        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
#endif
}

int CSockDgram::Leave(CSockAddr& aoGroup)
{
#if defined(MCAST_LEAVE_GROUP)
    struct group_req lsGrpReq;
    lsGrpReq.gr_interface = 0;

    if (aoGroup.GetLength() > sizeof(lsGrpReq.gr_group))
    {
        errno = EINVAL;
        return -1;
    }

    memcpy(&lsGrpReq.gr_group, aoGroup.GetAddr(), aoGroup.GetLength());
    return (int) setsockopt( miFd, FAMILY2LEVEL(aoGroup.GetAddr()->sa_family),
            MCAST_LEAVE_GROUP, &lsGrpReq, sizeof(lsGrpReq));
#else
    switch (aoGroup.GetAddr()->sa_family)
    {
        case AF_INET:
        {
            struct ip_mreq          lsMreq;

            memcpy(&lsMreq.imr_multiaddr,
                &((const struct sockaddr_in *) aoGroup.GetAddr())->sin_addr,
                sizeof(struct in_addr));

            lsMreq.imr_interface.s_addr = hton32(INADDR_ANY);

            return (int) setsockopt( miFd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                    &lsMreq, sizeof(lsMreq));
        }

#   if defined(IPV6_DROP_MEMBERSHIP)
        case AF_INET6:
        {
            struct ipv6_mreq        lsMreq6;

            memcpy(&lsMreq6.ipv6mr_multiaddr,
                &((const struct sockaddr_in6 *) aoGroup.GetAddr())->sin6_addr,
                sizeof(struct in6_addr));

            lsMreq6.ipv6mr_interface = 0;

            return (int) setsockopt( miFd, IPPROTO_IPV6, IPV6_LEAVE_GROUP,
                    &lsMreq6, sizeof(lsMreq6));
        }
#   endif
        default:
            errno = (EAFNOSUPPORT);
            return -1;
    }
#endif
}


int CSockDgram::GetFd( void )
{
    return this->miFd;
}

CSockAddr& CSockDgram::GetLocal( void )
{
    return this->moLocal;
}
MINA_NAMESPACE_END
