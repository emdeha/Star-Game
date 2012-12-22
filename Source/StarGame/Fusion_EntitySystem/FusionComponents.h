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


	class RenderMesh : public Component
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


		RenderMesh() : Component(CT_RENDERABLE) {}
		virtual ~RenderMesh() {} 
	};
}

#endif