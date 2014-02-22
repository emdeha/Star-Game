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
	const float RADIUS = 0.4f;
	satTransformComponent->scale = glm::vec3(RADIUS, RADIUS, RADIUS);
	satTransformComponent->rotation = glm::vec3();

	newSat->AddComponent(FE_COMPONENT_TRANSFORM, satTransformComponent);

	std::shared_ptr<FusionEngine::CollisionComponent> satCollisionComponent = std::make_shared<CollisionComponent>();
	satCollisionComponent->center = glm::vec3();
	satCollisionComponent->cType = CollisionComponent::FE_COLLISION_CIRCLE;
	satCollisionComponent->innerRadius = RADIUS;

	newSat->AddComponent(FE_COMPONENT_COLLISION, satCollisionComponent);
	
	satellites.push_back(newSat);
	currentSatelliteCount++;

	GetWorld().GetRenderer().SubscribeForRendering(newSat->GetID(), sunMesh);

	// Skill Chain
	std::string skillName = "skillChain";
	skillName += newSat->GetID();
	std::shared_ptr<Skill> newSkill = 
		std::shared_ptr<Skill>(new Skill(skillName, '0', '0', '0', 0, 0));

	newSkill->SetOnFusionCompletedCallback(Chaining_OnFusionCompleted);
	newSkill->SetOnUpdateCallback(Chaining_OnUpdate);

	std::shared_ptr<SkillGenericComponent> newGeneric = std::make_shared<SkillGenericComponent>();
	newGeneric->holderID = newSat->GetID();
	newGeneric->damage = 10;
	newGeneric->range = 2.0f;
	newGeneric->isActive = false;
	newGeneric->isChain = true;
	newGeneric->isDeployed = false;

	newSkill->AddComponent(FE_COMPONENT_SKILL_GENERIC, newGeneric);

	std::shared_ptr<TransformComponent> newTransform = std::make_shared<TransformComponent>();
	newTransform->position = glm::vec3();
	newTransform->rotation = glm::vec3();
	newTransform->scale = glm::vec3();

	newSkill->AddComponent(FE_COMPONENT_TRANSFORM, newTransform);

	std::shared_ptr<SkillAnimatedComponent> newSkillAnimated = std::make_shared<SkillAnimatedComponent>();

	newSkillAnimated->currentScale = 0.5f;
	newSkillAnimated->scaleRate = 0.02f;

	Utility::Primitives::Torus2D anim =
		Utility::Primitives::Torus2D(glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), 
									 glm::vec3(), 1.0f, 1.02f, 90);
	anim.Init();

	newSkillAnimated->anim = anim;

	newSkill->AddComponent(FE_COMPONENT_SKILL_ANIMATED, newSkillAnimated);

	std::shared_ptr<CollisionComponent> newSkillCollision = std::make_shared<CollisionComponent>();
	newSkillCollision->center = glm::vec3();
	newSkillCollision->innerRadius = newSkillAnimated->currentScale;
	newSkillCollision->outerRadius = newSkillAnimated->currentScale - 0.1f;
	newSkillCollision->cType = CollisionComponent::FE_COLLISION_TORUS;

	newSkill->AddComponent(FE_COMPONENT_COLLISION, newSkillCollision);
	// End Skill Chain

	// Skill Shield
	std::string shieldSkillName = "skillShield";
	shieldSkillName += newSat->GetID();
	std::shared_ptr<Skill> shield = 
		std::shared_ptr<Skill>(new Skill(shieldSkillName, 'q', 'w', 'q', 0, 0));

	shield->SetOnFusionCompletedCallback(Shield_OnFusionCompleted);
	shield->SetOnUpdateCallback(Shield_OnUpdate);

	std::shared_ptr<SkillGenericComponent> shieldGeneric = std::make_shared<SkillGenericComponent>();
	shieldGeneric->holderID = newSat->GetID();	
	shieldGeneric->damage = 0;
	shieldGeneric->range = 2.0f;
	shieldGeneric->isActive = false;
	shieldGeneric->isDeployed = false;
	shieldGeneric->isChain = false;
	shieldGeneric->isDefensive = true;

	shield->AddComponent(FE_COMPONENT_SKILL_GENERIC, shieldGeneric);

	std::shared_ptr<SkillDefensiveComponent> shieldDefensive = std::make_shared<SkillDefensiveComponent>();
	shieldDefensive->defensePoints = 3;
	shieldDefensive->currentDefensePoints = 3;

	shield->AddComponent(FE_COMPONENT_SKILL_DEFENSIVE, shieldDefensive);
	
	std::shared_ptr<TransformComponent> shieldTransform = std::make_shared<TransformComponent>();
	shieldTransform->position = glm::vec3();
	shieldTransform->scale = glm::vec3();
	shieldTransform->rotation = glm::vec3();

	shield->AddComponent(FE_COMPONENT_TRANSFORM, shieldTransform);

	std::shared_ptr<SkillSelectorAppliedComponent> shieldSelector = std::make_shared<SkillSelectorAppliedComponent>();
	Utility::Primitives::Circle selector = Utility::Primitives::Circle(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), 
																	   glm::vec3(), 1.0f, 90);
	selector.Init();
	shieldSelector->skillSelector = selector;

	shield->AddComponent(FE_COMPONENT_SKILL_SELECTOR_APPLIED, shieldSelector);

	std::shared_ptr<CollisionComponent> shieldCollision = std::make_shared<CollisionComponent>();
	shieldCollision->center = glm::vec3();
	shieldCollision->innerRadius = 1.0f;
	shieldCollision->cType = CollisionComponent::FE_COLLISION_CIRCLE;

	shield->AddComponent(FE_COMPONENT_COLLISION, shieldCollision);
	// End Skill Shield

	// Skill Frost
	std::string frostSkillName = "skillFrost";
	frostSkillName += newSat->GetID();
	std::shared_ptr<Skill> frost =
		std::shared_ptr<Skill>(new Skill(frostSkillName, 'e', 'e', 'q', 0, 0));

	frost->SetOnFusionCompletedCallback(Frost_OnFusionCompleted);
	frost->SetOnUpdateCallback(Frost_OnUpdate);

	std::shared_ptr<SkillGenericComponent> frostGeneric = std::make_shared<SkillGenericComponent>();
	frostGeneric->holderID = newSat->GetID();
	frostGeneric->damage = 20;
	frostGeneric->range = 2.0f;
	frostGeneric->isActive = false;
	frostGeneric->isDeployed = false;
	frostGeneric->isChain = false;
	frostGeneric->isDefensive = false;

	frost->AddComponent(FE_COMPONENT_SKILL_GENERIC, frostGeneric);

	std::shared_ptr<TransformComponent> frostTransform = std::make_shared<TransformComponent>();
	frostTransform->position = glm::vec3();
	frostTransform->scale = glm::vec3();
	frostTransform->rotation = glm::vec3();

	frost->AddComponent(FE_COMPONENT_TRANSFORM, frostTransform);

	std::shared_ptr<SkillAnimatedComponent> frostAnimated = std::make_shared<SkillAnimatedComponent>();
	frostAnimated->currentScale = 0.5f;
	frostAnimated->scaleRate = 0.02f;
	frostAnimated->anim = anim;

	frost->AddComponent(FE_COMPONENT_SKILL_ANIMATED, frostAnimated);

	std::shared_ptr<CollisionComponent> frostCollision = std::make_shared<CollisionComponent>();
	frostCollision->center = glm::vec3();
	frostCollision->innerRadius = 2.0f;
	frostCollision->cType = CollisionComponent::FE_COLLISION_TORUS;

	frost->AddComponent(FE_COMPONENT_COLLISION, frostCollision);
	// End Skill Frost

	newSat->AddSkill(skillName, newSkill);
	newSat->AddSkill(shieldSkillName, shield);
	newSat->AddSkill(frostSkillName, frost);

	return true;
}

bool CelestialBody::AddSkill(const std::string &skillName, const std::shared_ptr<Skill> newSkill)
{
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
						if ((*collider).get()->GetID().find("sunNova") != std::string::npos &&
							this->type != FE_SUN)
						{
							std::string skillName = "skillChain";
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