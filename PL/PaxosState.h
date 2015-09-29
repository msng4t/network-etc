#ifndef PAXOSSTATE_H
#define PAXOSSTATE_H

#include <math.h>
#include "Memblock.h"

class PaxosProposerState
{
public:	
	bool Active()
	{
		return (preparing || proposing);
	}
	
	void Init()
	{
		preparing =	false;
		proposing =	false;
		proposalID = 0;
		highestReceivedProposalID =	0;
		highestPromisedProposalID = 0;
		value.Reset();
		leader = false;
		numProposals = 0;
	}

public:
	bool		preparing;
	bool		proposing;
	uint64_t	proposalID;
	uint64_t	highestReceivedProposalID;
	uint64_t	highestPromisedProposalID;
    mina::CMemblock value;
	bool		leader;		  // multi paxos
	unsigned	numProposals; // number of proposal runs in this Paxos round
};

class PaxosAcceptorState
{
public:	
	void Init()
	{
		promisedProposalID = 0;
		accepted = false;
		acceptedProposalID = 0;
		acceptedValue.Reset();
	}

public:
	uint64_t	promisedProposalID;
	bool		accepted;	
	uint64_t	acceptedProposalID;
    mina::CMemblock acceptedValue;

};

class PaxosLearnerState
{
public:	
	void Init()
	{
		learned = 0;
		value.Reset();
	}

public:
	bool		learned;
    mina::CMemblock value;
};

#endif
