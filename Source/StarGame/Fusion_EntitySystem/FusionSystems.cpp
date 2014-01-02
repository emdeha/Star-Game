#include "stdafx.h"
#include "FusionSystems.h"

#include "../Fusion_Scene/Scene.h"
#include "../Fusion_Entities/CelestialBody.h"


using namespace FusionEngine;


/////////////////////
//  Update System  //
/////////////////////
void UpdateSystem::ProcessEntity(EntityManager *manager, Entity *entity)
{
	ComponentMapper<Updatable> functionalData = manager->GetComponentList(entity, CT_UPDATABLE_BEHAVIOR);

	functionalData[0]->updatedObject->Update();
}


////////////////////////
//  Collision System  //
////////////////////////
void CollisionSystem::ProcessEntity(EntityManager *manager, Entity *entity)
{
	ComponentMapper<Collidable> collidableData = manager->GetComponentList(entity, CT_COLLISION);
	if (collidableData[0]->isForCheck)
	{
		ComponentMapper<Transform> transformData = manager->GetComponentList(entity, CT_TRANSFORM);
						
		FusionEngine::DisplayData displayData = World::GetWorld().GetDisplayData();

		Utility::Ray mouseRay = 
			World::GetWorld().GetMouse().GetPickRay(displayData.projectionMatrix, displayData.modelMatrix.Top(),
													glm::vec4(World::GetWorld().GetCamera().ResolveCamPosition(), 1.0f), 
													displayData.windowWidth, displayData.windowHeight);
				
		if (Utility::Intersections::RayIntersectsSphere(mouseRay, transformData[0]->position, transformData[0]->scale.x))
		{
			World::GetWorld().GetEventManager().FireEvent(FusionEngine::OnClickEvent(FusionEngine::EVENT_ON_CLICK, true, "sun"));
		}
	}
}


////////////////////
//  Skill System  //
////////////////////
void SkillSystem::ProcessEntity(EntityManager *manager, Entity *entity)
{
	ComponentMapper<Skill> skillData = manager->GetComponentList(entity, CT_SKILL);
	if (skillData[0]->OnUpdate)
	{
		skillData[0]->OnUpdate(manager, entity);
	}

	/*
	ComponentMapper<Transform> transformData = manager->GetComponentList(entity, CT_TRANSFORM);
	if (transformData.GetSize() > 0)
	{
		ComponentMapper<SelectorAppliedSkill> selectorData = manager->GetComponentList(entity, CT_SELECTOR_APPLIED_SKILL);
		if (selectorData.GetSize() > 0)
		{			
			DisplayData currentDisplayData = GetWorld().GetDisplayData();

			glm::vec4 mousePos_atZ =
				GetWorld().GetMouse().GetPositionAtDimension(currentDisplayData.windowWidth,
															 currentDisplayData.windowHeight,
															 currentDisplayData.projectionMatrix,
															 currentDisplayData.modelMatrix.Top(),
															 glm::vec4(GetWorld().GetCamera().ResolveCamPosition(), 1.0f),
															 glm::comp::Y);

			transformData[0]->position = mousePos_atZ.swizzle(glm::comp::X, glm::comp::Y, glm::comp::Z);
		}
	}*/
}

bool SkillSystem::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch (type)
	{
	case FusionEngine::EVENT_ON_FUSION_COMPLETED:
		{
			const OnFusionCompletedEvent &data = static_cast<const OnFusionCompletedEvent&>(eventData);
			ComponentMapper<Skill> skillData = 
				GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(data.skillID), CT_SKILL);
			if (skillData[0]->OnFusionCompleted)
			{
				skillData[0]->OnFusionCompleted(GetScene().GetEntityManager(), GetScene().GetEntity(data.skillID),
												eventData);
			}

			/*
			const OnFusionCompletedEvent &data = static_cast<const OnFusionCompletedEvent&>(eventData);
					
			ComponentMapper<Skill> skillData = 
				GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(data.skillID), CT_SKILL);
			if (skillData.GetSize() > 0)
			{
				skillData[0]->isActive = true;
				if (skillData[0]->isAppliedOnActive)
				{
					GetWorld().GetEventManager().
						FireEvent(OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, 
													  skillData[0]->position, 
													  skillData[0]->range, 
													  skillData[0]->damage, 
													  false));
				}
			}

			ComponentMapper<SatelliteCreation> satCreationData = 
				GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(data.skillID), CT_SATELLITE_CREATION);
			if (satCreationData.GetSize() > 0)
			{
				ComponentMapper<Updatable> sunUpdatable =
					GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity("sun"), CT_UPDATABLE_BEHAVIOR);
				static_cast<CelestialBody*>(sunUpdatable[0]->updatedObject.get())->AddSatellite(satCreationData[0]->satType);
			*/
		}
		break;
	case FusionEngine::EVENT_ON_CLICK:
		{
			std::string skillName = GetWorld().GetActiveSkillName();
			if (skillName.length() > 0)
			{
				ComponentMapper<Skill> skillData = 
					GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(skillName), CT_SKILL);
				if (skillData[0]->OnClick)
				{
					skillData[0]->OnClick(GetScene().GetEntityManager(), GetScene().GetEntity(skillName), eventData);
				}
			}

			/*
			const OnClickEvent &data = static_cast<const OnClickEvent&>(eventData);
			
			std::string skillName = GetWorld().GetActiveSkillName();
			ComponentMapper<Skill> skillData = 
				GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(skillName), CT_SKILL);
			if (skillData[0]->isActive)
			{
				skillData[0]->isDeployed = true;
				if (skillName == "aoeSkill") // bad!!!
				{
					skillData[0]->isActive = false;

					ComponentMapper<Transform> transformData =
						GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(skillName), CT_TRANSFORM);

					OnSkillAppliedEvent _event = 
						OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, 
											transformData[0]->position, skillData[0]->range, skillData[0]->damage, false);
					GetWorld().GetEventManager().FireEvent(_event);
				}
				else if (skillName == "burnSkill") // bad ^ 2!!!
				{
					ComponentMapper<TimedSkill> timedSkillData =
						GetScene().GetEntityManager()->GetComponentList(GetScene().GetEntity(skillName), CT_TIMED_SKILL);
					timedSkillData[0]->attackTimer.SetPause(false);
					timedSkillData[0]->attackTimer.Reset();
				}
			}
			*/
		}
		break;
	}

	return false;
}