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


egpNetPlaygroundGameState::egpNetPlaygroundGameState(int ownerID)
	: egpGameState(ownerID)
{
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
}

egpNetPlaygroundGameState::~egpNetPlaygroundGameState()
{

}


int egpNetPlaygroundGameState::SerializeData(char *buffer, const unsigned int bufferCapacity, const unsigned int serializeCount, const double dt) const
{
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
				buffer += sizeof(NetPlaygroundAgent);
				break;
			}
		}
	}
	return (buffer - start);
}

int egpNetPlaygroundGameState::ProcessInput(const egpKeyboard *keyboard, const egpMouse *mouse, const unsigned int ctrlID, const double dt)
{
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
			agentPtr->velY = agentMoveSpeed * (float)(egpKeyboardKeyIsDown(keyboard, 'w') - egpKeyboardKeyIsDown(keyboard, 's'));
		//	updatedWhenNotMoving = false;
			// debug print
			printf(" vel (%d) = %f, %f \n\n", ctrlID, agentPtr->velX, agentPtr->velY);
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

	return 0;
}
