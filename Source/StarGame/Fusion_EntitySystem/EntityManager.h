#ifndef __ENTITY_MANAGER_H
#define __ENTITY_MANAGER_H


#include "Entity.h"
#include "ComponentType.h"
#include <vector>
#include <list>

namespace FusionEngine
{
	class Component;
	class EventManager;

	typedef std::vector<Component*> ComponentList;

	class EntityManager
	{
	private:
		static ComponentList EmptyList;

		// The next entity id to allocate.
		EntityID nextId;

		std::vector<unsigned int> removedIndices;
		std::vector<Entity*> entities;
		std::vector<std::vector<ComponentList>> components;

		// The event system we are using;
		EventManager *eventSystem;

	public:
		EntityManager(EventManager *newEventSystem);
		virtual ~EntityManager();

		// Allocates a new entity in the system and returns its handle.
		Entity *CreateEntity();

		// Destroys an entity in the system.
		void DestroyEntity(Entity *entity);

		// Insert a component.
		void InsertComponent(Entity *entity, Component *component);

		// Remove a component. It does not delete the component.
		void RemoveComponent(Entity *entity, Component *component);

		// Remove all components of an entity and delete them.
		void DestroyAllComponents(Entity *entity);

		// Remove all components of a given type and delete them.
		void DestroyAllComponents(Entity *entity, ComponentType type);

		// Get the list of components for an entity by type.
		const ComponentList &GetComponentList(const Entity *entity, ComponentType type) const;
	};
}


#endif