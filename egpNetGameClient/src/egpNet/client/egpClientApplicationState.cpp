/*
	egpClientApplicationState.cpp
	By Dan Buckstein

	Implementation for client application development.

	Additional contributions:
*/

#include "egpNet/client/egpClientApplicationState.h"

#include <stdio.h>
#include <Windows.h>

#include "egpNet/states/egpNetPlaygroundGameStateDrawable.h"

#include "egpNet/client/egpWindowState.h"
#include "egpNet/events/MyEvents.h"


// internal updates
int egpClientApplicationState::UpdateNetworking()
{
	if (mp_state != nullptr && mp_state->data.isLocal) return -1;

	unsigned int i = egpApplicationState::UpdateNetworking();

	// done
	return i;
}

int egpClientApplicationState::ProcessPacket(const RakNet::Packet *packet)
{
	if (mp_state != nullptr && mp_state->data.isLocal) return -1; //just in case

	egpApplicationState::ProcessPacket(packet);

	// additional processing
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
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				int otherID = *((int *)userData);
				userData += sizeof(otherID);
				//printf(" Other (%d) -> Server = %I64d;  Server -> Local (%d) = %I64d \n\n", otherID, sentToReadDiff_remote, m_myConnectionIndex, sentToReadDiff_local);
			}	break;

			case egpID_sendBall:
			{
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				int otherID = *((int *)userData);

				float tempX = *((float *)userData);
				userData += sizeof(float);
				int tempID = *((int *)userData);
				userData += sizeof(int);

				if (tempID < 0 || tempID > 2)
				{
					return 0; //DEBUG, why would we get a brrroken id???
				}

				/*SendBall *sendBall = (SendBall*)userData;
				float tempX = sendBall->posX;
				int tempID = sendBall->ballID;*/
				dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->AddBall(tempX, tempID);
			}
			break;

			case egpID_stateUpdate: {
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				int otherID = *((int *)userData);
				userData += sizeof(otherID);

		//		printf("Networked update\n");

				// ****TO-DO: 
				// state decodes data
				if (mp_state)
				{
					const double delay_s = (double)(sentToReadDiff_local + sentToReadDiff_remote) * 0.001;
					userData += mp_state->DeserializeData((char*)userData, 4096, 0, delay_s);
				}
			}	break;

			case egpID_serverGetBallRequest:
			{
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				int otherID = *((int *)userData);

				float tempX = *((float *)userData);
				userData += sizeof(float);
				int tempID = *((int *)userData);
				userData += sizeof(int);

				if (tempID < 0 || tempID > 2)
				{
					return 0; //DEBUG, why would we get a broken id???
				}

				/*SendBall *sendBall = (SendBall*)userData;
				float tempX = sendBall->posX;
				int tempID = sendBall->ballID;*/
				dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->AddBall(tempX, tempID);
			}
			break;

			case egpID_gameOver:
			{
				printf("game over");
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				/*int otherID = *((int *)userData);

				int tempID = *((int *)userData);
				userData += sizeof(int);*/

				//if (tempID < 0 || tempID > 2)
				//{
				//	return 0; //DEBUG, why would we get a SHITE id???
				//}

				EndGameEvent *endGame = new EndGameEvent(dynamic_cast<egpNetPlaygroundGameState*>(mp_state));
				dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->mpEventManager->AddEvent(endGame);
			}
			break;

			case egpID_resetGame:
			{
				printf("reset game");
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				//int otherID = *((int *)userData);

				//int tempID = *((int *)userData);
				//userData += sizeof(int);

				//if (tempID < 0 || tempID > 2)
				//{
				//	return 0; //DEBUG, why would we get a SHITE id???
				//}

				dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->ResetGame();
			}
			break;

			case 0:
				printf("No data. \n\n");
				break;
			}

		}	return 1;


			// create state after receiving connection index
		case egpID_connectionIndex:
		{
			// testing: create game state
			// normally this would be handled by a manager
			mp_state = new egpNetPlaygroundGameStateDrawable(m_myConnectionIndex);
			dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->AddAgent(1, false);
			dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->SetAppState(this);


			return 1;
		}
			break;

		//case egpID_sendBall:
		//{
		//	SendBall *sendBall = (SendBall*)packet->data;
		//	float tempX = sendBall->posX;
		//	int tempID = sendBall->ballID;
		//	dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->AddBall(tempX, tempID);
		//}
		//break;

			// destroy state if we get disconnected
		case ID_DISCONNECTION_NOTIFICATION: 
		case ID_CONNECTION_LOST:
			delete mp_state;
			mp_state = 0;
			return 1;


	//	default:
	//		printf("Message with identifier %i has arrived.\n", packet->data[0]);
	//		break;
		}
	}

	return 0;
}


int egpClientApplicationState::UpdateDisplay()
{
	if (mpk_windowState)
	{
		// tell game state to draw
		if (mp_state)
			mp_state->RenderState(mpk_windowState->width, mpk_windowState->height, mpk_windowState->posX, mpk_windowState->posY);
		else
			egpNetPlaygroundGameStateDrawable::ClearBuffers();

		// swap draw buffers
		SwapBuffers(wglGetCurrentDC());

		// done
		return 1;
	}
	return 0;
}



egpClientApplicationState::egpClientApplicationState()
	: egpApplicationState()
	, mpk_windowState(0)
{
	// set timers
	egpTimerSet(m_updateRenderTimer, 30.0);

	// start timers
	egpTimerStart(m_updateRenderTimer);
}

egpClientApplicationState::~egpClientApplicationState()
{

}


// callbacks
int egpClientApplicationState::OnIdle()
{
	// CLIENT IDLE: 
	//	-> process inputs
	//		-> devices
	//		-> remote state
	//		-> local state
	//	-> rendering (client probably has a render window, server probably does not)
	//	-> parent update where appropriate
	if (egpTimerUpdate(m_updateRenderTimer))
	{
		if (mp_state)
		{
			// ****TO-DO: send input to server
			if(!mp_state->data.isLocal) SendStateInput(0, m_maxIncomingConnections, 0, 0);

			// ****TO-DO: process input locally
			//tell the state to process input
			mp_state->ProcessInput(m_keyboard, m_mouse, m_myConnectionIndex, 0.0);

			// ****TO-DO: update state
			sentBallThisFrame = false;
			mp_state->UpdateState(m_updateRenderTimer->secondsPerTick); 
		}

		// display state
		UpdateDisplay();
	}

	// parent update
	egpApplicationState::OnIdle();

	// nothing happened
	return 0;
}

int egpClientApplicationState::OnKeyPress(unsigned char key)
{
	// process keys
	switch (key)
	{
	case 'C':
		if (mp_state == nullptr)
		{
			if (StartupNetworking(0, 1, GetDefaultPort() + 1))
			{
				char address[16] = { 0 };
				printf("\n Enter the host/server IP address using this 15-character format: ");
				printf("\n  ###.###.###.### \n    -> ");
				fscanf(stdin, "%s", address);
				address[sizeof(address) - 1] = 0;
				ConnectPeer(address, GetDefaultPort());
			}
		}
		break;
	case 'X':
		if (ShutdownNetworking())
		{
			printf("\n Shut down networking.");

			// testing: delete game state
			delete mp_state;
			mp_state = 0;
		}
		break;


		// send test time-stamped update message if connected to server
	case 't':
		if (mp_peer && m_myConnectionIndex >= 0)
		{
			// time-stamped message from client: 
			//	-> time stamp message ID
			//	-> time stamp x 2
			//	-> update message ID
			//	-> time diff x 2 (sends as zero because we are the originator)
			//	-> connection index (unique user ID)
			//	-> user data
			const RakNet::Time packetTime_local = RakNet::GetTime();
			char msg[64], *msgPtrTmp = msg + WriteTimeStamp(msg, packetTime_local, packetTime_local),
				*msgPtr = msgPtrTmp + WriteTimeStamp(msgPtrTmp, 0, 0);
			*msgPtrTmp = (char)egpID_currentTime;
			*((int *)msgPtr) = m_myConnectionIndex;
			msgPtr += sizeof(m_myConnectionIndex);

			SendPacket(msg, (int)(msgPtr - msg), m_maxIncomingConnections, 0, 0);
			printf(" Sent update message at time %I64d \n\n", packetTime_local);
		}
		break;
	case 'l':
	case 'L':
		if (mp_state == nullptr)
		{
			printf("You chose: Local!\n");
			mp_state = new egpNetPlaygroundGameStateDrawable(0); //no connection index because this is local //THIS USED TO BE DRAWABLE INCAS THIS DOESNT WORK
			m_myConnectionIndex = 0;
			mp_state->data.isLocal = true;
			dynamic_cast<egpNetPlaygroundGameState*>(mp_state)->AddAgent(1, true);
		}
		break;
	}

	// base update
	egpApplicationState::OnKeyPress(key);

	return 0;
}


// setters
void egpClientApplicationState::SetWindowState(const egpWindowState *windowState)
{
	if (windowState->userData == this)
		mpk_windowState = windowState;
}

//WE WILL SEND A REQUEST TO SPAWN BALL OF ID "ID". THE SERVER WILL USE ITS ESTIMATION OF OUR POSITION TO SPAWN THE BALL AT THAT ID NUMBER ON ALL CLIENTS.
void egpClientApplicationState::SendTheBall(float position, int ID)
{
	if (!sentBallThisFrame)
	{
		if (ID == -1) return;

		//GameOver sendBallLol[1] = { egpID_sendBall };
		//SendPacket((char*)sendBallLol, sizeof(sendBallLol), -1, 1, 1);
		const RakNet::Time packetTime_local = RakNet::GetTime();
		char msg[64], *msgPtrTmp = msg + WriteTimeStamp(msg, packetTime_local, packetTime_local),
			*msgPtr = msgPtrTmp + WriteTimeStamp(msgPtrTmp, 0, 0);
		*msgPtrTmp = (char)egpID_sendBall;
		*((int *)msgPtr) = ID;
		msgPtr += sizeof(ID);

		SendPacket(msg, (int)(msgPtr - msg), m_maxIncomingConnections, 0, 0);
		//printf(" Sent ball message at time %I64d \n\n", packetTime_local);

		//SendBall sendBall[1] = { egpID_serverGetBallRequest };
		//sendBall->ballID = ID;
		//sendBall->posX = position;
		//int didItSend = SendPacket((char*)sendBall, sizeof(sendBall), 1, 0, 0);

		sentBallThisFrame = true;
	}
}

void egpClientApplicationState::SendResetGame(int ID)
{
	const RakNet::Time packetTime_local = RakNet::GetTime();
	char msg[64], *msgPtrTmp = msg + WriteTimeStamp(msg, packetTime_local, packetTime_local),
	*msgPtr = msgPtrTmp + WriteTimeStamp(msgPtrTmp, 0, 0);
	*msgPtrTmp = (char)egpID_resetGame;
	*((int *)msgPtr) = ID;
	msgPtr += sizeof(ID);

	SendPacket(msg, (int)(msgPtr - msg), m_maxIncomingConnections, 0, 0);
}

void egpClientApplicationState::SendGameOver(int ID)
{
	const RakNet::Time packetTime_local = RakNet::GetTime();
	char msg[64], *msgPtrTmp = msg + WriteTimeStamp(msg, packetTime_local, packetTime_local),
	*msgPtr = msgPtrTmp + WriteTimeStamp(msgPtrTmp, 0, 0);
	*msgPtrTmp = (char)egpID_gameOver;
	*((int *)msgPtr) = ID;
	msgPtr += sizeof(ID);

	SendPacket(msg, (int)(msgPtr - msg), m_maxIncomingConnections, 0, 0);
}

void egpClientApplicationState::SendEmptyMessage(int ID)
{
}
