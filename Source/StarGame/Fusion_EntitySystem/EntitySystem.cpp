#include "stdafx.h"
#include "EntitySystem.h"
#include "EntityEvents.h"
#include "EventManager.h"

using namespace FusionEngine;

EntitySystem::EntitySystem(EventManager *newEventManager, EntityManager *newEntityManager, unsigned int newComponentsTypeBits)
{
	eventManager = newEventManager;
	entityManager = newEntityManager;
	componentsTypeBits = newComponentsTypeBits;

	// Register this system for component events.
	eventManager->AddListener(this, EVENT_INSERT_COMPONENT);
	eventManager->AddListener(this, EVENT_REMOVE_COMPONENT);
}

EntitySystem::~EntitySystem()
{
	eventManager->RemoveListener(this, EVENT_INSERT_COMPONENT);
	eventManager->RemoveListener(this, EVENT_REMOVE_COMPONENT);

	actives.clear();
}

void EntitySystem::Process()
{
	if(CheckProcessing())
	{
		Begin();
		ProcessEntities(entityManager, actives);
		End();
	}
}

bool EntitySystem::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch(type)
	{
	case EVENT_INSERT_COMPONENT:
	case EVENT_REMOVE_COMPONENT:
		{
			const EntityEvent &data = static_cast<const EntityEvent&>(eventData);
			Entity *entity = data.entity;

			// Does this entity meet our requirements?
			if((entity->typeBits & componentsTypeBits) == componentsTypeBits)
				actives.insert(entity);
			else 
				actives.erase(entity);
		}
		break;
	}

	return false;
}