/*
	EGP405 Networking for Online Games
	EGP Networking Playground
	By Dan Buckstein (c) 2017

	Server main file.

	Additional contributions: 
*/

//-----------------------------------------------------------------------------


#include "egpNet/server/egpServerApplicationState.h"


#include <stdio.h>
#include <Windows.h>


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int ServerApp(void *)
{
	// create application interface
	egpServerApplicationState appState_server[1];

	// esc
	const unsigned int terminateKey = 27;

	// start networking
	appState_server->StartupNetworking(4, 0, appState_server->GetDefaultPort());

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

	// stop networking
	appState_server->ShutdownNetworking();

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
