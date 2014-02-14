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
	World::GetWorld().GetEventManager().AddListener(this, FusionEngine::EVENT_ON_FUSION_COMPLETED);

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
	case FusionEngine::EVENT_ON_FUSION_COMPLETED:
		{
			const OnFusionCompletedEvent &data = static_cast<const OnFusionCompletedEvent&>(eventData);
			/*
			for (auto skill = skills.begin(); skill != skills.end(); ++skill)
			{
				if ((*skill)->IsForSequence(data.inputSequence))
				{
					(*skill)->Activate(this);
					break;
				}
			}*/
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
	std::shared_ptr<CelestialBody> newSat = std::make_shared<CelestialBody>(satType, 0, 0.3f, satOffset);
	
	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());

	FusionEngine::MeshAssetObject sunMesh = meshLoader.LoadAssetObject("mesh-files", GetSatMesh(satType));
	
	std::shared_ptr<FusionEngine::RenderComponent> satRenderComponent = std::make_shared<RenderComponent>();
	satRenderComponent->vao = sunMesh.vao;
	satRenderComponent->shaderProgramID = GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_LIT_TEXTURE).programId;
	satRenderComponent->renderType = RenderComponent::FE_RENDERER_LIT;
	
	newSat->AddComponent(FE_COMPONENT_RENDER, satRenderComponent);

	std::shared_ptr<FusionEngine::TransformComponent> satTransformComponent = std::make_shared<TransformComponent>();
	satTransformComponent->position = glm::vec3();
	satTransformComponent->scale = glm::vec3(0.4f);
	satTransformComponent->rotation = glm::vec3();

	newSat->AddComponent(FE_COMPONENT_TRANSFORM, satTransformComponent);
	
	satellites.push_back(newSat);
	currentSatelliteCount++;

	GetWorld().GetRenderer().SubscribeForRendering(newSat->GetID(), sunMesh);

	// Skill Chain
	std::string skillName = "skill";
	skillName += id;
	std::shared_ptr<Skill> newSkill = 
		std::shared_ptr<Skill>(new Skill(skillName, '0', '0', '0', 0, 0));

	newSkill->SetOnFusionCompletedCallback(Chaining_OnFusionCompleted);
	newSkill->SetOnUpdateCallback(Chaining_OnUpdate);

	std::shared_ptr<SkillGenericComponent> newGeneric = std::make_shared<SkillGenericComponent>();
	newGeneric->holderID = id;
	newGeneric->damage = 10;
	newGeneric->range = 2.0f;
	newGeneric->isActive = false;
	newGeneric->isDeployed = false;

	newSkill->AddComponent(FE_COMPONENT_SKILL_GENERIC, newGeneric);

	std::shared_ptr<TransformComponent> newTransform = std::make_shared<TransformComponent>();
	newTransform->position = glm::vec3();
	newTransform->rotation = glm::vec3();
	newTransform->scale = glm::vec3();

	newSkill->AddComponent(FE_COMPONENT_TRANSFORM, newTransform);

	std::shared_ptr<SkillAnimatedComponent> newSkillAnimated = std::make_shared<SkillAnimatedComponent>();

	newSkillAnimated->currentScale = 1.0f;
	newSkillAnimated->scaleRate = 0.02f;


	Utility::Primitives::Torus2D anim =
		Utility::Primitives::Torus2D(glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), 
									 glm::vec3(), 1.0f, 1.02f, 90);
	anim.Init();

	newSkillAnimated->anim = anim;

	newSkill->AddComponent(FE_COMPONENT_SKILL_ANIMATED, newSkillAnimated);
	// End Skill Chain

	newSat->AddSkill(skillName, newSkill);

	return true;
}

bool CelestialBody::AddSkill(const std::string &skillName, const std::shared_ptr<Skill> newSkill)
{
	skills.push_back(newSkill);
	GetWorld().AddFusionSequence(skillName, newSkill->GetFusionCombination());

	return true;
}

void CelestialBody::Update()
{
	TransformComponent *sunTransform = static_cast<TransformComponent*>(GetComponent(FE_COMPONENT_TRANSFORM).get());

	for (auto satellite = satellites.begin(); satellite != satellites.end(); ++satellite)
	{
		TransformComponent *satTransform = 
			static_cast<TransformComponent*>((*satellite)->GetComponent(FE_COMPONENT_TRANSFORM).get());

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