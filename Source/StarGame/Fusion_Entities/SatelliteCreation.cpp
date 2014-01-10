#include "stdafx.h"
#include "SatelliteCreation.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void FusionEngine::SatelliteCreation_OnFusionCompleted(EntityManager *entityManager, Entity *entity, 
													   const IEventData &eventData)
{
	//ComponentMapper<SatelliteCreation> satCreationData = 
	//	entityManager->GetComponentList(entity, CT_SATELLITE_CREATION);
	//if (satCreationData.GetSize() > 0)
	//{
	//	ComponentMapper<Updatable> sunUpdatable =
	//		entityManager->GetComponentList(GetScene().GetEntity("sun"), CT_UPDATABLE_BEHAVIOR);
	//	static_cast<CelestialBody*>(sunUpdatable[0]->updatedObject.get())->AddSatellite(satCreationData[0]->satType);
	//}
}