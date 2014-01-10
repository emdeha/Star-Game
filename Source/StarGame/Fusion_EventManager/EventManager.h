#ifndef __EVENT_SYSTEM_H
#define __EVENT_SYSTEM_H


#include "Event.h"

#pragma warning(push, 1)
#include <map>
#include <list>
#pragma warning(pop)


namespace FusionEngine
{
	class EventManager
	{
	protected:
		typedef std::list<IEventListener*> EventListenerList;
		typedef std::map<EventType, EventListenerList> EventListenerMap;
		typedef std::pair<EventType, EventListenerList> EventListenerMapPair;

		// A map of all the listeners connected to specific events.
		std::map<EventType, EventListenerList> listeners;

		// A list of global event listeners. These listeners receive all events regardless of type.
		EventListenerList globalEventListeners;

	public:
		EventManager();
		virtual ~EventManager();

		// Add a listener for a given event type.
		void AddListener(IEventListener *newListener, EventType type);

		// Remove a listener for a given event type.
		void RemoveListener(IEventListener *listenerToRemove, EventType type);

		// Add a global listener. This type of listener receives all types of events.
		// A listener that is registered as global as well as for event types will receive
		// duplicate events.
		void AddGlobalListener(IEventListener *newListener);

		// Remove a global listener.
		void RemoveGlobalListener(IEventListener *listenerToRemove);

		// Remove all listeners for all event types.
		void RemoveAllListeners();

		// Dispatch an event.
		bool FireEvent(const IEventData &eventData) const;
	};
}


#endif