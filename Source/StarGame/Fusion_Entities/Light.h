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


#ifndef FE_LIGHT_H
#define FE_LIGHTS_H

#include "../glsdk/glm/glm.hpp"
#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glutil/glutil.h"

#include "../Fusion_Renderer/ShaderManager.h"

struct LightBlockGamma
{
	glm::vec4 ambientIntensity;
	float lightAttenuation;
	float maxIntensity;
	float gamma;

	float padding;
};


class SunLight
{
private:
	glm::vec4 lightIntensity;
	glm::vec4 ambientIntensity;	

	glm::vec3 position; 

	float lightAttenuation;

	float maxIntensity;
	float gamma;

private:
	GLuint litProgId;
	GLuint litLightIntensityUnif;
	GLuint litCameraSpaceLightPosUnif;

	GLuint litTextureProgId;
	GLuint litTextureLightIntensityUnif;
	GLuint litTextureCameraSpaceLightPosUnif;

	unsigned int ubtLight;

public:
	SunLight();
	SunLight(glm::vec3 newPosition,
			 glm::vec4 newLightIntensity, glm::vec4 newAmbientIntensity,
			 float newLightAttenuation, 
			 float newMaxIntensity, float newGamma);

	void Render(glutil::MatrixStack &modelMatrix, FusionEngine::ShaderManager shaderManager);
};

#endif