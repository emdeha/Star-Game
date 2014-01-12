#include "stdafx.h"
#include "AOE.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void FusionEngine::AOE_OnClick(const std::string &skillID, const IEventData &eventData)
{
	SkillGenericComponent *skillData = 
		static_cast<SkillGenericComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());

	if (skillData->isActive)
	{
		skillData->isDeployed = true;
		skillData->isActive = false;
		
		TransformComponent *skillTransform =
			static_cast<TransformComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_TRANSFORM).get());
		GetWorld().GetEventManager().FireEvent(OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, 
																   skillTransform->position, 
																   skillData->range, 
																   skillData->damage, false));
	}
}
void FusionEngine::AOE_OnFusionCompleted(const std::string &skillID, const IEventData &eventData)
{
	SkillGenericComponent *skillData = 
		static_cast<SkillGenericComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());
	skillData->isActive = true;
}

void FusionEngine::AOE_OnUpdate(const std::string &skillID)
{
	SkillGenericComponent *skillData = 
		static_cast<SkillGenericComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_GENERIC).get());
	if (skillData->isActive)
	{
		TransformComponent *skillTransform =
			static_cast<TransformComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_TRANSFORM).get());
		SkillSelectorAppliedComponent *skillSelector =
			static_cast<SkillSelectorAppliedComponent*>(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_SELECTOR_APPLIED).get());

		DisplayData currentDisplayData = GetWorld().GetDisplayData();

		glm::vec4 mousePos_atZ =
			GetWorld().GetMouse().GetPositionAtDimension(currentDisplayData.windowWidth,
														 currentDisplayData.windowHeight,
														 currentDisplayData.projectionMatrix,
														 currentDisplayData.modelMatrix.Top(),
														 glm::vec4(GetWorld().GetCamera().ResolveCamPosition(), 1.0f),
														 glm::comp::Y);

		skillTransform->position = mousePos_atZ.swizzle(glm::comp::X, glm::comp::Y, glm::comp::Z);

		// Draw should be elsewhere
		//glutil::PushStack push(currentDisplayData.modelMatrix);
		//currentDisplayData.modelMatrix.Translate(skillTransform->position);

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//skillSelector->skillSelector.Draw(currentDisplayData.modelMatrix, 
		//								  GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_SIMPLE));

		//glDisable(GL_BLEND);
	}
}