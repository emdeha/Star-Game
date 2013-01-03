#include "stdafx.h"
#include "Scene.h"

#include <algorithm>
#include <assert.h>


using namespace FusionEngine;


Scene::~Scene()
{
	entities.clear();
	components.clear();
	systems.clear();
	removedEntitiesIDs.clear();
}

void Scene::Init()
{
	eventManager = std::unique_ptr<EventManager>(new EventManager());
	entityManager = std::unique_ptr<EntityManager>(new EntityManager(eventManager.get()));
	entities.resize(0);
	components.resize(0);
	systems.resize(0);
	removedEntitiesIDs.resize(0);
}


void Scene::AddEntity(const std::string &entityTag)
{
	std::shared_ptr<Entity> newEntity = 
		std::shared_ptr<Entity>(entityManager->CreateEntity());
	EntityProperties newProperties;
	newProperties.entityName = entityTag;
	newProperties.isActive = true;
	entities.push_back(std::pair<EntityProperties, std::shared_ptr<Entity>>
							(newProperties, newEntity));
}
void Scene::AddSystem(EntityProcessingSystem *system)
{
	systems.push_back(std::unique_ptr<EntityProcessingSystem>(system));
}

void Scene::AddComponent(const std::string &entityTag, Component *component)
{
#ifdef _DEBUG
	assert(!entities.empty());
#endif
	bool isFound = false;

	components.push_back(std::unique_ptr<Component>(component));
	Entity *foundEntity = entities[0].second.get();
	for(EntitiesMap::iterator iter = entities.begin(); iter != entities.end(); ++iter)
	{
		if(iter->first.entityName.compare(entityTag) == 0)
		{
			foundEntity = iter->second.get();
			isFound = true;
		}
	}
#ifdef _DEBUG
	assert(isFound);
#endif

	entityManager->InsertComponent(foundEntity, component);
}

bool Scene::HasEntity(const std::string &entityTag)
{
#ifdef _DEBUG
	assert(!entities.empty());
#endif

	for(EntitiesMap::iterator iter = entities.begin(); iter != entities.end(); ++iter)
	{
		if(iter->first.isActive)
		{
			if(iter->first.entityName.compare(entityTag) == 0)
			{
				return true;
			}
		}
	}

	return false;
}

bool Scene::RemoveEntity(const std::string &entityTag)
{	
#ifdef _DEBUG
	assert(entities.empty() == false);
#endif

	EntitiesMap::iterator iter = entities.begin();
	for(iter = entities.begin(); iter != entities.end(); ++iter)
	{
		if(iter->first.isActive)
		{
			if(iter->first.entityName.compare(entityTag) == 0)
			{
				entityManager->DestroyEntity(iter->second.get());		
				iter->first.isActive = false;
				return true;
			}	
		}
	}

	return false;
}

Entity *Scene::GetEntity(const std::string &entityTag)
{	
#ifdef _DEBUG
	assert(!entities.empty());
#endif
	
	bool isFound = false;
	
	Entity *foundEntity = entities[0].second.get();
	for(EntitiesMap::iterator iter = entities.begin(); iter != entities.end(); ++iter)
	{
		if(iter->first.isActive)
		{		
			if(iter->first.entityName.compare(entityTag) == 0)
			{
				foundEntity = iter->second.get();			
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

EntityManager *Scene::GetEntityManager()
{
	return entityManager.get();
}
EventManager *Scene::GetEventManager()
{
	return eventManager.get();
}


bool Scene::CheckIfRemoved(unsigned int entityId)
{
	for(std::vector<unsigned int>::iterator iter = removedEntitiesIDs.begin(); 
		iter != removedEntitiesIDs.end(); 
		++iter)
	{
		if((*iter) == entityId)
		{
			return true;
		}
	}

	return false;
}


void Scene::ProcessSystems()
{
	for(SystemsVector::iterator iter = systems.begin(); iter != systems.end(); ++iter)
	{
		(*iter)->Process();
	}
}