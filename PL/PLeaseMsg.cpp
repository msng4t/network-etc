#include "PLeaseMsg.h"
#include "common.h"
#include "Log.h"
#include "Singleton.h"
#include <stdio.h>

MINA_NAMESPACE_START
void PLeaseMsg::Init(char type_, unsigned nodeID_)
{
	type = type_;
	nodeID = nodeID_;
}

bool PLeaseMsg::PrepareRequest(unsigned nodeID_,
uint64_t proposalID_, uint64_t paxosID_)
{
	Init(PLEASE_PREPARE_REQUEST, nodeID_);
	proposalID = proposalID_;
	paxosID = paxosID_;

	return true;
}

bool PLeaseMsg::PrepareRejected(unsigned nodeID_, uint64_t proposalID_)
{
	Init(PLEASE_PREPARE_REJECTED, nodeID_);
	proposalID = proposalID_;

	return true;
}

bool PLeaseMsg::PreparePreviouslyAccepted(unsigned nodeID_,
uint64_t proposalID_, uint64_t acceptedProposalID_,
unsigned leaseOwner_, unsigned duration_)
{
	Init(PLEASE_PREPARE_PREVIOUSLY_ACCEPTED, nodeID_);
	proposalID = proposalID_;
	acceptedProposalID = acceptedProposalID_;
	leaseOwner = leaseOwner_;
	duration = duration_;

	return true;
}

bool PLeaseMsg::PrepareCurrentlyOpen(unsigned nodeID_, uint64_t proposalID_)
{
	Init(PLEASE_PREPARE_CURRENTLY_OPEN, nodeID_);
	proposalID = proposalID_;

	return true;
}

bool PLeaseMsg::ProposeRequest(unsigned nodeID_,
uint64_t proposalID_, unsigned int leaseOwner_, unsigned duration_)
{
	Init(PLEASE_PROPOSE_REQUEST, nodeID_);
	proposalID = proposalID_;
	leaseOwner = leaseOwner_;
	duration = duration_;

	return true;
}

bool PLeaseMsg::ProposeRejected(unsigned nodeID_, uint64_t proposalID_)
{
	Init(PLEASE_PROPOSE_REJECTED, nodeID_);
	proposalID = proposalID_;

	return true;
}

bool PLeaseMsg::ProposeAccepted(unsigned nodeID_, uint64_t proposalID_)
{
	Init(PLEASE_PROPOSE_ACCEPTED, nodeID_);
	proposalID = proposalID_;

	return true;
}

bool PLeaseMsg::LearnChosen(unsigned nodeID_,
unsigned leaseOwner_, unsigned duration_, uint64_t localExpireTime_)
{
	Init(PLEASE_LEARN_CHOSEN, nodeID_);
	leaseOwner = leaseOwner_;
	duration = duration_;
	localExpireTime = localExpireTime_;

	return true;
}

bool PLeaseMsg::IsRequest()
{
	return (type == PLEASE_PROPOSE_REQUEST ||
			type == PLEASE_PREPARE_REQUEST);
}

bool PLeaseMsg::IsResponse()
{
	return IsPrepareResponse() || IsProposeResponse();
}

bool PLeaseMsg::IsPrepareResponse()
{
	return (type == PLEASE_PREPARE_REJECTED ||
			type == PLEASE_PREPARE_PREVIOUSLY_ACCEPTED ||
			type == PLEASE_PREPARE_CURRENTLY_OPEN);
}

bool PLeaseMsg::IsProposeResponse()
{
	return (type == PLEASE_PROPOSE_REJECTED ||
			type == PLEASE_PROPOSE_ACCEPTED);
}

bool PLeaseMsg::Read(mina::CMemblock& data)
{
	int read;

	if (data.GetSize() < 1)
		return false;

	switch (data.GetRead()[0])
	{
		case PLEASE_PREPARE_REQUEST:
			read = snreadf(data.GetRead(), data.GetSize(), "%c:%u:%U:%U",
						   &type, &nodeID, &proposalID, &paxosID);
			break;
		case PLEASE_PREPARE_REJECTED:
			read = snreadf(data.GetRead(), data.GetSize(), "%c:%u:%U",
						   &type, &nodeID, &proposalID);
			break;
		case PLEASE_PREPARE_PREVIOUSLY_ACCEPTED:
			read = snreadf(data.GetRead(), data.GetSize(), "%c:%u:%U:%U:%u:%u",
						   &type, &nodeID, &proposalID, &acceptedProposalID,
						   &leaseOwner, &duration);
			break;
		case PLEASE_PREPARE_CURRENTLY_OPEN:
			read = snreadf(data.GetRead(), data.GetSize(), "%c:%u:%U",
						   &type, &nodeID, &proposalID);
			break;
		case PLEASE_PROPOSE_REQUEST:
			read = snreadf(data.GetRead(), data.GetSize(), "%c:%u:%U:%u:%u",
						   &type, &nodeID, &proposalID,
						   &leaseOwner, &duration);
			break;
		case PLEASE_PROPOSE_REJECTED:
			read = snreadf(data.GetRead(), data.GetSize(), "%c:%u:%U",
						   &type, &nodeID, &proposalID);
			break;
		case PLEASE_PROPOSE_ACCEPTED:
			read = snreadf(data.GetRead(), data.GetSize(), "%c:%u:%U",
						   &type, &nodeID, &proposalID);
			break;
		case PLEASE_LEARN_CHOSEN:
			read = snreadf(data.GetRead(), data.GetSize(), "%c:%u:%u:%u:%U",
						   &type, &nodeID, &leaseOwner, &duration,
						   &localExpireTime);
			break;
		default:
			return false;
	}

	return (read == (signed)data.GetSize() ? true : false);
}

bool PLeaseMsg::Write(mina::CMemblock& data)
{
    int liRet = -1;
	switch (type)
	{
		case PLEASE_PREPARE_REQUEST:
			liRet = snprintf(data.GetWrite(), data.GetDirectSpace(), "%c:%u:%llu:%llu",
						       type, nodeID, proposalID, paxosID);
            data.SetWrite(liRet);
            return liRet;
		case PLEASE_PREPARE_REJECTED:
            liRet = snprintf(data.GetWrite(), data.GetDirectSpace(),"%c:%u:%llu",
						       type, nodeID, proposalID);
            data.SetWrite(liRet);
            return liRet;
        case PLEASE_PREPARE_PREVIOUSLY_ACCEPTED:
            liRet = snprintf(data.GetWrite(), data.GetDirectSpace(), "%c:%u:%llu:%llu:%u:%u",
                    type, nodeID, proposalID, acceptedProposalID,
                    leaseOwner, duration);
            data.SetWrite(liRet);
            return liRet;
        case PLEASE_PREPARE_CURRENTLY_OPEN:
            liRet = snprintf(data.GetWrite(), data.GetDirectSpace(), "%c:%u:%llu",
                    type, nodeID, proposalID);
            data.SetWrite(liRet);
            return liRet;
        case PLEASE_PROPOSE_REQUEST:
            liRet = snprintf(data.GetWrite(), data.GetDirectSpace(), "%c:%u:%llu:%u:%u",
                    type, nodeID, proposalID,
                    leaseOwner, duration);
            data.SetWrite(liRet);
            return liRet;
        case PLEASE_PROPOSE_REJECTED:
            liRet = snprintf(data.GetWrite(), data.GetDirectSpace(), "%c:%u:%llu",
                    type, nodeID, proposalID);
            data.SetWrite(liRet);
            return liRet;
        case PLEASE_PROPOSE_ACCEPTED:
            liRet = snprintf(data.GetWrite(), data.GetDirectSpace(), "%c:%u:%llu",
                    type, nodeID, proposalID);
            data.SetWrite(liRet);
            return liRet;
        case PLEASE_LEARN_CHOSEN:
            liRet = snprintf(data.GetWrite(), data.GetDirectSpace(), "%c:%u:%u:%u:%llu",
                    type, nodeID, leaseOwner, duration,
                    localExpireTime);
            data.SetWrite(liRet);
            return liRet;
        default:
            return false;
    }
}


void PLeaseMsg::Dump( void )
{
	switch (type)
	{
		case PLEASE_PREPARE_REQUEST:
	        LOG_TRACE("Message:type, nodeid, proposalid, paxosid");
			LOG_TRACE("%s:%u:%llu:%llu", "PRE_REQ", nodeID, proposalID, paxosID);
			break;
		case PLEASE_PREPARE_REJECTED:
	        LOG_TRACE("Message:type, nodeid, proposalid");
			LOG_TRACE("%s:%u:%llu",
						   "PRE_REJ", nodeID, proposalID);
			break;
		case PLEASE_PREPARE_PREVIOUSLY_ACCEPTED:
	        LOG_TRACE("Message:type, nodeid, proposalid, accedProposalid, leasOwner, duration");
			LOG_TRACE("%s:%u:%llu:%llu:%u:%u",
						   "PRE_PRE_ACCED", nodeID, proposalID,
                           acceptedProposalID, leaseOwner, duration);
			break;
		case PLEASE_PREPARE_CURRENTLY_OPEN:
	        LOG_TRACE("Message:type, nodeid, proposalid");
			LOG_TRACE("%s:%u:%llu", "PRE_CUR_OPEN", nodeID, proposalID);
			break;
		case PLEASE_PROPOSE_REQUEST:
	        LOG_TRACE("Message:type, nodeid, proposalid, leaseOwner, duration");
			LOG_TRACE("%s:%u:%llu:%u:%u", "PRO_REQ", nodeID, proposalID,
						   leaseOwner, duration);
			break;
		case PLEASE_PROPOSE_REJECTED:
	        LOG_TRACE("Message:type, nodeid, proposalid");
			LOG_TRACE("%s:%u:%llu", "PRO_REJ", nodeID, proposalID);
			break;
		case PLEASE_PROPOSE_ACCEPTED:
	        LOG_TRACE("Message:type, nodeid, proposalid");
			LOG_TRACE("%s:%u:%llu", "PRO_ACCED", nodeID, proposalID);
			break;
		case PLEASE_LEARN_CHOSEN:
	        LOG_TRACE("Message:type, nodeid, leaseOwner, duration, localExTime");
			LOG_TRACE("%s:%u:%u:%u:%llu", "LEARN_CHOS", nodeID, leaseOwner,
                    duration, localExpireTime);
			break;
		default:
			LOG_TRACE("WRONG MESSAGE.");
	}

}

MINA_NAMESPACE_END
