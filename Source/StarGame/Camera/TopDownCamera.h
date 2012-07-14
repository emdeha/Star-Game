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


#ifndef TOP_DOWN_CAMERA_H
#define TOP_DOWN_CAMERA_H

#include <glm/glm.hpp>

class TopDownCamera
{
private:
	glm::vec3 camTarget;

	float height;
	float anglePhi_degs;
	float angleTheta_degs;

public:
	TopDownCamera();
	TopDownCamera(glm::vec3 newCamTarget, 
				  float newHeight, float newAnglePhi_degs, float newAngleTheta_degs);

	glm::mat4 CalcMatrix();

private:
	glm::vec3 ResolveCamPosition();
};

#endif