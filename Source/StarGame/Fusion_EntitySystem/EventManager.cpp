#include "stdafx.h"
#include "EventManager.h"
#include <assert.h>


using namespace FusionEngine;

EventManager::EventManager()
{

}

EventManager::~EventManager()
{
	RemoveAllListeners();
}

void EventManager::AddListener(IEventListener *newListener, EventType type)
{
	// Add the type to the listeners map if it doesn't exist.
	EventListenerMap::iterator iter = listeners.find(type);
	if(iter == listeners.end())
		iter = listeners.insert(EventListenerMapPair(type, EventListenerList())).first;

	EventListenerList &listenerList = iter->second;

#ifdef _DEBUG
	// Check for duplicates.
	for(EventListenerList::const_iterator it = listenerList.begin(); it != listenerList.end(); ++it)
	{
		if(*it == newListener)
		{
			assert("Duplicate event listener registered!");
		}
	}
#endif

	// Add the listener to the map.
	listenerList.push_back(newListener);
}

void EventManager::RemoveListener(IEventListener *listenerToRemove, EventType type)
{
	EventListenerMap::iterator iter = listeners.find(type);
	if(iter != listeners.end())
	{
		EventListenerList &listenerList = iter->second;
		for(EventListenerList::iterator it = listenerList.begin(); it != listenerList.end(); ++it)
		{
			if(*it == listenerToRemove)
			{
				listenerList.erase(it);
				break;
			}
		}
	}
}

void EventManager::AddGlobalListener(IEventListener *newListener)
{
#ifdef _DEBUG
	for(EventListenerList::iterator it = globalEventListeners.begin(); it != globalEventListeners.end(); ++it)
	{
		if(*it == newListener)
		{
			assert("Duplicate global listener registered!");
		}
	}
#endif

	globalEventListeners.push_back(newListener);
}

void EventManager::RemoveGlobalListener(IEventListener *listenerToRemove)
{
	EventListenerList &listenerList = globalEventListeners;
	for(EventListenerList::iterator it = listenerList.begin(); it != listenerList.end(); ++it)
	{
		if(*it == listenerToRemove)
		{
			globalEventListeners.erase(it);
			return;
		}
	}
}

void EventManager::RemoveAllListeners()
{
	for(EventListenerMap::iterator it = listeners.begin(); it != listeners.end(); ++it)
	{
		it->second.clear();
	}

	listeners.clear();
	globalEventListeners.clear();
}

bool EventManager::FireEvent(const IEventData &eventData) const
{
	// Notify all global listeners.
	for(EventListenerList::const_iterator it = globalEventListeners.begin(); it != globalEventListeners.end(); ++it)
	{
		if((*it)->HandleEvent(eventData))
		{
			return true;
		}
	}

	EventListenerMap::const_iterator iter = listeners.find(eventData.GetType());
	if(iter != listeners.end())
	{
		const EventListenerList &listenerList = iter->second;
		for(EventListenerList::const_iterator it = listenerList.begin(); it != listenerList.end(); ++it)
		{
			if((*it)->HandleEvent(eventData))
			{
				return true;
			}
		}
	}

	return false;
}