#ifndef __EVENT_TYPE_H
#define __EVENT_TYPE_H


#pragma warning(push, 0)
#include <string>
#pragma warning(pop)

#include "ComponentType.h"
#include "Event.h"

namespace FusionEngine
{
	class Entity;

	enum 
	{
		EVENT_CREATE_ENTITY = 1,
		EVENT_DESTROY_ENTITY,
		EVENT_INSERT_COMPONENT,
		EVENT_REMOVE_COMPONENT,
		
		EVENT_ON_CLICK,
		EVENT_ON_RESHAPE,
		EVENT_ON_KEY_PRESSED,
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
		std::string objectId; // TODO: replace with int

		OnClickEvent(EventType eventType, bool newIsLeftButtonDown, const std::string &newObjectId) 
			: IEventData(eventType), isLeftButtonDown(newIsLeftButtonDown), objectId(newObjectId) {}
	};

	struct OnReshapeEvent : public IEventData
	{
		unsigned short windowWidth;
		unsigned short windowHeight;

		OnReshapeEvent(EventType eventType, unsigned short newWindowWidth, unsigned short newWindowHeight) 
			: IEventData(eventType),
			  windowWidth(newWindowWidth), windowHeight(newWindowHeight) {}
	};

	struct OnKeyPressedEvent : public IEventData
	{
		char key;
		std::string objectId; // TODO: replace with int

		OnKeyPressedEvent(EventType eventType, char newKey, const std::string &newObjectId)
			: IEventData(eventType), key(newKey), objectId(newObjectId) {}
	};
}


#endif