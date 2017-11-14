/*
	EGP405 Networking for Online Games
	EGP Networking Playground
	By Dan Buckstein (c) 2017

	Server main file.

	Additional contributions: 
*/

//-----------------------------------------------------------------------------


#include "egpNet/server/egpServerApplicationState.h"
#include "egpNet/server/egpMasterServerApplicationState.h"


#include <stdio.h>
#include <Windows.h>


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int ServerApp(void *)
{
	// esc = 27
	const unsigned int terminateKey = 0;//27;

	// test value
	const unsigned int maxIncomingConnections = 4;

	// create application interface
	egpApplicationState *appState_server;

	// determine if we are a game server or master server
	bool server = true;

	printf("\n EGP Net Server: \n Type 'M' or 'm' to run a master server: ");
	if (toupper(getc(stdin)) == 'M')
		server = false;

	if (server)
		appState_server = new egpServerApplicationState(maxIncomingConnections);
	else
		appState_server = new egpMasterServerApplicationState(maxIncomingConnections);

	// console persists until told to stop
	int running = 1;
	while (running)
	{
		if (!GetAsyncKeyState(terminateKey))
		{
			appState_server->OnIdle();
		}
		else
		{
			running = 0;
		}
	}

	// the end
	delete appState_server;

	return 0;
}


int main(int argc, char **argv)
{
	printf("\n----------------------------------------");
	printf("\n--     EGP NET GAME ****SERVER****    --");
	printf("\n----------------------------------------");
	printf("\n\n");

	// launch app thread
	void *args[2] = { &argc, argv };

	ServerApp(args);

//	egpThread appThread[1] = { 0 };
//	egpCreateThread(appThread, ServerApp, args);

	// wait for thread to begin and end
//	while (!appThread->running);
//	while (appThread->running);
}



//-----------------------------------------------------------------------------
