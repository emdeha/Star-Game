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


#ifndef ENTITY_SYSTEM_H
#define ENTITY_SYSTEM_H


#include <map>
#include <vector>

/*
typedef int FamilyId;

struct EntitySystem;
struct Component {};


struct Entity
{
	static std::auto_ptr<EntitySystem> entitySystem;
	std::map<FamilyId, std::shared_ptr<Component>> components;

	Entity() {}

	template<typename Type> 
	std::shared_ptr<Type> GetAs()
	{
		return entitySystem->GetComponent<Type>(this);
	}
};


std::auto_ptr<EntitySystem> Entity::entitySystem = std::auto_ptr<EntitySystem>();
struct EntitySystem
{
protected:
	std::multimap<FamilyId, std::shared_ptr<Entity>> componentStore;
	
public:
	EntitySystem()
	{
		Entity::entitySystem = std::auto_ptr<EntitySystem>(this);
	}
	

	template<typename T>
	std::auto_ptr<T> GetComponent(std::auto_ptr<Entity> entity)
	{
		return (T*)entity->components[T::familyId];
	}

	template<typename T>
	void GetEntities(std::vector<std::shared_ptr<Entity> &result)
	{
		auto iterPair = componentStore.equal_range(T::familyId);
		for(auto iter = iterPair.first; iter != iterPair.second; ++iter)
		{
			result.push_back(iter->second);
		}
	}

	template<typename T>
	void AddComponent(std::shared_ptr<Entity> entity, std::shared_ptr<T> component)
	{
		componentStore.insert(std::pair<FamilyId, std::shared_ptr<Entity>>(T::familyId, entity));
		entiy->components.insert(std::pair<FamilyId, std::shared_ptr<Component>>(T::familyId, component));
	}
};
*/


#endif