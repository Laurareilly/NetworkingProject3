/*
	egpGameState.cpp
	By Dan Buckstein

	Base implementation for networked game state.

	Additional contributions: 
*/

#include "egpNet/egpGameState.h"


egpGameState::egpGameState(int ownerID)
	: m_ownerID(ownerID)
{
	// do nothing
}

egpGameState::~egpGameState()
{
	// do nothing
}


int egpGameState::GetOwnerID() const
{
	return m_ownerID;
}


int egpGameState::SerializeData(char *buffer, const unsigned int bufferCapacity, const unsigned int serializeCount, const double dt) const
{
	return 0;
}

int egpGameState::DeserializeData(const char *buffer, const unsigned int bufferCapacity, const unsigned int deserializeCount, const double dt)
{
	return 0;
}

int egpGameState::ProcessInput(const egpKeyboard *keyboard, const egpMouse *mouse, const unsigned int ctrlID, const double dt)
{
	return 0;
}

int egpGameState::UpdateState(double dt)
{
	return 0;
}

int egpGameState::RenderState(int canvasWidth, int canvasHeight, int canvasPosX, int canvasPosY) const
{
	return 0;
}
