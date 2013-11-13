#include "stdafx.h"
#include "AssetLoader.h"

#include "../framework/ErrorAPI.h"


using namespace FusionEngine;


const std::vector<std::pair<std::string, TweakVarData>> TweakVarAssetObject::GetAllLoadedVars() const
{
	return loadedTweaks;
}


std::pair<std::string, TweakVarData> TweakVarLoader::ToInt(const std::string &command, int value, int enumIndex) const
{
	TweakVarData tweakData;
    tweakData.currentType = TweakVarData::TYPE_TWEAK_INT;
    tweakData.varInt = value;
    if(enumIndex != -999)
    {
        tweakData.itemIndex = enumIndex;
    }
	return std::pair<std::string, TweakVarData>(command, tweakData);
}

std::pair<std::string, TweakVarData> TweakVarLoader::ToFloat(const std::string &command, float value, int enumIndex) const
{
	TweakVarData tweakData;
    tweakData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
    tweakData.varFloat = value;
    if(enumIndex != -999)
    {
        tweakData.itemIndex = enumIndex;
    }
	return std::pair<std::string, TweakVarData>(command, tweakData);
}

std::pair<std::string, TweakVarData> TweakVarLoader::ToString(const std::string &command, const std::string &value, int enumIndex) const
{
	TweakVarData tweakData;
    tweakData.currentType = TweakVarData::TYPE_TWEAK_STRING;
    std::strcpy(tweakData.varString, value.c_str());
    if(enumIndex != -999)
    {
        tweakData.itemIndex = enumIndex;
    }
	return std::pair<std::string, TweakVarData>(command, tweakData);
}

TweakVarAssetObject TweakVarLoader::Load(const std::string &type, const std::string &name)
{
	YAML::Node tweaks = YAML::LoadFile("../data/" + type + "/" + name);
    
	std::vector<std::pair<std::string, TweakVarData>> loadedTweaks;
    for(YAML::Node::const_iterator tweak = tweaks.begin();
        tweak != tweaks.end(); ++tweak)
    {
        TweakVarData tweakData;
        std::string command = tweak->first.as<std::string>();

        if(command == "currentEnemyCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "maxEnemyCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "initialSpawnTime")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "endSpawnTime")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "timeDecrement")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "enemyDestructionRadius")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "resourceCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "resourceGainTime")
        {
            for(YAML::Node::const_iterator satType = tweak->second.begin();
                satType != tweak->second.end(); ++satType)
            {
                if(satType->first.as<std::string>() == "sat-all") 
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 4));
                }
                else if(satType->first.as<std::string>() == "sat-fire") 
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 0));
                }
                else if(satType->first.as<std::string>() == "sat-water")
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 1));
                }
                else if(satType->first.as<std::string>() == "sat-air")
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 2));
                }
                else if(satType->first.as<std::string>() == "sat-earth")
                {
                    loadedTweaks.push_back(ToFloat(command, satType->second.as<float>(), 3));
                }
            }
        }
        else if(command == "resourceGainPerTime")
        {
            for(YAML::Node::const_iterator satType = tweak->second.begin();
                satType != tweak->second.end(); ++satType)
            {
                if(satType->first.as<std::string>() == "sat-all") 
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 4));
                }
                else if(satType->first.as<std::string>() == "sat-fire") 
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 0));
                }
                else if(satType->first.as<std::string>() == "sat-water")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 1));
                }
                else if(satType->first.as<std::string>() == "sat-air")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 2));
                }
                else if(satType->first.as<std::string>() == "sat-earth")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 3));
                }
            }
        }
        else if(command == "satConstructionCost")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "satHealth")
        {
            for(YAML::Node::const_iterator satType = tweak->second.begin();
                satType != tweak->second.end(); ++satType)
            {
                if(satType->first.as<std::string>() == "sat-all") 
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 4));
                }
                else if(satType->first.as<std::string>() == "sat-fire") 
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 0));
                }
                else if(satType->first.as<std::string>() == "sat-water")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 1));
                }
                else if(satType->first.as<std::string>() == "sat-air")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 2));
                }
                else if(satType->first.as<std::string>() == "sat-earth")
                {
                    loadedTweaks.push_back(ToInt(command, satType->second.as<int>(), 3));
                }
            }
        }
        else if(command == "health")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "enemyDamage")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 4));
                }
            }
        }
        else if(command == "enemyChargeSpeed")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "enemySpawnInnerRad")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 4));
                }
            }
        }
        else if(command == "enemySpawnOuterRad")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 4));
                }
            }
        }
        else if(command == "enemySpeed")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 4));
                }
            }
        }
        else if(command == "enemyLOS")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToFloat(command, enemyType->second.as<float>(), 4));
                }
            }
        }
        else if(command == "enemyHealth")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 4));
                }
            }
        }
        else if(command == "enemyResource")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 5));
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 0));
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 1));
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 2));
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 3));
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    loadedTweaks.push_back(ToInt(command, enemyType->second.as<int>(), 4));
                }
            }
        }
        else if(command == "swarmersCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "swarmersAttackTime")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "enemyProjectileSpeed")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "deployUnitsCount")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "deployUnitsLife")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "deployUnitsResourceGivenOnKill")
        {
            loadedTweaks.push_back(ToInt(command, tweak->second.as<int>()));
        }
        else if(command == "deployUnitsSpeed")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "deployUnitsLOS")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "deployUnitsProjSpeed")
        {
            loadedTweaks.push_back(ToFloat(command, tweak->second.as<float>()));
        }
        else if(command == "skillDamage")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillRange")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillApplyCost")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillScaleRate")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillDamageApplyTime")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillDuration")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillDefensePoints")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillStunTime")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToFloat(command, skillType->second.as<float>(), 7));
                }
            }
        }
        else if(command == "skillUpgradeBoxIndex")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillForWhichSatellite")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillUpgradedTexture")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 7));
                }
            }
        }
        else if(command == "skillToolTipText")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 7));
                }
            }
        }
        else if(command == "skillResearchCost")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToInt(command, skillType->second.as<int>(), 7));
                }
            }
        }
        else if(command == "skillHintText")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 7));
                }
                else if(skillType->first.as<std::string>() == "skill-fire-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-earth-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 9));
                }
                else if(skillType->first.as<std::string>() == "skill-water-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 10));
                }
                else if(skillType->first.as<std::string>() == "skill-air-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 11));
                }
            }
        }
        else if(command == "skillHintImage")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 0));
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 1));
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 2));
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 3));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 4));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 5));
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 6));
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 7));
                }
                else if(skillType->first.as<std::string>() == "skill-fire-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 8));
                }
                else if(skillType->first.as<std::string>() == "skill-earth-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 9));
                }
                else if(skillType->first.as<std::string>() == "skill-water-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 10));
                }
                else if(skillType->first.as<std::string>() == "skill-air-sat")
                {
                    loadedTweaks.push_back(ToString(command, skillType->second.as<std::string>(), 11));
                }
            }
        }
    }

	return TweakVarAssetObject(loadedTweaks);
}