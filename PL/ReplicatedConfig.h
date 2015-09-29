#ifndef REPLICATEDCONFIG_H
#define REPLICATEDCONFIG_H

#include "SockAddr.h"

#define MAX_CELL_SIZE				256
#define WIDTH_NODEID				8
#define WIDTH_RESTART_COUNTER		16

#define RCONF (ReplicatedConfig::Get())

MINA_NAMESPACE_START

class ReplicatedConfig
{
public:
	static ReplicatedConfig* Get();
	
	bool			Init();
	
	unsigned		MinMajority();
	uint64_t		NextHighest(uint64_t proposalID);
	unsigned		GetPort();
	unsigned		GetNodeID();
	unsigned		GetNumNodes();
	uint64_t		GetRestartCounter();
    bool            GetMultiMode();
	CSockAddr&	    GetAddr(unsigned i);

private:
	unsigned		nodeID;
	unsigned		numNodes; // same as endpoints.size
	uint64_t		restartCounter;	
    CSockAddr		maoAddresses[MAX_CELL_SIZE];
    bool            mbMultiMode;

private:
	void			InitRestartCounter();
};

MINA_NAMESPACE_END
#endif
