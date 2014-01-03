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


#ifndef FE_COMPOSABLE_H
#define FE_COMPOSABLE_H


#pragma warning(push, 1)
#include <memory>
#include <map>
#include <sstream>
#pragma warning(pop)


#include "../framework/ErrorAPI.h"
#include "IComponent.h"

class Composable
{
private:
	std::map<unsigned int, std::shared_ptr<IComponent>> components;

public:
	virtual ~Composable() {}

	void AddComponent(unsigned int componentID, IComponent *newComponent)
	{
		components.insert(std::make_pair(componentID, std::shared_ptr<IComponent>(newComponent)));
	}
	void RemoveComponent(unsigned int componentID)
	{
		auto componentToRemove = components.find(componentID);
		if (componentToRemove != components.end())
		{
			components.erase(componentToRemove);
		}
		else
		{
			std::ostringstream errorMsg;
			errorMsg << "Component with id " << componentID << " not found!\n";
			HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
		}
	}
};


#endif