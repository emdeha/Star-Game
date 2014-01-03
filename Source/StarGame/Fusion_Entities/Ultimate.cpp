#include "stdafx.h"
#include "Ultimate.h"

#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_EntitySystem/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Scene/World.h"
#include "../Fusion_Scene/Scene.h"

using namespace FusionEngine;


void FusionEngine::Ultimate_OnFusionCompleted(EntityManager *entityManager, Entity *entity, 
											  const IEventData &eventData)
{
	ComponentMapper<Skill> skillData = entityManager->GetComponentList(entity, CT_SKILL);
	GetWorld().GetEventManager().FireEvent(OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED,
															   glm::vec3(),
															   skillData[0]->range,
															   skillData[0]->damage,
															   false));
}