#include "stdafx.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "EntityEvents.h"
#include "Component.h"


using namespace FusionEngine;

ComponentList EntityManager::EmptyList;

EntityManager::EntityManager(EventManager *newEventSystem)
	: eventSystem(newEventSystem)
{
	components.resize(CT_LAST);
}

EntityManager::~EntityManager()
{
	for(std::vector<Entity*>::iterator it = entities.begin(); it != entities.end(); ++it)
	{
		if(*it)
		{
			DestroyAllComponents(*it);
		}
	}

	entities.clear();

	for(int i = 0; i < CT_LAST; i++)
	{
		components[i].clear();
	}
	components.clear();
}

Entity *EntityManager::CreateEntity()
{
	Entity *entity = new Entity(++nextId);

	// Do we have previously removed slot indices that we can reuse?
	if(removedIndices.size())
	{
		entity->index = removedIndices.back();
		removedIndices.pop_back();

		// Resize the vector if needed.
		if(entity->index >= entities.size())
		{
			entities.resize(entity->index + 1, NULL);
		}

		entities[entity->index] = entity;
	}
	else
	{
		entity->index = entities.size();
		entities.push_back(entity);
	}

	// Inform the world that we just created an entity.
	eventSystem->FireEvent(EntityEvent(EVENT_CREATE_ENTITY, entity));

	// Return a reference to the element itself.
	return entity;
}

void EntityManager::DestroyEntity(Entity *entity)
{
	entities[entity->index] = nullptr;
	removedIndices.push_back(entity->index);
	DestroyAllComponents(entity);

	// Inform the world that we just destroyed an entity.
	eventSystem->FireEvent(EntityEvent(EVENT_DESTROY_ENTITY, entity));

	delete entity;
}

void EntityManager::InsertComponent(Entity *entity, Component *component)
{
	std::vector<ComponentList> &entities = components[component->GetType()];

	// Resize the vector if needed.
	if(entity->index >= entities.size())
	{
		entities.resize(entity->index + 1);
	}

	// Grab the component list for this entity and add the component.
	ComponentList &list = entities[entity->index];
	list.push_back(component);

	// Set the entity's bit for this component.
	entity->typeBits |= component->GetTypeBit();

	// Inform the world that we just inserted a component.
	eventSystem->FireEvent(EntityComponentEvent(EVENT_INSERT_COMPONENT, entity, component->GetType()));
}

void EntityManager::RemoveComponent(Entity *entity, Component *component)
{
	std::vector<ComponentList> &entities = components[component->GetType()];

	// Skip if we don't even have the space allocated.
	if(entity->index >= entities.size()) return;

	// Grab the component list for this entity and remove the component.
	ComponentList &list = entities[entity->index];
	for(ComponentList::iterator it = list.begin(); it != list.end(); ++it)
	{
		if(*it == component)
			list.erase(it);
	}

	// Remove the component bit if we don't have any more of this component type.
	if(list.size() == 0) 
	{
		entity->typeBits &= ~component->GetTypeBit();
	}

	// Inform the world that we just removed a component.
	eventSystem->FireEvent(EntityComponentEvent(EVENT_REMOVE_COMPONENT, entity, component->GetType()));
}

void EntityManager::DestroyAllComponents(Entity *entity)
{
	for(int type = 0; type < CT_LAST; type++)
	{
		DestroyAllComponents(entity, (ComponentType)type);
	}
}

void EntityManager::DestroyAllComponents(Entity *entity, ComponentType type)
{
	std::vector<ComponentList> &entities = components[type];

	// Skip if we don't even have the space allocated.
	if(entity->index >= entities.size()) return;

	// Grab the component list for this entity and remove the component.
	ComponentList &list = entities[entity->index];

	// Delete the components.
	while(list.size())
	{
		delete list.back();
		list.pop_back();

		// Remove the bit precisely when we get to the last component, prior to firing the event.
		if(list.size() == 0)
		{
			entity->typeBits &= ~(1 << type);
		}

		// Inform the world that we just removed a component.
		eventSystem->FireEvent(EntityComponentEvent(EVENT_REMOVE_COMPONENT, entity, (ComponentType)type));
	}

	list.clear();
}

const ComponentList &EntityManager::GetComponentList(const Entity *entity, ComponentType type) const
{
	const std::vector<ComponentList> &entities = components[type];

	// Return a blank list if we don't even have the space allocated.
	if(entity->index >= entities.size()) return EmptyList;

	// Grab the component list for this entity and remove the component.
	return entities[entity->index];
}
const ComponentList &EntityManager::GetComponentList(unsigned int entityIndex, ComponentType type) const
{
	const std::vector<ComponentList> &entities = components[type];

	if(entityIndex >= entities.size()) return EmptyList;

	return entities[entityIndex];
}

bool EntityManager::CheckIfRemoved(unsigned int entityIndex)
{
	for(std::vector<unsigned int>::iterator iter = removedIndices.begin();
		iter != removedIndices.end(); ++iter)
	{
		if((*iter) == entityIndex)
		{
			std::printf("Index removed: %ui", *iter);
			return true;
		}
	}

	return false;
}