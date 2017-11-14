/*
	egpServerApplicationState.h
	By Dan Buckstein

	Interface for server application development.

	Additional contributions:
*/

#ifndef __EGP_SERVERAPPLICATIONSTATE_H_
#define __EGP_SERVERAPPLICATIONSTATE_H_


#include "egpNet/egpApplicationState.h"


class egpServerApplicationState : public egpApplicationState
{
	
	// internal updates
	virtual int UpdateNetworking();
	virtual int ProcessPacket(const RakNet::Packet *packet);


	// state update timer
	egpTimer m_updateTimer[1];


public:

	egpServerApplicationState();
	virtual ~egpServerApplicationState();

	// callbacks
	virtual int OnIdle();

};



#endif	// !__EGP_SERVERAPPLICATIONSTATE_H_