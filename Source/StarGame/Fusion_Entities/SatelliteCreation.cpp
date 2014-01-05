#include "stdafx.h"
#include "SatelliteCreation.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Scene/World.h"

using namespace FusionEngine;


void FusionEngine::SatelliteCreation_OnFusionCompleted(const IEventData &eventData)
{
	GetWorld().GetSun()->AddSatellite(FE_AIR_SAT);
}