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


#ifndef FE_FUSION_SYSTEMS_H
#define FE_FUSION_SYSTEMS_H


#include "../glsdk/glm/glm.hpp"
#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glutil/glutil.h"
#include "../glsdk/glload/gll.hpp"
#include <algorithm>

#include "../Fusion_EntitySystem/EntityManager.h"
#include "../Fusion_EntitySystem/EntityEvents.h"
#include "../Fusion_EntitySystem/EntityProcessingSystem.h"
#include "../Fusion_EntitySystem/Component.h"
#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_Scene/World.h"


#define PI 3.14159f


namespace FusionEngine
{
	template<class ObjectToUpdate>
	class FunctionalSystem : public EntityProcessingSystem
    {
    protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
        {
			ComponentMapper<Functional<ObjectToUpdate>> functionalData = manager->GetComponentList(entity, CT_FUNCTIONAL_BEHAVIOR);

			functionalData[0]->updatedObject->Update();
        }

    public:
		FunctionalSystem(EventManager *eventManager, EntityManager *entityManager) 
            : EntityProcessingSystem(eventManager, entityManager, CT_FUNCTIONAL_BEHAVIOR_BIT) {}
        virtual ~FunctionalSystem() {}
    };


	class CollisionSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<Collidable> collidableData = manager->GetComponentList(entity, CT_COLLISION);
			if (collidableData[0]->isForCheck)
			{
				ComponentMapper<Transform> transformData = manager->GetComponentList(entity, CT_TRANSFORM);
						
				FusionEngine::DisplayData displayData = World::GetWorld().GetDisplayData();

				Utility::Ray mouseRay = 
					World::GetWorld().GetMouse().GetPickRay(displayData.projectionMatrix, displayData.modelMatrix.Top(),
															glm::vec4(World::GetWorld().GetCamera().ResolveCamPosition(), 1.0f), 
															displayData.windowWidth, displayData.windowHeight);
				
				if (Utility::Intersections::RayIntersectsSphere(mouseRay, transformData[0]->position, transformData[0]->scale.x))
				{
					World::GetWorld().GetEventManager().FireEvent(FusionEngine::OnClickEvent(FusionEngine::EVENT_ON_CLICK, true, "sun"));
				}
			}
		}

	public:
		CollisionSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_COLLISION_BIT) {}
		virtual ~CollisionSystem() {}
	};


	/////////////////////
	///  Old Systems  ///
	/////////////////////


	class ClickSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<Click> clickData = manager->GetComponentList(entity, CT_CLICKABLE);
			ComponentMapper<Transform> transformData = manager->GetComponentList(entity, CT_TRANSFORM);

			Utility::Ray mouseRay =
				clickData[0]->userMouse.GetPickRay(clickData[0]->projMatrix, clickData[0]->modelMatrix, 
												   clickData[0]->cameraPosition, 
												   clickData[0]->windowWidth, clickData[0]->windowHeight);

			clickData[0]->isClicked = 
				Utility::Intersections::RayIntersectsSphere(mouseRay, transformData[0]->position, transformData[0]->scale.x);			
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
			ComponentMapper<Click> clickData = manager->GetComponentList(entity, CT_CLICKABLE);
			if(clickData[0]->isClicked)
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
			ComponentMapper<Transform> transformData = 
				manager->GetComponentList(entity, CT_TRANSFORM);
			ComponentMapper<RotationOrigin> rotationOriginData = 
				manager->GetComponentList(entity, CT_ROTATE_ORIGIN);

			rotationOriginData[0]->revolutionDuration.Update();
			float currentTimeThroughLoop = rotationOriginData[0]->revolutionDuration.GetAlpha();
			transformData[0]->position.x = 
				cosf(currentTimeThroughLoop * (2.0f * PI)) * 
				rotationOriginData[0]->offsetFromOrigin + rotationOriginData[0]->origin.x;
			transformData[0]->position.y = 
				sinf(currentTimeThroughLoop * (2.0f * PI)) * 
				rotationOriginData[0]->offsetFromOrigin + rotationOriginData[0]->origin.y;
		}

	public:
		RotationOriginSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_ROTATE_ORIGIN_BIT) {}
		virtual ~RotationOriginSystem() {}
	};

	class RenderFromGeneratedDataSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<RenderGenData> renderGenData = manager->GetComponentList(entity, CT_RENDERABLE_GEN_DATA);
			ComponentMapper<Transform> transformData = manager->GetComponentList(entity, CT_TRANSFORM);

			if(renderGenData[0]->isVisible)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glUseProgram(renderGenData[0]->shaderProgram);
				glBindVertexArray(renderGenData[0]->vao);
				{
					glutil::PushStack push(renderGenData[0]->transformStack);

					renderGenData[0]->transformStack.Translate(transformData[0]->position);
					renderGenData[0]->transformStack.RotateX(transformData[0]->rotation.x);
					renderGenData[0]->transformStack.RotateY(transformData[0]->rotation.y);
					renderGenData[0]->transformStack.RotateZ(transformData[0]->rotation.z);
					renderGenData[0]->transformStack.Scale(transformData[0]->scale);

					glUniformMatrix4fv(
						glGetUniformLocation(renderGenData[0]->shaderProgram, "modelToCameraMatrix"),
						1, GL_FALSE, glm::value_ptr(renderGenData[0]->transformStack.Top()));
					glUniform4f(
						glGetUniformLocation(renderGenData[0]->shaderProgram, "color"),
						renderGenData[0]->color.r, renderGenData[0]->color.g, 
						renderGenData[0]->color.b, renderGenData[0]->color.a);

					glBindBuffer(GL_ARRAY_BUFFER, renderGenData[0]->vertexBO);
					glEnableVertexAttribArray(
						glGetAttribLocation(renderGenData[0]->shaderProgram, "position"));
					glVertexAttribPointer(
						glGetAttribLocation(renderGenData[0]->shaderProgram, "position"), 
						4, GL_FLOAT, GL_FALSE, 0, 0);

					glDrawElements(GL_TRIANGLE_STRIP, renderGenData[0]->indicesCount, GL_UNSIGNED_SHORT, 0);
				}
				glUseProgram(0);

				glDisable(GL_BLEND);
			}
		}

	public:
		RenderFromGeneratedDataSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_RENDERABLE_GEN_DATA_BIT) {}
		virtual ~RenderFromGeneratedDataSystem() {}
	};

	class RenderUnlitSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<RenderUnlit> renderUnlitData = manager->GetComponentList(entity, CT_RENDERABLE_UNLIT);
			ComponentMapper<Transform> transformData = manager->GetComponentList(entity, CT_TRANSFORM);


			glutil::PushStack push(renderUnlitData[0]->transformStack);
			renderUnlitData[0]->transformStack.Translate(transformData[0]->position);
			renderUnlitData[0]->transformStack.RotateX(transformData[0]->rotation.x);
			renderUnlitData[0]->transformStack.RotateY(transformData[0]->rotation.y);
			renderUnlitData[0]->transformStack.RotateZ(transformData[0]->rotation.z);
			renderUnlitData[0]->transformStack.Scale(transformData[0]->scale);

			
			glUseProgram(renderUnlitData[0]->program);
			glUniformMatrix4fv(
				glGetUniformLocation(renderUnlitData[0]->program, "modelToCameraMatrix"),
				1, GL_FALSE, glm::value_ptr(renderUnlitData[0]->transformStack.Top()));
			glUniform4f(
				glGetUniformLocation(renderUnlitData[0]->program, "color"),
				renderUnlitData[0]->color.r, renderUnlitData[0]->color.g, 
				renderUnlitData[0]->color.b, renderUnlitData[0]->color.a);

			renderUnlitData[0]->mesh->Render("flat");
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
			ComponentMapper<RenderLit> renderLitData = manager->GetComponentList(entity, CT_RENDERABLE_LIT); 
			ComponentMapper<Transform> transformData = manager->GetComponentList(entity, CT_TRANSFORM);


			{
				glutil::PushStack push(renderLitData[0]->transformStack);
				renderLitData[0]->transformStack.Translate(transformData[0]->position);
				renderLitData[0]->transformStack.RotateX(transformData[0]->rotation.x);
				renderLitData[0]->transformStack.RotateY(transformData[0]->rotation.y);
				renderLitData[0]->transformStack.RotateZ(transformData[0]->rotation.z);
				renderLitData[0]->transformStack.Scale(transformData[0]->scale);


				glBindBufferRange(GL_UNIFORM_BUFFER, renderLitData[0]->materialBlockIndex,
								  renderLitData[0]->materialUniformBuffer,
								  0, sizeof(Material));
				glm::mat3 normalMatrix(renderLitData[0]->transformStack.Top());
				normalMatrix = glm::transpose(glm::inverse(normalMatrix));

				glUseProgram(renderLitData[0]->program);
				glUniformMatrix4fv(glGetUniformLocation(renderLitData[0]->program, "modelToCameraMatrix"),
								   1, GL_FALSE, glm::value_ptr(renderLitData[0]->transformStack.Top()));
				glUniformMatrix3fv(glGetUniformLocation(renderLitData[0]->program, "normalModelToCameraMatrix"),
								   1, GL_FALSE, glm::value_ptr(normalMatrix));

				renderLitData[0]->mesh->Render("lit");
				
				glUseProgram(0);
				glBindBufferBase(GL_UNIFORM_BUFFER, renderLitData[0]->materialBlockIndex, 0);
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
			ComponentMapper<Light> lightData = manager->GetComponentList(entity, CT_LIGHT);


			glm::vec4 lightPosition_cameraSpace = 
				lightData[0]->modelMatrix * glm::vec4(lightData[0]->position, 1.0f);
			glUseProgram(lightData[0]->shaderProgram);

			glUniform4fv(glGetUniformLocation(lightData[0]->shaderProgram, "lightIntensity"),
						 1, glm::value_ptr(lightData[0]->intensity));
			glUniform3fv(glGetUniformLocation(lightData[0]->shaderProgram, "cameraSpaceLightPos"),
						 1, glm::value_ptr(lightPosition_cameraSpace));

			glBindBuffer(GL_UNIFORM_BUFFER, lightData[0]->lightUniformBuffer);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, 
							sizeof(lightData[0]->lightProperties), &lightData[0]->lightProperties);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glUseProgram(0);
		}

	public:
		LightSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_LIGHT_BIT) {}
		virtual ~LightSystem() {}
	};


	class SatelliteUpdateSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<Satellite> satelliteData = manager->GetComponentList(entity, CT_SATELLITE);
			ComponentMapper<Transform> transformData = manager->GetComponentList(entity, CT_TRANSFORM);


			satelliteData[0]->rotationDuration.Update();
			float currentTimeThroughLoop = satelliteData[0]->rotationDuration.GetAlpha();
			transformData[0]->position.x = 
				cosf(currentTimeThroughLoop * (2.0f * PI)) * 
				satelliteData[0]->offsetFromSun + satelliteData[0]->rotationOrigin.x;
			transformData[0]->position.y = 
				sinf(currentTimeThroughLoop * (2.0f * PI)) * 
				satelliteData[0]->offsetFromSun + satelliteData[0]->rotationOrigin.y;
		}

	public:
		SatelliteUpdateSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_SATELLITE_BIT) {}
		virtual ~SatelliteUpdateSystem() {}
	};
	/*class SunUpdateSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<Sun> sun = manager->GetComponentList(entity, CT_SUN);			
		}

	public:
		SunUpdateSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_SUN_BIT) {}
		virtual ~SunUpdateSystem() {}
	};*/

	
	/*class SatelliteOrbitRenderSystem : public EntityProcessingSystem
	{
	protected:
		virtual void ProcessEntity(EntityManager *manager, Entity *entity)
		{
			ComponentMapper<SatelliteOrbit> satelliteOrbitData = manager->GetComponentList(entity, CT_SATELLITE_ORBIT);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			satelliteOrbitData[0]->mainOrbit.Draw(satelliteOrbitData[0]->transformStack, 
												  
		}

	public:
		SatelliteOrbitRenderSystem(EventManager *eventManager, EntityManager *entityManager)
			: EntityProcessingSystem(eventManager, entityManager, CT_SATELLITE_ORBIT_BIT) {}
		virtual ~SatelliteOrbitRenderSystem() {}
	};*/
}


#endif