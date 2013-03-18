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


void TweakableVarsLoader::PushInt(std::string command, int value, int enumIndex)
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

void TweakableVarsLoader::PushFloat(std::string command, float value, int enumIndex)
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
		else if(command == "timDecrement")
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
		else if(command == "resourceGainPerTime")
		{
			for(YAML::Node::const_iterator satType = tweak->begin();
				satType != tweak->end(); ++satType)
			{
				if(satType->second["sat-all"]) 
				{
					PushInt(command, satType->second["sat-all"].as<int>(), 4);
				}
				else if(satType->second["sat-fire"]) 
				{
					PushInt(command, satType->second["sat-fire"].as<int>(), 0);
				}
				else if(satType->second["sat-water"])
				{
					PushInt(command, satType->second["sat-water"].as<int>(), 1);
				}
				else if(satType->second["sat-air"])
				{
					PushInt(command, satType->second["sat-air"].as<int>(), 2);
				}
				else if(satType->second["sat-earth"])
				{
					PushInt(command, satType->second["sat-earth"].as<int>(), 3);
				}
			}
		}
	}

	/*
	std::string line;
	std::ifstream data(fileName);

	loadedTweaks.clear();

	if(!data.is_open())
	{
		std::string errorMessage = "cannot open tweak config file ";
		errorMessage += fileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}

	while(data)
	{
		getline(data, line);
		char command[50];
		sscanf(line.c_str(), "%s ", &command);

		if(strcmp(command, "currentEnemyCount") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i ", &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "maxEnemyCount") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i ", &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "initialSpawnTime") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "endSpawnTime") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "timeDecrement") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemyDestructionRadius") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "resourceCount") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i ", &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "resourceGainTime") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "resourceGainPerTime") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "satConstructionCost") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i ", &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "satHealth") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "health") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i ", &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemyDamage") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemyChargeSpeed") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemySpawnInnerRad") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemySpawnOuterRad") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemySpeed") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemyLOS") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemyHealth") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemyResource") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "swarmersCount") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i ", &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "swarmersAttackTime") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "enemyProjectileSpeed") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "deployUnitsCount") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i ", &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "deployUnitsLife") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i ", &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "deployUnitsResourceGivenOnKill") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i ", &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "deployUnitsSpeed") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "deployUnitsLOS") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "deployUnitsProjSpeed") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%f ", &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillDamage") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillRange") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillApplyCost") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillScaleRate") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillDamageApplyTime") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillDuration") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillDefensePoints") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillStunTime") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_FLOAT;
			sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillUpgradeBoxIndex") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillForWhichSatellite") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_INT;
			sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillUpgradedTexture") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_STRING;
			sscanf(line.c_str(), "%i %s ", &varData.itemIndex, &varData.varString);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "skillToolTipText") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(command));
			line[0] = ' ';

			TweakVarData varData;
			varData.currentType = TweakVarData::TYPE_TWEAK_STRING;
			sscanf(line.c_str(), "%i %s ", &varData.itemIndex, &varData.varString);
			loadedTweaks.push_back(std::pair<std::string, TweakVarData>(command, varData));
		}
		if(strcmp(command, "**") == 0)
		{
			continue;
		}
		if(strcmp(command, "endfile") == 0)
		{
			data.close();
			return;
		}
	}
	data.close();
	*/
}

const std::vector<std::pair<std::string, TweakVarData>> TweakableVarsLoader::GetAllLoadedVars()
{
	return loadedTweaks;
}