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

class HitPlayerEvent : public Event
{
public:
	HitPlayerEvent()
	{

	}

	~HitPlayerEvent() { };

	virtual int Execute()
	{
		
	}
private:

};

#endif