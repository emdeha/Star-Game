#include "stdafx.h"
#include "PassiveAOE.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void FusionEngine::PassiveAOE_OnFusionCompleted(const std::string &skillID, const IEventData &eventData)
{
	SkillGenericComponent *skillData = 
		static_cast<SkillGenericComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());

	skillData->isActive = true;

	SkillTimedComponent *skillTimed = 
		static_cast<SkillTimedComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_TIMED).get());
	
	skillTimed->skillLifeTimer.SetPause(false);
	skillTimed->attackTimer.SetPause(false);
}

void FusionEngine::PassiveAOE_OnUpdate(const std::string &skillID)
{
	SkillGenericComponent *skillData = 
		static_cast<SkillGenericComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());

	if (skillData->isActive)
	{
		SkillTimedComponent *skillTimed = 
			static_cast<SkillTimedComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_TIMED).get());
		TransformComponent *skillTransform =
			static_cast<TransformComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_TRANSFORM).get());

		if (!skillTimed->skillLifeTimer.Update())
		{
			skillTimed->attackTimer.Update();
			if (skillTimed->attackTimer.GetTimeSinceStart() >= skillTimed->damageApplyTime_seconds)
			{
				OnSkillAppliedEvent _event = 
					OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, skillTransform->position, 
										skillData->range, skillData->damage, false);
				GetWorld().GetEventManager().FireEvent(_event);

				skillTimed->attackTimer.Reset();
			}
			// update anim
		}
		else
		{
			skillData->isActive = false;
			skillTimed->skillLifeTimer.Reset();
			skillTimed->skillLifeTimer.SetPause(true);
		}
	}
}