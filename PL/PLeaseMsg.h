#ifndef PLEASEMSG_H
#define PLEASEMSG_H

#include "Memblock.h"

MINA_NAMESPACE_START
#define PLEASE_PREPARE_REQUEST				'1'
#define PLEASE_PREPARE_REJECTED				'2'
#define PLEASE_PREPARE_PREVIOUSLY_ACCEPTED	'3'
#define PLEASE_PREPARE_CURRENTLY_OPEN		'4'
#define PLEASE_PROPOSE_REQUEST				'5'
#define PLEASE_PROPOSE_REJECTED				'6'
#define PLEASE_PROPOSE_ACCEPTED				'7'
#define PLEASE_LEARN_CHOSEN					'8'

class PLeaseMsg
{
public:
	char			type;
	unsigned		nodeID;
	uint64_t		proposalID;

	uint64_t		acceptedProposalID;
	unsigned		leaseOwner;
	unsigned		duration;
	uint64_t		localExpireTime;
	uint64_t		paxosID; // so only up-to-date nodes can become masters
	
	void			Init(char type_, unsigned nodeID_);
	void            Dump( void );
		
	bool			PrepareRequest(unsigned nodeID_,
					uint64_t proposalID_, uint64_t paxosID_);
	bool			PrepareRejected(unsigned nodeID_, uint64_t proposalID_);
	bool			PreparePreviouslyAccepted(unsigned nodeID_,
					uint64_t proposalID_, uint64_t acceptedProposalID_,
					unsigned leaseOwner_, unsigned duration_);
	bool			PrepareCurrentlyOpen(unsigned nodeID_,
					uint64_t proposalID_);	
	bool			ProposeRequest(unsigned nodeID_, uint64_t proposalID_,
					unsigned leaseOwner_, unsigned duration_);
	bool			ProposeRejected(unsigned nodeID_, uint64_t proposalID_);
	bool			ProposeAccepted(unsigned nodeID_, uint64_t proposalID_);
	bool			LearnChosen(unsigned nodeID,
					unsigned leaseOwner_, unsigned duration_,
					uint64_t localExpireTime_);
	
	bool			IsRequest();
	bool			IsPrepareResponse();
	bool			IsProposeResponse();
	bool			IsResponse();
	
	bool			Read(mina::CMemblock& data);
	bool			Write(mina::CMemblock& data);	
};

MINA_NAMESPACE_END
#endif
