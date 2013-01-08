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
	glutil::PushStack push(modelMatrix);

	modelMatrix.Translate(position);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	selectorBody.Draw(modelMatrix, simpleData);

	glDisable(GL_BLEND);
}

void AOESelector::Update(glm::vec3 newPosition)
{
	position = newPosition;
}