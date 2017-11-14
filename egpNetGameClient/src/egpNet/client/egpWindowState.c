/*
	egpWindowState.c
	By Dan Buckstein

	Wrapper for global window state and freeglut window initialization.
*/


#include "egpNet/client/egpWindowState.h"


#include "GL/freeglut.h"


//-----------------------------------------------------------------------------
// CALLBACK DECLARATIONS
//-----------------------------------------------------------------------------

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


//-----------------------------------------------------------------------------
// PUBLIC FUNCTIONS
//-----------------------------------------------------------------------------

// manage a singleton instance of window state in this file
egpWindowState *egpGlobalWindowState(const int alloc)
{
	static egpWindowState *g_windowState = 0;

	// if alloc flag is up, create new state if one doesn't exist already
	// the caller will be responsible for configuring the state
	if (alloc)
	{
		if (g_windowState == 0)
		{
			g_windowState = (egpWindowState *)malloc(sizeof(egpWindowState));
			memset(g_windowState, 0, sizeof(egpWindowState));
		}
	}

	// if alloc flag is down, delete state if it exists
	else
	{
		if (g_windowState != 0)
		{
			free(g_windowState);
			g_windowState = 0;
		}
	}

	// return state pointer
	return g_windowState;
}


// create a window with a rendering context
int egpInitGraphicsWindow(const egpWindowState *windowState, int *argcp, char **argv)
{
	if (windowState && argcp && argv)
	{
		// create window
		glutInit(argcp, argv);
		glutInitWindowPosition(windowState->posX, windowState->posY);
		glutInitWindowSize(windowState->width, windowState->height);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
		glutCreateWindow(windowState->title);

		// set callbacks
		glutDisplayFunc(egpDisplay);
		glutIdleFunc(egpIdle);
		glutPositionFunc(egpPositionWindow);
		glutReshapeFunc(egpResizeWindow);
		glutKeyboardFunc(egpKeyPressASCII);
		glutKeyboardUpFunc(egpKeyReleaseASCII);
		glutSpecialFunc(egpKeyPressSpecial);
		glutSpecialUpFunc(egpKeyReleaseSpecial);
		glutMouseFunc(egpMouseButton);
		glutMouseWheelFunc(egpMouseWheel);
		glutMotionFunc(egpMouseMove);
		glutPassiveMotionFunc(egpMouseMove);

		// done
		return 1;
	}
	return 0;
}



//-----------------------------------------------------------------------------
// INTERNAL FUNCTIONS
//-----------------------------------------------------------------------------

// internal GLUT main loop launcher
int egpGraphicsWindowStartMainLoop()
{
	glutMainLoop();
	return 1;
}

// internal GLUT main loop killer
int egpGraphicsWindowStopMainLoop()
{
	glutLeaveMainLoop();
	return 1;
}


//-----------------------------------------------------------------------------
