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


#include "stdafx.h"
#include "GameSpecificGUI.h"


AOESelector::AOESelector(glm::vec3 newPosition, float newRadius,
						 glm::vec4 newColor)
{
	selectorBody = 
		Utility::Primitives::Circle(newColor, glm::vec4(newPosition, 0.0f), newRadius, RESOLUTION);

	position = newPosition;
	radius = newRadius;
}

void AOESelector::Init()
{
	selectorBody.Init();
}

void AOESelector::Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &simpleData)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	selectorBody.Draw(modelMatrix, simpleData);

	glDisable(GL_BLEND);
}

void AOESelector::Update(glm::vec3 newPosition)
{
	position = newPosition;
}


SatelliteOrbit::SatelliteOrbit()
{
	mainColor = glm::vec4();
	outlineColor = glm::vec4();

	position = glm::vec4();

	outerRadius = 0.0f;
	innerRadius = 0.0f;
}

SatelliteOrbit::SatelliteOrbit(glm::vec4 newMainColor, glm::vec4 newOutlineColor,
							   glm::vec4 newPosition, 
							   float newOuterRadius, float newInnerRadius,
							   float gamma)
{
	mainColor = Utility::CorrectGamma(newMainColor, gamma);
	outlineColor = Utility::CorrectGamma(newOutlineColor, gamma);
	position = newPosition;
	outerRadius = newOuterRadius;
	innerRadius = newInnerRadius;

	mainOrbit = Utility::Primitives::Torus2D(mainColor,
											 position,
											 innerRadius,
											 outerRadius,
											 90);
	orbitOutlineOne = Utility::Primitives::Torus2D(outlineColor,
												   position,
												   innerRadius - 0.05f,
												   innerRadius,
												   90);
	orbitOutlineTwo = Utility::Primitives::Torus2D(outlineColor,
												   position,
												   outerRadius,
												   outerRadius + 0.05f,
												   90);
}

void SatelliteOrbit::Init()
{
	mainOrbit.Init();
	orbitOutlineOne.Init();
	orbitOutlineTwo.Init();
}

void SatelliteOrbit::Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &simpleData)
{
	// TODO: Play with blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mainOrbit.Draw(modelMatrix, simpleData);

	glDisable(GL_BLEND);

	orbitOutlineOne.Draw(modelMatrix, simpleData);
	orbitOutlineTwo.Draw(modelMatrix, simpleData);
}