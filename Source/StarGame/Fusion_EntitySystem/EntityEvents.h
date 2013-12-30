#ifndef __EVENT_TYPE_H
#define __EVENT_TYPE_H


#pragma warning(push, 0)
#include <string>
#include "../glsdk/glm/glm.hpp"
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
		EVENT_ON_FUSION_COMPLETED,
		EVENT_ON_SKILL_APPLIED,
		EVENT_ON_COLLIDE,
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

	struct OnFusionCompletedEvent : public IEventData
	{
		std::string inputSequence;

		OnFusionCompletedEvent(EventType eventType, const std::string &newInputSequence)
			: IEventData(eventType), inputSequence(newInputSequence) {}
	};

	struct OnSkillAppliedEvent : public IEventData
	{
		glm::vec3 position;
		float radius;
		int damage;
		bool isNova;
		
		OnSkillAppliedEvent(EventType eventType,  glm::vec3 newPosition, float newRadius, int newDamage, bool newIsNova)
			: IEventData(eventType), position(newPosition), radius(newRadius), damage(newDamage), isNova(newIsNova) {}
	};

	struct OnCollideEvent : public IEventData
	{
		std::string collidedBodyID;

		OnCollideEvent(EventType eventType, const std::string &newCollidedBodyID)
			: IEventData(eventType), collidedBodyID(newCollidedBodyID) {}
	};
}


#endif