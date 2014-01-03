#include "stdafx.h"
#include "AOE.h"

#include "../Fusion_EntitySystem/ComponentMapper.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_EntitySystem/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Scene/World.h"
#include "../Fusion_Scene/Scene.h"

using namespace FusionEngine;


void FusionEngine::AOE_OnClick(EntityManager *entityManager, Entity *entity, 
							   const IEventData &eventData)
{
	ComponentMapper<Skill> skillData = entityManager->GetComponentList(entity, CT_SKILL);
	if (skillData[0]->isActive)
	{
		skillData[0]->isDeployed = true;
		skillData[0]->isActive = false;
		
		ComponentMapper<Transform> skillTransform = entityManager->GetComponentList(entity, CT_TRANSFORM);
		GetWorld().GetEventManager().FireEvent(OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, 
																   skillTransform[0]->position, 
																   skillData[0]->range, 
																   skillData[0]->damage, false));
	}
}
void FusionEngine::AOE_OnFusionCompleted(EntityManager *entityManager, Entity *entity, 
										 const IEventData &eventData)
{
	ComponentMapper<Skill> skillData = entityManager->GetComponentList(entity, CT_SKILL);
	skillData[0]->isActive = true;
}

void FusionEngine::AOE_OnUpdate(EntityManager *entityManager, Entity *entity)
{
	ComponentMapper<Skill> skillData = entityManager->GetComponentList(entity, CT_SKILL);
	if (skillData[0]->isActive)
	{
		ComponentMapper<Transform> aoeTransform = entityManager->GetComponentList(entity, CT_TRANSFORM);
		ComponentMapper<SelectorAppliedSkill> aoeSelector = 
			entityManager->GetComponentList(entity, CT_SELECTOR_APPLIED_SKILL);

		DisplayData currentDisplayData = GetWorld().GetDisplayData();

		glm::vec4 mousePos_atZ =
			GetWorld().GetMouse().GetPositionAtDimension(currentDisplayData.windowWidth,
														 currentDisplayData.windowHeight,
														 currentDisplayData.projectionMatrix,
														 currentDisplayData.modelMatrix.Top(),
														 glm::vec4(GetWorld().GetCamera().ResolveCamPosition(), 1.0f),
														 glm::comp::Y);

		aoeTransform[0]->position = mousePos_atZ.swizzle(glm::comp::X, glm::comp::Y, glm::comp::Z);

		// Draw should be elsewhere
		glutil::PushStack push(currentDisplayData.modelMatrix);
		currentDisplayData.modelMatrix.Translate(aoeTransform[0]->position);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		aoeSelector[0]->skillSelector.Draw(currentDisplayData.modelMatrix, 
										   GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_SIMPLE));

		glDisable(GL_BLEND);
	}
}