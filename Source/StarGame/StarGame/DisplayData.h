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


#ifndef DISPLAY_DATA_H
#define DISPLAY_DATA_H

#include <glm/glm.hpp>

struct DisplayData
{
	float gamma;

	float zNear;
	float zFar;

	glm::mat4 projectionMatrix;
	glm::mat4 modelMatrix;

	DisplayData()
	{
		gamma = 2.2f;

		zNear = 1.0f;
		zFar = 1000.0f;
	}
};

#endif