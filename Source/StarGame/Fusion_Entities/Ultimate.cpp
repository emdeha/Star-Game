#include "stdafx.h"
#include "Ultimate.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void FusionEngine::Ultimate_OnFusionCompleted(const std::string &skillID, const IEventData &eventData)
{
	SkillGenericComponent *skillData = 
		static_cast<SkillGenericComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());

	GetWorld().GetEventManager().FireEvent(OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED,
															   glm::vec3(),
															   skillData->range,
															   skillData->damage,
															   false));
}