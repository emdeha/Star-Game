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
#include "../Fusion_Scene/World.h"

#include <time.h>


using namespace FusionEngine;


NewCelestialBody::NewCelestialBody() :
	maxSatelliteCount(0), currentSatelliteCount(1),
	satellites(0), diameter(0.0f),
	offsetFromSun(0.0f), revolutionTimer()
{
	char stringedOffset[15];
	_snprintf(stringedOffset, 15, "satellite%f", offsetFromSun);
	id.assign(stringedOffset);
}

NewCelestialBody::NewCelestialBody(int newMaxSatelliteCount, float newDiameter, 
								   float newOffsetFromSun, float cycleDuration) :
	maxSatelliteCount(newMaxSatelliteCount), currentSatelliteCount(1),
	satellites(0), diameter(newDiameter),
	offsetFromSun(newOffsetFromSun), revolutionTimer(Framework::Timer::TT_LOOP, cycleDuration) 
{
	World::GetWorld().GetEventManager().AddListener(this, FusionEngine::EVENT_ON_CLICK);

	char stringedOffset[15];
	_snprintf(stringedOffset, 15, "satellite%f", offsetFromSun);
	id.assign(stringedOffset);
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

			if(data.isLeftButtonDown && currentSatelliteCount <= maxSatelliteCount)
			{
				AddSatellite(this);
				return true;
			}
		}
		break;
	}
	
	return false;
}

std::string GetSatMesh(int satCount)
{
	std::string satMesh = "";
	switch (satCount)
	{
	case 1:
		satMesh = "fire_planet.obj";
		break;
	case 2: 
		satMesh = "air_planet.obj";
		break;
	case 3: 
		satMesh = "earth_planet.obj";
		break;
	case 4: 
		satMesh = "water_planet.obj";
		break;
	default:
		break;
	}

	return satMesh;
}

bool AddSatellite(NewCelestialBody *celestialBody)
{
	float satOffset = celestialBody->currentSatelliteCount * 1.2f;
	std::shared_ptr<NewCelestialBody> newSat(new NewCelestialBody(0, 0.3f, satOffset, 5.0f));
	celestialBody->satellites.push_back(newSat);

	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());	
	FusionEngine::MeshAssetObject loadedMesh = meshLoader.LoadAssetObject("mesh-files", GetSatMesh(celestialBody->currentSatelliteCount));
	
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
	satRender->rendererType = Render::FE_RENDERER_LIT;
	satRender->shaderProgram = World::GetWorld().GetShaderManager().GetLitProgData().theProgram;
	satRender->vao = loadedMesh.vao;

	FusionEngine::Scene::GetScene().AddEntity(newSat->GetID());
	FusionEngine::Scene::GetScene().AddComponent(newSat->GetID(), satRender);

	FusionEngine::Transform *satTransform = new FusionEngine::Transform();
	satTransform->position = glm::vec3();
	satTransform->rotation = glm::vec3();
	satTransform->scale = glm::vec3(newSat->diameter);
	FusionEngine::Scene::GetScene().AddComponent(newSat->GetID(), satTransform);

	World::GetWorld().GetRenderer().SubscribeForRendering(FusionEngine::Scene::GetScene().GetEntity(newSat->GetID()));

	celestialBody->currentSatelliteCount++;

	return true;
}

void Update(NewCelestialBody *celestialBody)
{
	FusionEngine::ComponentMapper<FusionEngine::Transform> transformData =
		FusionEngine::Scene::GetScene().GetEntityManager()->GetComponentList(FusionEngine::Scene::GetScene().GetEntity("sun"), FusionEngine::CT_TRANSFORM);

	for(auto satellite = celestialBody->satellites.begin(); satellite != celestialBody->satellites.end(); ++satellite)
	{
		// TODO: Get entity by id
		FusionEngine::ComponentMapper<FusionEngine::Transform> satTransformData =
			FusionEngine::Scene::GetScene().GetEntityManager()->GetComponentList(FusionEngine::Scene::GetScene().GetEntity((*satellite)->GetID()), FusionEngine::CT_TRANSFORM);

		(*satellite)->revolutionTimer.Update();
		
		float currentTimeThroughLoop = (*satellite)->revolutionTimer.GetAlpha();

		glutil::MatrixStack relativeTransformStack;
		relativeTransformStack.Translate(transformData[0]->position);
		satTransformData[0]->position.x = sinf(currentTimeThroughLoop * (2.0f * PI)) * (*satellite)->offsetFromSun;
		satTransformData[0]->position.z = cosf(currentTimeThroughLoop * (2.0f * PI)) * (*satellite)->offsetFromSun;
		
		satTransformData[0]->position = glm::vec3(relativeTransformStack.Top() * glm::vec4(satTransformData[0]->position, 1.0f));
	}
}