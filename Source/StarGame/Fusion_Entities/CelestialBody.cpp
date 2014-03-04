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

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Scene/World.h"
#include "../Fusion_Entities/Components.h"

#include "../Fusion_Entities/Chaining.h"
#include "../Fusion_Entities/Shield.h"
#include "../Fusion_Entities/Frost.h"

#pragma warning(push, 1)
#include <time.h>
#pragma warning(pop)


using namespace FusionEngine;


std::string GetSatMesh(CelestialBodyType satType)
{
	std::string satMesh = "";
	switch (satType)
	{
	case FE_FIRE_SAT:
		satMesh = "fire_planet.obj";
		break;
	case FE_AIR_SAT:
		satMesh = "air_planet.obj";
		break;
	case FE_EARTH_SAT:
		satMesh = "earth_planet.obj";
		break;
	case FE_WATER_SAT:
		satMesh = "water_planet.obj";
		break;
	default:
		std::ostringstream errorMsg;
		errorMsg << "no such satType: " << (int)satType;
		HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
		return "";
	}

	return satMesh;
}

float GetSatelliteOffset(CelestialBodyType satType)
{
	float satOffset = 0.0f;
	switch (satType)
	{
	case FE_FIRE_SAT:
		satOffset = 1.2f;
		break;
	case FE_AIR_SAT:
		satOffset = 2.4f;
		break;
	case FE_EARTH_SAT:
		satOffset = 3.6f;
		break;
	case FE_WATER_SAT:
		satOffset = 4.8f;
		break;
	default:
		std::ostringstream errorMsg;
		errorMsg << "no such satType: " << (int)satType;
		HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
		return -1.0f;
	}

	return satOffset;
}


CelestialBody::CelestialBody() :
	Composable(""),
	maxSatelliteCount(0), currentSatelliteCount(1),
	satellites(0), diameter(0.0f),
	offsetFromSun(0.0f), currentRotationAngle(0.0f), angularVelocity(5.0f)
{
	srand(time(0));
	std::ostringstream name;
	name << "satellite" << rand()%1000;
	id = name.str();
}

CelestialBody::CelestialBody(CelestialBodyType newType,
							 int newMaxSatelliteCount, float newDiameter, 
							 float newOffsetFromSun) :
	Composable(""),
	type(newType),	
	maxSatelliteCount(newMaxSatelliteCount), currentSatelliteCount(1),
	satellites(0), diameter(newDiameter),
	offsetFromSun(newOffsetFromSun),
	currentRotationAngle(0.0f), angularVelocity(5.0f)
{
	World::GetWorld().GetEventManager().AddListener(this, FusionEngine::EVENT_ON_CLICK);

	if (type == FE_SUN)
	{
		std::ostringstream name;
		name << "sun";
		id = name.str();
	}
	else
	{
		srand(time(0));
		std::ostringstream name;
		name << "satellite" << rand()%1000;
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
	switch (type)
	{
	case FusionEngine::EVENT_ON_CLICK:
		{
			const OnClickEvent &data = static_cast<const OnClickEvent&>(eventData);
	
			//if (data.isLeftButtonDown && data.objectId == id && currentSatelliteCount <= maxSatelliteCount)
			//{
			//	AddSatellite();
			//	return true;
			//}
		}
		break;
	}
	
	return false;
}

bool CelestialBody::AddSatellite(CelestialBodyType satType)
{
	if (currentSatelliteCount > maxSatelliteCount)
	{
		return false;
	}

	float satOffset = GetSatelliteOffset(satType); 
	auto newSat = std::make_shared<CelestialBody>(satType, 0, 0.3f, satOffset);
	
	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());

	FusionEngine::MeshAssetObject sunMesh = meshLoader.LoadAssetObject("mesh-files", GetSatMesh(satType));
	
	auto satRenderComponent = std::make_shared<RenderComponent>();
	satRenderComponent->vao = sunMesh.vao;
	satRenderComponent->shaderProgramID = GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_LIT_TEXTURE).programId;
	satRenderComponent->renderType = RenderComponent::FE_RENDERER_LIT;
	
	newSat->AddComponent(FE_COMPONENT_RENDER, satRenderComponent);

	auto satTransformComponent = std::make_shared<TransformComponent>();
	satTransformComponent->position = glm::vec3();
	const float RADIUS = 0.4f;
	satTransformComponent->scale = glm::vec3(RADIUS, RADIUS, RADIUS);
	satTransformComponent->rotation = glm::vec3();

	newSat->AddComponent(FE_COMPONENT_TRANSFORM, satTransformComponent);

	auto satCollisionComponent = std::make_shared<CollisionComponent>();
	satCollisionComponent->center = glm::vec3();
	satCollisionComponent->cType = CollisionComponent::FE_COLLISION_CIRCLE;
	satCollisionComponent->innerRadius = RADIUS;

	newSat->AddComponent(FE_COMPONENT_COLLISION, satCollisionComponent);
	
	satellites.push_back(newSat);
	currentSatelliteCount++;

	GetWorld().GetRenderer().SubscribeForRendering(newSat->GetID(), sunMesh);

	auto loadedSkills = GetWorld().GetLoadedEntityData().GetLoadedSkills();
	for (auto skill = loadedSkills.begin(); skill != loadedSkills.end(); ++skill)
	{
		if ((*skill).first.find("sat") != std::string::npos)
		{
			newSat->AddSkill((*skill).first, (*skill).second);
		}
	}

	return true;
}

bool CelestialBody::AddSkill(const std::string &skillName, const SkillData &newSkillData) 
{
	std::string skillNameWithID = skillName + id;
	auto newSkill = std::shared_ptr<Skill>(new Skill(skillNameWithID, 
													 newSkillData.fusionCombination[0],
													 newSkillData.fusionCombination[1],
													 newSkillData.fusionCombination[2], 
													 newSkillData.applyCost, newSkillData.researchCost));

	newSkill->SetOnClickCallback(newSkillData.onClick);
	newSkill->SetOnFusionCompletedCallback(newSkillData.onFusionCompleted);
	newSkill->SetOnUpdateCallback(newSkillData.onUpdate);

	for (auto component = newSkillData.components.begin(); component != newSkillData.components.end();
		 ++component)
	{
		std::shared_ptr<IComponent> newComponent = 
			std::shared_ptr<IComponent>((*component).second->Clone());
		if ((*component).first == FE_COMPONENT_SKILL_GENERIC)
		{
			SkillGenericComponent *generic = static_cast<SkillGenericComponent*>(newComponent.get());
			generic->holderID = id;
		}
		newSkill->AddComponent((*component).first, newComponent);
	}

	skills.push_back(newSkill);
	GetWorld().AddFusionSequence(skillName, newSkill->GetFusionCombination());

	return true;
}

void CelestialBody::UpdateCollision()
{
	auto collidableObjects = GetWorld().GetObjectsWithComponent(FE_COMPONENT_COLLISION);

	for (auto collider = collidableObjects.begin(); collider != collidableObjects.end(); ++collider)
	{
		if ((*collider).get()->GetID() != id)
		{
			CollisionComponent *colliderCollision = static_cast<CollisionComponent*>(
				(*collider).get()->GetComponent(FE_COMPONENT_COLLISION).get());
			CollisionComponent *satCollision = static_cast<CollisionComponent*>(
				GetComponent(FE_COMPONENT_COLLISION).get());

			switch (colliderCollision->cType)
			{
			case CollisionComponent::FE_COLLISION_CIRCLE:
				{
					float distanceBetweenColliders = glm::length(colliderCollision->center - satCollision->center); 
					float minDistance = colliderCollision->innerRadius + satCollision->innerRadius;
					if (distanceBetweenColliders < minDistance)
					{
						// handle circle-based collision
					}  
				}
				break;
			case CollisionComponent::FE_COLLISION_TORUS:
				{
					float distanceBetweenColliders = glm::length(colliderCollision->center - satCollision->center); 
					float minDistance = colliderCollision->innerRadius + satCollision->innerRadius;
					float maxDistance = colliderCollision->outerRadius + satCollision->innerRadius;
					if (distanceBetweenColliders < minDistance && distanceBetweenColliders >= maxDistance)
					{
						if ((*collider).get()->GetID().find("sun-nova") != std::string::npos &&
							this->type != FE_SUN)
						{
							std::string skillName = "sat-chain";
							skillName += id;
							OnFusionCompletedEvent _event = OnFusionCompletedEvent(EVENT_ON_FUSION_COMPLETED, skillName, "000", true);
							GetWorld().GetEventManager().FireEvent(_event);
						}
					}  
				}
				break;
			}
		}
	}
}

void CelestialBody::Update()
{
	UpdateCollision();

	TransformComponent *sunTransform = static_cast<TransformComponent*>(GetComponent(FE_COMPONENT_TRANSFORM).get());

	for (auto satellite = satellites.begin(); satellite != satellites.end(); ++satellite)
	{
		TransformComponent *satTransform = static_cast<TransformComponent*>(
			(*satellite)->GetComponent(FE_COMPONENT_TRANSFORM).get());

		if (satTransform)
		{
			glutil::MatrixStack relativeTransformStack;
			relativeTransformStack.Translate(sunTransform->position);
			relativeTransformStack.RotateY((*satellite)->currentRotationAngle);

			(*satellite)->currentRotationAngle += (*satellite)->angularVelocity * World::GetWorld().interpolation;
			if ((*satellite)->currentRotationAngle >= 360.0f)
			{
				(*satellite)->currentRotationAngle -= 360.0f;
			}

			float offset = (*satellite)->offsetFromSun;
			satTransform->position = glm::vec3(offset, 0.0f, offset);
			satTransform->position = glm::vec3(relativeTransformStack.Top() * glm::vec4(satTransform->position, 1.0f));

			CollisionComponent *satCollision = static_cast<CollisionComponent*>(
				(*satellite)->GetComponent(FE_COMPONENT_COLLISION).get());
			satCollision->center = satTransform->position;
		}

		(*satellite)->Update(); // Or maybe just update sat skills here.
	}

	for (auto skill = skills.begin(); skill != skills.end(); ++skill)
	{
		(*skill)->Update();
	}
}

std::vector<std::shared_ptr<Skill>> CelestialBody::GetAllSkills()
{
	std::vector<std::shared_ptr<Skill>> skillsToReturn(skills);
	
	for (auto sat = satellites.begin(); sat != satellites.end(); ++sat)
	{
		auto satSkills = (*sat)->GetAllSkills();
		skillsToReturn.insert(skillsToReturn.begin(), satSkills.begin(), satSkills.end());
	}

	return skillsToReturn;
}