#include "stdafx.h"
#include "Scene.h"
#include <algorithm>


using namespace FusionEngine;

/*Scene::Scene(EntityManager *newEntityManager)
{
	entityManager = newEntityManager;
}*/
Scene::~Scene()
{
	entities.clear();
	components.clear();
	systems.clear();
	/*for(std::vector<Entity*>::iterator iter = entities.begin(); 
		iter != entities.end(); ++iter)
	{
		entityManager->DestroyAllComponents((*iter));
		entityManager->DestroyEntity((*iter));
	}
	std::remove_if(entities.begin(), entities.end(), 
				   [](Entity *theElement)
				   {
					   return delete theElement;
				   });
	std::remove_if(systems.begin(), systems.end(),
				   [](EntityProcessingSystem *theElement)
				   {
					   return delete theElement;
				   });

	if(entityManager)
	{
		delete entityManager;
	}*/
}

void Scene::Init()
{
	eventManager = std::unique_ptr<EventManager>(new EventManager());
	entityManager = std::unique_ptr<EntityManager>(new EntityManager(eventManager.get()));
}

void Scene::AddEntity(const std::string &entityTag)
{
	std::shared_ptr<Entity> newEntity = std::shared_ptr<Entity>(entityManager->CreateEntity());
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
	if(entities.empty())
	{
		throw std::exception("Entity not found!!!").what();
	}
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
	if(!isFound)
	{
		throw std::exception("Entity not found!!!").what();
	}
#endif
	entityManager->InsertComponent(foundEntity, component);
}



EntityManager *Scene::GetEntityManager()
{
	return entityManager.get();
}
EventManager *Scene::GetEventManager()
{
	return eventManager.get();
}


void Scene::ProcessSystems()
{
	for(SystemsVector::iterator iter = systems.begin(); iter != systems.end(); ++iter)
	{
		(*iter)->Process();
	}
}

/*
void Scene::InsertSystem(EntityProcessingSystem *system)
{
	systems.push_back(system);
}

void Scene::InsertComponent(Entity *entity, Component *component)
{
	entityManager->InsertComponent(entity, component);
}

void Scene::ProcessSystems()
{
	for(std::vector<EntityProcessingSystem*>::iterator iter = systems.begin();
		iter != systems.end(); ++iter)
	{
		(*iter)->Process();
	}
}

EntityManager *Scene::GetEntityManager()
{
	return entityManager;
}
const ComponentList &Scene::GetComponentList(Entity *entity, ComponentType type)
{
	return entityManager->GetComponentList(entity, type);
}*/