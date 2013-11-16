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


/// \ingroup module_StarGame

/// \file Lights.h
/// \brief Implements the different types of lights for the scene.

#ifndef LIGHTS_H
#define LIGHTS_H

#include "../glsdk/glm/glm.hpp"
#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glutil/glutil.h"

#include "../Fusion_Renderer/ShaderManager.h"

/// \struct LightBlockGamma
/// \brief Contains the lighting model information.
struct LightBlockGamma
{
	glm::vec4 ambientIntensity;
	float lightAttenuation;
	float maxIntensity;
	float gamma;

	float padding; ///< Padding for compatibility with GLSL.
};


/// \class SunLight
/// \brief Implements the sun lighting effects.

/// The sun light is not just a simple directional light with only a diffuse color. 
/// If the scene was an FPS game or something similar we won't need the 'complex' 
/// implementation made here, but in our case things are different.
///
/// The distance between the light and the bodies which it illuminates is very small. Therefore
/// a simple directional light won't suit our needs. Things just won't look pretty.
/// 
/// The implementation of this light is based on calculating the gaussian distribution of
/// microfacets and calculating each fragment's color based on it. Simply put, we calculate the 
/// otput color of each pixel based on some roughness of our surface.
///
/// The final lighting model is based on the contribution of the `diffuse color`, 
/// `specular color` and `ambient intensity`. 
///		- The `diffuse color` is based on a light/surface interaction where the light is 
///		  reflected at many angles. 
///		- The `specular color` stands for the shinies of the surface.
///		- The `ambient intensity` is the random light which comes from all other 
///		  reflected lights in the scene. It is kind of 'made up'.
///
/// Other contributions to the final color are made by: 
///		- The `attenuation intensity` which is the decrease of the light's intensity
///		  based on the distance from the light source.
///		- The cosine of the angle of incidence between the light ray and the surface.
///		- The `gaussian term`.
///
/// For further research you can check out 
///	[this tutorial](http://www.arcsynthesis.org/gltut/Illumination/Illumination.html).
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

	/// \fn Render
	/// \brief Applies the light to the scene.
	void Render(glutil::MatrixStack &modelMatrix, 
				FusionEngine::ShaderManager shaderManager);
};

#endif