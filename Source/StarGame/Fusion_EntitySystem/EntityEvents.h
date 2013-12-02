#ifndef __EVENT_TYPE_H
#define __EVENT_TYPE_H


#include "ComponentType.h"
#include "Event.h"

namespace FusionEngine
{
	class Entity;

	enum 
	{
		EVENT_CREATE_ENTITY = 1,
		EVENT_DESTROY_ENTITY = 2,
		EVENT_INSERT_COMPONENT = 3,
		EVENT_REMOVE_COMPONENT = 4,
		
		EVENT_ON_CLICK = 5,
		EVENT_ON_RESHAPE = 6,
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

	struct OnClickEvent : public IEventData
	{
		bool isLeftButtonDown;
		std::string objectId; // replace with int

		OnClickEvent(EventType eventType, bool newIsLeftButtonDown, const std::string &newObjectId) 
			: IEventData(eventType), isLeftButtonDown(newIsLeftButtonDown), objectId(newObjectId) {}
	};

	struct OnReshapeEvent : public IEventData
	{
		int windowWidth;
		int windowHeight;

		OnReshapeEvent(EventType eventType, int newWindowWidth, int newWindowHeight) 
			: IEventData(eventType),
			  windowWidth(newWindowWidth), windowHeight(newWindowHeight) {}
	};
}


#endif