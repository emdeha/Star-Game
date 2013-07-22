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
#include "Spaceship.h"

#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_Scene/Scene.h"


void Projectile::Update(FusionEngine::Scene &scene)
{
	if(lifeSpan <= 0)
	{
		isDestroyed = true;
	}

	if(!isDestroyed)
	{
		FusionEngine::ComponentMapper<FusionEngine::Transform> transformData =
			scene.GetEntityManager()->GetComponentList(scene.GetEntity("spaceshipProjectile"), FusionEngine::CT_TRANSFORM);

		transformData[0]->position += velocity;
		lifeSpan--;

		CheckTargetHit(scene);
	}
}

void Projectile::CheckTargetHit(FusionEngine::Scene &scene)
{
	FusionEngine::ComponentMapper<FusionEngine::Transform> tranformData =
		scene.GetEntityManager()->GetComponentList(scene.GetEntity("spaceshipProjectile"), FusionEngine::CT_TRANSFORM);

	// Get sun sats
}