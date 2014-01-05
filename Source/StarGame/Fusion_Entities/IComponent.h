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


#ifndef FE_ICOMPONENT_H
#define FE_ICOMPONENT_H


namespace FusionEngine
{	
	enum ComponentType
	{
		FE_COMPONENT_RENDER,
		FE_COMPONENT_TRANSFORM,
		FE_COMPONENT_SKILL_SATELLITE_CREATION,
	};

	class IComponent 
	{
	public:
		virtual ~IComponent() {}
	};
}


#endif