/*
	egpNetPlaygroundGameState.cpp
	By Dan Buckstein

	Test game state implementation.

	Additional contributions: 
*/

#include "egpNet/states/egpNetPlaygroundGameState.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include "egpNet\events\MyEvents.h"
#include "../egpNetGameClient/include/egpNet/client/egpClientApplicationState.h"




egpNetPlaygroundGameState::egpNetPlaygroundGameState(int ownerID)
	: egpGameState(ownerID)
{
	mpEventManager = new EventManager();
	//clientState = nullptr;
	memset(m_data, 0, sizeof(m_data));


	// setup all object headers
	unsigned int i;
	for (i = 0; i < objLimit_agent; ++i)
	{
		m_data->m_agentStatus[i].objType = objID_agent;
		m_data->m_agentStatus[i].objID = i;
		m_data->m_agentStatus[i].ownerID = -1;
	}

	// agent at our index is owned by us
	// this may not always be the case, e.g. if one player has multiple agents
	m_data->m_agentStatus[ownerID].ownerID = ownerID;
	m_data->m_agentStatus[ownerID].flags = objFlag_active;

	for (i = 0; i < objLimit_ball; ++i)
	{
		m_data->m_balls[i].posX = 60;
		m_data->m_balls[i].posY = -450;
		m_data->m_balls[i].velY = -200;
	}
}

egpNetPlaygroundGameState::~egpNetPlaygroundGameState()
{

}


int egpNetPlaygroundGameState::SerializeData(char *buffer, const unsigned int bufferCapacity, const unsigned int serializeCount, const double dt) const
{
	if (!gameActive)
	{
		return -1;
	}
	char *const start = buffer;
	if (buffer)
	{
		// for each object with the changed flag raised, stuff the data into the buffer
		// advance the write head with each object written
		const NetPlaygroundAgent *agentPtr;
		const NetPlaygroundObjectStatus *agentStatusPtr;
		unsigned int i;
		for (i = 0, agentPtr = m_data->m_agent, agentStatusPtr = m_data->m_agentStatus;
			i < objLimit_agent;
			++i, ++agentPtr, ++agentStatusPtr)
		{
			if (agentStatusPtr->flags & objFlag_change)
			{
				// write object header and advance buffer
				*((NetPlaygroundObjectStatus *)buffer) = *agentStatusPtr;
				buffer += sizeof(NetPlaygroundObjectStatus);
				*((NetPlaygroundAgent *)buffer) = *agentPtr;
				buffer += sizeof(NetPlaygroundAgent);
			}
		}

		// write zero byte to indicate end
		*buffer = 0;
	}
	return (buffer - start);
}

int egpNetPlaygroundGameState::DeserializeData(const char *buffer, const unsigned int bufferCapacity, const unsigned int deserializeCount, const double dt)
{
	
	const char *const start = buffer;
	if (buffer)
	{
		const NetPlaygroundObjectStatus *objectStatusPtr;

		// unpack objects in buffer, look for newly activated objects
		NetPlaygroundAgent *agentPtr;
		NetPlaygroundObjectStatus *agentStatusPtr;
		while (*buffer)
		{
			// read next object info
			objectStatusPtr = (NetPlaygroundObjectStatus *)buffer;
			buffer += sizeof(NetPlaygroundObjectStatus);
			switch (objectStatusPtr->objType)
			{
				// agent
			case objID_agent:
				agentStatusPtr = m_data->m_agentStatus + objectStatusPtr->objID;
				agentPtr = m_data->m_agent + objectStatusPtr->objID;
				*agentStatusPtr = *objectStatusPtr;
				*agentPtr = *((NetPlaygroundAgent *)buffer);

				//agentPtr->posY = -200;

				buffer += sizeof(NetPlaygroundAgent);
				break;
			}
		}
	}
	return (buffer - start);
}

int egpNetPlaygroundGameState::ProcessInput(const egpKeyboard *keyboard, const egpMouse *mouse, const unsigned int ctrlID, const double dt)
{
	if (!gameActive)
	{
		return -1;
	}

	if (ctrlID >= 0)
	{
		NetPlaygroundObjectStatus *status = m_data->m_agentStatus + ctrlID;
		NetPlaygroundAgent *agentPtr = m_data->m_agent + ctrlID;
		const float agentMoveSpeed = 100.0f;

		status->ownerID = ctrlID;
		status->flags |= objFlag_active;

		if (keyboard)
		{
			agentPtr->velX = agentMoveSpeed * (float)(egpKeyboardKeyIsDown(keyboard, 'd') - egpKeyboardKeyIsDown(keyboard, 'a'));
			agentPtr->velY = 0;// agentMoveSpeed * (float)(egpKeyboardKeyIsDown(keyboard, 'w') - egpKeyboardKeyIsDown(keyboard, 's'));

			if (status->ownerID == 0)
			{
				cooldownBall--;
				if (egpKeyboardKeyIsDown(keyboard, 's'))
				{
					//if connected to server, call "spawn ball" event, using AddBall return value for ball ID
					
					if (cooldownBall <= 0)
					{
						if (!data.isLocal)
						{
							if (clientState == nullptr)
							{
								printf("darn");
							}
							//int tempBallID = AddBall(agentPtr->posX);
							int tempBallID = AddBall(agentXUpdated);
							sentBallID = tempBallID;
							printf("Sending ball at x position: %d\n", agentPtr->posX); //so this is coming out at 0 no matter where we are
						}
						else
						{
							//AddBall(agentPtr->posX);
							AddBall(agentXUpdated);
						}
						cooldownBall = 5;
					}
				}
			}
		//	updatedWhenNotMoving = false;
			// debug print
			//printf(" vel (%d) = %f, %f \n\n", ctrlID, agentPtr->velX, agentPtr->velY);
		}
		if (mouse)
		{

		}
	}
	return 0;
}

int egpNetPlaygroundGameState::UpdateState(double dt)
{
	// simple time-based animation
//	m_data->m_t += dt;
//	if (m_data->m_t >= 1.0)
//		m_data->m_t -= 1.0;
	if (!gameActive)
	{
		return -1;
	}

	if (sentBallID != -1)
	{
		//clientState->SendTheBall(m_data->m_agent[0].posX, sentBallID);
		clientState->SendTheBall(agentXUpdated, sentBallID);
		sentBallID = -1;
	}

	mpEventManager->ExecuteAll();

	// apply movement to all agents
	NetPlaygroundAgent *agentPtr, agentPrev[1];
	NetPlaygroundObjectStatus *agentStatusPtr;
	unsigned int i;
	for (i = 0, agentPtr = m_data->m_agent, agentStatusPtr = m_data->m_agentStatus; 
		i < objLimit_agent;
		++i, ++agentPtr, ++agentStatusPtr)
	{
		if (agentStatusPtr->flags & objFlag_active)
		{
			// copy to compare new state with previous state
			*agentPrev = *agentPtr;

			// update agent
			agentPtr->posX += (float)dt * agentPtr->velX;
			agentXUpdated += (float)dt * agentPtr->velX;
			printf("Pos x: %d\n", agentPtr->posX); //when the agent stops moving, his position is reset to zero somehow no matter where he is
			//agentPtr->velY = 0; //For some reason if we don't do this, we can't move left or right at all
			agentPtr->posY += (float)dt * agentPtr->velY;

//			printf("Local update\n");

			// check for changes, raise flag if changes occur
			//if (memcmp(agentPrev, agentPtr, sizeof(NetPlaygroundAgent)) || !updatedWhenNotMoving)
			//{
				agentStatusPtr->flags |= objFlag_change;
				//updatedWhenNotMoving = true;
			//}
			// lower changed flags so objects don't automatically get serialized next update
			//else
			//{
				//agentStatusPtr->flags &= ~(objFlag_change);
			//}
		}
	}

	NetPlaygroundAgent *ballPtr;
	//MOVE BALL
	for (i = 0, ballPtr = m_data->m_balls;
		i < objLimit_ball;
		++i, ++ballPtr)
	{
			// update bal
			ballPtr->posY += (float)dt * ballPtr->velY;
			
			//if collision (also, if server->send event to clientel)
			//agent 1 is the dodgerr!!!
			agentPtr = m_data->m_agent + 1;
			agentStatusPtr = m_data->m_agentStatus + 1;
			if (agentStatusPtr->flags & objFlag_active) //not alone single playe
			{
				float xDiff = (agentPtr->posX - ballPtr->posX) * (agentPtr->posX - ballPtr->posX);
				float yDiff = (agentPtr->posY - ballPtr->posY) * (agentPtr->posY - ballPtr->posY);
				float distance = sqrt(xDiff + yDiff);

				bool collisionHappened = false;

				//ball hit the player-- send a collision event and check if the top player wins
				if (distance < 10)
				{
					if (!collisionHappened)
					{
						collisionHappened = true;
						ballPtr->posY = -450;
						AddScoreEvent *addScore = new AddScoreEvent(&mScore, 1);
						mpEventManager->AddEvent(addScore);

						if (mScore >= 9)
						{
							EndGameEvent *endGame = new EndGameEvent(this);
							mpEventManager->AddEvent(endGame);
						}
					}
				}

			}
	}

	return 0;
}

//better name is ActivateAgentAtID(int)
//also it's made to move ID 1 down
void egpNetPlaygroundGameState::AddAgent(int ID, bool isLocal)
{
	if (isLocal)
		m_data->m_agentStatus[ID].flags = objFlag_active;
	m_data->m_agent[ID].posX = 0;
	m_data->m_agent[ID].posY = -200;
	//m_data->m_agent->posX = 0;
	//m_data->m_agent->posY = -200;
}

int egpNetPlaygroundGameState::AddBall(float posX, int id)
{
	int ballID = id;

	int yLimit = -450;

	unsigned int i = 0;
	bool ballIsValid = false;
	if (id == -1)
	{
		for (i = 0; i < objLimit_ball; ++i)
		{
			if (m_data->m_balls[i].posY < yLimit)
			{		
				ballIsValid = true;
				ballID = i;
				break;
			}
		}
	}
	else
	{
		ballIsValid = true;
	}

	if (ballIsValid)
	{
		m_data->m_balls[i].posX = posX;
		m_data->m_balls[i].posY = 0;
	}

	return ballID;
}

void egpNetPlaygroundGameState::SetAppState(egpApplicationState *cState)
{
	clientState = cState;
}

//void egpNetPlaygroundGameState::SetAppState(egpClientApplicationState * cState)
//{
//	clientState = cState;
//}
