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


#ifndef MATERIAL_BLOCK
#define MATERIAL_BLOCK


#include "../glsdk/glm/glm.hpp"


/// \struct MaterialBlock
/// \brief Contains the material characteristics of an object.
// TODO: Move in a separate file
struct MaterialBlock
{
	glm::vec4 diffuseColor; 
	glm::vec4 specularColor; 
	float shininessFactor;

	float padding[3]; ///< Padding for compatibility with GLSL
};

#endif