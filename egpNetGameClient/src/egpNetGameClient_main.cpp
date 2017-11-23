/*
	EGP405 Networking for Online Games
	EGP Networking Playground
	By Dan Buckstein (c) 2017

	Client main file.

	Additional contributions: 

	This file was modified by Laura Reilly and Robert Mitchell with permission by author.
*/

//-----------------------------------------------------------------------------


#include "egpNet/client/egpClientApplicationState.h"
#include "egpNet/client/egpWindowState.h"
#include "egpNet/client/egpImageLibraryWrapper.h"


#include <stdio.h>
#include <string.h>


#include "GL/glew.h"
#include "GL/wglew.h"


//-----------------------------------------------------------------------------
// WINDOW CALLBACKS
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus

	void egpDisplay(void);
	void egpIdle(void);
	void egpPositionWindow(int posX, int posY);
	void egpResizeWindow(int windowWidth, int windowHeight);
	void egpKeyPressASCII(unsigned char key, int cursorX, int cursorY);
	void egpKeyReleaseASCII(unsigned char key, int cursorX, int cursorY);
	void egpKeyPressSpecial(int key, int cursorX, int cursorY);
	void egpKeyReleaseSpecial(int key, int cursorX, int cursorY);
	void egpMouseButton(int button, int state, int cursorX, int cursorY);
	void egpMouseWheel(int wheel, int delta, int cursorX, int cursorY);
	void egpMouseMove(int cursorX, int cursorY);

#ifdef __cplusplus
}
#endif	// __cplusplus


#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus

	int egpGraphicsWindowStartMainLoop();
	int egpGraphicsWindowStopMainLoop();

#ifdef __cplusplus
}
#endif	// __cplusplus



// callbacks
void egpDisplay()
{
	// DO NOT USE
}

void egpIdle()
{
	egpClientApplicationState *appState_client = (egpClientApplicationState *)egpGlobalWindowState(1)->userData;

	// timers tell us when to do things
	appState_client->OnIdle();
}

void egpPositionWindow(int posX, int posY)
{
	egpWindowState *windowState = egpGlobalWindowState(1);

	// window is moved
	windowState->posX = posX;
	windowState->posY = posY;
}

void egpResizeWindow(int windowWidth, int windowHeight)
{
	egpWindowState *windowState = egpGlobalWindowState(1);

	// window is resized
	windowState->width = windowWidth;
	windowState->height = windowHeight;
	windowState->aspect = (double)windowState->width / (double)windowState->height;
}

void egpKeyPressASCII(unsigned char key, int cursorX, int cursorY)
{
	const egpWindowState *windowState = egpGlobalWindowState(1);
	egpClientApplicationState *appState_client = (egpClientApplicationState *)windowState->userData;

	// ASCII key pressed
	if (key != windowState->terminateKey)
	{
		appState_client->OnKeyPress(key);
	}
	else
	{
		egpGraphicsWindowStopMainLoop();
	}
}

void egpKeyReleaseASCII(unsigned char key, int cursorX, int cursorY)
{
	const egpWindowState *windowState = egpGlobalWindowState(1);
	egpClientApplicationState *appState_client = (egpClientApplicationState *)windowState->userData;

	// ASCII key released
	if (key != windowState->terminateKey)
	{
		appState_client->OnKeyRelease(key);
	}
	else
	{
		egpGraphicsWindowStopMainLoop();
	}
}

void egpKeyPressSpecial(int key, int cursorX, int cursorY)
{
	const egpWindowState *windowState = egpGlobalWindowState(1);
	egpClientApplicationState *appState_client = (egpClientApplicationState *)windowState->userData;

	// non-ASCII key pressed
	appState_client->OnKeyPressSpecial(key);
}

void egpKeyReleaseSpecial(int key, int cursorX, int cursorY)
{
	const egpWindowState *windowState = egpGlobalWindowState(1);
	egpClientApplicationState *appState_client = (egpClientApplicationState *)windowState->userData;

	// non-ASCII key released
	appState_client->OnKeyReleaseSpecial(key);
}

void egpMouseButton(int button, int state, int cursorX, int cursorY)
{
	const egpWindowState *windowState = egpGlobalWindowState(1);
	egpClientApplicationState *appState_client = (egpClientApplicationState *)windowState->userData;

	// mouse button changed
	if (state)	// released
		appState_client->OnMouseButtonRelease((unsigned char)button, cursorX, cursorY);
	else
		appState_client->OnMouseButtonPress((unsigned char)button, cursorX, cursorY);
}

void egpMouseWheel(int wheel, int delta, int cursorX, int cursorY)
{
	const egpWindowState *windowState = egpGlobalWindowState(1);
	egpClientApplicationState *appState_client = (egpClientApplicationState *)windowState->userData;

	// mouse wheel changed
	appState_client->OnMouseWheel(delta, cursorX, cursorY);
}

void egpMouseMove(int cursorX, int cursorY)
{
	const egpWindowState *windowState = egpGlobalWindowState(1);
	egpClientApplicationState *appState_client = (egpClientApplicationState *)windowState->userData;

	// mouse moved
	appState_client->OnMouseMove(cursorX, cursorY);
}



//-----------------------------------------------------------------------------
// INITIALIZATION & TERMINATION
//-----------------------------------------------------------------------------

int egpInitGraphicsState()
{
	const float lineWidth = 2.0f;
	const float pointSize = 10.0f;

	// lines and points
	glLineWidth(lineWidth);
	glPointSize(pointSize);

	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// alpha blending
	// result = ( new*[new alpha] ) + ( old*[1 - new alpha] )
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// textures
	glEnable(GL_TEXTURE_2D);

	// background
	glClearColor(0.0f, 0.0f, 0.0, 0.0f);


	// extensions
	glewInit();

	// set vsync
	wglSwapIntervalEXT(1);


	return 1;
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int ClientApp(void *params)
{
	// reinterpret command line arguments
	void **args = (void **)params;
	int *argcp = (int *)(args[0]);
	char **argv = (char **)(args[1]);

	// allocate window
	egpWindowState *windowState = egpGlobalWindowState(1);

	// initialize window state
	strcpy(windowState->title, "Hello RakNet and OpenGL");
	windowState->terminateKey = 27;	// esc
	egpPositionWindow(0, 0);
	egpResizeWindow(1024, 768);


	// create application interface
	egpClientApplicationState appState_client[1];

	// link app to window
	windowState->userData = appState_client;
	appState_client->SetWindowState(windowState);


	// initialize graphics window with context
	if (egpInitGraphicsWindow(windowState, argcp, argv))
	{
		// other initializations
		egpInitGraphicsState();
		egpImageLibraryInitialize();

		// run graphics loop
		egpGraphicsWindowStartMainLoop();
	}


	// end
	windowState = egpGlobalWindowState(0);
	return 0;
}


int main(int argc, char **argv)
{
	printf("\n----------------------------------------");
	printf("\n--     EGP NET GAME ****CLIENT****    --");
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

	ClientApp(args);

//	egpThread appThread[1] = { 0 };
//	egpCreateThread(appThread, ClientApp, args);

	// wait for thread to begin and end
//	while (!appThread->running);
//	while (appThread->running);
}



//-----------------------------------------------------------------------------
