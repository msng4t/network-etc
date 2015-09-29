#include "ReplicatedConfig.h"
#include "Config.h"
#include "Table.h"
#include "Transaction.h"
#include "SockAddr.h"

MINA_NAMESPACE_START
ReplicatedConfig config;

ReplicatedConfig* ReplicatedConfig::Get()
{
	return &config;
}

bool ReplicatedConfig::Init()
{
    CSockAddr	loAddress;

	nodeID = Config::GetIntValue("paxos.nodeID", 0);
	numNodes = Config::GetListNum("paxos.addresses");

	if (numNodes == 0)
	{
		loAddress.Set("0.0.0.0:10000");
		maoAddresses[0] = loAddress;
		nodeID = 0;
	}
	else
	{
		if (nodeID < 0 || nodeID >= numNodes)
			STOP_FAIL("Configuration error, "
					   "check your paxos.nodeID and paxos.maoAddresses entry" ,0);

		for (unsigned i = 0; i < numNodes; i++)
		{
			loAddress.Set(Config::GetListValue("paxos.addresses", i, NULL));
			maoAddresses[i] = loAddress;
		}
        mbMultiMode = true;
		for (unsigned i = 0; i < numNodes - 1; i++)
		{
			if (maoAddresses[i].GetPort() != maoAddresses[i+1].GetPort())
            {
                mbMultiMode = false;
                break;
            }
		}
	}

    LOG_TRACE("multicast mode %d", mbMultiMode);
	if (strcmp("replicated", Config::GetValue("mode", "")) != 0)
    {
        LOG_TRACE("not replicated");
    }
    else
    {
        LOG_TRACE("replicated, init restartCounter");
		InitRestartCounter();
    }

	return true;
}

void ReplicatedConfig::InitRestartCounter()
{
	bool			ret;
	unsigned		nread = 0;
    CMemblock       buf(1024);
	CTable*			table;
    uint64_t        restartCounter = 0;
    int             liRet = 0;

	table = database.GetTable("keyspace");
	if (table == NULL)
	{
		restartCounter = 0;
	    LOG_ERROR("Running with null tabel, restartCounter = %llu", restartCounter);
		return;
	}

	CTransaction tx(table);
	tx.Begin();

	ret = table->Get(&tx, "@@restartCounter", buf);

	if (ret)
	{
		restartCounter = strntouint64(buf.GetRead(), buf.GetSize(), &nread);
		if (nread != (unsigned) buf.GetSize())
			restartCounter = 0;
	}
	else
		restartCounter = 0;

	restartCounter++;

    buf.Reset();
    liRet = snprintf(buf.GetRead(), buf.GetCapacity(), "%llu", restartCounter);
    buf.SetWrite(liRet);

	table->Set(&tx, "@@restartCounter", buf);
	tx.Commit();

	LOG_ERROR("Running with restartCounter = %llu", restartCounter);
}

unsigned ReplicatedConfig::MinMajority()
{
	return (unsigned)(floor((double)numNodes / 2) + 1);
}

uint64_t ReplicatedConfig::NextHighest(uint64_t proposalID)
{
	// <proposal count since restart> <restartCounter> <nodeID>

	uint64_t left, middle, right, nextProposalID;

	left = proposalID >> (WIDTH_NODEID + WIDTH_RESTART_COUNTER);

	left++;

	left = left << (WIDTH_NODEID + WIDTH_RESTART_COUNTER);

	middle = restartCounter << WIDTH_NODEID;

	right = nodeID;

	nextProposalID = left | middle | right;

	return nextProposalID;
}

unsigned ReplicatedConfig::GetPort()
{
	return maoAddresses[nodeID].GetPort();
}

unsigned ReplicatedConfig::GetNodeID()
{
	return nodeID;
}

unsigned ReplicatedConfig::GetNumNodes()
{
	return numNodes;
}

uint64_t ReplicatedConfig::GetRestartCounter()
{
	return restartCounter;
}

CSockAddr& ReplicatedConfig::GetAddr(unsigned i)
{
	if (i < 0 || i >= numNodes)
		ASSERT_FAIL();

	return maoAddresses[i];
}

bool ReplicatedConfig::GetMultiMode()
{
    return mbMultiMode;
}
MINA_NAMESPACE_END
