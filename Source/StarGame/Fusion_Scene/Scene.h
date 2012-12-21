//Copyright 2012, 2013 Tsvetan Tsvetanov
//This file is part of the Star Game.
//
//The Star Game is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//The Star Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with the Star Game.  If not, see <http://www.gnu.org/licenses/>.


#ifndef __SCENE_H
#define __SCENE_H


#include "../Fusion_EntitySystem/EventManager.h"
#include "../Fusion_EntitySystem/EntityManager.h"
#include "../Fusion_EntitySystem/EntityProcessingSystem.h"
#include "../Fusion_EntitySystem/Component.h"
#include "../Fusion_EntitySystem/ComponentMapper.h"
#include <vector>


namespace FusionEngine
{
	class Scene
	{
	private:
		typedef std::vector<std::pair<std::string, std::shared_ptr<Entity>>> EntitiesMap;
		typedef std::vector<std::shared_ptr<Component>> ComponentsVector;
		typedef std::vector<std::shared_ptr<EntityProcessingSystem>> SystemsVector;


		std::unique_ptr<EventManager> eventManager;
		std::unique_ptr<EntityManager> entityManager;
		EntitiesMap entities;
		ComponentsVector components;
		SystemsVector systems;

	public:
		Scene() {}
		//Scene(EntityManager *newEntityManager);
		~Scene();

		void Init();

		void AddEntity(const std::string &entityTag);
		void AddSystem(EntityProcessingSystem *system);
		void AddComponent(const std::string &entityTag, Component *component);
		

		EntityManager *GetEntityManager();
		EventManager *GetEventManager();

		
		void ProcessSystems();
		/*
		void InsertSystem(EntityProcessingSystem *system);
		void InsertComponent(Entity *entity, Component *component);

		void ProcessSystems();


		EntityManager *GetEntityManager();
		const ComponentList &GetComponentList(Entity *entity, ComponentType type);*/
	};
};


#endif