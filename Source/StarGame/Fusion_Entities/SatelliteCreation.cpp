#include "stdafx.h"
#include "SatelliteCreation.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void FusionEngine::SatelliteCreation_OnFusionCompleted(const std::string &skillID, const IEventData &eventData)
{
	SkillSatelliteCreationComponent *satCreation =
		static_cast<SkillSatelliteCreationComponent*>
			(GetWorld().GetComponentForObject(skillID, FE_COMPONENT_SKILL_SATELLITE_CREATION).get());
	GetWorld().GetSun()->AddSatellite(satCreation->satelliteType);
}