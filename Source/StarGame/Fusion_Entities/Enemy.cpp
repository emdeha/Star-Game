#include "stdafx.h"
#include "Enemy.h"


#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Scene/World.h"
#include "Components.h"

#include "CelestialBody.h"


using namespace FusionEngine;


Enemy::Enemy(const std::string &newName, float newSpeed, glm::vec3 newFrontVector)
	: Composable(newName), speed(newSpeed), health(100), frontVector(newFrontVector), currentState(FE_STATE_IDLE) 
{
	GetWorld().GetEventManager().AddListener(this, EVENT_ON_SKILL_APPLIED);
	GetWorld().GetEventManager().AddListener(this, EVENT_ON_COLLIDE);
}

bool Enemy::HandleEvent(const FusionEngine::IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch (type)
	{
	case FusionEngine::EVENT_ON_SKILL_APPLIED:
		{
			const OnSkillAppliedEvent &data = static_cast<const OnSkillAppliedEvent&>(eventData);

			TransformComponent *enemyTransformData = 
				static_cast<TransformComponent*>(GetComponent(FE_COMPONENT_TRANSFORM).get());

			if (data.radius <= -1.0f ||
				glm::length(data.position - enemyTransformData->position) < data.radius)
			{
				health -= data.damage;
				//std::printf("CRITICAL: %i, %s", health, id.c_str());
			}
		}
		break;
	}
	
	return false;
}

void Enemy::UpdateAI()
{
	if (currentState == FE_STATE_ATTACK)
	{
		speed = 0.0f;
	}
	else if (currentState == FE_STATE_EVADE)
	{
		TransformComponent *sunTransformData =
			static_cast<TransformComponent*>(GetWorld().GetComponentForObject("sun", FE_COMPONENT_TRANSFORM).get());

		TransformComponent *enemyTransformData =
			static_cast<TransformComponent*>(GetComponent(FE_COMPONENT_TRANSFORM).get());

		glm::vec3 vectorFromSunToSpaceship = sunTransformData->position - enemyTransformData->position;

		vectorFromSunToSpaceship = glm::normalize(vectorFromSunToSpaceship);
		glm::vec3 spaceshipDirection = glm::normalize(frontVector * speed);

		if (glm::dot(vectorFromSunToSpaceship, spaceshipDirection) > 0)
		{
			speed *= -1.0f;
		}
	}
	else if (currentState == FE_STATE_IDLE)
	{
		TransformComponent *sunTransformData =
			static_cast<TransformComponent*>(GetWorld().GetComponentForObject("sun", FE_COMPONENT_TRANSFORM).get());

		TransformComponent *enemyTransformData =
			static_cast<TransformComponent*>(GetComponent(FE_COMPONENT_TRANSFORM).get());

		glm::vec3 vectorFromSunToSpaceship = sunTransformData->position - enemyTransformData->position;

		float distanceBetweenSunAndSpaceship = glm::length(vectorFromSunToSpaceship);

		if (distanceBetweenSunAndSpaceship < attackRange) // 3.0f - lineOfSight
		{
			currentState = FE_STATE_ATTACK;
		}
	}
}

void Enemy::UpdateCollision()
{
	auto collidableObjects = GetWorld().GetObjectsWithComponent(FE_COMPONENT_COLLISION);	

	for (auto collider = collidableObjects.begin(); collider != collidableObjects.end(); ++collider)
	{
		if ((*collider).get()->GetID() != id)
		{
			CollisionComponent *colliderCollision = static_cast<CollisionComponent*>(
				(*collider).get()->GetComponent(FE_COMPONENT_COLLISION).get());
			CollisionComponent *enemyCollision = static_cast<CollisionComponent*>(
				GetComponent(FE_COMPONENT_COLLISION).get());

			switch (colliderCollision->cType)
			{
			case CollisionComponent::FE_COLLISION_CIRCLE:
				{
					float distanceBetweenColliders = glm::length(colliderCollision->center - enemyCollision->center); 
					float minDistance = colliderCollision->innerRadius + enemyCollision->innerRadius;
					if (distanceBetweenColliders < minDistance)
					{
						if ((*collider).get()->GetID().find("skillShield") != std::string::npos)
						{
							health = 0;
							std::printf("\n\nKILLED BY SHIELD\n\n");
							// KILL ENEMY
						}
						// Handle collision
					}  
				}
				break;
			case CollisionComponent::FE_COLLISION_TORUS:
				{
					float distanceBetweenColliders = glm::length(colliderCollision->center - enemyCollision->center); 
					float minDistance = colliderCollision->innerRadius + enemyCollision->innerRadius;
					float maxDistance = colliderCollision->outerRadius + enemyCollision->innerRadius;
					if (distanceBetweenColliders < minDistance && distanceBetweenColliders >= maxDistance)
					{
						SkillGenericComponent *skillGeneric = static_cast<SkillGenericComponent*>(
							(*collider).get()->GetComponent(FE_COMPONENT_SKILL_GENERIC).get());
						health -= skillGeneric->damage;
						// Handle collision
					}  
				}
				break;
			}
		}
	}
}

void Enemy::Update()
{
	// if (GetWorld().IsEntityKilled("sun") == false)
	TransformComponent *enemyTransformData =
		static_cast<TransformComponent*>(GetComponent(FE_COMPONENT_TRANSFORM).get());

	enemyTransformData->rotation = glm::vec3(0.0f, glm::degrees(atan2f(frontVector.x, frontVector.z)), 0.0f);

	UpdateCollision();

	if (currentState != FE_STATE_STOPPED)
	{
		enemyTransformData->position += frontVector * World::GetWorld().interpolation * speed;
		UpdateAI();

		CollisionComponent *enemyCollisionData =
			static_cast<CollisionComponent*>(GetComponent(FE_COMPONENT_COLLISION).get());
		enemyCollisionData->center = enemyTransformData->position;
	}

	if (health <= 20)
	{
		currentState = FE_STATE_EVADE;
	}

	if (health <= 0)
	{
		// Destroy
		//std::printf("KILLED IS ME!!! %s\n", id.c_str());
	}

	if (health > 0)
	{/*
		auto collidableSkills = GetWorld().GetCollidableSkills();
		for (auto skill = collidableSkills.begin(); skill != collidableSkills.end(); ++skill)
		{
			if (glm::length((*skill)->GetPosition() - transformData[0]->position) < (*skill)->GetRange())
			{
				health = 0; // Kill enemy
				GetWorld().GetEventManager().FireEvent(OnCollideEvent(EVENT_ON_COLLIDE, name));
			}
		}*/
	}
}