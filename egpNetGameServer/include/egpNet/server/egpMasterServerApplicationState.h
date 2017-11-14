/*
	egpMasterServerApplicationState.h
	By Dan Buckstein

	Interface for master server.

	Additional contributions:
*/

#ifndef __EGP_MASTERSERVERAPPLICATIONSTATE_H_
#define __EGP_MASTERSERVERAPPLICATIONSTATE_H_


#include "egpNet/egpApplicationState.h"


class egpMasterServerApplicationState : public egpApplicationState
{
	// internal updates
	virtual int UpdateNetworking();
	virtual int ProcessPacket(const RakNet::Packet *packet);


	// connection list in base class is shared
	// could use separate list to distinguish servers from players
	int *mp_isServer;
	unsigned int m_numServers;

public:

	egpMasterServerApplicationState(const unsigned int maxIncomingConnections);
	virtual ~egpMasterServerApplicationState();

	// callbacks
	virtual int OnIdle();

};


#endif	// __EGP_MASTERSERVERAPPLICATIONSTATE_H_