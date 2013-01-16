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


#ifndef GAME_SPECIFIC_GUI
#define GAME_SPECIFIC_GUI


#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>
#include "../framework/EventSystem.h"
#include "../framework/Utility.h"


#define RESOLUTION 90


class AOESelector
{
private:
	Utility::Primitives::Circle selectorBody;
	glm::vec3 position;
	float radius;
	
public:
	AOESelector() {}
	AOESelector(glm::vec3 newPosition, float newRadius,
				glm::vec4 newColor);

	void Init();

	void Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &simpleData);

	void Update(glm::vec3 newPosition);
};


class SatelliteOrbit
{
private:
	glm::vec4 mainColor; 
	glm::vec4 outlineColor;

	glm::vec4 position;

	float outerRadius;
	float innerRadius;


	Utility::Primitives::Torus2D mainOrbit;
	Utility::Primitives::Torus2D orbitOutlineOne;
	Utility::Primitives::Torus2D orbitOutlineTwo;

public:
	SatelliteOrbit();
	SatelliteOrbit(glm::vec4 newMainColor, glm::vec4 newOutlineColor,
				   glm::vec4 newPosition, 
				   float newOuterRadius, float newInnerRadius,
				   float gamma);

	void Init();

	/// \fn Draw
	/// \brief Draws the orbit.
	void Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &simpleData);
};


#endif