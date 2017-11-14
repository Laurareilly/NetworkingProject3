/*
	egpMasterServerApplicationState.cpp
	By Dan Buckstein

	Implementation for master server.

	Additional contributions:
*/

#include "egpNet/server/egpMasterServerApplicationState.h"

#include <stdio.h>


// internal updates
int egpMasterServerApplicationState::UpdateNetworking()
{
	unsigned int i = egpApplicationState::UpdateNetworking();

	// done
	return i;
}

int egpMasterServerApplicationState::ProcessPacket(const RakNet::Packet *packet)
{
	int i = egpApplicationState::ProcessPacket(packet);

	// additional processing opportunity
	if (mp_peer && packet)
	{
		// peek at first byte, process this peer's cases
		// return 1 if case was processed
		switch (packet->data[0])
		{
			// handle disconnection
		case ID_DISCONNECTION_NOTIFICATION: 
		case ID_CONNECTION_LOST: 
		{
			// check if it was a server or a client that got removed
			// got index from superclass processing
			// if disconnected was server, lower flag
			if (mp_isServer[i])
			{
				mp_isServer[i] = 0;
				--m_numServers;
			}
		} break;


			// ******TO-DO: 
			// server connected, send connection index
		case egpID_serverFlag: {
			i = GetConnectionIndex(packet->systemAddress);
			mp_isServer[i] = 1;
			++m_numServers;

			printf("\n New server joined: #%d %s", i, packet->systemAddress.ToString(false));

			char msg[5] = { (char)egpID_connectionIndex };
			*((int*)(msg + 1));
			SendPacket(msg, sizeof(msg), i, 0, 1); //make it reliable to make sure so and so gets their id

		} break;


			// ******TO-DO: 
			// client connected, compile and send server list
		case egpID_clientFlag: 
		{
			//step 1. put together the list (stack-allocated list of IP addresses)
			const unsigned int numAddresses = 256;
			const unsigned int addressSize = 16;
			char addresses[numAddresses][addressSize];

			unsigned int c, count;
			for (c = count = 0; c < m_maxIncomingConnections && count < m_numServers; ++c)
			{
				//if server found, copy address into list
				if (mp_isServer[c])
				{
					//copy the string into the list
					strncpy(addresses[count], mp_connection[c].m_address.ToString(false), addressSize);
					addresses[count][addressSize - 1] = 0; //make sure theres a null terminator
					++count;
				}
			}

			//send the message-- the size will be enough to hold
			//msg
			// -> ID
			// -> ID
			// -> Addresses
			char msg[sizeof(char) + sizeof(int) + sizeof(addresses)] = { (char)egpID_serverList },
			*msgPtr = msg + 1;

			*((int *)msgPtr) = count;
			msgPtr += sizeof(int);

			if (count)
			{
				memcpy(msgPtr, addresses, count * addressSize);
				msgPtr += count * addressSize;
				//msgPtr = (char*)memcpy(msgPtr, addresses, count * addressSize) + count * addressSize; //dan likes this because its fewer lines of code
			}

			i = GetConnectionIndex(packet->systemAddress);
			SendPacket(msg, (int)(msgPtr - msg), i, 0, 1);

		} break;


	//	default:
	//		printf("Message with identifier %i has arrived.\n", packet->data[0]);
	//		break;
		}
	}

	return i;
}



egpMasterServerApplicationState::egpMasterServerApplicationState(const unsigned int maxIncomingConnections)
	: egpApplicationState()
{
	// no state required
	mp_state = 0;

	// start networking and connect to master server
	if (StartupNetworking(maxIncomingConnections, 0, GetDefaultPort()))
	{
		// allocate server flag list
		mp_isServer = new int[maxIncomingConnections];
		memset(mp_isServer, 0, sizeof(int)*maxIncomingConnections);
	}
	m_numServers = 0;
}

egpMasterServerApplicationState::~egpMasterServerApplicationState()
{
	// stop networking
	ShutdownNetworking();

	// delete server connection flags
	delete[] mp_isServer;
}


int egpMasterServerApplicationState::OnIdle()
{
	unsigned int i = egpApplicationState::OnIdle();

	return i;
}
