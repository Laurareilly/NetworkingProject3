/*
	egpWindowState.h
	By Dan Buckstein

	Window state and global window state.
*/

#ifndef __EGP_WINDOWSTATE_H_
#define __EGP_WINDOWSTATE_H_


#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct egpWindowState egpWindowState;
#endif	// __cplusplus


struct egpWindowState
{
	char title[32];
	int terminateKey;
	int posX, posY;
	int width, height;
	double aspect;

	// pointer to generic user data
	void *userData;
};


// manage a singleton instance of window state in this file
egpWindowState *egpGlobalWindowState(const int alloc);

// create a window with a rendering context
int egpInitGraphicsWindow(const egpWindowState *windowState, int *argcp, char **argv);


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__EGP_WINDOWSTATE_H_