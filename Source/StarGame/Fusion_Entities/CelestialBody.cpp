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
#include "CelestialBody.h"

#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_EntitySystem/FusionSystems.h"
#include "../Fusion_EntitySystem/EntityEvents.h"
#include "../Fusion_Scene/Scene.h"

#include <time.h>


using namespace FusionEngine;


NewCelestialBody::NewCelestialBody() :
	scene(FusionEngine::Scene()), satellites(0), diameter(0.0f),
	offsetFromSun(0.0f), revolutionTimer()
{
}

NewCelestialBody::NewCelestialBody(
	FusionEngine::Scene &newScene, 
	float newDiameter, float newOffsetFromSun, float cycleDuration) :
	scene(newScene),
	satellites(0), diameter(newDiameter),
	offsetFromSun(newOffsetFromSun), revolutionTimer(Framework::Timer::TT_LOOP, cycleDuration) 
{
	scene.GetEventManager()->AddListener(this, FusionEngine::EVENT_ON_CLICK);
}

NewCelestialBody::~NewCelestialBody()
{
	//scene.GetEventManager()->RemoveListener(this, EVENT_ON_CLICK);
}

bool NewCelestialBody::HandleEvent(const FusionEngine::IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch(type)
	{
	case FusionEngine::EVENT_ON_CLICK:
		{
			const OnClickEvent &data = static_cast<const OnClickEvent&>(eventData);

			if(data.isLeftButtonDown)
			{
				AddSatellite(this, &this->scene.GetRenderer(), data.shaderProgram, 0.2f, 4.0f, 3.0f);
				return true;
			}
		}
		break;
	}
	
	return false;
}


bool AddSatellite(NewCelestialBody *celestialBody, FusionEngine::Renderer *renderer,
				  GLuint shaderProg, 
				  float newDiameter, float newOffsetFromSun, float cycleDuration)
{
	srand(time(0));
	std::ostringstream satName;
	satName<<"satellite"<<rand() % 9999;

	// Load mesh
	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());
	FusionEngine::MeshAssetObject loadedMesh = meshLoader.LoadAssetObject("mesh-files", "sun.obj");
	
	FusionEngine::Render *satRender = new FusionEngine::Render();
	
	std::vector<std::shared_ptr<FusionEngine::MeshEntry>> meshEntries = loadedMesh.GetMeshEntries();
	for(auto meshEntry = meshEntries.begin(); meshEntry != meshEntries.end(); ++meshEntry)
	{
		satRender->mesh.AddEntry((*meshEntry));
	}
	std::vector<std::shared_ptr<Texture2D>> textures = loadedMesh.GetTextures();
	for(auto texture = textures.begin(); texture != textures.end(); ++texture)
	{
		satRender->mesh.AddTexture((*texture));
	}
	satRender->rendererType = FusionEngine::Render::FE_RENDERER_LIT;
	satRender->shaderProgram = shaderProg;
	satRender->vao = loadedMesh.vao;

	celestialBody->scene.AddEntity(satName.str());
	//FusionEngine::FunctionalSystem<NewCelestialBody> *satFunctional = 
	//	new FusionEngine::FunctionalSystem<NewCelestialBody>(celestialBody->scene.GetEventManager(), 
	//									   celestialBody->scene.GetEntityManager());
	//celestialBody->scene.AddSystem(satFunctional);
	celestialBody->scene.AddComponent(satName.str(), satRender);

	FusionEngine::Transform *satTransform = new FusionEngine::Transform();
	satTransform->position = glm::vec3();
	satTransform->rotation = glm::vec3();
	satTransform->scale = glm::vec3(celestialBody->diameter);
	celestialBody->scene.AddComponent(satName.str(), satTransform);

	//FusionEngine::Functional<NewCelestialBody> *satFuncComp = new FusionEngine::Functional<NewCelestialBody>();
	//satFuncComp->UpdateFunction = Update;
	//celestialBody->scene.AddComponent("satellite", satFuncComp);


	std::shared_ptr<NewCelestialBody> 
		newSat(new NewCelestialBody(celestialBody->scene, 1.0f, 2.0f, 5.0f));
	celestialBody->satellites.push_back(newSat);
	//newSat->parent = celestialBody;
	renderer->SubscribeForRendering(celestialBody->scene.GetEntityManager(), celestialBody->scene.GetEntity(satName.str()));

	return true;
}

void Update(NewCelestialBody *celestialBody)
{
	FusionEngine::ComponentMapper<FusionEngine::Transform> transformData =
		celestialBody->scene.GetEntityManager()->GetComponentList(celestialBody->scene.GetEntity("sun"), FusionEngine::CT_TRANSFORM);

	for(auto satellite = celestialBody->satellites.begin(); satellite != celestialBody->satellites.end(); ++satellite)
	{
		// TODO: Get entity by id
		FusionEngine::ComponentMapper<FusionEngine::Transform> satTransformData =
			celestialBody->scene.GetEntityManager()->GetComponentList(celestialBody->scene.GetEntity("satellite"), FusionEngine::CT_TRANSFORM);

		(*satellite)->revolutionTimer.Update();
		
		float currentTimeThroughLoop = (*satellite)->revolutionTimer.GetAlpha();
		
		satTransformData[0]->position = transformData[0]->position;

		satTransformData[0]->position.x += sinf(currentTimeThroughLoop * (2.0f * PI)) * (*satellite)->offsetFromSun;
		satTransformData[0]->position.z += cosf(currentTimeThroughLoop * (2.0f * PI)) * (*satellite)->offsetFromSun;
	}
}