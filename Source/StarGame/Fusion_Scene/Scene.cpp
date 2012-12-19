#include "stdafx.h"
#include "Scene.h"
#include <algorithm>


using namespace FusionEngine;

Scene::Scene(EntityManager *newEntityManager)
{
	entityManager = newEntityManager;
}
Scene::~Scene()
{
	for(std::vector<Entity*>::iterator iter = entities.begin(); 
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
	}
}

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