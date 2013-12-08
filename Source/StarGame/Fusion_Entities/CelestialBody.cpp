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


CelestialBody::CelestialBody() :
	maxSatelliteCount(0), currentSatelliteCount(1),
	satellites(0), diameter(0.0f),
	offsetFromSun(0.0f), currentRotationAngle(0.0f), angularVelocity(5.0f)
{
	std::ostringstream name;
	name << "satellite" << offsetFromSun;
	id = name.str();
}

CelestialBody::CelestialBody(int newMaxSatelliteCount, float newDiameter, 
							 float newOffsetFromSun) :
	maxSatelliteCount(newMaxSatelliteCount), currentSatelliteCount(1),
	satellites(0), diameter(newDiameter),
	offsetFromSun(newOffsetFromSun),
	currentRotationAngle(0.0f), angularVelocity(5.0f)
{
	World::GetWorld().GetEventManager().AddListener(this, FusionEngine::EVENT_ON_CLICK);

	if (offsetFromSun <= 0.0f)
	{
		std::ostringstream name;
		name << "sun";
		id = name.str();
	}
	else
	{
		std::ostringstream name;
		name << "satellite" << offsetFromSun;
		id = name.str();
	}
}

CelestialBody::~CelestialBody()
{
	//scene.GetEventManager()->RemoveListener(this, EVENT_ON_CLICK);
}

bool CelestialBody::HandleEvent(const FusionEngine::IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch(type)
	{
	case FusionEngine::EVENT_ON_CLICK:
		{
			const OnClickEvent &data = static_cast<const OnClickEvent&>(eventData);
	
			if(data.isLeftButtonDown && data.objectId == id && currentSatelliteCount <= maxSatelliteCount)
			{
				AddSatellite();
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

bool CelestialBody::AddSatellite()
{
	float satOffset = this->currentSatelliteCount * 1.2f;
	std::shared_ptr<CelestialBody> newSat(new CelestialBody(0, 0.3f, satOffset));
	this->satellites.push_back(newSat);

	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());	
	FusionEngine::MeshAssetObject loadedMesh = meshLoader.LoadAssetObject("mesh-files", GetSatMesh(this->currentSatelliteCount));
	
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
	satRender->shaderProgram = 
		World::GetWorld().GetShaderManager().GetProgram(FusionEngine::FE_PROGRAM_LIT).programId;
	satRender->vao = loadedMesh.vao;

	GetScene().AddEntity(newSat->GetID());
	GetScene().AddComponent(newSat->GetID(), satRender);

	FusionEngine::Transform *satTransform = new FusionEngine::Transform();
	satTransform->position = glm::vec3();
	satTransform->rotation = glm::vec3();
	satTransform->scale = glm::vec3(newSat->diameter);
	GetScene().AddComponent(newSat->GetID(), satTransform);

	World::GetWorld().GetRenderer().SubscribeForRendering(GetScene().GetEntity(newSat->GetID()));

	this->currentSatelliteCount++;

	return true;
}

void CelestialBody::Update()
{
	FusionEngine::ComponentMapper<FusionEngine::Transform> transformData =
		GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity("sun"), FusionEngine::CT_TRANSFORM);

	for(auto satellite = this->satellites.begin(); satellite != this->satellites.end(); ++satellite)
	{
		// TODO: Get entity by id
		FusionEngine::ComponentMapper<FusionEngine::Transform> satTransformData =
			GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity((*satellite)->GetID()), FusionEngine::CT_TRANSFORM);

		
		glutil::MatrixStack relativeTransformStack;
		relativeTransformStack.Translate(transformData[0]->position);
		relativeTransformStack.RotateY((*satellite)->currentRotationAngle);
		(*satellite)->currentRotationAngle += (*satellite)->angularVelocity * World::GetWorld().interpolation;
		if ((*satellite)->currentRotationAngle >= 360.0f)
		{
			(*satellite)->currentRotationAngle = 0.0f;
		}
		float offset = (*satellite)->offsetFromSun;
		satTransformData[0]->position = glm::vec3(offset, 0.0f, offset);
		
		satTransformData[0]->position = glm::vec3(relativeTransformStack.Top() * glm::vec4(satTransformData[0]->position, 1.0f));
	}
}