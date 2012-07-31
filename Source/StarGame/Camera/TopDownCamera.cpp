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
#include "TopDownCamera.h"


TopDownCamera::TopDownCamera()
{
	camTarget = glm::vec3();
	height = 0.0f;
	anglePhi_degs = 0.0f;
	angleTheta_degs = 0.0f;
}
TopDownCamera::TopDownCamera(glm::vec3 newCamTarget,
							 float newHeight, float newAnglePhi_degs, float newAngleTheta_degs)
{
	camTarget = newCamTarget;
	height = newHeight;
	anglePhi_degs = newAnglePhi_degs;
	angleTheta_degs = newAngleTheta_degs;
}

glm::vec3 TopDownCamera::ResolveCamPosition()
{
	float phi = glm::radians(anglePhi_degs);
	float theta = glm::radians(angleTheta_degs);
	
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);	
	float sinPhi = sinf(phi);
	float cosPhi = cosf(phi);

	glm::vec3 dirToCamera(sinTheta * cosPhi, cosTheta, sinTheta * sinPhi);

	return (dirToCamera * height) + camTarget;
}

glm::mat4 TopDownCamera::CalcMatrix()
{
	glm::vec3 camPos = ResolveCamPosition();

	glm::vec3 lookDir = glm::normalize(camTarget - camPos);
	glm::vec3 upDir = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 rightDir = glm::normalize(glm::cross(lookDir, upDir));
	glm::vec3 perpUpDir = glm::cross(rightDir, lookDir);

	glm::mat4 rotMat(1.0f);
	rotMat[0] = glm::vec4(rightDir, 0.0f);
	rotMat[1] = glm::vec4(perpUpDir, 0.0f);
	rotMat[2] = glm::vec4(-lookDir, 0.0f);

	rotMat = glm::transpose(rotMat);

	glm::mat4 transMat(1.0f);
	transMat[3] = glm::vec4(-camPos, 1.0f);

	return rotMat * transMat;
}