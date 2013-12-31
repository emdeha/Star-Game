#include "stdafx.h"
#include "FusionSystems.h"

#include "../Fusion_Scene/Scene.h"


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
	ComponentMapper<Transform> transformData = manager->GetComponentList(entity, CT_TRANSFORM);
	if (transformData.GetSize() > 0)
	{
		// Apply transformations
	}
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
			if (skillData.GetSize() > 0)
			{
				skillData[0]->isActive = true;
				if (skillData[0]->isAppliedOnActive)
				{
					GetWorld().GetEventManager().
						FireEvent(OnSkillAppliedEvent(EVENT_ON_SKILL_APPLIED, glm::vec3(), -1.0f, 300, false));
				}
			}
		}
		break;
	}

	return false;
}