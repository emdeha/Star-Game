#include "stdafx.h"
#include "EntityProcessingSystem.h"


using namespace FusionEngine;

EntityProcessingSystem::EntityProcessingSystem(EventManager *eventManager, EntityManager *entityManager, unsigned int typeBits)
	: EntitySystem(eventManager, entityManager, typeBits) {}

EntityProcessingSystem::~EntityProcessingSystem() {}

bool EntityProcessingSystem::CheckProcessing() 
{
	return true;
}

void EntityProcessingSystem::Begin() {}

void EntityProcessingSystem::ProcessEntities(EntityManager *manager, const std::set<Entity*> &entities)
{
	for(std::set<Entity*>::const_iterator it = entities.begin(); it != entities.end(); ++it)
	{
		ProcessEntity(manager, *it);
	}
}

void EntityProcessingSystem::End() {}