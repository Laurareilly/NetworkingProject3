/*
LobbyState.h

By Laura Reilly & Robby Mitchell

Lobby State that allows the user to choose a local/networked game or exit

Additional contributions:
*/

#ifndef __LOBBYSTATE_H_
#define __LOBBYSTATE_H

#include "egpNet/egpGameState.h"


class LobbyState : public egpGameState
{
protected:
public:

	LobbyState(int ownerID);
	virtual ~LobbyState();

	virtual int SerializeData(char *buffer, const unsigned int bufferCapacity, const unsigned int serializeCount, const double dt) const;
	virtual int DeserializeData(const char *buffer, const unsigned int bufferCapacity, const unsigned int deserializeCount, const double dt);
	virtual int ProcessInput(const egpKeyboard *keyboard, const egpMouse *mouse, const unsigned int ctrlID, const double dt);
	virtual int UpdateState(double dt);


	virtual int OnArriveFromPrevious(egpGameState *state);
	virtual int OnGoToNextState(egpGameState *state);
};


#endif