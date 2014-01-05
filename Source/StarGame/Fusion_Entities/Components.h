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


#ifndef FE_COMPONENTS_H
#define FE_COMPONENTS_H


#pragma warning(push, 1)
#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glm/glm.hpp"
#pragma warning(pop)

#include "IComponent.h"
#include "../Fusion_Entities/CelestialBody.h"


namespace FusionEngine
{
	class RenderComponent : public IComponent
	{
	public:
		enum RenderType
		{
			FE_RENDERER_SIMPLE,
			FE_RENDERER_LIT,
		};

		RenderType renderType;

		GLuint vao;
		GLuint materialUniformBuffer;
		GLuint shaderProgramID;

		virtual ~RenderComponent() {}
	};

	class TransformComponent : public IComponent
	{
	public:
		glm::vec3 position;
		glm::vec3 rotation; // TODO: Use quats.
		glm::vec3 scale;

		virtual ~TransformComponent() {}
	};

	class SkillSatelliteCreationComponent : public IComponent
	{
	public:
		CelestialBodyType satelliteType;

		virtual ~SkillSatelliteCreationComponent() {}
	};

	class SkillGenericComponent : public IComponent
	{
	public:
		int damage;
		float range;

		bool isActive;
		bool isDeployed;

		virtual ~SkillGenericComponent() {}
	};

	
	struct Material
	{
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;
		float shininessFactor;
	};
}


#endif