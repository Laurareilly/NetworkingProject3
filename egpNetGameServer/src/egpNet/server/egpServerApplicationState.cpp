/*
	egpServerApplicationState.cpp
	By Dan Buckstein

	Implementation for server application development.

	Additional contributions:
*/

#include "egpNet/server/egpServerApplicationState.h"

#include <stdio.h>


#include "egpNet/states/egpNetPlaygroundGameState.h"


// internal updates
int egpServerApplicationState::UpdateNetworking()
{
	unsigned int i = egpApplicationState::UpdateNetworking();

	// done
	return i;
}

int egpServerApplicationState::ProcessPacket(const RakNet::Packet *packet)
{
	egpApplicationState::ProcessPacket(packet);

	// additional processing opportunity
	if (mp_peer && packet)
	{
		// peek at first byte, process this peer's cases
		// return 1 if case was processed
		switch (packet->data[0])
		{
			// handle time-stamped messages
		case ID_TIMESTAMP: {
			RakNet::Time readTime_local = RakNet::GetTime(), sentTime_local, sentTime_remote, sentToReadDiff_local;
			const unsigned char *userData = packet->data + ReadTimeStamp((char *)packet->data, sentTime_local, sentTime_remote);
			const int lastPing = mp_peer->GetLastPing(packet->systemAddress);
			sentToReadDiff_local = (readTime_local - sentTime_local);

		//	printf(" Read time (local) = %I64d (last ping = %d) \n", readTime_local, lastPing);
		//	printf(" Sent time (local) = %I64d;  Sent time (remote) = %I64d \n", sentTime_local, sentTime_remote);
		//	printf(" Sent -> Read diff = %I64d;  Clock diff = %I64d \n\n", sentToReadDiff_local, (sentTime_local - sentTime_remote));

			switch (userData[0])
			{
			case egpID_currentTime: {
				// process data from client
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				int otherID = *((int *)userData);
				userData += sizeof(otherID);
				printf(" Other (%d) -> Server = %I64d;  Server -> Local (%d) = %I64d \n\n", otherID, sentToReadDiff_remote, m_myConnectionIndex, sentToReadDiff_local);

				// pack data and send to other clients
				RakNet::Time packetTime_local = readTime_local;
				char msg[64], *msgPtrTmp = msg + WriteTimeStamp(msg, packetTime_local, packetTime_local),
					*msgPtr = msgPtrTmp + WriteTimeStamp(msgPtrTmp, sentToReadDiff_local, sentToReadDiff_remote);
				*msgPtrTmp = (char)egpID_currentTime;
				*((int *)msgPtr) = otherID;
				msgPtr += sizeof(otherID);

				SendPacket(msg, (int)(msgPtr - msg), otherID, 1, 0);
				printf(" Broadcast update message from (%d) at time %I64d \n\n", otherID, packetTime_local);
			}	break;

			case egpID_stateInput: {
				// receive input from other, process input as if it were input into our state
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				int otherID = *((int *)userData);
				userData += sizeof(otherID);

				const egpKeyboard *keyboard = (egpKeyboard *)userData;
				userData += sizeof(egpKeyboard);
				const egpMouse *mouse = (egpMouse *)userData;
				userData += sizeof(egpMouse);

				// ****TO-DO: 
				// process input
				if (mp_state)
				{
					const double delay_s = (double)(sentToReadDiff_local + sentToReadDiff_remote) * 0.001;
					mp_state->ProcessInput(keyboard, mouse, otherID, delay_s);
				}
			} break;

			case egpID_stateUpdate: {
				// forward to others
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				int otherID = *((int *)userData);
				userData += sizeof(otherID);

				// ****TO-DO: 
				// state decodes data
				if (mp_state)
				{
					const double delay_s = (double)(sentToReadDiff_local + sentToReadDiff_remote) * 0.001;
					mp_state->DeserializeData((char*)userData, 4096, 0, delay_s);
				}
			} break;

			case egpID_sendBall:
			{
				if (mp_state)
				{
					float tempX = dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->GetAgentPosition(0);
					int ball = dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->AddBall(tempX);
					SendBall sendBall[1] = { egpID_sendBall, tempX, ball };

					SendPacket((char*)sendBall, sizeof(sendBall), -1, 0, 1);

				}
			}
				break;

			case 0:
				printf("No data. \n\n");
				break;
			}

		}	return 1;

		case ID_NEW_INCOMING_CONNECTION: {
			// additional processing for accepted request: 
			// send connection index of new participant
			int i = GetConnectionIndex(packet->systemAddress);
			if (i >= 0)
			{
				char msg[8] = { 0 }, *msgPtr = msg;
				*(msgPtr++) = (char)egpID_connectionIndex;
				*((int *)msgPtr) = i;
				msgPtr += sizeof(i);
				SendPacket(msg, (int)(msgPtr - msg), i, 0, 1);
			}
		}	return 1;


	//	default:
	//		printf("Message with identifier %i has arrived.\n", packet->data[0]);
	//		break;
		}
	}

	return 0;
}



egpServerApplicationState::egpServerApplicationState()
	: egpApplicationState()
{
	// set timer
	egpTimerSet(m_updateTimer, 10.0);

	// start timer
	egpTimerStart(m_updateTimer);


	// testing: create game state
	// normally this would be handled by a manager
	mp_state = new egpNetPlaygroundGameState(-1);
	dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->clientState = this;
}

egpServerApplicationState::~egpServerApplicationState()
{

}


int egpServerApplicationState::OnIdle()
{
	if (egpTimerUpdate(m_updateTimer))
	{
		if (mp_state)
		{
			// ****TO-DO: simulate state given all inputs
			mp_state->UpdateState(m_updateTimer->secondsPerTick);
			// ****TO-DO: send complete set of updated data
			SendStateUpdate(0, -1, 1, 0);
		}
	}

	// update base
	egpApplicationState::OnIdle();

	return 0;
}
