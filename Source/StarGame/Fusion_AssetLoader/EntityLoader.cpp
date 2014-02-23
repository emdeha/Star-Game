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

CelestialBodyType GetCelestialBodyTypeByID(const std::string &id)
{
	if (id == "FIRE")
	{
		return FE_FIRE_SAT;
	}
	else if (id == "WATER")
	{
		return FE_WATER_SAT;
	}
	else if (id == "AIR")
	{
		return FE_AIR_SAT;
	}
	else if (id == "EARTH")
	{
		return FE_EARTH_SAT;
	}

	std::string errorMsg = " no celestial body with ID: ";
	errorMsg += id;
	HandleUnexpectedError(errorMsg, __LINE__, __FILE__, FE_SEVERITY_MED);
	return FE_CELESTIAL_BODY_BAD;
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

				std::shared_ptr<Skill> newSkill(new Skill(skillID, fComb[0], fComb[1], fComb[2],
												applyCost, researchCost));
				
				std::string onClickID = skill->second["scripts"][0].as<std::string>();
				std::string onFusionCompletedID = skill->second["scripts"][1].as<std::string>();
				std::string onUpdateID = skill->second["scripts"][2].as<std::string>();

				newSkill->SetOnClickCallback(GetOnClickMethodByID(onClickID));
				newSkill->SetOnFusionCompletedCallback(GetOnFusionCompletedMethodByID(onFusionCompletedID));
				newSkill->SetOnUpdateCallback(GetOnUpdateMethodByID(onUpdateID));

				if (skill->second["skill-sat-creation"])
				{
					std::shared_ptr<SkillSatelliteCreationComponent> satCreation(new SkillSatelliteCreationComponent);

					satCreation->satelliteType = 
						GetCelestialBodyTypeByID(skill->second["skill-sat-creation"]["sat-type"].as<std::string>());

					newSkill->AddComponent(FE_COMPONENT_SKILL_SATELLITE_CREATION, satCreation);
				}
				if (skill->second["skill-generic"])
				{
					std::shared_ptr<SkillGenericComponent> generic(new SkillGenericComponent);

					generic->holderID = skill->second["skill-generic"]["holder-id"].as<std::string>();
					generic->damage = skill->second["skill-generic"]["damage"].as<int>();
					generic->range = skill->second["skill-generic"]["range"].as<float>();
					generic->isChain = skill->second["skill-generic"]["is-chain"].as<bool>();
					generic->isActive = false;
					generic->isDeployed = false;
					generic->isDefensive = skill->second["skill-defensive"] ? true : false;

					newSkill->AddComponent(FE_COMPONENT_SKILL_GENERIC, generic);
				}
				if (skill->second["skill-selector-applied"])
				{
					std::shared_ptr<SkillSelectorAppliedComponent> selectorApplied(new SkillSelectorAppliedComponent);

					glm::vec4 color(skill->second["skill-selector-applied"]["selector-color"][0].as<float>(),
									skill->second["skill-selector-applied"]["selector-color"][1].as<float>(),
									skill->second["skill-selector-applied"]["selector-color"][2].as<float>(),
									skill->second["skill-selector-applied"]["selector-color"][3].as<float>());
					float range = skill->second["skill-generic"]["range"].as<float>();

					Utility::Primitives::Circle selector(color, glm::vec3(), range, 90); 

					selectorApplied->skillSelector = selector;

					newSkill->AddComponent(FE_COMPONENT_SKILL_SELECTOR_APPLIED, selectorApplied);
				}
				if (skill->second["skill-timed"])
				{
					std::shared_ptr<SkillTimedComponent> timed(new SkillTimedComponent); 

					timed->attackTimer = Framework::Timer(Framework::Timer::TT_INFINITE);
					timed->attackTimer.SetPause(true);
					if (skill->second["skill-timed"]["skill-life-secs"])
					{
						float skillLife_secs = skill->second["skill-timed"]["skill-life-secs"].as<float>();
						timed->skillLifeTimer = Framework::Timer(Framework::Timer::TT_SINGLE, skillLife_secs);
					}
					float skillDuration_secs = skill->second["skill-timed"]["damage-duration-secs"].as<float>();
					timed->damageApplyTimeDuration_seconds = skillDuration_secs;
					timed->damageApplyTime_seconds = skillDuration_secs;
					timed->duration_seconds = skill->second["skill-timed"]["skill-duration-secs"].as<float>();

					newSkill->AddComponent(FE_COMPONENT_SKILL_TIMED, timed);
				}
				if (skill->second["skill-animated"])
				{
					std::shared_ptr<SkillAnimatedComponent> animated(new SkillAnimatedComponent);

					animated->currentScale = skill->second["skill-animated"]["current-scale"].as<float>();
					animated->scaleRate = skill->second["skill-animated"]["scale-rate"].as<float>();

					float torusOuterRadius = 
						skill->second["skill-animated"]["torus-radius"].as<float>() + animated->currentScale;
					glm::vec4 torusColor(skill->second["skill-animated"]["color"][0].as<float>(),
										 skill->second["skill-animated"]["color"][1].as<float>(),
										 skill->second["skill-animated"]["color"][2].as<float>(),
										 skill->second["skill-animated"]["color"][3].as<float>());
					Utility::Primitives::Torus2D animTorus(torusColor, glm::vec3(), 
														   animated->currentScale, torusOuterRadius, 90); 
					animated->anim = animTorus;

					newSkill->AddComponent(FE_COMPONENT_SKILL_ANIMATED, animated);
				}
				if (skill->second["skill-defensive"])
				{
					std::shared_ptr<SkillDefensiveComponent> defensive(new SkillDefensiveComponent);

					int defensePoints = skill->second["skill-defensive"]["defense-points"].as<float>();
					defensive->currentDefensePoints = defensePoints;
					defensive->defensePoints = defensePoints;

					newSkill->AddComponent(FE_COMPONENT_SKILL_DEFENSIVE, defensive);
				}
				if (skill->second["transform"])
				{
					std::shared_ptr<TransformComponent> transform(new TransformComponent);

					transform->position = glm::vec3(skill->second["transform"]["position"][0].as<float>(),
												    skill->second["transform"]["position"][1].as<float>(),
												    skill->second["transform"]["position"][2].as<float>());

					newSkill->AddComponent(FE_COMPONENT_TRANSFORM, transform);
				}
				if (skill->second["collision"])
				{
					std::shared_ptr<CollisionComponent> collision(new CollisionComponent);
					glm::vec3 center(skill->second["transform"]["position"][0].as<float>(),
									 skill->second["transform"]["position"][1].as<float>(),
									 skill->second["transform"]["position"][2].as<float>());
					collision->center = center;

					std::string collisionType = skill->second["collision"]["type"].as<std::string>();
					if (type == "TORUS")
					{
						collision->cType = CollisionComponent::FE_COLLISION_TORUS;
						float innerRadius = skill->second["skill-animated"]["current-scale"].as<float>();
						collision->innerRadius = innerRadius;
						collision->outerRadius = innerRadius + skill->second["skill-animated"]["torus-radius"].as<float>();
					}
					else if (type == "CIRCLE")
					{
						collision->cType = CollisionComponent::FE_COLLISION_CIRCLE;
						collision->innerRadius = skill->second["skill-generic"]["range"].as<float>();
					}

					newSkill->AddComponent(FE_COMPONENT_COLLISION, collision);
				}

				loadedEntities.push_back(newSkill);
			}
		}
	}

	return loadedEntities;
}