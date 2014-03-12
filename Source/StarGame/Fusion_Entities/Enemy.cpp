#include "stdafx.h"
#include "Enemy.h"


#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Scene/World.h"
#include "Components.h"

#include "CelestialBody.h"


using namespace FusionEngine;


Enemy::Enemy(const std::string &newName)
	: Composable(newName)
{
	GetWorld().GetEventManager().AddListener(this, EVENT_ON_SKILL_APPLIED);
	GetWorld().GetEventManager().AddListener(this, EVENT_ON_COLLIDE);

	OnUpdateAI = nullptr;
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
				EnemyGenericComponent *enemyGeneric = static_cast<EnemyGenericComponent*>(
					GetComponent(FE_COMPONENT_ENEMY_GENERIC).get());
				enemyGeneric->health -= data.damage;
				std::printf("ENEMY ATTACKED: %i, %s", enemyGeneric->health, id.c_str());
			}
		}
		break;
	}
	
	return false;
}

void Enemy::UpdateCollision()
{
	auto collidableObjects = GetWorld().GetObjectsWithComponent(FE_COMPONENT_COLLISION);	

	for (auto collider = collidableObjects.begin(); collider != collidableObjects.end(); ++collider)
	{
		if ((*collider)->GetID() != id)
		{
			CollisionComponent *colliderCollision = static_cast<CollisionComponent*>(
				(*collider)->GetComponent(FE_COMPONENT_COLLISION).get());
			CollisionComponent *enemyCollision = static_cast<CollisionComponent*>(
				GetComponent(FE_COMPONENT_COLLISION).get());

			switch (colliderCollision->cType)
			{
			case CollisionComponent::FE_COLLISION_CIRCLE:
				{
					float distanceBetweenColliders = 
						glm::length(colliderCollision->center - enemyCollision->center); 
					float minDistance = colliderCollision->innerRadius + enemyCollision->innerRadius;
					if (distanceBetweenColliders < minDistance)
					{
						if ((*collider)->GetID().find("shield") != std::string::npos)
						{
							EnemyGenericComponent *enemyGeneric = static_cast<EnemyGenericComponent*>(
								GetComponent(FE_COMPONENT_ENEMY_GENERIC).get());
							enemyGeneric->health = 0;
							std::printf("\n\nKILLED BY SHIELD\n\n");
						}
					}  
				}
				break;
			case CollisionComponent::FE_COLLISION_TORUS:
				{
					float distanceBetweenColliders = 
						glm::length(colliderCollision->center - enemyCollision->center); 
					float minDistance = colliderCollision->innerRadius + enemyCollision->innerRadius;
					float maxDistance = colliderCollision->outerRadius + enemyCollision->innerRadius;
					if (distanceBetweenColliders < minDistance && distanceBetweenColliders >= maxDistance)
					{
						SkillGenericComponent *skillGeneric = static_cast<SkillGenericComponent*>(
							(*collider)->GetComponent(FE_COMPONENT_SKILL_GENERIC).get());
						EnemyGenericComponent *enemyGeneric = static_cast<EnemyGenericComponent*>(
							GetComponent(FE_COMPONENT_ENEMY_GENERIC).get());
						enemyGeneric->health -= skillGeneric->damage;
						std::printf("ATTACKED BY nova%s: %i\n", (*collider)->GetID().c_str(), 
																enemyGeneric->health);
					}  
				}
				break;
			}
		}
	}
}

void Enemy::Update()
{
	EnemyGenericComponent *enemyGeneric = static_cast<EnemyGenericComponent*>(
		GetComponent(FE_COMPONENT_ENEMY_GENERIC).get());

	TransformComponent *enemyTransformData = static_cast<TransformComponent*>(
		GetComponent(FE_COMPONENT_TRANSFORM).get());
	enemyTransformData->rotation = 
		glm::vec3(0.0f, 
				  glm::degrees(atan2f(enemyGeneric->frontVector.x, enemyGeneric->frontVector.z)),			  // Here's the D
				  0.0f);
	enemyTransformData->position += 
		enemyGeneric->frontVector * World::GetWorld().interpolation * enemyGeneric->speed;

	CollisionComponent *enemyCollisionData =
		static_cast<CollisionComponent*>(GetComponent(FE_COMPONENT_COLLISION).get());
	enemyCollisionData->center = enemyTransformData->position;

	if (OnUpdateAI)
	{
		OnUpdateAI(id);
	}
	else
	{
		std::string errorMsg = "Enemy with id: \"" + id + "\" doesn't have an UpdateAI func";
		HandleUnexpectedError(errorMsg, __LINE__, __FILE__, FE_SEVERITY_MED);
	}
	UpdateCollision();
}