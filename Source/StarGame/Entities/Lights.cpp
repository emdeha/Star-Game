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
#include "Lights.h"

SunLight::SunLight()
{
	position = glm::vec3();
	lightIntensity = glm::vec4();
	ambientIntensity = glm::vec4();
	baseDiffuseColor = glm::vec4();
	lightAttenuation = 0.0f;
	shininessFactor = 0.0f;
}
SunLight::SunLight(glm::vec3 newPosition,
				   glm::vec4 newLightIntensity, glm::vec4 newAmbientIntensity,
				   glm::vec4 newBaseDiffuseColor,
				   float newLightAttenuation, float newShininesFactor)
{
	position = newPosition;
	lightIntensity = newLightIntensity;
	ambientIntensity = newAmbientIntensity;
	baseDiffuseColor = newBaseDiffuseColor;
	lightAttenuation = newLightAttenuation;
	shininessFactor = newShininesFactor;
}

void SunLight::Render(glutil::MatrixStack &modelMatrix, const LitProgData &lightData)
{
	glm::vec4 position_cameraSpace = modelMatrix.Top() * glm::vec4(position, 1.0f);

	glUseProgram(lightData.theProgram);

	glUniform4fv(lightData.lightIntensityUnif, 1, glm::value_ptr(lightIntensity));
	glUniform4fv(lightData.ambientIntensityUnif, 1, glm::value_ptr(ambientIntensity));
	glUniform4fv(lightData.baseDiffuseColorUnif, 1, glm::value_ptr(baseDiffuseColor));
	glUniform3fv(lightData.cameraSpaceLightPosUnif, 1, glm::value_ptr(position_cameraSpace));
	glUniform1f(lightData.lightAttenuationUnif, lightAttenuation);
	glUniform1f(lightData.shininessFactorUnif, shininessFactor);

	glUseProgram(0);
}