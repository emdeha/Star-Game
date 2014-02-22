#include "stdafx.h"
#include "AssetLoader.h"

#include "../Fusion_Entities/Skill.h"
#include "../Fusion_Entities/Components.h"

using namespace FusionEngine;


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