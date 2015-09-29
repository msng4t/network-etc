#include "ReplicatedLogMsg.h"
#include "common.h"
#include <stdio.h>

MINA_NAMESPACE_START
bool ReplicatedLogMsg::Init(unsigned nodeID_, uint64_t restartCounter_,
	uint64_t leaseEpoch_, mina::CMemblock& value_)
{
	nodeID = nodeID_;
	restartCounter = restartCounter_;
	leaseEpoch = leaseEpoch_;
	value.Init(value_.GetSize());
    value.Write(value_.GetRead(), value.GetSize());

	return true;
}

bool ReplicatedLogMsg::Read(mina::CMemblock& data)
{
	int read;
	
	read = snreadf(data.GetRead(), data.GetSize(), "%u:%U:%U:%N",
				   &nodeID, &restartCounter, &leaseEpoch, &value);
	
	return (read == (signed)data.GetSize() ? true : false);
}

bool ReplicatedLogMsg::Write(mina::CMemblock& data)
{
    int liRet = -1;
    liRet = snprintf(data.GetWrite(), data.GetDirectSpace(), "%u:%llu:%llu:%d:",
					   nodeID, restartCounter, leaseEpoch, value.GetSize());
    data.SetWrite(liRet);
    data.Write(value.GetWrite(), value.GetSize());
    return true;
}
MINA_NAMESPACE_END
