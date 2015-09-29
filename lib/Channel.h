#ifndef _mina_CHANNEL_H_INCLUDE_
#define _mina_CHANNEL_H_INCLUDE_

#include <sys/param.h>
#include <sys/epoll.h>
#include "Define.h"
#include "Cycle.h"

#define MINA_CMD_OPEN_CHANNEL   1
#define MINA_CMD_CLOSE_CHANNEL  2
#define MINA_CMD_QUIT           3
#define MINA_CMD_TERMINATE      4
#define MINA_CMD_REOPEN         5

MINA_NAMESPACE_START
typedef struct
{
    unsigned int muCommand;
    pid_t muPid;
    int miSlot;
    int miFd;
}SChannel;

int MinaWriteChannel(int aiSock, SChannel *lpoChanel);
int MinaReadChannel(int aiSock, SChannel *lpoChanel);
int MinaAddChannelEvent(CCycle* apoCycle, int aiSock, int aiEvent);
void MinaCloseChannel(int *aiFd);
void MinaChannelHandler(CIoEvent* apoEvent);



MINA_NAMESPACE_END
#endif
