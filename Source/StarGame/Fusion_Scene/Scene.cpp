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
}

void Scene::Init()
{
	eventManager = std::unique_ptr<EventManager>(new EventManager());
	entityManager = std::unique_ptr<EntityManager>(new EntityManager(eventManager.get()));
}

bool Scene::HasEntity(const std::string &entityTag)
{
#ifdef _DEBUG
	assert(!entities.empty());
#endif
	bool isFound = false;

	for(EntitiesMap::iterator iter = entities.begin(); iter != entities.end(); ++iter)
	{
		if(entityManager->CheckIfRemoved(iter->second->GetIndex()) == false)
		{
			if(iter->first.compare(entityTag) == 0)
			{
				return true;
			}
		}
	}

	return false;
}

void Scene::AddEntity(const std::string &entityTag)
{
	std::shared_ptr<Entity> newEntity = 
		std::shared_ptr<Entity>(entityManager->CreateEntity());
	entities.push_back(std::pair<std::string, std::shared_ptr<Entity>>
							(entityTag, newEntity));
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
		if(iter->first.compare(entityTag) == 0)
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


void Scene::RemoveEntity(const std::string &entityTag)
{	
#ifdef _DEBUG
	assert(entities.empty() == false);
#endif

	bool isFound = false;
	EntitiesMap::iterator iter = entities.begin();
	for(iter = entities.begin(); iter != entities.end(); ++iter)
	{
		if(entityManager->CheckIfRemoved(iter->second->GetIndex()) == false)
		{
			if(iter->first.compare(entityTag) == 0)
			{
				entityManager->DestroyEntity(iter->second.get());
				isFound = true;
				return;
			}	
		}
	}
	/*while(iter != entities.end())
	{
		if(entityManager->CheckIfRemoved(iter->second->GetIndex()) == false)
		{
			if(iter->first.compare(entityTag) == 0)
			{
				entityManager->DestroyEntity(iter->second.get());
				isFound = true;
				return;
			}	
			else
			{
				++iter;
			}
		}
		else
		{
			isFound = false;
		}
	}*/

#ifdef _DEBUG
	assert(isFound == true);
#endif
}


EntityManager *Scene::GetEntityManager()
{
	return entityManager.get();
}
EventManager *Scene::GetEventManager()
{
	return eventManager.get();
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
		if(entityManager->CheckIfRemoved(iter->second->GetIndex()) == false)
		{		
			if(iter->first.compare(entityTag) == 0)
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
	assert(isFound);
#endif
	
	return foundEntity;
}


void Scene::ProcessSystems()
{
	for(SystemsVector::iterator iter = systems.begin(); iter != systems.end(); ++iter)
	{
		(*iter)->Process();
	}
}