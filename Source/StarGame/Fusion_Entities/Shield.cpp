#include "stdafx.h"
#include "Shield.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void FusionEngine::Shield_OnFusionCompleted(const std::string &skillID, const IEventData &eventData)
{
	SkillGenericComponent *skillGeneric = static_cast<SkillGenericComponent*>(
		GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());
	skillGeneric->isActive = true;
}

void FusionEngine::Shield_OnUpdate(const std::string &skillID)
{
	SkillGenericComponent *skillGeneric = static_cast<SkillGenericComponent*>(
		GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());
	if (skillGeneric->isActive)
	{
		SkillDefensiveComponent *skillDefensive = static_cast<SkillDefensiveComponent*>(
			GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_DEFENSIVE).get());
		
		if (skillDefensive->defensePoints == 0)
		{
			skillGeneric->isActive = false;
			skillDefensive->currentDefensePoints = skillDefensive->defensePoints;
		}
		else
		{
			TransformComponent *skillTransform = static_cast<TransformComponent*>(
				GetWorld().GetComponentForObject(skillID, FE_COMPONENT_TRANSFORM).get());

			OnSkillAppliedEvent _event = 
				OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, skillTransform->position, skillGeneric->range, 0, false);

			GetWorld().GetEventManager().FireEvent(_event);
		}
	}
}