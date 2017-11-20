#ifndef EVENT_H
#define EVENT_H

#include "Queue.h"

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