#include "stdafx.h"
#include "Chaining.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void FusionEngine::Chaining_OnFusionCompleted(const std::string &skillID, const IEventData &eventData)
{
	SkillGenericComponent *skillData = static_cast<SkillGenericComponent*>(
		GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());

	skillData->isActive = true;
	
	TransformComponent *skillTransform = static_cast<TransformComponent*>(
		GetWorld().GetComponentForObject(skillID, FE_COMPONENT_TRANSFORM).get());
	TransformComponent *holderTransform = static_cast<TransformComponent*>(
		GetWorld().GetComponentForObject(skillData->holderID, FE_COMPONENT_TRANSFORM).get());

	skillTransform->position = holderTransform->position;
}

void FusionEngine::Chaining_OnUpdate(const std::string &skillID)
{
	SkillGenericComponent *skillData = static_cast<SkillGenericComponent*>(
		GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());

	if (skillData->isActive)
	{
		SkillAnimatedComponent *skillAnimated = static_cast<SkillAnimatedComponent*>(
			GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_ANIMATED).get());

		if (skillAnimated->currentScale <= skillData->range)
		{
			skillAnimated->currentScale += skillAnimated->scaleRate;

			TransformComponent *skillTransform = static_cast<TransformComponent*>(
				GetWorld().GetComponentForObject(skillID, FE_COMPONENT_TRANSFORM).get());

			TransformComponent *holderTransform = static_cast<TransformComponent*>(
				GetWorld().GetComponentForObject(skillData->holderID, FE_COMPONENT_TRANSFORM).get());

			skillTransform->position = holderTransform->position;
		}
		else
		{
			skillAnimated->currentScale = 1.0f;
			skillData->isActive = false;
		}
	}
}