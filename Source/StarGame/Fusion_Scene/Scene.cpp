#include "stdafx.h"
#include "Scene.h"

#pragma warning(push, 0)
#include <algorithm>
#include <assert.h>
#pragma warning(pop)


using namespace FusionEngine;

FusionEngine::Scene::~Scene()
{
	entities.clear();
	components.clear();
	systems.clear();
	removedEntitiesIDs.clear();
}

FusionEngine::Scene& FusionEngine::Scene::GetScene()
{
	static Scene sceneInstance;
	return sceneInstance;
}

void FusionEngine::Scene::Init(EventManager &eventManager)
{
	entityManager = std::unique_ptr<EntityManager>(new EntityManager(&eventManager));
	entities.resize(0);
	components.resize(0);
	systems.resize(0);
	removedEntitiesIDs.resize(0);
}


void FusionEngine::Scene::AddEntity(const std::string &entityTag)
{
	std::shared_ptr<Entity> newEntity = std::shared_ptr<Entity>(entityManager->CreateEntity());
	EntityProperties newProperties;
	newProperties.entityName = entityTag;
	newProperties.isActive = true;
	entities.push_back(std::pair<EntityProperties, std::shared_ptr<Entity>>(newProperties, newEntity));
}

void FusionEngine::Scene::AddSystem(EntityProcessingSystem *system)
{
	systems.push_back(std::unique_ptr<EntityProcessingSystem>(system));
}

void FusionEngine::Scene::AddComponent(const std::string &entityTag, Component *component)
{
#ifdef _DEBUG
	assert(!entities.empty());
#endif
	bool isFound = false;

	components.push_back(std::unique_ptr<Component>(component));
	Entity *foundEntity = entities[0].second.get();
	for(EntitiesMap::iterator entity = entities.begin(); entity != entities.end(); ++entity)
	{
		if(entity->first.entityName.compare(entityTag) == 0)
		{
			foundEntity = entity->second.get();
			isFound = true;
		}
	}
#ifdef _DEBUG
	assert(isFound);
#endif

	entityManager->InsertComponent(foundEntity, component);
}

bool FusionEngine::Scene::HasEntity(const std::string &entityTag) const
{
#ifdef _DEBUG
	assert(!entities.empty());
#endif

	for(EntitiesMap::const_iterator entity = entities.begin(); entity != entities.end(); ++entity)
	{
		if(entity->first.isActive)
		{
			if(entity->first.entityName.compare(entityTag) == 0)
			{
				return true;
			}
		}
	}

	return false;
}

bool FusionEngine::Scene::RemoveEntityFirst(const std::string &entityTag)
{	
#ifdef _DEBUG
	assert(entities.empty() == false);
#endif

	for(EntitiesMap::iterator entity = entities.begin(); entity != entities.end(); ++entity)
	{
		if(entity->first.isActive)
		{
			if(entity->first.entityName.compare(entityTag) == 0)
			{
				entityManager->DestroyEntity(entity->second.get());		
				entity->first.isActive = false;
				return true;
			}	
		}
	}

	return false;
}

bool FusionEngine::Scene::RemoveEntityLast(const std::string &entityTag)
{
#ifdef _DEBUG
	assert(entities.empty() == false);
#endif

	for(EntitiesMap::reverse_iterator entity = entities.rbegin(); entity <= entities.rend(); --entity)
	{
		if(entity->first.isActive)
		{
			if(entity->first.entityName.compare(entityTag) == 0)
			{
				entityManager->DestroyEntity(entity->second.get());
				entity->first.isActive = false;
				return true;
			}
		}
	}

	return false;
}

Entity *FusionEngine::Scene::GetEntity(const std::string &entityTag) const
{	
#ifdef _DEBUG
	assert(!entities.empty());
#endif
	
	bool isFound = false;
	
	Entity *foundEntity = entities[0].second.get();
	for(EntitiesMap::const_iterator entity = entities.begin(); entity != entities.end(); ++entity)
	{
		if(entity->first.isActive)
		{		
			if(entity->first.entityName.compare(entityTag) == 0)
			{
				foundEntity = entity->second.get();			
				isFound = true;
			}
		}
		else
		{
			isFound = false;
		}
	}
	
#ifdef _DEBUG
	//assert(isFound == true);
#endif
	
	return foundEntity;
}

EntityManager *FusionEngine::Scene::GetEntityManager() const
{
	return entityManager.get();
}

bool FusionEngine::Scene::CheckIfRemoved(unsigned int entityId) const
{
	std::vector<unsigned int>::const_iterator id = removedEntitiesIDs.begin();
	for ( ; id != removedEntitiesIDs.end(); ++id)
	{
		if((*id) == entityId)
		{
			return true;
		}
	}

	return false;
}


void FusionEngine::Scene::ProcessSystems()
{
	for(SystemsVector::iterator system = systems.begin(); system != systems.end(); ++system)
	{
		(*system)->Process();
	}
}