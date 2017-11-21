/*
	egpApplicationState.h
	By Dan Buckstein

	Base interface for networked application development.
	NOTE: THIS IS NOT A GAME STATE; IT IS HIGHER LEVEL.

	Additional contributions: 
*/

#ifndef __EGP_APPLICATIONSTATE_H_
#define __EGP_APPLICATIONSTATE_H_


#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/GetTime.h"

#include "utils/egpInput.h"
#include "utils/egpTimer.h"
#include "utils/egpThread.h"

#include "egpGameState.h"


class egpApplicationState abstract
{
protected:

	egpApplicationState();
	virtual ~egpApplicationState();

	// common input device tracking
	egpKeyboard m_keyboard[1];
	egpMouse m_mouse[1];


	// RakNet peer
	RakNet::RakPeerInterface *mp_peer;

	// connections
	struct ms_ConnectionDescriptor
	{
		RakNet::SystemAddress m_address;
		int m_active;
	} *mp_connection;
	unsigned int m_maxIncomingConnections, m_numIncomingConnections, m_maxOutgoingConnections, m_numOutgoingConnections;
	unsigned short m_portIncoming, m_portOutgoing;
	int m_myConnectionIndex;

	// find connection descriptor index
	int GetConnectionIndex(const RakNet::SystemAddress &address);
	int GetConnectionIndexOpen();


	// internal updates
	virtual int UpdateNetworking();
	virtual int ProcessPacket(const RakNet::Packet *packet);

	// send packet
	int SendPacket(const char *buffer, const unsigned int bufferLength, const int connectionIndex, const int broadcast, const int reliable) const;
	int SendPacket(const RakNet::BitStream *bs, const int connectionIndex, const int broadcast, const int reliable) const;

	// write time stamp message ID and precise system time to buffer or bit stream
	static int WriteTimeStamp(char *buffer, const RakNet::Time &t, const RakNet::Time &t0);
	static int WriteTimeStamp(RakNet::BitStream *bs, const RakNet::Time &t, const RakNet::Time &t0);

	// read time stamp from buffer or bit stream
	static int ReadTimeStamp(const char *buffer, RakNet::Time &t, RakNet::Time &t0);
	static int ReadTimeStamp(RakNet::BitStream *bs, RakNet::Time &t, RakNet::Time &t0);

	// enumerator of packet identifiers
	enum me_PacketIdentifier
	{
		egpID_packetBegin = ID_USER_PACKET_ENUM,
		egpID_connectionIndex,
		egpID_currentTime,
		egpID_stateInput,
		egpID_stateUpdate,
		egpID_sendBall,
		egpID_gameOver
	};

	struct SendBall
	{
		char ID;
		float posX;
		int ballID;
	};

	struct GameOver 
	{
		char ID;
	};

	// current game state
	// this should be contained in a state manager
	egpGameState *mp_state;

	// utility to send raw input
	int SendStateInput(const RakNet::Time delay, const int connectionIndex, const int broadcast, const int reliable) const;

	// utility to tell the state to package up its data and 
	//	send if there is any data to be packaged
	int SendStateUpdate(const RakNet::Time delay, const int connectionIndex, const int broadcast, const int reliable) const;


private:

	// networking management with thread
	egpThread m_networkThread[1];
	egpTimer m_networkTimer[1];
	int m_networkThreadAllowed;

	static int NetworkingThread(void *);
	void InitializePeer();
	void TerminatePeer();


public: 

	// common callbacks
	virtual int OnIdle();
	virtual int OnKeyPress(unsigned char key);
	virtual int OnKeyRelease(unsigned char key);
	virtual int OnKeyPressSpecial(unsigned char key);
	virtual int OnKeyReleaseSpecial(unsigned char key);
	virtual int OnMouseButtonPress(unsigned char button, int cursorX, int cursorY);
	virtual int OnMouseButtonRelease(unsigned char button, int cursorX, int cursorY);
	virtual int OnMouseWheel(int delta, int cursorX, int cursorY);
	virtual int OnMouseMove(int cursorX, int cursorY);

	// common networking features
	virtual int StartupNetworking(const unsigned int maxIncomingConnections, const int doesConnect, const unsigned short portIncoming);
	virtual int ShutdownNetworking();
	virtual int ConnectPeer(const char address[16], const unsigned short port);

	static unsigned short GetDefaultPort();
	static unsigned short GetUserPort();

	virtual void SendTheBall(float position, int ID);
};


#endif	// !__EGP_APPLICATIONSTATE_H_