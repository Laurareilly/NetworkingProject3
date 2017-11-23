/*
	EGP405 Networking for Online Games
	EGP Networking Playground
	By Dan Buckstein (c) 2017

	Server main file.

	Additional contributions: 

	This file was modified by Laura Reilly and Robert Mitchell with permission by author.
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
	printf("\n--     CONTROLS:--");
	printf("\n--     Networked movement: A/D moves left right--");
	printf("\n--     Local movement: A/D moves top player left right. J/L moves bottom player left right--");
	printf("\n--     L- Local game--");
	printf("\n--     C- Connect to a networked game--");
	printf("\n--     R- Replay game after game over--");
	printf("\n--     S- Top player drops bombs-- try to hit bottom player--");
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
