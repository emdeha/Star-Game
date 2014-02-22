#include "stdafx.h"
#include "AssetLoader.h"

#include "../Fusion_Entities/Skill.h"
#include "../Fusion_Entities/Components.h"
#include "../Fusion_Entities/Scripts.h"

using namespace FusionEngine;


OnUpdateFunc GetOnUpdateMethodByID(const std::string &id)
{
	if (id == "AOE_OnUpdate")
	{
		return AOE_OnUpdate;
	}
	else if (id == "Burn_OnUpdate")
	{
		return Burn_OnUpdate;
	}
	else if (id == "SunNova_OnUpdate")
	{
		return SunNova_OnUpdate;
	}
	else if (id == "Chaining_OnUpdate")
	{
		return Chaining_OnUpdate;
	}
	else if (id == "Shield_OnUpdate")
	{
		return Shield_OnUpdate;
	}
	else if (id == "Frost_OnUpdate")
	{
		return Frost_OnUpdate;
	}

	return nullptr;
}
OnEventFunc GetOnFusionCompletedMethodByID(const std::string &id)
{
	if (id == "SatelliteCreation_OnFusionCompleted")
	{
		return SatelliteCreation_OnFusionCompleted;
	}
	else if (id == "Ultimate_OnFusionCompleted")
	{
		return Ultimate_OnFusionCompleted;
	}
	else if (id == "AOE_OnFusionCompleted")
	{
		return AOE_OnFusionCompleted;
	}
	else if (id == "Burn_OnFusionCompleted")
	{
		return Burn_OnFusionCompleted;
	}
	else if (id == "SunNova_OnFusionCompleted")
	{
		return SunNova_OnFusionCompleted;
	}
	else if (id == "PassiveAOE_OnFusionCompleted")
	{
		return PassiveAOE_OnFusionCompleted;
	}
	else if (id == "Chaining_OnFusionCompleted")
	{
		return Chaining_OnFusionCompleted;
	}
	else if (id == "Shield_OnFusionCompleted")
	{
		return Shield_OnFusionCompleted;
	}
	else if (id == "Frost_OnFusionCompleted")
	{
		return Frost_OnFusionCompleted;
	}

	return nullptr;
}
OnEventFunc GetOnClickMethodByID(const std::string &id)
{
	if (id == "AOE_OnClick")
	{
		return AOE_OnClick;
	}
	else if (id == "Burn_OnClick")
	{
		return Burn_OnClick;
	}

	return nullptr;
}

EntityAssetObject EntityLoader::Load(const std::string &type, const std::string &name)
{
	YAML::Node entityData = YAML::LoadFile("../data/" + type + "/" + name);	
	std::vector<std::shared_ptr<Composable>> loadedEntities;

	for (YAML::Node::const_iterator entityNode = entityData.begin();
		 entityNode != entityData.end(); ++entityNode)
	{
		if (entityNode->first.as<std::string>() == "entity-skill")
		{
			for (YAML::Node::const_iterator skill = entityNode->second.begin();
				 skill != entityNode->second.end(); ++skill)
			{
				std::string skillID = skill->first.as<std::string>();
				std::string fComb = skill->second["fusion-comb"].as<std::string>();
				int applyCost = skill->second["apply-cost"].as<int>();
				int researchCost = skill->second["research-cost"].as<int>();

				std::shared_ptr<Skill> newSkill = 
					std::shared_ptr<Skill>(new Skill(skillID, fComb[0], fComb[1], fComb[2],
													 applyCost, researchCost));

				loadedEntities.push_back(newSkill);
			}
		}
	}

	return loadedEntities;
}