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
				line.erase(command[0], strlen("currentEnemyCount"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("currentEnemyCount", varData));
			}
			if(strcmp(command, "maxEnemyCount") == 0)
			{
				line.erase(command[0], strlen("maxEnemyCount"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("maxEnemyCount", varData));
			}
			if(strcmp(command, "initialSpawnTime") == 0)
			{
				line.erase(command[0], strlen("initialSpawnTime"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%f ", varData.varFloat);
				loadedTweaks.insert(std::make_pair("initialSpawnTime", varData));
			}
			if(strcmp(command, "endSpawnTime") == 0)
			{
				line.erase(command[0], strlen("endSpawnTime"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%f ", varData.varFloat);
				loadedTweaks.insert(std::make_pair("endSpawnTime", varData));
			}
			if(strcmp(command, "timeDecrement") == 0)
			{
				line.erase(command[0], strlen("timeDecrement"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%f ", varData.varFloat);
				loadedTweaks.insert(std::make_pair("timeDecrement", varData));
			}
			if(strcmp(command, "enemyDestructionRadius") == 0)
			{
				line.erase(command[0], strlen("enemyDestructionRadius"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%f ", varData.varFloat);
				loadedTweaks.insert(std::make_pair("enemyDestructionRadius", varData));
			}
			if(strcmp(command, "resourceCount") == 0)
			{
				line.erase(command[0], strlen("resourceCount"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("resourceCount", varData));
			}
			if(strcmp(command, "resourceGainTime") == 0)
			{
				line.erase(command[0], strlen("resourceGainTime"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("resourceGainTime", varData));
			}
			if(strcmp(command, "resourceGainPerTime") == 0)
			{
				line.erase(command[0], strlen("resourceGainPerTime"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", varData.itemIndex, varData.varInt);
				loadedTweaks.insert(std::make_pair("resourceGainPerTime", varData));
			}
			if(strcmp(command, "satConstructionCost") == 0)
			{
				line.erase(command[0], strlen("satConstructionCost"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("satConstructionCost", varData));
			}
			if(strcmp(command, "satHealth") == 0)
			{
				line.erase(command[0], strlen("satHealth"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", varData.itemIndex, varData.varInt);
				loadedTweaks.insert(std::make_pair("satHealth", varData));
			}
			if(strcmp(command, "health") == 0)
			{
				line.erase(command[0], strlen("health"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("health", varData));
			}
			if(strcmp(command, "enemyDamage") == 0)
			{
				line.erase(command[0], strlen("enemyDamage"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", varData.itemIndex, varData.varInt);
				loadedTweaks.insert(std::make_pair("enemyDamage", varData));
			}
			if(strcmp(command, "enemyChargeSpeed") == 0)
			{
				line.erase(command[0], strlen("enemyChargeSpeed"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("enemyChargeSpeed", varData));
			}
			if(strcmp(command, "enemySpawnInnerRad") == 0)
			{
				line.erase(command[0], strlen("enemySpawnInnerRad"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("enemySpawnInnerRad", varData));
			}
			if(strcmp(command, "enemySpawnOuterRad") == 0)
			{
				line.erase(command[0], strlen("enemySpawnOuterRad"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("enemySpawnOuterRad", varData));
			}
			if(strcmp(command, "enemySpeed") == 0)
			{
				line.erase(command[0], strlen("enemySpeed"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("enemySpeed", varData));
			}
			if(strcmp(command, "enemyLOS") == 0)
			{
				line.erase(command[0], strlen("enemyLOS"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("enemyLOS", varData));
			}
			if(strcmp(command, "enemyHealth") == 0)
			{
				line.erase(command[0], strlen("enemyHealth"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", varData.itemIndex, varData.varInt);
				loadedTweaks.insert(std::make_pair("enemyHealth", varData));
			}
			if(strcmp(command, "enemyResource") == 0)
			{
				line.erase(command[0], strlen("enemyResource"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", varData.itemIndex, varData.varInt);
				loadedTweaks.insert(std::make_pair("enemyResource", varData));
			}
			if(strcmp(command, "swarmersCount") == 0)
			{
				line.erase(command[0], strlen("swarmersCount"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("swarmersCount", varData));
			}
			if(strcmp(command, "swarmersAttackTime") == 0)
			{
				line.erase(command[0], strlen("swarmersAttackTime"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%f ", varData.varFloat);
				loadedTweaks.insert(std::make_pair("swarmersAttackTime", varData));
			}
			if(strcmp(command, "enemyProjectileSpeed") == 0)
			{
				line.erase(command[0], strlen("enemyProjectileSpeed"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("enemyProjectileSpeed", varData));
			}
			if(strcmp(command, "deployUnitsCount") == 0)
			{
				line.erase(command[0], strlen("deployUnitsCount"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("deployUnitsCount", varData));
			}
			if(strcmp(command, "deployUnitsLife") == 0)
			{
				line.erase(command[0], strlen("deployUnitsLife"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("deployUnitsLife", varData));
			}
			if(strcmp(command, "deployUnitsResourceGivenOnKill") == 0)
			{
				line.erase(command[0], strlen("deployUnitsResourceGivenOnKill"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("deployUnitsResourceGivenOnKill", varData));
			}
			if(strcmp(command, "deployUnitsSpeed") == 0)
			{
				line.erase(command[0], strlen("deployUnitsSpeed"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%f ", varData.varFloat);
				loadedTweaks.insert(std::make_pair("deployUnitsSpeed", varData));
			}
			if(strcmp(command, "deployUnitsLOS") == 0)
			{
				line.erase(command[0], strlen("deployUnitsLOS"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%f ", varData.varFloat);
				loadedTweaks.insert(std::make_pair("deployUnitsLOS", varData));
			}
			if(strcmp(command, "skillDamage") == 0)
			{
				line.erase(command[0], strlen("skillDamage"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", varData.itemIndex, varData.varInt);
				loadedTweaks.insert(std::make_pair("skillDamage", varData));
			}
			if(strcmp(command, "skillRange") == 0)
			{
				line.erase(command[0], strlen("skillRange"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("skillRange", varData));
			}
			if(strcmp(command, "skillApplyCost") == 0)
			{
				line.erase(command[0], strlen("skillApplyCost"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %i ", varData.itemIndex, varData.varInt);
				loadedTweaks.insert(std::make_pair("skillApplyCost", varData));
			}
			if(strcmp(command, "skillScaleRate") == 0)
			{
				line.erase(command[0], strlen("skillScaleRate"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("skillScaleRate", varData));
			}
			if(strcmp(command, "skillDamageApplyTime") == 0)
			{
				line.erase(command[0], strlen("skillDamageApplyTime"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("skillDamageApplyTime", varData));
			}
			if(strcmp(command, "skillDuration") == 0)
			{
				line.erase(command[0], strlen("skillDuration"));
				line[0] = ' ';

				TweakVarData varData;
				sscanf(line.c_str(), "%i %f ", varData.itemIndex, varData.varFloat);
				loadedTweaks.insert(std::make_pair("skillDuration", varData));
			}
			if(strcmp(command, "skillDefensePoints") == 0)
			{
				line.erase(command[0], strlen("skillDefensePoints"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%i ", varData.varInt);
				loadedTweaks.insert(std::make_pair("skillDefensePoints", varData));
			}
			if(strcmp(command, "skillStunTime") == 0)
			{
				line.erase(command[0], strlen("skillStunTime"));
				line[0] = ' ';

				TweakVarData varData;
				varData.itemIndex = -1;
				sscanf(line.c_str(), "%f ", varData.varFloat);
				loadedTweaks.insert(std::make_pair("skillStunTime", varData));
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