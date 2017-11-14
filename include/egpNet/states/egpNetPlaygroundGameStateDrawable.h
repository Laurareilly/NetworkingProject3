/*
	egpNetPlaygroundGameStateDrawable.h
	By Dan Buckstein

	Game state with rendering.

	Additional contributions: 
*/

#ifndef __EGP_NETPLAYGROUNDGAMESTATEDRAWABLE_H_
#define __EGP_NETPLAYGROUNDGAMESTATEDRAWABLE_H_

#include "egpNet/states/egpNetPlaygroundGameState.h"


class egpNetPlaygroundGameStateDrawable : public egpNetPlaygroundGameState
{
	virtual int RenderState(int canvasWidth, int canvasHeight, int canvasPosX, int canvasPosY) const;
public: 
	static void ClearBuffers();
	egpNetPlaygroundGameStateDrawable(int ownerID);
};


#endif	// !__EGP_NETPLAYGROUNDGAMESTATEDRAWABLE_H_