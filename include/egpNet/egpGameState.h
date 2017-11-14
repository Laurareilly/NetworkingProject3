/*
	egpGameState.h
	By Dan Buckstein

	Base interface for networked game state. Inherit and build other states 
	to form a finite state machine with transitions.
	NOTE: GAME STATE INTERFACES SHOULD NOT DIFFER WHETHER YOU'RE A SERVER OR 
	CLIENT; IT'S YOUR APPLICATION'S REPRESENTATION OF THE GAME "RIGHT NOW".

	Additional contributions: 
*/

#ifndef __EGP_GAMESTATE_H_
#define __EGP_GAMESTATE_H_


#include "utils/egpInput.h"


class egpGameState abstract
{
	int m_ownerID;
protected: 

	egpGameState(int ownerID);

	int GetOwnerID() const;

public: 

	// virtual dtor
	virtual ~egpGameState();
	
	// primary tasks: 
	//	- serialize internal data to be processed externally
	//	- deserialize external data to be processed internally
	//	- process input devices
	//	- update state given time step
	//	- draw state

	virtual int SerializeData(char *buffer, const unsigned int bufferCapacity, const unsigned int serializeCount, const double dt) const;
	virtual int DeserializeData(const char *buffer, const unsigned int bufferCapacity, const unsigned int deserializeCount, const double dt);
	virtual int ProcessInput(const egpKeyboard *keyboard, const egpMouse *mouse, const unsigned int ctrlID, const double dt);
	virtual int UpdateState(double dt);
	virtual int RenderState(int canvasWidth, int canvasHeight, int canvasPosX, int canvasPosY) const;

};


#endif	// !__EGP_GAMESTATE_H_