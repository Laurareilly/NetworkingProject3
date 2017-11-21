/*
	egpNetPlaygroundGameState.h
	By Dan Buckstein

	Game state for testing networked game objects.

	Additional contributions: 
*/

#ifndef __EGP_NETPLAYGROUNDGAMESTATE_H_
#define __EGP_NETPLAYGROUNDGAMESTATE_H_

#include "egpNet/egpGameState.h"
#include "egpNet\events\Event.h"
#include "egpNet\egpApplicationState.h"

class egpClientApplicationState;


class egpNetPlaygroundGameState : public egpGameState
{
	// data identifiers
	enum ObjectIdentifier
	{
		// end of stream
		objID_null,

		// object identifiers
		objID_agent,

		objID_BALL
	};

	// serialization and deserialization algorithms
	// general rule: 
	//	- object header/status, object data, header, data, ...

	EventManager *mpEventManager;

protected:

	int sentBallID = -1;

	bool updatedWhenNotMoving = false;

	enum NetPlaygroundObjectLimits
	{
		objLimit_agent = 8,
		objLimit_ball = 3
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

	struct NetPlaygroundAgent //repurposing this to also account for the b all
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
		NetPlaygroundAgent m_balls[objLimit_ball];

	} m_data[1];

	int cooldownBall = 0;

public: 

	bool gameActive = true;
	egpNetPlaygroundGameState(int ownerID);
	virtual ~egpNetPlaygroundGameState();

	virtual int SerializeData(char *buffer, const unsigned int bufferCapacity, const unsigned int serializeCount, const double dt) const;
	virtual int DeserializeData(const char *buffer, const unsigned int bufferCapacity, const unsigned int deserializeCount, const double dt);
	virtual int ProcessInput(const egpKeyboard *keyboard, const egpMouse *mouse, const unsigned int ctrlID, const double dt);
	virtual int UpdateState(double dt);

	virtual int OnArriveFromPrevious(egpGameState *state) { return 0; };
	virtual int OnGoToNextState(egpGameState *state) { return 0; };

	void AddAgent(int ID, bool isLocal);
	int AddBall(float posX, int id = -1);

	int mScore = 0;
	float agentXUpdated = 0;

	float GetAgentPosition(int ID) { return m_data->m_agent[ID].posX; }

	egpApplicationState* clientState;
	void SetAppState(egpApplicationState * cState);
};


#endif	// !__EGP_NETPLAYGROUNDGAMESTATE_H_