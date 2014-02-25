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

		virtual RenderComponent* Clone() const 
		{
			return new RenderComponent(*this);
		}
		virtual ~RenderComponent() {}
	};

	class TransformComponent : public IComponent
	{
	public:
		glm::vec3 position;
		glm::vec3 rotation; // TODO: Use quats.
		glm::vec3 scale;

		virtual TransformComponent* Clone() const 
		{
			return new TransformComponent(*this);
		}
		virtual ~TransformComponent() {}
	};

	class SkillSatelliteCreationComponent : public IComponent
	{
	public:
		CelestialBodyType satelliteType;

		virtual SkillSatelliteCreationComponent* Clone() const 
		{
			return new SkillSatelliteCreationComponent(*this);
		}
		virtual ~SkillSatelliteCreationComponent() {}
	};
	
	class SkillSelectorAppliedComponent : public IComponent
	{
	public:
		Utility::Primitives::Circle skillSelector;

		virtual SkillSelectorAppliedComponent* Clone() const 
		{
			return new SkillSelectorAppliedComponent(*this);
		}
		virtual ~SkillSelectorAppliedComponent() {}
	};

	class SkillTimedComponent : public IComponent
	{
	public:
		float damageApplyTime_seconds;
		float damageApplyTimeDuration_seconds;
		float duration_seconds;

		Framework::Timer attackTimer;
		Framework::Timer skillLifeTimer;

		virtual SkillTimedComponent* Clone() const 
		{
			return new SkillTimedComponent(*this);
		}
		virtual ~SkillTimedComponent() {}
	};

	class SkillAnimatedComponent : public IComponent
	{
	public:
		Utility::Primitives::Torus2D anim;

		float currentScale;
		float scaleRate;

		virtual SkillAnimatedComponent* Clone() const 
		{
			return new SkillAnimatedComponent(*this);
		}
		virtual ~SkillAnimatedComponent() {}
	};

	class SkillDefensiveComponent : public IComponent
	{
	public:
		int defensePoints;
		int currentDefensePoints;

		virtual SkillDefensiveComponent* Clone() const 
		{
			return new SkillDefensiveComponent(*this);
		}
		virtual ~SkillDefensiveComponent() {}
	};

	class SkillGenericComponent : public IComponent
	{
	public:
		std::string holderID;

		int damage;
		float range;

		bool isActive;
		bool isDeployed;

		bool isChain;
		bool isDefensive;

		virtual SkillGenericComponent* Clone() const 
		{
			return new SkillGenericComponent(*this);
		}
		virtual ~SkillGenericComponent() {}
	};

	class CollisionComponent : public IComponent
	{
	public:
		enum CollisionType
		{
			FE_COLLISION_CIRCLE,
			FE_COLLISION_TORUS,
		};

		std::string parentObjectID;

		CollisionType cType;
		glm::vec3 center;
		float innerRadius; 
		float outerRadius;
		
		virtual CollisionComponent* Clone() const 
		{
			return new CollisionComponent(*this);
		}
		virtual ~CollisionComponent() {}
	};

	
	struct Material
	{
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;
		float shininessFactor;
	};
}


#endif