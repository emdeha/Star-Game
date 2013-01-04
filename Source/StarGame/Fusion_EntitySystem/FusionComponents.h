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


#ifndef __FUSION_COMPONENTS_H
#define __FUSION_COMPONENTS_H


#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include "../Fusion_EntitySystem/EntityManager.h"
#include "../Fusion_EntitySystem/Component.h"
#include "../ProgramData/ProgramData.h"
#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/Timer.h"
#include "../framework/Utility.h"
#include "../framework/EventSystem.h"
#include "../Mouse/Mouse.h"



struct Material
{
	glm::vec4 diffuseColor; 
	glm::vec4 specularColor;
	float shininessFactor;

	float padding[3]; ///< Padding for compatibility with GLSL
};

struct LightBlockData
{
	glm::vec4 ambientIntensiy;
	float lightAttenuation;
	float maxIntensity;
	float gamma;

	float padding; ///< Padding for compatibility with GLSL.
};


namespace FusionEngine
{
	class Click : public Component
	{
	public:
		Mouse userMouse;
		glm::mat4 projMatrix;
		glm::mat4 modelMatrix;
		glm::vec4 cameraPosition;
		int windowWidth;
		int windowHeight;
		bool isClicked;

		Click() : Component(CT_CLICKABLE) {}
		virtual ~Click() {}
	};


	class FunctionalBehavior : public Component
	{
	public:
		std::function<void ()> UpdateFunction;
		std::function<void (Event)> OnEventFunction;

		FunctionalBehavior() : Component(CT_FUNCTIONAL_BEHAVIOR) {}
		virtual ~FunctionalBehavior() {}
	};

	
	class RotationOrigin : public Component
	{
	public:
		Framework::Timer revolutionDuration;
		glm::vec3 origin;
		float offsetFromOrigin;

		RotationOrigin() : Component(CT_ROTATE_ORIGIN) {}
		virtual ~RotationOrigin() {}
	};


	class Transform : public Component
	{
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		Transform() : Component(CT_TRANSFORM) {}
		virtual ~Transform() {}
	};


	class RenderUnlit : public Component
	{	
	public:
		std::unique_ptr<Framework::Mesh> mesh;
		glutil::MatrixStack transformStack;
		glm::vec4 color;
		GLuint program;

		void Init(const std::string &meshFileName)
		{
			try
			{
				mesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(meshFileName));
			}
			catch(std::exception &except)
			{
				printf("%s\n", except.what());
				throw;
			}

			transformStack.SetIdentity();
		}


		RenderUnlit() : Component(CT_RENDERABLE_UNLIT) {}
		virtual ~RenderUnlit() {} 
	};


	class RenderLit : public Component
	{
	public:
		std::unique_ptr<Framework::Mesh> mesh;
		glutil::MatrixStack transformStack;
		Material material;
		int materialBlockIndex;
		int materialBlockSize;
		GLuint materialUniformBuffer;
		GLuint program;

		void Init(const std::string &meshFileName)
		{
			try
			{
				mesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(meshFileName));
			}
			catch(std::exception &except)
			{
				printf("%s\n", except.what());
				throw;
			}

			int uniformBufferAlignSize = 0;
			glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);

			materialBlockSize = sizeof(Material);
			materialBlockSize += uniformBufferAlignSize -
				(materialBlockSize % uniformBufferAlignSize);

			glGenBuffers(1, &materialUniformBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, materialUniformBuffer);
			glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, &material, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			transformStack.SetIdentity();
		}

		RenderLit() : Component(CT_RENDERABLE_LIT) {}
		virtual ~RenderLit() {}
	};


	class Light : public Component
	{
	public:
		glm::mat4 modelMatrix;
		glm::vec3 position;
		glm::vec4 intensity;
		LightBlockData lightProperties;
		GLuint shaderProgram;
		GLuint lightUniformBuffer;

		Light() : Component(CT_LIGHT) {}
		virtual ~Light() {}
	};

	
	class Sun : public Component
	{
	public:
		int maxSatelliteCount;
		int currentSatelliteCount;
		float satelliteOffsetIncrement;
		float lastSatelliteOffset;

		Sun() : Component(CT_SUN) {}
		virtual ~Sun() {}
	};


	class Satellite : public Component
	{
	private:
		enum SatelliteType
		{
			FIRE, 
			WATER, 
			AIR,
			EARTH,
		};

	public:
		Framework::Timer rotationDuration;
		glm::vec3 rotationOrigin;
		SatelliteType type;
		float offsetFromSun;

		Satellite() : Component(CT_SATELLITE) {}
		virtual ~Satellite() {}
	};
}

#endif