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
#include <sstream>
#include <string>
#include <memory>
#include <map>
#pragma warning(pop)

#include "../framework/ErrorAPI.h"
#include "IComponent.h"


namespace FusionEngine
{
	class Composable
	{
	private:
		std::map<unsigned int, std::shared_ptr<IComponent>> components;
		
	protected:
		std::string id;

	public:
		Composable(const std::string &newID) 
			: id(newID) {}
		virtual ~Composable() {}

		void AddComponent(unsigned int componentID, std::shared_ptr<IComponent> newComponent);
		void RemoveComponent(unsigned int componentID);

		std::string GetID() { return id; }

		std::shared_ptr<IComponent> GetComponent(unsigned int componentID);
	};
}


#endif