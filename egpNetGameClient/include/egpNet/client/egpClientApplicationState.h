/*
	egpClientApplicationState.h
	By Dan Buckstein

	Interface for client application development.

	Additional contributions:
*/

#ifndef __EGP_CLIENTAPPLICATIONSTATE_H_
#define __EGP_CLIENTAPPLICATIONSTATE_H_


#include "egpNet/egpApplicationState.h"
//#include "egpNet/client/egpWindowState.h"


class egpWindowState;

class egpClientApplicationState : public egpApplicationState
{

	const egpWindowState *mpk_windowState;

	// throttles
	egpTimer m_updateRenderTimer[1];


	// internal updates
	virtual int UpdateNetworking();
	virtual int ProcessPacket(const RakNet::Packet *packet);

	virtual int UpdateDisplay();

	bool sentBallThisFrame;

public: 

	egpClientApplicationState();
	virtual ~egpClientApplicationState();


	// overloaded callbacks
	virtual int OnIdle();
	virtual int OnKeyPress(unsigned char key);


	// set window state
	void SetWindowState(const egpWindowState *windowState);

	void SendTheBall(float position, int ID);
	void SendResetGame(int ID);
	void SendGameOver(int ID);
	
	void SendEmptyMessage(int ID);


};


#endif	// !__EGP_CLIENTAPPLICATIONSTATE_H_