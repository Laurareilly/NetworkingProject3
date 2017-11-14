/*
	egpNetPlaygroundGameState.h
	By Dan Buckstein

	Game state for testing networked game objects.

	Additional contributions: 
*/

#ifndef __EGP_NETPLAYGROUNDGAMESTATE_H_
#define __EGP_NETPLAYGROUNDGAMESTATE_H_

#include "egpNet/egpGameState.h"


class egpNetPlaygroundGameState : public egpGameState
{
	// data identifiers
	enum ObjectIdentifier
	{
		// end of stream
		objID_null,

		// object identifiers
		objID_agent,
	};

	// serialization and deserialization algorithms
	// general rule: 
	//	- object header/status, object data, header, data, ...

protected:

	enum NetPlaygroundObjectLimits
	{
		objLimit_agent = 8,
	};

	enum NetPlaygroundObjectFlags
	{
		objFlag_none,
		objFlag_active,
		objFlag_change,
	};

	// general object descriptor: 
	//	- type of object
	//	- object ID
	//	- owner ID
	//	- flags
	struct NetPlaygroundObjectStatus
	{
		char objType, objID, ownerID, flags;
	};

	struct NetPlaygroundAgent
	{
		// position
		float posX, posY;

		// what else might help here?
		float velX, velY;
	};

	struct NetPlaygroundData
	{
		// test data
	//	double m_t;

		NetPlaygroundAgent m_agent[objLimit_agent];
		NetPlaygroundObjectStatus m_agentStatus[objLimit_agent];

	} m_data[1];

public: 

	egpNetPlaygroundGameState(int ownerID);
	virtual ~egpNetPlaygroundGameState();

	virtual int SerializeData(char *buffer, const unsigned int bufferCapacity, const unsigned int serializeCount, const double dt) const;
	virtual int DeserializeData(const char *buffer, const unsigned int bufferCapacity, const unsigned int deserializeCount, const double dt);
	virtual int ProcessInput(const egpKeyboard *keyboard, const egpMouse *mouse, const unsigned int ctrlID, const double dt);
	virtual int UpdateState(double dt);

};


#endif	// !__EGP_NETPLAYGROUNDGAMESTATE_H_