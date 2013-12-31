#ifndef __ENTITY_SYSTEM_H
#define __ENTITY_SYSTEM_H


#include "Event.h"
#include "Entity.h"
#include "ComponentType.h"

#pragma warning(push, 1)
#include <set>
#pragma warning(pop)


namespace FusionEngine
{
	class Entity;
	class EntityManager;
	class EventManager;

	class EntitySystem : public IEventListener
	{
	private:
		// The bits representing the component types that we care about.
		unsigned int componentsTypeBits; 

		// The set of entities that this system cares about.
		std::set<Entity*> actives;
		
		// The entity manager that we pull our information from.
		EntityManager *entityManager;

		EventManager *eventManager;

	protected:
		// Should we update?
		virtual bool CheckProcessing() = 0;

		// A preprocessing update call.
		virtual void Begin() = 0;

		// A post processing update call.
		virtual void End() = 0;

		// Any implementing entity system must implement this method and the logic 
		// to process the given entities of the system.
		virtual void ProcessEntities(EntityManager *manager, const std::set<Entity*> &entities) = 0;

	public:
		EntitySystem(EventManager *newEventManager, EntityManager *newEntityManager, unsigned int newComponentsTypeBits);
		virtual ~EntitySystem();

		// Event handler for the system.
		virtual bool HandleEvent(const IEventData &eventData);

		// The processing function that handles calling the pure virtual methods.
		void Process();
	};
}


#endif