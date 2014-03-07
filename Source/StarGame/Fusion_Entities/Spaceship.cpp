#include "stdafx.h"
#include "Spaceship.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void Spaceship_EvadeState(const std::string &enemyID, EnemyGenericComponent *enemyGeneric)
{
	TransformComponent *sunTransformData = static_cast<TransformComponent*>(
		GetWorld().GetComponentForObject("sun", FE_COMPONENT_TRANSFORM).get());

	TransformComponent *enemyTransformData = static_cast<TransformComponent*>(
		GetWorld().GetComponentForObject(enemyID, FE_COMPONENT_TRANSFORM).get());

	glm::vec3 vectorFromSunToSpaceship = sunTransformData->position - enemyTransformData->position;

	vectorFromSunToSpaceship = glm::normalize(vectorFromSunToSpaceship);
	if (enemyGeneric->speed == 0.0f)
	{
		enemyGeneric->speed = enemyGeneric->previousSpeed;
	}
	glm::vec3 spaceshipDirection = glm::normalize(enemyGeneric->frontVector * enemyGeneric->speed);

	if (glm::dot(vectorFromSunToSpaceship, spaceshipDirection) >= 0)
	{
		enemyGeneric->frontVector *= -1.0f;
	}
}

void Spaceship_IdleState(const std::string &enemyID, EnemyGenericComponent *enemyGeneric)
{
	TransformComponent *sunTransformData = static_cast<TransformComponent*>(
		GetWorld().GetComponentForObject("sun", FE_COMPONENT_TRANSFORM).get());

	TransformComponent *enemyTransformData = static_cast<TransformComponent*>(
		GetWorld().GetComponentForObject(enemyID, FE_COMPONENT_TRANSFORM).get());

	glm::vec3 vectorFromSunToSpaceship = sunTransformData->position - enemyTransformData->position;

	float distanceBetweenSunAndSpaceship = glm::length(vectorFromSunToSpaceship);

	if (distanceBetweenSunAndSpaceship < enemyGeneric->attackRange)
	{
		enemyGeneric->currentState = FE_STATE_ATTACK;
	}
}

void Spaceship_AttackState(EnemyGenericComponent *enemyGeneric)
{
	if (enemyGeneric->speed > 0.0f)
	{
		enemyGeneric->previousSpeed = enemyGeneric->speed;
	}
	enemyGeneric->speed = 0.0f;
}

void FusionEngine::Spaceship_OnUpdate(const std::string &enemyID)
{
	EnemyGenericComponent *enemyGeneric = static_cast<EnemyGenericComponent*>(
		GetWorld().GetComponentForObject(enemyID, FE_COMPONENT_ENEMY_GENERIC).get());
	if (enemyGeneric->currentState == FE_STATE_ATTACK)
	{
		Spaceship_AttackState(enemyGeneric);
	}
	else if (enemyGeneric->currentState == FE_STATE_EVADE)
	{
		Spaceship_EvadeState(enemyID, enemyGeneric);
	}
	else if (enemyGeneric->currentState == FE_STATE_IDLE)
	{
		Spaceship_IdleState(enemyID, enemyGeneric);
	}

	if (enemyGeneric->health <= 20)
	{
		enemyGeneric->currentState = FE_STATE_EVADE;
	}
}