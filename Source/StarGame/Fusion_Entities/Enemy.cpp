#include "stdafx.h"
#include "Enemy.h"


#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_Scene/Scene.h"
#include "../Fusion_Scene/World.h"


using namespace FusionEngine;

void Enemy::Update()
{
	ComponentMapper<Transform> transformData =
		GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(name), CT_TRANSFORM);

	transformData[0]->position += World::GetWorld().interpolation * speed;
}