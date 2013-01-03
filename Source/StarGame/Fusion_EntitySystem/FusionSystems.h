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


#ifndef __FUSION_SYSTEMS_H
#define __FUSION_SYSTEMS_H


#include <glm/glm.hpp>
#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <glload/gll.hpp>
#include <algorithm>
#include "../Fusion_EntitySystem/EntityManager.h"
#include "../Fusion_EntitySystem/EntityProcessingSystem.h"
#include "../Fusion_EntitySystem/Component.h"
#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionComponents.h"


#define PI 3.14159f


namespace FusionEngine
{
	class ClickSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<Click> clickInfo = manager->GetComponentList(entity, CT_CLICKABLE);
			ComponentMapper<Transform> transform = manager->GetComponentList(entity, CT_TRANSFORM);

			Utility::Ray mouseRay =
				clickInfo[0]->userMouse.GetPickRay(clickInfo[0]->projMatrix, clickInfo[0]->modelMatrix, 
												   clickInfo[0]->cameraPosition, 
												   clickInfo[0]->windowWidth, clickInfo[0]->windowHeight);

			clickInfo[0]->isClicked = 
				Utility::Intersections::RayIntersectsSphere(mouseRay, transform[0]->position, transform[0]->scale.x);			
		}

	public:
		ClickSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_CLICKABLE_BIT) {}
		virtual ~ClickSystem() {}
	};

	// Should be replaced with an event
	class ClickResponseSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<Click> clickInfo = manager->GetComponentList(entity, CT_CLICKABLE);
			if(clickInfo[0]->isClicked)
			{
				std::printf("CLICKED!!!");
			}
		}
	
	public:
		ClickResponseSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_CLICKABLE_BIT) {}
		virtual ~ClickResponseSystem() {}
	};

	class RotationOriginSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<Transform> transform = manager->GetComponentList(entity, CT_TRANSFORM);
			ComponentMapper<RotationOrigin> rotationOrigin = manager->GetComponentList(entity, CT_ROTATE_ORIGIN);

			rotationOrigin[0]->revolutionDuration.Update();
			float currentTimeThroughLoop = rotationOrigin[0]->revolutionDuration.GetAlpha();
			transform[0]->position.x = cosf(currentTimeThroughLoop * (2.0f * PI)) * rotationOrigin[0]->offsetFromOrigin + rotationOrigin[0]->origin.x;
			transform[0]->position.y = sinf(currentTimeThroughLoop * (2.0f * PI)) * rotationOrigin[0]->offsetFromOrigin + rotationOrigin[0]->origin.y;
		}

	public:
		RotationOriginSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_ROTATE_ORIGIN_BIT) {}
		virtual ~RotationOriginSystem() {}
	};

	class RenderUnlitSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<RenderUnlit> render = manager->GetComponentList(entity, CT_RENDERABLE_UNLIT);
			ComponentMapper<Transform> transform = manager->GetComponentList(entity, CT_TRANSFORM);


			glutil::PushStack push(render[0]->transformStack);
			render[0]->transformStack.Translate(transform[0]->position);
			render[0]->transformStack.RotateX(transform[0]->rotation.x);
			render[0]->transformStack.RotateY(transform[0]->rotation.y);
			render[0]->transformStack.RotateZ(transform[0]->rotation.z);
			render[0]->transformStack.Scale(transform[0]->scale);

			
			glUseProgram(render[0]->program);
			glUniformMatrix4fv(
				glGetUniformLocation(render[0]->program, "modelToCameraMatrix"),
				1, GL_FALSE, glm::value_ptr(render[0]->transformStack.Top()));
			glUniform4f(
				glGetUniformLocation(render[0]->program, "color"),
				render[0]->color.r, render[0]->color.g, render[0]->color.b, render[0]->color.a);

			render[0]->mesh->Render("flat");
			glUseProgram(0);
		}

	public:
		RenderUnlitSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_RENDERABLE_UNLIT_BIT) {}
		virtual ~RenderUnlitSystem() {}
	};

	class RenderLitSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<RenderLit> renderLit = manager->GetComponentList(entity, CT_RENDERABLE_LIT); 
			ComponentMapper<Transform> transform = manager->GetComponentList(entity, CT_TRANSFORM);


			{
				glutil::PushStack push(renderLit[0]->transformStack);
				renderLit[0]->transformStack.Translate(transform[0]->position);
				renderLit[0]->transformStack.RotateX(transform[0]->rotation.x);
				renderLit[0]->transformStack.RotateY(transform[0]->rotation.y);
				renderLit[0]->transformStack.RotateZ(transform[0]->rotation.z);
				renderLit[0]->transformStack.Scale(transform[0]->scale);


				glBindBufferRange(GL_UNIFORM_BUFFER, renderLit[0]->materialBlockIndex, renderLit[0]->materialUniformBuffer,
								  0, sizeof(Material));
				glm::mat3 normalMatrix(renderLit[0]->transformStack.Top());
				normalMatrix = glm::transpose(glm::inverse(normalMatrix));

				glUseProgram(renderLit[0]->program);
				glUniformMatrix4fv(glGetUniformLocation(renderLit[0]->program, "modelToCameraMatrix"),
								   1, GL_FALSE, glm::value_ptr(renderLit[0]->transformStack.Top()));
				glUniformMatrix3fv(glGetUniformLocation(renderLit[0]->program, "normalModelToCameraMatrix"),
								   1, GL_FALSE, glm::value_ptr(normalMatrix));

				renderLit[0]->mesh->Render("lit");
				
				glUseProgram(0);
				glBindBufferBase(GL_UNIFORM_BUFFER, renderLit[0]->materialBlockIndex, 0);
			}
		}

	public:
		RenderLitSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_RENDERABLE_LIT_BIT) {}
		virtual ~RenderLitSystem() {}
	};

	class LightSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<Light> light = manager->GetComponentList(entity, CT_LIGHT);


			glm::vec4 lightPosition_cameraSpace = light[0]->modelMatrix * glm::vec4(light[0]->position, 1.0f);
			glUseProgram(light[0]->shaderProgram);

			glUniform4fv(glGetUniformLocation(light[0]->shaderProgram, "lightIntensity"),
						 1, glm::value_ptr(light[0]->intensity));
			glUniform3fv(glGetUniformLocation(light[0]->shaderProgram, "cameraSpaceLightPos"),
						 1, glm::value_ptr(lightPosition_cameraSpace));

			glBindBuffer(GL_UNIFORM_BUFFER, light[0]->lightUniformBuffer);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(light[0]->lightProperties), &light[0]->lightProperties);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glUseProgram(0);
		}

	public:
		LightSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_LIGHT_BIT) {}
		virtual ~LightSystem() {}
	};
}


#endif