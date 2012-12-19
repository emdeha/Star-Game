#ifndef __ENTITY_PROCESSING_SYSTEM_H
#define __ENTITY_PROCESSING_SYSTEM_H


#include "ComponentType.h"
#include "EntitySystem.h"

namespace FusionEngine
{
	class EventManager;
	class EntityManager;

	class EntityProcessingSystem : public EntitySystem
	{
	protected:
		// A preprocessing update call.
		virtual void Begin();

		// Process an entity this system is interested in.
		virtual void ProcessEntity(EntityManager *manager, Entity *entity) = 0;

		// A post processing update call.
		virtual void End();

		// Should we update?
		virtual bool CheckProcessing();

		// Any implementing entity system must implement this method and the logic
		// to process the given entities of the system.
		virtual void ProcessEntities(EntityManager *manager, const std::set<Entity*> &entities);

	public:
		EntityProcessingSystem(EventManager *eventManager, EntityManager *entityManager, unsigned int typeBits);
		virtual ~EntityProcessingSystem();
	};
}


#endif