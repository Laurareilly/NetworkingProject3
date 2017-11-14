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


// internal updates
int egpClientApplicationState::UpdateNetworking()
{
	unsigned int i = egpApplicationState::UpdateNetworking();

	// done
	return i;
}

int egpClientApplicationState::ProcessPacket(const RakNet::Packet *packet)
{
	int i = egpApplicationState::ProcessPacket(packet);

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
				printf(" Other (%d) -> Server = %I64d;  Server -> Local (%d) = %I64d \n\n", otherID, sentToReadDiff_remote, m_myConnectionIndex, sentToReadDiff_local);
			}	break;

			case egpID_stateUpdate: {
				RakNet::Time sentToReadDiff_remote, sentToReadDiff_other;
				userData += ReadTimeStamp((char *)userData, sentToReadDiff_remote, sentToReadDiff_other);
				int otherID = *((int *)userData);
				userData += sizeof(otherID);

				// ****TO-DO: 
				// state decodes data
				if (mp_state)
				{
					const double delay_s = (double)(sentToReadDiff_local + sentToReadDiff_remote) * 0.001;
					userData += mp_state->DeserializeData((char*)userData, 4096, 0, delay_s);
				}
			} break;

			case 0:
				printf("No data. \n\n");
				break;
			}

		} break;


			// ******TO-DO: 
			// connected to server, tell it we're a client
			//	and see what we get back
		case ID_CONNECTION_REQUEST_ACCEPTED: {
			char msg[1] = { (char)egpID_clientFlag };

			SendPacket(msg, sizeof(msg), m_maxIncomingConnections, 0, 1);

		} break;


			// create state after receiving connection index
		case egpID_connectionIndex:
			// testing: create game state
			// normally this would be handled by a manager
			mp_state = new egpNetPlaygroundGameStateDrawable(m_myConnectionIndex);
			break;


			// ******TO-DO: 
			// server list update
			// should be able to ask for this repeatedly 
			//	by sending the 'client flag' message
		case egpID_serverList: {
			const unsigned int count = *((int *)(packet->data + 1));
			printf("\n Server list: ");
			if (count > 0)
			{
				const unsigned int addressSize = 16;
				const unsigned char *address = packet->data + sizeof(char) + sizeof(int);
				unsigned int c;
				for (c = 0; c < count; ++c, address += addressSize)
				{
					printf("\n   %s", address);
				}
			}

		} break;


			// destroy state if we get disconnected
		case ID_DISCONNECTION_NOTIFICATION: 
		case ID_CONNECTION_LOST:
			delete mp_state;
			mp_state = 0;
			break;


	//	default:
	//		printf("Message with identifier %i has arrived.\n", packet->data[0]);
	//		break;
		}
	}

	return i;
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

	// start out not connected to game
	m_connectedGameServer = 0;
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
			SendStateInput(0, m_maxIncomingConnections, 0, 0);

			// ****TO-DO: process input locally
			//tell the state to process input
			mp_state->ProcessInput(m_keyboard, m_mouse, m_myConnectionIndex, 0.0);

			// ****TO-DO: update state
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
		if (StartupNetworking(0, 1, GetDefaultPort() + 2))
		{
			bool server = true;
			char address[16] = { 0 };
			printf("\n Enter the master or game server IP address using this 15-character format: ");
			printf("\n  ###.###.###.### \n    -> ");
			fscanf(stdin, "%s", address);
			address[sizeof(address) - 1] = 0;

			//printf("\n Type 'M' or 'm' if target IP is a master server: ");
			//if (toupper(getc(stdin)) == 'M')
			//	server = false;

			// connect
			//ConnectPeer(address, GetDefaultPort() + server ? 1 : 0);
			ConnectPeer(address, GetDefaultPort());
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
