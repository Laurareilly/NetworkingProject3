#ifndef MYEVENTS
#define MYEVENTS

/*
Project 3- Synchronization
EGP 405-02
Laura Reilly -- 0972157
Robert Mitchell -- 0935286

We certify that this work is entirely our own.
The assessor of this project may reproduce this
project and provide copies to other academic staff,
and/or communicate a copy of this project to a plagiarism
- checking service, which may retain a copy of the project on its database.”
*/

#include "Event.h"
#include <iostream>
#include "egpNet/egpGameState.h"

class AddScoreEvent : public Event
{
public:
	AddScoreEvent(int *score,  int scoreIncrease)
	{
		mpScore = score;
		mScoreIncrease = scoreIncrease;
	}

	~AddScoreEvent() { };

	virtual int Execute()
	{
		*mpScore += mScoreIncrease;
		printf("score: %i \n", *mpScore);

		return 1;
	}

private:
	int *mpScore;
	int mScoreIncrease;
};

class EndGameEvent : public Event
{
public:
	EndGameEvent(egpNetPlaygroundGameState* gameState)
	{
		mpState = gameState;
	}

	~EndGameEvent() { };

	virtual int Execute()
	{
		printf("game over");
		mpState->gameActive = false;
		return 1;
	}

private:
	egpNetPlaygroundGameState* mpState;
};

#endif