#ifndef __EVENT_TYPE_H
#define __EVENT_TYPE_H


#include "ComponentType.h"
#include "Event.h"

namespace FusionEngine
{
	enum 
	{
		EVENT_CREATE_ENTITY = 1,
		EVENT_DESTROY_ENTITY = 2,
		EVENT_INSERT_COMPONENT = 3,
		EVENT_REMOVE_COMPONENT = 4,
	};

	struct EntityEvent : public IEventData
	{
		Entity *entity;

		EntityEvent(EventType eventType, Entity *newEntity) 
			: IEventData(eventType), entity(newEntity) {}
	};

	struct EntityComponentEvent : public IEventData
	{
		Entity *entity;
		ComponentType componentType;

		EntityComponentEvent(EventType eventType, Entity *newEntity, ComponentType newComponentType)
			: IEventData(eventType), entity(newEntity), componentType(newComponentType) {}
	};
}


#endif