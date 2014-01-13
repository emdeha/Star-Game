#include "stdafx.h"
#include "Burn.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void FusionEngine::Burn_OnClick(const std::string &skillID, const IEventData &eventData)
{
	SkillGenericComponent *skillData = 
		static_cast<SkillGenericComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());

	if (skillData->isActive)
	{
		SkillTimedComponent *skillTimed = 
			static_cast<SkillTimedComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_TIMED).get());

		skillTimed->attackTimer.SetPause(false);
		skillTimed->attackTimer.Reset();

		skillData->isDeployed = true;
	}
}
void FusionEngine::Burn_OnFusionCompleted(const std::string &skillID, const IEventData &eventData)
{
	SkillGenericComponent *skillData = 
		static_cast<SkillGenericComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());

	skillData->isActive = true;
	skillData->isDeployed = false;

	SkillTimedComponent *skillTimed = 
		static_cast<SkillTimedComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_TIMED).get());

	skillTimed->damageApplyTime_seconds = skillTimed->damageApplyTimeDuration_seconds;
	skillTimed->attackTimer.SetPause(true);
}

void FusionEngine::Burn_OnUpdate(const std::string &skillID)
{
	SkillGenericComponent *skillData = 
		static_cast<SkillGenericComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());

	if (skillData->isActive)
	{
		SkillTimedComponent *skillTimed = 
			static_cast<SkillTimedComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_TIMED).get());
		TransformComponent *skillTransform =
			static_cast<TransformComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_TRANSFORM).get());

		skillTimed->attackTimer.Update();
		if (skillTimed->attackTimer.GetTimeSinceStart() > skillTimed->duration_seconds)
		{
			skillTimed->attackTimer.Reset();
			skillTimed->attackTimer.SetPause(true);
			skillData->isActive = false;
			skillData->isDeployed = false;
		}
		if (skillTimed->attackTimer.GetTimeSinceStart() > skillTimed->damageApplyTime_seconds)
		{
			OnSkillAppliedEvent _event = 
				OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, skillTransform->position, 
									skillData->range, skillData->damage, false);
			GetWorld().GetEventManager().FireEvent(_event);

			skillTimed->damageApplyTime_seconds += skillTimed->damageApplyTimeDuration_seconds;
		}

		if (!skillData->isDeployed)
		{
			DisplayData currentDisplayData = GetWorld().GetDisplayData();

			glm::vec4 mousePos_atZ =
				GetWorld().GetMouse().GetPositionAtDimension(currentDisplayData.windowWidth,
															 currentDisplayData.windowHeight,
															 currentDisplayData.projectionMatrix,
															 currentDisplayData.modelMatrix.Top(),
															 glm::vec4(GetWorld().GetCamera().ResolveCamPosition(), 1.0f),
															 glm::comp::Y);

			skillTransform->position = mousePos_atZ.swizzle(glm::comp::X, glm::comp::Y, glm::comp::Z);
		}
	}
}