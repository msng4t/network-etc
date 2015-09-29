#ifndef REPLICATEDLOGMSG_H
#define REPLICATEDLOGMSG_H

#include "Memblock.h"

#define BS_MSG_NOP	"NOP"

MINA_NAMESPACE_START
class ReplicatedLogMsg
{
public:
	unsigned	nodeID;
	uint64_t	restartCounter;
	uint64_t	leaseEpoch;
	mina::CMemblock	value;
	
	bool		Init(unsigned nodeID_, uint64_t restartCounter_,
				uint64_t leaseEpoch_, mina::CMemblock& value_);
		
	bool		Read(mina::CMemblock& data);
	bool		Write(mina::CMemblock& data);
};

MINA_NAMESPACE_END
#endif
