/*
	egpClientApplicationState.h
	By Dan Buckstein

	Interface for client application development.

	Additional contributions:
*/

#ifndef __EGP_CLIENTAPPLICATIONSTATE_H_
#define __EGP_CLIENTAPPLICATIONSTATE_H_


#include "egpNet/egpApplicationState.h"
#include "egpNet/client/egpWindowState.h"


class egpClientApplicationState : public egpApplicationState
{

	const egpWindowState *mpk_windowState;

	// throttles
	egpTimer m_updateRenderTimer[1];


	// internal updates
	virtual int UpdateNetworking();
	virtual int ProcessPacket(const RakNet::Packet *packet);

	virtual int UpdateDisplay();


public: 

	egpClientApplicationState();
	virtual ~egpClientApplicationState();


	// overloaded callbacks
	virtual int OnIdle();
	virtual int OnKeyPress(unsigned char key);


	// set window state
	void SetWindowState(const egpWindowState *windowState);

};


#endif	// !__EGP_CLIENTAPPLICATIONSTATE_H_