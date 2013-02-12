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

#include <fstream>


TweakableVarsLoader::TweakableVarsLoader(const std::string &fileName)
{
	std::string line;
	std::ifstream data(fileName);

	loadedTweaks.clear();

	if(data.is_open())
	{
		while(!data.eof())
		{
			getline(data, line);
			char command[50];
			sscanf(line.c_str(), "%s ", &command);

			if(strcmp(command, "currentEnemyCount") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("currentEnemyCount", varData));
			}
			if(strcmp(command, "maxEnemyCount") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("maxEnemyCount", varData));
			}
			if(strcmp(command, "initialSpawnTime") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%f ", &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("initialSpawnTime", varData));
			}
			if(strcmp(command, "endSpawnTime") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%f ", &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("endSpawnTime", varData));
			}
			if(strcmp(command, "timeDecrement") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%f ", &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("timeDecrement", varData));
			}
			if(strcmp(command, "enemyDestructionRadius") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%f ", &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemyDestructionRadius", varData));
			}
			if(strcmp(command, "resourceCount") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("resourceCount", varData));
			}
			if(strcmp(command, "resourceGainTime") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("resourceGainTime", varData));
			}
			if(strcmp(command, "resourceGainPerTime") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("resourceGainPerTime", varData));
			}
			if(strcmp(command, "satConstructionCost") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("satConstructionCost", varData));
			}
			if(strcmp(command, "satHealth") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("satHealth", varData));
			}
			if(strcmp(command, "health") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("health", varData));
			}
			if(strcmp(command, "enemyDamage") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemyDamage", varData));
			}
			if(strcmp(command, "enemyChargeSpeed") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemyChargeSpeed", varData));
			}
			if(strcmp(command, "enemySpawnInnerRad") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemySpawnInnerRad", varData));
			}
			if(strcmp(command, "enemySpawnOuterRad") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemySpawnOuterRad", varData));
			}
			if(strcmp(command, "enemySpeed") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemySpeed", varData));
			}
			if(strcmp(command, "enemyLOS") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemyLOS", varData));
			}
			if(strcmp(command, "enemyHealth") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemyHealth", varData));
			}
			if(strcmp(command, "enemyResource") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemyResource", varData));
			}
			if(strcmp(command, "swarmersCount") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("swarmersCount", varData));
			}
			if(strcmp(command, "swarmersAttackTime") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%f ", &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("swarmersAttackTime", varData));
			}
			if(strcmp(command, "enemyProjectileSpeed") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("enemyProjectileSpeed", varData));
			}
			if(strcmp(command, "deployUnitsCount") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("deployUnitsCount", varData));
			}
			if(strcmp(command, "deployUnitsLife") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("deployUnitsLife", varData));
			}
			if(strcmp(command, "deployUnitsResourceGivenOnKill") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("deployUnitsResourceGivenOnKill", varData));
			}
			if(strcmp(command, "deployUnitsSpeed") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%f ", &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("deployUnitsSpeed", varData));
			}
			if(strcmp(command, "deployUnitsLOS") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%f ", &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("deployUnitsLOS", varData));
			}
			if(strcmp(command, "deployUnitsProjSpeed") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%f ", &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("deployUnitsProjSpeed", varData));
			}
			if(strcmp(command, "skillDamage") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("skillDamage", varData));
			}
			if(strcmp(command, "skillRange") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("skillRange", varData));
			}
			if(strcmp(command, "skillApplyCost") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", &varData.itemIndex, &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("skillApplyCost", varData));
			}
			if(strcmp(command, "skillScaleRate") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("skillScaleRate", varData));
			}
			if(strcmp(command, "skillDamageApplyTime") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("skillDamageApplyTime", varData));
			}
			if(strcmp(command, "skillDuration") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", &varData.itemIndex, &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("skillDuration", varData));
			}
			if(strcmp(command, "skillDefensePoints") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i ", &varData.varInt);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("skillDefensePoints", varData));
			}
			if(strcmp(command, "skillStunTime") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%f ", &varData.varFloat);
				loadedTweaks.push_back(std::pair<std::string, TweakVarData>("skillStunTime", varData));
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
	}
	else
	{
		// TODO: Better error handling
		std::printf("Cannot open config file.\n");
		return;
	}
}

const std::vector<std::pair<std::string, TweakVarData>> &TweakableVarsLoader::GetAllLoadedVars()
{
	return loadedTweaks;
}