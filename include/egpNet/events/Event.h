#ifndef EVENT_H
#define EVENT_H

#include "Queue.h"

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

class Event
{
public:
	Event() {};
	virtual ~Event() {};

	virtual int Execute() = 0;

private:
};


class EventManager
{
public:
	static EventManager* InitInstance()
	{
		if (!mpInstance)
			mpInstance = new EventManager;

		return mpInstance;;
	}

	static EventManager* GetInstance()
	{
		return mpInstance;;
	}

	EventManager() { }
	~EventManager() { };

	int AddEvent(Event *e)
	{
		eventQueue.enqueue(e);
		return 0;
	}

	int ExecuteAll()
	{
		Event* tmpEvent;

		while (eventQueue.getAmountInQueue() > 0)
		{
			tmpEvent = eventQueue.dequeue();
			tmpEvent->Execute();
			delete tmpEvent;
		}
		return 0;
	}

	static EventManager *mpInstance;
private:
	Queue<Event*> eventQueue;
};

#endif	