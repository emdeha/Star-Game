#include "stdafx.h"
#include "Enemy.h"


#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_Scene/Scene.h"
#include "../Fusion_Scene/World.h"

#include "CelestialBody.h"


using namespace FusionEngine;


void Enemy::UpdateAI()
{
	if (currentState == FE_STATE_ATTACK)
	{
		speed = 0.0f;
	}
	else if (currentState == FE_STATE_EVADE)
	{
		ComponentMapper<Transform> sunTransformData =
			GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity("sun"), CT_TRANSFORM);

		ComponentMapper<Transform> enemyTransformData =
			GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(name), CT_TRANSFORM);

		glm::vec3 vectorFromSunToSpaceship = sunTransformData[0]->position - enemyTransformData[0]->position;

		vectorFromSunToSpaceship = glm::normalize(vectorFromSunToSpaceship);
		glm::vec3 spaceshipDirection = glm::normalize(frontVector * speed);

		if (glm::dot(vectorFromSunToSpaceship, spaceshipDirection) > 0)
		{
			speed *= -1.0f;
		}
	}
	else if (currentState == FE_STATE_IDLE)
	{
		ComponentMapper<Transform> sunTransformData =
			GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity("sun"), CT_TRANSFORM);

		ComponentMapper<Transform> enemyTransformData =
			GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(name), CT_TRANSFORM);

		glm::vec3 vectorFromSunToSpaceship = sunTransformData[0]->position - enemyTransformData[0]->position;

		float distanceBetweenSunAndSpaceship = glm::length(vectorFromSunToSpaceship);

		if (distanceBetweenSunAndSpaceship < attackRange) // 3.0f - lineOfSight
		{
			currentState = FE_STATE_ATTACK;
		}
	}
}

void Enemy::Update()
{
	// if (GetWorld().IsEntityKilled("sun") == false)
	ComponentMapper<Transform> transformData =
		GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(name), CT_TRANSFORM);

	transformData[0]->rotation = glm::vec3(0.0f, glm::degrees(atan2f(frontVector.x, frontVector.z)), 0.0f);

	if (currentState != FE_STATE_STOPPED)
	{
		transformData[0]->position += frontVector * World::GetWorld().interpolation * speed;
		UpdateAI();
	}

	if (health <= 20)
	{
		currentState == FE_STATE_EVADE;
	}

	if (health <= 0)
	{
		// Destroy
	}
}