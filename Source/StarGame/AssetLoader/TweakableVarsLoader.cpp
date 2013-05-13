//Copyright 2012, 2013 Tsvetan Tsvetanov
//This file is part of the Star Game.
//
//The Star Game is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//The Star Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with the Star Game.  If not, see <http://www.gnu.org/licenses/>.


#include "stdafx.h"
#include "TweakableVarsLoader.h"
#include "../framework/ErrorAPI.h"

#include <fstream>


// TODO: It has room for further generalization


void TweakableVarsLoader::PushInt(const std::string &command, int value, int enumIndex)
{
    TweakVarData tweakData;
    tweakData.currentType = TweakVarData::TYPE_TWEAK_INT;
    tweakData.varInt = value;
    if(enumIndex != -999)
    {
        tweakData.itemIndex = enumIndex;
    }
    loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, tweakData));
}

void TweakableVarsLoader::PushFloat(const std::string &command, float value, int enumIndex)
{
    TweakVarData tweakData;
    tweakData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
    tweakData.varFloat = value;
    if(enumIndex != -999)
    {
        tweakData.itemIndex = enumIndex;
    }
    loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, tweakData));
}

void TweakableVarsLoader::PushString(const std::string &command, const std::string &value, int enumIndex)
{
    TweakVarData tweakData;
    tweakData.currentType = TweakVarData::TYPE_TWEAK_STRING;
    std::strcpy(tweakData.varString, value.c_str());
    if(enumIndex != -999)
    {
        tweakData.itemIndex = enumIndex;
    }
    loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, tweakData));
}

TweakableVarsLoader::TweakableVarsLoader(const std::string &fileName)
{
    YAML::Node tweaks = YAML::LoadFile(fileName);
    
    for(YAML::Node::const_iterator tweak = tweaks.begin();
        tweak != tweaks.end(); ++tweak)
    {
        TweakVarData tweakData;
        std::string command = tweak->first.as<std::string>();

        if(command == "currentEnemyCount")
        {
            PushInt(command, tweak->second.as<int>());
        }
        else if(command == "maxEnemyCount")
        {
            PushInt(command, tweak->second.as<int>());
        }
        else if(command == "initialSpawnTime")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "endSpawnTime")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "timeDecrement")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "enemyDestructionRadius")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "resourceCount")
        {
            PushInt(command, tweak->second.as<int>());
        }
        else if(command == "resourceGainTime")
        {
            for(YAML::Node::const_iterator satType = tweak->second.begin();
                satType != tweak->second.end(); ++satType)
            {
                if(satType->first.as<std::string>() == "sat-all") 
                {
                    PushFloat(command, satType->second.as<float>(), 4);
                }
                else if(satType->first.as<std::string>() == "sat-fire") 
                {
                    PushFloat(command, satType->second.as<float>(), 0);
                }
                else if(satType->first.as<std::string>() == "sat-water")
                {
                    PushFloat(command, satType->second.as<float>(), 1);
                }
                else if(satType->first.as<std::string>() == "sat-air")
                {
                    PushFloat(command, satType->second.as<float>(), 2);
                }
                else if(satType->first.as<std::string>() == "sat-earth")
                {
                    PushFloat(command, satType->second.as<float>(), 3);
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
                    PushInt(command, satType->second.as<int>(), 4);
                }
                else if(satType->first.as<std::string>() == "sat-fire") 
                {
                    PushInt(command, satType->second.as<int>(), 0);
                }
                else if(satType->first.as<std::string>() == "sat-water")
                {
                    PushInt(command, satType->second.as<int>(), 1);
                }
                else if(satType->first.as<std::string>() == "sat-air")
                {
                    PushInt(command, satType->second.as<int>(), 2);
                }
                else if(satType->first.as<std::string>() == "sat-earth")
                {
                    PushInt(command, satType->second.as<int>(), 3);
                }
            }
        }
        else if(command == "satConstructionCost")
        {
            PushInt(command, tweak->second.as<int>());
        }
        else if(command == "satHealth")
        {
            for(YAML::Node::const_iterator satType = tweak->second.begin();
                satType != tweak->second.end(); ++satType)
            {
                if(satType->first.as<std::string>() == "sat-all") 
                {
                    PushInt(command, satType->second.as<int>(), 4);
                }
                else if(satType->first.as<std::string>() == "sat-fire") 
                {
                    PushInt(command, satType->second.as<int>(), 0);
                }
                else if(satType->first.as<std::string>() == "sat-water")
                {
                    PushInt(command, satType->second.as<int>(), 1);
                }
                else if(satType->first.as<std::string>() == "sat-air")
                {
                    PushInt(command, satType->second.as<int>(), 2);
                }
                else if(satType->first.as<std::string>() == "sat-earth")
                {
                    PushInt(command, satType->second.as<int>(), 3);
                }
            }
        }
        else if(command == "health")
        {
            PushInt(command, tweak->second.as<int>());
        }
        else if(command == "enemyDamage")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    PushInt(command, enemyType->second.as<int>(), 5);
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    PushInt(command, enemyType->second.as<int>(), 0);
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    PushInt(command, enemyType->second.as<int>(), 1);
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    PushInt(command, enemyType->second.as<int>(), 2);
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    PushInt(command, enemyType->second.as<int>(), 3);
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    PushInt(command, enemyType->second.as<int>(), 4);
                }
            }
        }
        else if(command == "enemyChargeSpeed")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "enemySpawnInnerRad")
        {
            for(YAML::Node::const_iterator enemyType = tweak->second.begin();
                enemyType != tweak->second.end(); ++enemyType)
            {
                if(enemyType->first.as<std::string>() == "enemy-all")
                {
                    PushFloat(command, enemyType->second.as<float>(), 5);
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    PushFloat(command, enemyType->second.as<float>(), 0);
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    PushFloat(command, enemyType->second.as<float>(), 1);
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    PushFloat(command, enemyType->second.as<float>(), 2);
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    PushFloat(command, enemyType->second.as<float>(), 3);
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    PushFloat(command, enemyType->second.as<float>(), 4);
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
                    PushFloat(command, enemyType->second.as<float>(), 5);
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    PushFloat(command, enemyType->second.as<float>(), 0);
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    PushFloat(command, enemyType->second.as<float>(), 1);
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    PushFloat(command, enemyType->second.as<float>(), 2);
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    PushFloat(command, enemyType->second.as<float>(), 3);
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    PushFloat(command, enemyType->second.as<float>(), 4);
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
                    PushFloat(command, enemyType->second.as<float>(), 5);
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    PushFloat(command, enemyType->second.as<float>(), 0);
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    PushFloat(command, enemyType->second.as<float>(), 1);
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    PushFloat(command, enemyType->second.as<float>(), 2);
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    PushFloat(command, enemyType->second.as<float>(), 3);
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    PushFloat(command, enemyType->second.as<float>(), 4);
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
                    PushFloat(command, enemyType->second.as<float>(), 5);
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    PushFloat(command, enemyType->second.as<float>(), 0);
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    PushFloat(command, enemyType->second.as<float>(), 1);
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    PushFloat(command, enemyType->second.as<float>(), 2);
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    PushFloat(command, enemyType->second.as<float>(), 3);
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    PushFloat(command, enemyType->second.as<float>(), 4);
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
                    PushInt(command, enemyType->second.as<int>(), 5);
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    PushInt(command, enemyType->second.as<int>(), 0);
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    PushInt(command, enemyType->second.as<int>(), 1);
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    PushInt(command, enemyType->second.as<int>(), 2);
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    PushInt(command, enemyType->second.as<int>(), 3);
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    PushInt(command, enemyType->second.as<int>(), 4);
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
                    PushInt(command, enemyType->second.as<int>(), 5);
                }
                else if(enemyType->first.as<std::string>() == "enemy-swarm")
                {
                    PushInt(command, enemyType->second.as<int>(), 0);
                }
                else if(enemyType->first.as<std::string>() == "enemy-spaceship")
                {
                    PushInt(command, enemyType->second.as<int>(), 1);
                }
                else if(enemyType->first.as<std::string>() == "enemy-mothership")
                {
                    PushInt(command, enemyType->second.as<int>(), 2);
                }
                else if(enemyType->first.as<std::string>() == "enemy-fast-suicide-bomber")
                {
                    PushInt(command, enemyType->second.as<int>(), 3);
                }
                else if(enemyType->first.as<std::string>() == "enemy-asteroid")
                {
                    PushInt(command, enemyType->second.as<int>(), 4);
                }
            }
        }
        else if(command == "swarmersCount")
        {
            PushInt(command, tweak->second.as<int>());
        }
        else if(command == "swarmersAttackTime")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "enemyProjectileSpeed")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "deployUnitsCount")
        {
            PushInt(command, tweak->second.as<int>());
        }
        else if(command == "deployUnitsLife")
        {
            PushInt(command, tweak->second.as<int>());
        }
        else if(command == "deployUnitsResourceGivenOnKill")
        {
            PushInt(command, tweak->second.as<int>());
        }
        else if(command == "deployUnitsSpeed")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "deployUnitsLOS")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "deployUnitsProjSpeed")
        {
            PushFloat(command, tweak->second.as<float>());
        }
        else if(command == "skillDamage")
        {
            for(YAML::Node::const_iterator skillType = tweak->second.begin();
                skillType != tweak->second.end(); ++skillType)
            {
                if(skillType->first.as<std::string>() == "skill-all")
                {
                    PushInt(command, skillType->second.as<int>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushInt(command, skillType->second.as<int>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushInt(command, skillType->second.as<int>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushInt(command, skillType->second.as<int>(), 7);
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
                    PushFloat(command, skillType->second.as<float>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushFloat(command, skillType->second.as<float>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushFloat(command, skillType->second.as<float>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushFloat(command, skillType->second.as<float>(), 7);
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
                    PushInt(command, skillType->second.as<int>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushInt(command, skillType->second.as<int>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushInt(command, skillType->second.as<int>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushInt(command, skillType->second.as<int>(), 7);
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
                    PushFloat(command, skillType->second.as<float>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushFloat(command, skillType->second.as<float>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushFloat(command, skillType->second.as<float>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushFloat(command, skillType->second.as<float>(), 7);
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
                    PushFloat(command, skillType->second.as<float>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushFloat(command, skillType->second.as<float>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushFloat(command, skillType->second.as<float>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushFloat(command, skillType->second.as<float>(), 7);
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
                    PushFloat(command, skillType->second.as<float>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushFloat(command, skillType->second.as<float>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushFloat(command, skillType->second.as<float>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushFloat(command, skillType->second.as<float>(), 7);
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
                    PushInt(command, skillType->second.as<int>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushInt(command, skillType->second.as<int>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushInt(command, skillType->second.as<int>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushInt(command, skillType->second.as<int>(), 7);
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
                    PushFloat(command, skillType->second.as<float>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushFloat(command, skillType->second.as<float>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushFloat(command, skillType->second.as<float>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushFloat(command, skillType->second.as<float>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushFloat(command, skillType->second.as<float>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushFloat(command, skillType->second.as<float>(), 7);
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
                    PushInt(command, skillType->second.as<int>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushInt(command, skillType->second.as<int>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushInt(command, skillType->second.as<int>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushInt(command, skillType->second.as<int>(), 7);
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
                    PushInt(command, skillType->second.as<int>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushInt(command, skillType->second.as<int>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushInt(command, skillType->second.as<int>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushInt(command, skillType->second.as<int>(), 7);
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
                    PushString(command, skillType->second.as<std::string>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushString(command, skillType->second.as<std::string>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushString(command, skillType->second.as<std::string>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushString(command, skillType->second.as<std::string>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushString(command, skillType->second.as<std::string>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushString(command, skillType->second.as<std::string>(), 7);
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
                    PushString(command, skillType->second.as<std::string>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushString(command, skillType->second.as<std::string>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushString(command, skillType->second.as<std::string>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushString(command, skillType->second.as<std::string>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushString(command, skillType->second.as<std::string>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushString(command, skillType->second.as<std::string>(), 7);
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
                    PushInt(command, skillType->second.as<int>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushInt(command, skillType->second.as<int>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushInt(command, skillType->second.as<int>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushInt(command, skillType->second.as<int>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushInt(command, skillType->second.as<int>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushInt(command, skillType->second.as<int>(), 7);
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
                    PushString(command, skillType->second.as<std::string>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushString(command, skillType->second.as<std::string>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushString(command, skillType->second.as<std::string>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushString(command, skillType->second.as<std::string>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushString(command, skillType->second.as<std::string>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushString(command, skillType->second.as<std::string>(), 7);
                }
                else if(skillType->first.as<std::string>() == "skill-fire-sat")
                {
                    PushString(command, skillType->second.as<std::string>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-earth-sat")
                {
                    PushString(command, skillType->second.as<std::string>(), 9);
                }
                else if(skillType->first.as<std::string>() == "skill-water-sat")
                {
                    PushString(command, skillType->second.as<std::string>(), 10);
                }
                else if(skillType->first.as<std::string>() == "skill-air-sat")
                {
                    PushString(command, skillType->second.as<std::string>(), 11);
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
                    PushString(command, skillType->second.as<std::string>(), 0);
                }
                else if(skillType->first.as<std::string>() == "skill-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 1);
                }
                else if(skillType->first.as<std::string>() == "skill-sun-nova")
                {
                    PushString(command, skillType->second.as<std::string>(), 2);
                }
                else if(skillType->first.as<std::string>() == "skill-burn")
                {
                    PushString(command, skillType->second.as<std::string>(), 3);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-passive-aoe")
                {
                    PushString(command, skillType->second.as<std::string>(), 4);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-chain")
                {
                    PushString(command, skillType->second.as<std::string>(), 5);
                }
                else if(skillType->first.as<std::string>() == "skill-frost-nova")
                {
                    PushString(command, skillType->second.as<std::string>(), 6);
                }
                else if(skillType->first.as<std::string>() == "skill-sat-shield")
                {
                    PushString(command, skillType->second.as<std::string>(), 7);
                }
                else if(skillType->first.as<std::string>() == "skill-fire-sat")
                {
                    PushString(command, skillType->second.as<std::string>(), 8);
                }
                else if(skillType->first.as<std::string>() == "skill-earth-sat")
                {
                    PushString(command, skillType->second.as<std::string>(), 9);
                }
                else if(skillType->first.as<std::string>() == "skill-water-sat")
                {
                    PushString(command, skillType->second.as<std::string>(), 10);
                }
                else if(skillType->first.as<std::string>() == "skill-air-sat")
                {
                    PushString(command, skillType->second.as<std::string>(), 11);
                }
            }
        }
    }
}

const std::vector<std::pair<std::string, TweakVarData>> TweakableVarsLoader::GetAllLoadedVars()
{
    return loadedTweaks;
}