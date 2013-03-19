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
#include "Scene.h"
#include "../framework/ErrorAPI.h"

#include <algorithm>
#include <ctime>
#include <sstream>


#define DEPLOY_UNITS_COUNT 4
static int countedUnits = 0;

Scene::Scene(float newSceneGamma,
			 float newInitialSpawnTime_secs, float newEndSpawnTime_secs, float newTimeDecrement_secs,
			 int newCurrentEnemyCount, int newMaxEnemyCount, float newEnemyDestructionRadius)
{
	lights.resize(0);
	suns.resize(0);
	enemies.resize(0);
	sceneLayouts.clear();

	enemyDestructionRadius = newEnemyDestructionRadius;
	sceneGamma = newSceneGamma;
	spawnData.currentEnemyCount = newCurrentEnemyCount;
	spawnData.maxEnemyCount = newMaxEnemyCount;
	spawnData.initialSpawnTime_secs = newInitialSpawnTime_secs;
	spawnData.endSpawnTime_secs = newEndSpawnTime_secs;
	spawnData.timeDecrement_secs = newTimeDecrement_secs;
	spawnData.waveSpawnTimer = Framework::Timer(Framework::Timer::TT_SINGLE, spawnData.initialSpawnTime_secs);

	lastUsedExplosion = 0;

	isSpawning = true;
}


void Scene::InitEnemyStats()
{
	enemyStats[ENEMY_TYPE_SWARM].damage = 5;
	enemyStats[ENEMY_TYPE_SWARM].health = 50;
	enemyStats[ENEMY_TYPE_SWARM].lineOfSight = 3.0f;
	enemyStats[ENEMY_TYPE_SWARM].resourceGivenOnKill = 10;
	enemyStats[ENEMY_TYPE_SWARM].speed = 0.02f;
	enemyStats[ENEMY_TYPE_SWARM].spawnRangeInnerRad = 4.0f;
	enemyStats[ENEMY_TYPE_SWARM].spawnRangeOuterRad = 4.0f;
	enemyStats[ENEMY_TYPE_SWARM].swarmersAttackTime_secs = 1;
	enemyStats[ENEMY_TYPE_SWARM].swarmersCount = 100;

	enemyStats[ENEMY_TYPE_SPACESHIP].damage = 1;
	enemyStats[ENEMY_TYPE_SPACESHIP].projectileSpeed = 0.3f;
	enemyStats[ENEMY_TYPE_SPACESHIP].health = 50;
	enemyStats[ENEMY_TYPE_SPACESHIP].lineOfSight = 2.0f;
	enemyStats[ENEMY_TYPE_SPACESHIP].resourceGivenOnKill = 10;
	enemyStats[ENEMY_TYPE_SPACESHIP].speed = 0.05f;
	enemyStats[ENEMY_TYPE_SPACESHIP].spawnRangeInnerRad = 2.0f;
	enemyStats[ENEMY_TYPE_SPACESHIP].spawnRangeOuterRad = 4.0f;
	
	enemyStats[ENEMY_TYPE_MOTHERSHIP].damage = 1;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].health = 300;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].lineOfSight = 5.0f;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].resourceGivenOnKill = 100;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].speed = 0.03f;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].spawnRangeInnerRad = 6.0f;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].spawnRangeOuterRad = 8.0f;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsCount = 4;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsProjSpeed = 0.3f; // WARN: may bug
	enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsLife = 50;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsResourceGivenOnKill = 10;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsSpeed = 0.05f;
	enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsLineOfSight = 2.0f;

	enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].damage = 50;
	enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].health = 50;
	enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].lineOfSight = 2.0f;
	enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].resourceGivenOnKill = 10;
	enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].speed = 0.02f;
	enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].fastSuicideBomberChargeSpeed = 0.1f;
	enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].spawnRangeInnerRad = 2.0f;
	enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].spawnRangeOuterRad = 4.0f;

	enemyStats[ENEMY_TYPE_ASTEROID].damage = 20;
	enemyStats[ENEMY_TYPE_ASTEROID].health = 50;
	enemyStats[ENEMY_TYPE_ASTEROID].lineOfSight = 0.0f;
	enemyStats[ENEMY_TYPE_ASTEROID].resourceGivenOnKill = 10;
	enemyStats[ENEMY_TYPE_ASTEROID].speed = 0.03f;
	enemyStats[ENEMY_TYPE_ASTEROID].spawnRangeInnerRad = 6.0f;
	enemyStats[ENEMY_TYPE_ASTEROID].spawnRangeOuterRad = 8.0f;
}
void Scene::InitSkillStats()
{
	skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].damage = 20;
	skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].damageApplyTime_secs = 1.0f;
	skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].duration_secs = 2.0f;
	skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].range = 1.0f;
	skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].skillApplyCost = 10;
	skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].skillResearchCost = 10;
	skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].upgradeBoxIndex = 0;
	skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].forWhichSatellite = SATELLITE_EARTH;

	skillsStats[SKILL_TYPE_SAT_CHAIN].damage = 20;
	skillsStats[SKILL_TYPE_SAT_CHAIN].range = 2.0f;
	skillsStats[SKILL_TYPE_SAT_CHAIN].scaleRate = 0.08f;
	skillsStats[SKILL_TYPE_SAT_CHAIN].upgradeBoxIndex = 0;
	skillsStats[SKILL_TYPE_SAT_CHAIN].forWhichSatellite = SATELLITE_COUNT;

	skillsStats[SKILL_TYPE_SAT_FROSTNOVA].damage = 5;
	skillsStats[SKILL_TYPE_SAT_FROSTNOVA].stunTime_secs = 3.0f;
	skillsStats[SKILL_TYPE_SAT_FROSTNOVA].range = 2.0f;
	skillsStats[SKILL_TYPE_SAT_FROSTNOVA].scaleRate = 0.1f;
	skillsStats[SKILL_TYPE_SAT_FROSTNOVA].skillApplyCost = 20;
	skillsStats[SKILL_TYPE_SAT_FROSTNOVA].skillResearchCost = 10;
	skillsStats[SKILL_TYPE_SAT_FROSTNOVA].upgradeBoxIndex = 0;
	skillsStats[SKILL_TYPE_SAT_FROSTNOVA].forWhichSatellite = SATELLITE_WATER;

	skillsStats[SKILL_TYPE_SAT_SHIELD].defensePoints = 3;
	skillsStats[SKILL_TYPE_SAT_SHIELD].range = 0.5f;
	skillsStats[SKILL_TYPE_SAT_SHIELD].skillApplyCost = 10;
	skillsStats[SKILL_TYPE_SAT_SHIELD].skillResearchCost = 10;
	skillsStats[SKILL_TYPE_SAT_SHIELD].upgradeBoxIndex = 0;
	skillsStats[SKILL_TYPE_SAT_SHIELD].forWhichSatellite = SATELLITE_AIR;

	// WARN: DRY violation!!!
	suns[0]->satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].damage = skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].damage;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].damageApplyTime_secs = skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].damageApplyTime_secs;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].duration_secs = skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].duration_secs;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].range = skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].range;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].skillApplyCost = skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].skillApplyCost;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].skillResearchCost = skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].skillResearchCost;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].upgradeBoxIndex = skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].upgradeBoxIndex;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].forWhichSatellite = skillsStats[SKILL_TYPE_SAT_PASSIVE_AOE].forWhichSatellite;

	suns[0]->satSkillStats[SKILL_TYPE_SAT_CHAIN].damage = skillsStats[SKILL_TYPE_SAT_CHAIN].damage;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_CHAIN].range = skillsStats[SKILL_TYPE_SAT_CHAIN].range;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_CHAIN].scaleRate = skillsStats[SKILL_TYPE_SAT_CHAIN].scaleRate;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_CHAIN].skillResearchCost = skillsStats[SKILL_TYPE_SAT_CHAIN].skillResearchCost;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_CHAIN].upgradeBoxIndex = skillsStats[SKILL_TYPE_SAT_CHAIN].upgradeBoxIndex;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_CHAIN].forWhichSatellite = skillsStats[SKILL_TYPE_SAT_CHAIN].forWhichSatellite;

	suns[0]->satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].damage = skillsStats[SKILL_TYPE_SAT_FROSTNOVA].damage;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].stunTime_secs = skillsStats[SKILL_TYPE_SAT_FROSTNOVA].stunTime_secs;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].range = skillsStats[SKILL_TYPE_SAT_FROSTNOVA].range;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].scaleRate = skillsStats[SKILL_TYPE_SAT_FROSTNOVA].scaleRate;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].skillApplyCost = skillsStats[SKILL_TYPE_SAT_FROSTNOVA].skillApplyCost;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].skillResearchCost = skillsStats[SKILL_TYPE_SAT_FROSTNOVA].skillResearchCost;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].upgradeBoxIndex = skillsStats[SKILL_TYPE_SAT_FROSTNOVA].upgradeBoxIndex;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].forWhichSatellite = skillsStats[SKILL_TYPE_SAT_FROSTNOVA].forWhichSatellite;

	suns[0]->satSkillStats[SKILL_TYPE_SAT_SHIELD].defensePoints = skillsStats[SKILL_TYPE_SAT_SHIELD].defensePoints;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_SHIELD].range = skillsStats[SKILL_TYPE_SAT_SHIELD].range;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_SHIELD].skillApplyCost = skillsStats[SKILL_TYPE_SAT_SHIELD].skillApplyCost;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_SHIELD].skillResearchCost = skillsStats[SKILL_TYPE_SAT_SHIELD].skillResearchCost;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_SHIELD].upgradeBoxIndex = skillsStats[SKILL_TYPE_SAT_SHIELD].upgradeBoxIndex;
	suns[0]->satSkillStats[SKILL_TYPE_SAT_SHIELD].forWhichSatellite = skillsStats[SKILL_TYPE_SAT_SHIELD].forWhichSatellite;

	skillsStats[SKILL_TYPE_AOE].damage = 20;
	skillsStats[SKILL_TYPE_AOE].range = 2.0f;
	skillsStats[SKILL_TYPE_AOE].skillApplyCost = 10;
	skillsStats[SKILL_TYPE_AOE].skillResearchCost = 10;
	skillsStats[SKILL_TYPE_AOE].upgradeBoxIndex = 3;

	skillsStats[SKILL_TYPE_SUN_NOVA].damage = 40;
	skillsStats[SKILL_TYPE_SUN_NOVA].range = 6.0f;
	skillsStats[SKILL_TYPE_SUN_NOVA].scaleRate = 0.05f;
	skillsStats[SKILL_TYPE_SUN_NOVA].skillApplyCost = 50;
	skillsStats[SKILL_TYPE_SUN_NOVA].skillResearchCost = 10;
	skillsStats[SKILL_TYPE_SUN_NOVA].upgradeBoxIndex = 2;

	skillsStats[SKILL_TYPE_BURN].damage = 30;
	skillsStats[SKILL_TYPE_BURN].damageApplyTime_secs = 1.0f;
	skillsStats[SKILL_TYPE_BURN].duration_secs = 3.0f;
	skillsStats[SKILL_TYPE_BURN].range = 2.0f;
	skillsStats[SKILL_TYPE_BURN].skillApplyCost = 20;
	skillsStats[SKILL_TYPE_BURN].skillResearchCost = 10;
	skillsStats[SKILL_TYPE_BURN].upgradeBoxIndex = 1;

	skillsStats[SKILL_TYPE_PASSIVE_AOE].damage = 20;
	skillsStats[SKILL_TYPE_PASSIVE_AOE].damageApplyTime_secs = 1.0f;
	skillsStats[SKILL_TYPE_PASSIVE_AOE].duration_secs = 4.0f;
	skillsStats[SKILL_TYPE_PASSIVE_AOE].range = 2.0f;
	skillsStats[SKILL_TYPE_PASSIVE_AOE].skillApplyCost = 10;
	skillsStats[SKILL_TYPE_PASSIVE_AOE].skillResearchCost = 10;
	skillsStats[SKILL_TYPE_PASSIVE_AOE].upgradeBoxIndex = 0;


	// WARN: This is only for testing
	std::shared_ptr<AOESkill> aoeSkill =
		std::shared_ptr<AOESkill>(new AOESkill(glm::vec3(),
											   skillsStats[SKILL_TYPE_AOE].damage, 
											   skillsStats[SKILL_TYPE_AOE].range, 
											   "aoeSkill",
											   'q', 'q', 'w',
											   skillsStats[SKILL_TYPE_AOE].skillApplyCost,
											   skillsStats[SKILL_TYPE_AOE].skillResearchCost,
											   skillsStats[SKILL_TYPE_AOE].upgradeBoxIndex));
	std::shared_ptr<PassiveAOESkill> passiveAOESkill =
		std::shared_ptr<PassiveAOESkill>(new PassiveAOESkill(glm::vec3(),
															 skillsStats[SKILL_TYPE_PASSIVE_AOE].damage, 
															 skillsStats[SKILL_TYPE_PASSIVE_AOE].damageApplyTime_secs, 
															 skillsStats[SKILL_TYPE_PASSIVE_AOE].duration_secs,
															 skillsStats[SKILL_TYPE_PASSIVE_AOE].range,
															 "passiveAOESkill",
															 'q', 'q', 'e', 
															 skillsStats[SKILL_TYPE_PASSIVE_AOE].skillApplyCost,
															 skillsStats[SKILL_TYPE_PASSIVE_AOE].skillResearchCost,
															 skillsStats[SKILL_TYPE_PASSIVE_AOE].upgradeBoxIndex));
	std::shared_ptr<SunNovaSkill> sunNovaSkill =
		std::shared_ptr<SunNovaSkill>(new SunNovaSkill(glm::vec3(), 
													   skillsStats[SKILL_TYPE_SUN_NOVA].damage, 
													   skillsStats[SKILL_TYPE_SUN_NOVA].range, 
													   skillsStats[SKILL_TYPE_SUN_NOVA].scaleRate, 
													   "sunNovaSkill", 
													   'w', 'w', 'e', 
													   skillsStats[SKILL_TYPE_SUN_NOVA].skillApplyCost,
													   skillsStats[SKILL_TYPE_SUN_NOVA].skillResearchCost,
													   skillsStats[SKILL_TYPE_SUN_NOVA].upgradeBoxIndex));
	std::shared_ptr<BurnSkill> burnSkill =
		std::shared_ptr<BurnSkill>(new BurnSkill(glm::vec3(), 
												 skillsStats[SKILL_TYPE_BURN].damage, 
												 skillsStats[SKILL_TYPE_BURN].damageApplyTime_secs, 
												 skillsStats[SKILL_TYPE_BURN].duration_secs,
												 skillsStats[SKILL_TYPE_BURN].range, 
												 "burnSkill",
												 'w', 'e', 'q',
												 skillsStats[SKILL_TYPE_BURN].skillApplyCost,
												 skillsStats[SKILL_TYPE_BURN].skillResearchCost,
												 skillsStats[SKILL_TYPE_BURN].upgradeBoxIndex));

	suns[0]->AddSkill(aoeSkill);
	suns[0]->AddSkill(passiveAOESkill);
	suns[0]->AddSkill(sunNovaSkill);
	suns[0]->AddSkill(burnSkill);
}

void Scene::InitTweakableVariables(bool isLoadedFromConfig, const std::string &configFileName)
{
	if(isLoadedFromConfig)
	{
		TweakableVarsLoader loader = TweakableVarsLoader(configFileName);
		std::vector<std::pair<std::string, TweakVarData>> loadedData = loader.GetAllLoadedVars();
		
		for(std::vector<std::pair<std::string, TweakVarData>>::iterator iter = loadedData.begin();
			iter != loadedData.end(); ++iter)
		{
			std::string command = "";
			command.append(iter->first);
			command.append(" ");
			std::ostringstream ss;
			if(iter->second.itemIndex != -1)
			{
				char indexToChar[2];
				itoa(iter->second.itemIndex, indexToChar, 10);
				command.append(indexToChar);
				command.append(" ");
			}
			switch(iter->second.currentType)
			{
			case TweakVarData::TYPE_TWEAK_INT:
				char intToChar[8];
				itoa(iter->second.varInt, intToChar, 10);
				command.append(intToChar);
				break;
			case TweakVarData::TYPE_TWEAK_FLOAT:
				ss<<iter->second.varFloat;
				command.append(ss.str());
				break;
			case TweakVarData::TYPE_TWEAK_STRING:
				ss<<iter->second.varString;
				command.append(ss.str());
				break;
			default:
				HandleUnexpectedError("invalid tweak var type", __LINE__, __FILE__);
				break;
			}

			ProcessVariablesTweak(command);
		}

		// WARN: BADDDD!!!
		std::shared_ptr<AOESkill> aoeSkill =
			std::shared_ptr<AOESkill>(new AOESkill(glm::vec3(),
												   skillsStats[SKILL_TYPE_AOE].damage, 
												   skillsStats[SKILL_TYPE_AOE].range, 
												   "aoeSkill",
												   'q', 'q', 'w',
												   skillsStats[SKILL_TYPE_AOE].skillApplyCost,
												   skillsStats[SKILL_TYPE_AOE].skillResearchCost,
												   skillsStats[SKILL_TYPE_AOE].upgradeBoxIndex));
		std::shared_ptr<PassiveAOESkill> passiveAOESkill =
			std::shared_ptr<PassiveAOESkill>(new PassiveAOESkill(glm::vec3(),
																 skillsStats[SKILL_TYPE_PASSIVE_AOE].damage, 
																 skillsStats[SKILL_TYPE_PASSIVE_AOE].damageApplyTime_secs, 
																 skillsStats[SKILL_TYPE_PASSIVE_AOE].duration_secs,
																 skillsStats[SKILL_TYPE_PASSIVE_AOE].range,
																 "passiveAOESkill",
																 'q', 'q', 'e', 
																 skillsStats[SKILL_TYPE_PASSIVE_AOE].skillApplyCost,
																 skillsStats[SKILL_TYPE_PASSIVE_AOE].skillResearchCost,
																 skillsStats[SKILL_TYPE_PASSIVE_AOE].upgradeBoxIndex));
		std::shared_ptr<SunNovaSkill> sunNovaSkill =
			std::shared_ptr<SunNovaSkill>(new SunNovaSkill(glm::vec3(), 
														   skillsStats[SKILL_TYPE_SUN_NOVA].damage, 
														   skillsStats[SKILL_TYPE_SUN_NOVA].range, 
														   skillsStats[SKILL_TYPE_SUN_NOVA].scaleRate, 
														   "sunNovaSkill", 
														   'w', 'w', 'e', 
														   skillsStats[SKILL_TYPE_SUN_NOVA].skillApplyCost,
														   skillsStats[SKILL_TYPE_SUN_NOVA].skillResearchCost,
														   skillsStats[SKILL_TYPE_SUN_NOVA].upgradeBoxIndex));
		std::shared_ptr<BurnSkill> burnSkill =
			std::shared_ptr<BurnSkill>(new BurnSkill(glm::vec3(), 
													 skillsStats[SKILL_TYPE_BURN].damage, 
													 skillsStats[SKILL_TYPE_BURN].damageApplyTime_secs, 
													 skillsStats[SKILL_TYPE_BURN].duration_secs,
													 skillsStats[SKILL_TYPE_BURN].range, 
													 "burnSkill",
													 'w', 'e', 'q',
													 skillsStats[SKILL_TYPE_BURN].skillApplyCost,
													 skillsStats[SKILL_TYPE_BURN].skillResearchCost,
													 skillsStats[SKILL_TYPE_BURN].upgradeBoxIndex));

		suns[0]->AddSkill(aoeSkill);
		suns[0]->AddSkill(passiveAOESkill);
		suns[0]->AddSkill(sunNovaSkill);
		suns[0]->AddSkill(burnSkill);

		std::vector<std::string> skillTextures;
		skillTextures.push_back("../data/images/" + skillsStats[SKILL_TYPE_PASSIVE_AOE].skillUpgradedTexture);
		skillTextures.push_back("../data/images/" + skillsStats[SKILL_TYPE_BURN].skillUpgradedTexture);
		skillTextures.push_back("../data/images/" + skillsStats[SKILL_TYPE_SUN_NOVA].skillUpgradedTexture);
		skillTextures.push_back("../data/images/" + skillsStats[SKILL_TYPE_AOE].skillUpgradedTexture);
		skillTextures.push_back("../data/images/skill-noupgrade.jpg");
		suns[0]->InitSunSkillUpgradeButtons(skillTextures);
	}
	else
	{
		InitEnemyStats();
		InitSkillStats();
	}
}
void Scene::LoadAudio(const std::string &configFileName)
{
	AudioLoader audioLoader = AudioLoader(configFileName);
	std::vector<std::pair<ChannelType, AudioData>> loadedAudio = audioLoader.GetAllLoadedAudios();

	for(std::vector<std::pair<ChannelType, AudioData>>::iterator iter = loadedAudio.begin();
		iter != loadedAudio.end(); ++iter)
	{
		for(std::vector<AudioFile>::iterator iterFile = iter->second.audioFiles.begin();
			iterFile != iter->second.audioFiles.end(); ++iterFile)
		{
			std::string path = "../data/music/";
			path += iterFile->path;
			sceneMusic.SetFileForPlay(path, iterFile->soundType);
			sceneMusic.SetVolume(iter->second.channelVolume, iter->second.channel);
		}
	}
}

void Scene::SpawnSwarm()
{
	BillboardProgDataNoTexture billboardNoTextureProgData = shaderManager.GetBillboardProgDataNoTexture();
	
	float range = ((float)rand() / (float)RAND_MAX) * 
		enemyStats[ENEMY_TYPE_SWARM].spawnRangeInnerRad + enemyStats[ENEMY_TYPE_SWARM].spawnRangeOuterRad;
	float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

	float posX = cosf(posOnCircle * (2.0f * PI)) * range;
	float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

	glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
	glm::vec3 frontVector = glm::normalize(glm::vec3() - position); // WARN: should be relative to sun
		
	std::shared_ptr<Swarm> randSwarm = 
		std::shared_ptr<Swarm>(new Swarm(enemyStats[ENEMY_TYPE_SWARM].swarmersCount, 
										 enemyStats[ENEMY_TYPE_SWARM].swarmersAttackTime_secs, 
										 enemyStats[ENEMY_TYPE_SWARM].damage, 
										 billboardNoTextureProgData, 
										 glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 
										 position, frontVector, 
										 enemyStats[ENEMY_TYPE_SWARM].speed,
										 enemyStats[ENEMY_TYPE_SWARM].lineOfSight, 
										 enemyStats[ENEMY_TYPE_SWARM].health, 
										 enemyStats[ENEMY_TYPE_SWARM].resourceGivenOnKill));

	enemies.push_back(randSwarm);
}
void Scene::SpawnSpaceship()
{
	float range = ((float)rand() / (float)RAND_MAX) * 
		enemyStats[ENEMY_TYPE_SPACESHIP].spawnRangeInnerRad + enemyStats[ENEMY_TYPE_SPACESHIP].spawnRangeOuterRad;
	float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

	float posX = cosf(posOnCircle * (2.0f * PI)) * range;
	float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

	glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
	glm::vec3 frontVector = glm::normalize(glm::vec3() - position); // WARN: should be relative to sun
	float speed = 0.05f;

	std::shared_ptr<Spaceship> randSpaceship =
		std::shared_ptr<Spaceship>(new Spaceship(enemyStats[ENEMY_TYPE_SPACESHIP].projectileSpeed, 
												 20,
												 enemyStats[ENEMY_TYPE_SPACESHIP].damage, 
												 glm::vec4(0.21f, 0.42f, 0.34f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
												 position, frontVector, 
												 enemyStats[ENEMY_TYPE_SPACESHIP].speed, 
												 enemyStats[ENEMY_TYPE_SPACESHIP].lineOfSight,
												 enemyStats[ENEMY_TYPE_SPACESHIP].health, 
												 enemyStats[ENEMY_TYPE_SPACESHIP].resourceGivenOnKill));

	randSpaceship->LoadMesh("../data/mesh-files/spaceship.obj");
	randSpaceship->LoadProjectileMesh("mesh-files/UnitSphere.xml");
			
	enemies.push_back(randSpaceship);
}
void Scene::SpawnFastSuicideBomber()
{
	float range = ((float)rand() / (float)RAND_MAX) * 
		enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].spawnRangeInnerRad + enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].spawnRangeOuterRad;
	float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

	float posX = cosf(posOnCircle * (2.0f * PI)) * range;
	float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

	glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
	glm::vec3 frontVector = glm::normalize(glm::vec3() - position); // WARN: should be relative to sun
	float speed = 0.02f;
	float chargeSpeed = 0.1f;
			
	std::shared_ptr<FastSuicideBomber> randBomber = 
		std::shared_ptr<FastSuicideBomber>(new FastSuicideBomber(enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].damage, 
																 enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].fastSuicideBomberChargeSpeed,
																 glm::vec4(0.5f, 0.5f, 0.7f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
																 position, frontVector,
																 enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].speed,
																 enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].lineOfSight, 
																 enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].health, 
																 enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].resourceGivenOnKill));
	randBomber->LoadMesh("../data/mesh-files/suicide_bomber.obj");

	enemies.push_back(randBomber);
}
void Scene::SpawnAsteroid()
{
	float range = ((float)rand() / (float)RAND_MAX) * 
		enemyStats[ENEMY_TYPE_ASTEROID].spawnRangeInnerRad + enemyStats[ENEMY_TYPE_ASTEROID].spawnRangeOuterRad;
	float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

	float posX = cosf(posOnCircle * (2.0f * PI)) * range;
	float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

	posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;
	float dirX = cosf(posOnCircle * (2.0f * PI)) * range;
	float dirZ = sinf(posOnCircle * (2.0f * PI)) * range;

	glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
	glm::vec3 frontVector = glm::normalize(glm::vec3(dirX, 0.0f, dirZ));

	std::shared_ptr<Asteroid> randAsteroid = 
		std::shared_ptr<Asteroid>(new Asteroid(enemyStats[ENEMY_TYPE_ASTEROID].damage, 
											   glm::vec4(0.57, 0.37, 0.26, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
											   position, frontVector, 
											   enemyStats[ENEMY_TYPE_ASTEROID].speed, 
											   enemyStats[ENEMY_TYPE_ASTEROID].lineOfSight, 
											   enemyStats[ENEMY_TYPE_ASTEROID].health, 
											   enemyStats[ENEMY_TYPE_ASTEROID].resourceGivenOnKill));

	randAsteroid->LoadMesh("../data/mesh-files/meteorite.obj");

	enemies.push_back(randAsteroid);
}
void Scene::SpawnMothership()
{
	float range = ((float)rand() / (float)RAND_MAX) * 
		enemyStats[ENEMY_TYPE_MOTHERSHIP].spawnRangeInnerRad + enemyStats[ENEMY_TYPE_MOTHERSHIP].spawnRangeOuterRad;
	float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

	float posX = cosf(posOnCircle * (2.0f * PI)) * range;
	float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

	glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
	glm::vec3 frontVector = glm::normalize(glm::vec3() - position); // WARN: should be relative to sun

	std::shared_ptr<Mothership> randMothership =
		std::shared_ptr<Mothership>(new Mothership(glm::vec4(0.21f, 0.42f, 0.34f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
												   position, frontVector, 
												   enemyStats[ENEMY_TYPE_MOTHERSHIP].speed, 
												   enemyStats[ENEMY_TYPE_MOTHERSHIP].lineOfSight, 
												   enemyStats[ENEMY_TYPE_MOTHERSHIP].health, 
												   enemyStats[ENEMY_TYPE_MOTHERSHIP].resourceGivenOnKill));
	randMothership->LoadMesh("../data/mesh-files/mothership.obj");

	int deployUnitsCount = 4;
	randMothership->InitDeployUnits("mesh-files/Ship.xml", 
									enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsCount, 
									enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsProjSpeed, // WARN: may bug 
									20, 
									enemyStats[ENEMY_TYPE_MOTHERSHIP].damage, 
									glm::vec4(0.21f, 0.42f, 0.34f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
									enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsSpeed, 
									enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsLineOfSight, 
									enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsLife, 
									enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsResourceGivenOnKill);
	enemies.push_back(randMothership);
		
	std::vector<std::shared_ptr<DeployUnit>> deployUnits = randMothership->GetDeployUnits();
	for(int i = 0; i < deployUnits.size(); i++)
	{
		enemies.push_back(deployUnits[i]);
	}
}
void Scene::SpawnEnemies()
{
	if(spawnData.waveSpawnTimer.Update() == true)
	{
		srand(time(0));

		int enemyCount = 0;
		enemyCount = Utility::GetFibonacciNumber(spawnData.currentEnemyCount);
		if(spawnData.currentEnemyCount <= spawnData.maxEnemyCount)
		{
			spawnData.currentEnemyCount++;
		}

		EnemyType chosenType = EnemyType(rand() % ENEMY_TYPE_COUNT);
		if(spawnData.initialSpawnTime_secs > spawnData.endSpawnTime_secs)
		{
			while(chosenType == ENEMY_TYPE_MOTHERSHIP)
			{
				chosenType = EnemyType(rand() % ENEMY_TYPE_COUNT);
			}
		}

		switch(chosenType)
		{
		case ENEMY_TYPE_SWARM:
			for(int i = 0; i < enemyCount; i++) 
			{
				SpawnSwarm();
			}
			break;
		case ENEMY_TYPE_SPACESHIP:
			for(int i = 0; i < enemyCount; i++)
			{
				SpawnSpaceship();
			}
			break;
		case ENEMY_TYPE_FAST_SUICIDE_BOMBER:
			for(int i = 0; i < enemyCount; i++)
			{
				SpawnFastSuicideBomber();
			}
			break;
		case ENEMY_TYPE_ASTEROID:
			for(int i = 0; i < enemyCount; i++)
			{
				SpawnAsteroid();
			}
			break;
		case ENEMY_TYPE_MOTHERSHIP:			
			{
				SpawnMothership();
			}
			break;
		default:
			HandleUnexpectedError("ivalid enemy type", __LINE__, __FILE__);
			break;
		}

		if(spawnData.initialSpawnTime_secs >= spawnData.endSpawnTime_secs)
		{
			spawnData.initialSpawnTime_secs -= spawnData.timeDecrement_secs;
			spawnData.waveSpawnTimer = Framework::Timer(Framework::Timer::TT_SINGLE, spawnData.initialSpawnTime_secs);
		}
		else
		{
			spawnData.waveSpawnTimer.Reset();
		}
	}
}

void Scene::ProcessVariablesTweak(const std::string &command)
{
	// TODO: Make it safer.
	if(!command.empty())
	{
		std::vector<std::string> splittedCommand = Utility::SplitString(command, ' ');
	
		std::string cmd = splittedCommand[0];
		if(strcmp(cmd.c_str(), "currentEnemyCount") == 0)
		{
			// fibonacci!!!!
			int newEnemyCount = atoi(splittedCommand[1].c_str());
			spawnData.currentEnemyCount = newEnemyCount;
		}
		if(strcmp(cmd.c_str(), "maxEnemyCount") == 0)
		{
			int maxEnemyCount = atoi(splittedCommand[1].c_str());
			spawnData.maxEnemyCount = maxEnemyCount;
		}
		if(strcmp(cmd.c_str(), "initialSpawnTime") == 0)
		{
			float initialSpawnTime = atof(splittedCommand[1].c_str());
			spawnData.initialSpawnTime_secs = initialSpawnTime;
			spawnData.waveSpawnTimer = Framework::Timer(Framework::Timer::TT_SINGLE, spawnData.initialSpawnTime_secs);
		}
		if(strcmp(cmd.c_str(), "endSpawnTime") == 0) 
		{
			float endSpawnTime = atof(splittedCommand[1].c_str());
			spawnData.endSpawnTime_secs = endSpawnTime;
			spawnData.waveSpawnTimer = Framework::Timer(Framework::Timer::TT_SINGLE, spawnData.initialSpawnTime_secs);
		}
		if(strcmp(cmd.c_str(), "timeDecrement") == 0) 
		{
			float timeDecrement = atof(splittedCommand[1].c_str());
			spawnData.timeDecrement_secs = timeDecrement;
			spawnData.waveSpawnTimer = Framework::Timer(Framework::Timer::TT_SINGLE, spawnData.initialSpawnTime_secs);
		}
		if(strcmp(cmd.c_str(), "enemyDestructionRadius") == 0) 
		{
			float enemyDestrRad = atof(splittedCommand[1].c_str());
			enemyDestructionRadius = enemyDestrRad;
		}
		if(strcmp(cmd.c_str(), "resourceCount") == 0)
		{
			int resource = atoi(splittedCommand[1].c_str());
			suns[0]->currentResource = resource;
		}
		if(strcmp(cmd.c_str(), "resourceGainTime") == 0) 
		{
			// Takes effect after new satellites are created
			// 4 for all sats;
			SatelliteType type = SatelliteType(atoi(splittedCommand[1].c_str()));
			float resourceGainTime = atof(splittedCommand[2].c_str());
			if(type == SATELLITE_COUNT)
			{
				for(int i = 0; i < SATELLITE_COUNT/*suns[0]->satellites.size()*/; i++)
				{
					suns[0]->satelliteStats[i].resourceGainTime = resourceGainTime;
					//suns[0]->satellites[i]->resource.resourceGainTime = resourceGainTime;
					//suns[0]->satellites[i]->resource.resourceTimer = Framework::Timer(Framework::Timer::TT_SINGLE, resourceGainTime);
				}
			}
			else
			{
				suns[0]->satelliteStats[type].resourceGainTime = resourceGainTime;
			}
		}
		if(strcmp(cmd.c_str(), "resourceGainPerTime") == 0)
		{
			// Takes effect after new satellites are created, except for the all_satellites.
			// 4 for all sats;
			SatelliteType type = SatelliteType(atoi(splittedCommand[1].c_str()));
			float resourceGainPerTime = atof(splittedCommand[2].c_str());
			if(type == SATELLITE_COUNT)
			{
				for(int i = 0; i < SATELLITE_COUNT/*suns[0]->satellites.size()*/; i++)
				{
					suns[0]->satelliteStats[i].resourceGain_perTime = resourceGainPerTime;
					//suns[0]->satellites[i]->resource.resourceGain_perTime = resourceGainPerTime;
				}
			}
			else
			{
				suns[0]->satelliteStats[type].resourceGain_perTime = resourceGainPerTime;
			}
		}
		if(strcmp(cmd.c_str(), "satConstructionCost") == 0)
		{
			int satConstructionCost = atoi(splittedCommand[1].c_str());
			suns[0]->satelliteConstructionCost = satConstructionCost;
		}
		if(strcmp(cmd.c_str(), "satHealth") == 0)
		{
			// Takes effect after new satellites are created, except for the all_satellites.
			// 4 for all sats;
			SatelliteType type = SatelliteType(atoi(splittedCommand[1].c_str()));
			int satHealth = atoi(splittedCommand[2].c_str());
			if(type == SATELLITE_COUNT)
			{
				for(int i = 0; i < SATELLITE_COUNT/*suns[0]->satellites.size()*/; i++)
				{
					suns[0]->satelliteStats[i].health = satHealth;
					//suns[0]->satellites[i]->health = satHealth;
				}
			}
			else
			{
				suns[0]->satelliteStats[type].health = satHealth;
			}
		}
		if(strcmp(cmd.c_str(), "health") == 0)
		{
			int health = atoi(splittedCommand[1].c_str());
			suns[0]->health = health;
		}	
		if(strcmp(cmd.c_str(), "enemyDamage") == 0)
		{
			EnemyType enemyType = EnemyType(atoi(splittedCommand[1].c_str()));
			int enemyDamage = atoi(splittedCommand[2].c_str());
			enemyStats[enemyType].damage = enemyDamage;
		}
		if(strcmp(cmd.c_str(), "enemyChargeSpeed") == 0) 
		{
			float enemyChargeSpeed = atof(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_FAST_SUICIDE_BOMBER].fastSuicideBomberChargeSpeed = enemyChargeSpeed;
		}	
		if(strcmp(cmd.c_str(), "enemySpawnInnerRad") == 0)
		{
			EnemyType enemyType = EnemyType(atoi(splittedCommand[1].c_str()));
			float enemySpawnInnerRad = atof(splittedCommand[2].c_str());
			enemyStats[enemyType].spawnRangeInnerRad = enemySpawnInnerRad;
		}
		if(strcmp(cmd.c_str(), "enemySpawnOuterRad") == 0)
		{
			EnemyType enemyType = EnemyType(atoi(splittedCommand[1].c_str()));
			float enemySpawnOuterRad = atof(splittedCommand[2].c_str());
			enemyStats[enemyType].spawnRangeOuterRad = enemySpawnOuterRad;
		}
		if(strcmp(cmd.c_str(), "enemySpeed") == 0) 
		{
			EnemyType enemyType = EnemyType(atoi(splittedCommand[1].c_str()));
			float enemySpeed = atof(splittedCommand[2].c_str());
			enemyStats[enemyType].speed = enemySpeed;
		}
		if(strcmp(cmd.c_str(), "enemyLOS") == 0)
		{
			EnemyType enemyType = EnemyType(atoi(splittedCommand[1].c_str()));
			float enemyLOS = atof(splittedCommand[2].c_str());
			enemyStats[enemyType].lineOfSight = enemyLOS;
		}
		if(strcmp(cmd.c_str(), "enemyHealth") == 0)
		{
			EnemyType enemyType = EnemyType(atoi(splittedCommand[1].c_str()));
			int enemyHealth = atoi(splittedCommand[2].c_str());
			enemyStats[enemyType].health = enemyHealth;
		}
		if(strcmp(cmd.c_str(), "enemyResource") == 0)
		{
			EnemyType enemyType = EnemyType(atoi(splittedCommand[1].c_str()));
			int enemyResource = atoi(splittedCommand[2].c_str());
			if(enemyType == ENEMY_TYPE_COUNT)
			{
				for(int type = 0; type < ENEMY_TYPE_COUNT; type++)
				{
					enemyStats[type].resourceGivenOnKill = enemyResource;
				}
			}
			else
			{
				enemyStats[enemyType].resourceGivenOnKill = enemyResource;
			}
		}
		if(strcmp(cmd.c_str(), "swarmersCount") == 0)
		{
			int swarmersCount = atoi(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_SWARM].swarmersCount = swarmersCount;
		}
		if(strcmp(cmd.c_str(), "swarmersAttackTime") == 0)
		{
			float swarmersAttackTime = atof(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_SWARM].swarmersAttackTime_secs = swarmersAttackTime;
		}
		if(strcmp(cmd.c_str(), "enemyProjectileSpeed") == 0)
		{
			float projectileSpeed = atof(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_SPACESHIP].projectileSpeed = projectileSpeed;
		}
		if(strcmp(cmd.c_str(), "deployUnitsCount") == 0) 
		{
			int deployUnitsCount = atoi(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsCount = deployUnitsCount;
		}
		if(strcmp(cmd.c_str(), "deployUnitsLife") == 0)
		{
			int deployUnitsLife = atoi(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsLife = deployUnitsLife;
		}
		if(strcmp(cmd.c_str(), "deployUnitsResourceGivenOnKill") == 0)
		{
			int deployUnitsResourceGivenOnKill = atoi(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsResourceGivenOnKill = deployUnitsResourceGivenOnKill;
		}
		if(strcmp(cmd.c_str(), "deployUnitsSpeed") == 0)
		{
			float deployUnitsSpeed = atof(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsSpeed = deployUnitsSpeed;
		}
		if(strcmp(cmd.c_str(), "deployUnitsLOS") == 0)
		{
			float deployUnitsLineOfSight = atof(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsLineOfSight = deployUnitsLineOfSight;
		}
		if(strcmp(cmd.c_str(), "deployUnitsProjSpeed") == 0)
		{
			float deployUnitsProjSpeed = atof(splittedCommand[1].c_str());
			enemyStats[ENEMY_TYPE_MOTHERSHIP].deployUnitsProjSpeed = deployUnitsProjSpeed;
		}
		if(strcmp(cmd.c_str(), "skillDamage") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			int damage = atoi(splittedCommand[2].c_str());
			skillsStats[skillType].damage = damage;/*
			if(skillType > SKILL_TYPE_MIDDLE) // WARN: Not sure if this is the right way!!!
			{
				suns[0]->satSkillStats[skillType].damage = damage;
			}
			//suns[0]->satSkillStats[skillType].damage = damage;*/
		}
		if(strcmp(cmd.c_str(), "skillRange") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			float range = atof(splittedCommand[2].c_str());
			skillsStats[skillType].range = range;/*
			if(skillType > SKILL_TYPE_MIDDLE) // WARN: Not sure if this is the right way!!!
			{
				suns[0]->satSkillStats[skillType].range = range;
			}
			//suns[0]->satSkillStats[skillType].range = range;*/
		}
		if(strcmp(cmd.c_str(), "skillApplyCost") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			int skillApplyCost = atoi(splittedCommand[2].c_str());
			skillsStats[skillType].skillApplyCost = skillApplyCost;/*
			if(skillType > SKILL_TYPE_MIDDLE) // WARN: Not sure if this is the right way!!!
			{
				suns[0]->satSkillStats[skillType].skillApplyCost = skillApplyCost;
			}
			//suns[0]->satSkillStats[skillType].skillApplyCost = skillApplyCost;*/
		}
		if(strcmp(cmd.c_str(), "skillScaleRate") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			float skillScaleRate = atof(splittedCommand[2].c_str());
			skillsStats[skillType].scaleRate = skillScaleRate;/*
			if(skillType > SKILL_TYPE_MIDDLE) // WARN: Not sure if this is the right way!!!
			{
				suns[0]->satSkillStats[skillType].scaleRate = skillScaleRate;
			}
			//suns[0]->satSkillStats[skillType].scaleRate = skillScaleRate;*/
		}
		if(strcmp(cmd.c_str(), "skillDamageApplyTime") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			float damageApplyTime = atof(splittedCommand[2].c_str());
			skillsStats[skillType].damageApplyTime_secs = damageApplyTime;/*
			if(skillType > SKILL_TYPE_MIDDLE) // WARN: Not sure if this is the right way!!!
			{
				suns[0]->satSkillStats[skillType].damageApplyTime_secs = damageApplyTime;
			}
			//suns[0]->satSkillStats[skillType].damageApplyTime_secs = damageApplyTime;*/
		}
		if(strcmp(cmd.c_str(), "skillDuration") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			float duration = atof(splittedCommand[2].c_str());
			skillsStats[skillType].duration_secs = duration;/*
			if(skillType > SKILL_TYPE_MIDDLE) // WARN: Not sure if this is the right way!!!
			{
				suns[0]->satSkillStats[skillType].duration_secs = duration;
			}
			//suns[0]->satSkillStats[skillType].duration_secs = duration;*/
		}
		if(strcmp(cmd.c_str(), "skillDefensePoints") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			int defensePoints = atoi(splittedCommand[2].c_str());
			skillsStats[skillType].defensePoints = defensePoints;
			/*
			if(skillType > SKILL_TYPE_MIDDLE) // WARN: Not sure if this is the right way!!!
			{
				suns[0]->satSkillStats[skillType].defensePoints = defensePoints;
			}
			//suns[0]->satSkillStats[SKILL_TYPE_SAT_SHIELD].defensePoints = defensePoints;*/
		}
		if(strcmp(cmd.c_str(), "skillStunTime") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			float stunTime = atof(splittedCommand[2].c_str());
			skillsStats[skillType].stunTime_secs = stunTime;/*
			if(skillType > SKILL_TYPE_MIDDLE) // WARN: Not sure if this is the right way!!!
			{
				suns[0]->satSkillStats[skillType].stunTime_secs = stunTime;
			}
			//suns[0]->satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].stunTime_secs = stunTime;*/
		}
		if(strcmp(cmd.c_str(), "skillUpgradeBoxIndex") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			int boxIndex = atoi(splittedCommand[2].c_str());
			skillsStats[skillType].upgradeBoxIndex = boxIndex;
		}
		if(strcmp(cmd.c_str(), "skillForWhichSatellite") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			SatelliteType satType = SatelliteType(atoi(splittedCommand[2].c_str()));
			skillsStats[skillType].forWhichSatellite = satType;
		}
		if(strcmp(cmd.c_str(), "skillUpgradedTexture") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			std::string skillTexture = splittedCommand[2];
			skillsStats[skillType].skillUpgradedTexture = skillTexture;
		}
		if(strcmp(cmd.c_str(), "skillToolTipText") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			std::string skillToolTipText = "";
			// TODO: little ugly but functional. for now...
			for(int i = 2; i < splittedCommand.size(); i++)
			{
				skillToolTipText += splittedCommand[i] + " ";
			}
			skillsStats[skillType].skillToolTipText = skillToolTipText;
		}
		if(strcmp(cmd.c_str(), "skillResearchCost") == 0)
		{
			SkillType skillType = SkillType(atoi(splittedCommand[1].c_str()));
			int skillResearchCost = atoi(splittedCommand[2].c_str());
			skillsStats[skillType].skillResearchCost = skillResearchCost;
		}
		if(strcmp(cmd.c_str(), "spawnMothership") == 0)
		{
			SpawnMothership();
		}
		if(strcmp(cmd.c_str(), "spawnSwarm") == 0)
		{
			SpawnSwarm();
		}
		if(strcmp(cmd.c_str(), "spawnSpaceship") == 0)
		{
			SpawnSpaceship();
		}
		if(strcmp(cmd.c_str(), "spawnAsteroid") == 0)
		{
			SpawnAsteroid();
		}
		if(strcmp(cmd.c_str(), "spawnFastSuicideBomber") == 0)
		{
			SpawnFastSuicideBomber();
		}
		if(strcmp(cmd.c_str(), "controlledMode") == 0)
		{
			isSpawning = false;
		}
		if(strcmp(cmd.c_str(), "gameMode") == 0)
		{
			isSpawning = true;
		}
		if(strcmp(cmd.c_str(), "default") == 0)
		{
			InitTweakableVariables(true, "../data/loader-files/tweak-config.txt");
		}
		if(strcmp(cmd.c_str(), "save") == 0)
		{
			// Implement in the future
		}
	}

	// WARN: BAD!!!
	for(int i = 0; i < SKILL_TYPE_COUNT; i++)
	{
		suns[0]->satSkillStats[i] = skillsStats[i];
	}
}

ShaderManager &Scene::GetShaderManager()
{
	return shaderManager;
}

void Scene::RenderScene(glutil::MatrixStack &modelMatrix, float interpolation)
{
	GLuint materialBlockIndex = shaderManager.GetBlockIndex(BT_MATERIAL);
	GLuint lightUniformBuffer = shaderManager.GetUniformBuffer(UBT_LIGHT);
	LitProgData litData = shaderManager.GetLitProgData();
	UnlitProgData unLitData = shaderManager.GetUnlitProgData();
	SimpleProgData simpleData = shaderManager.GetSimpleProgData();
	BillboardProgDataNoTexture billboardNoTextureData = shaderManager.GetBillboardProgDataNoTexture();
	SimpleTextureProgData textureData = shaderManager.GetSimpleTextureProgData();
	LitTextureProgData litTextureData = shaderManager.GetLitTextureProgData();
	
	for(int i = 0; i < 20; i++)
	{
		if(explosion[i].IsActive())
		{
			explosion[i].Render(modelMatrix, sceneTopDownCamera.ResolveCamPosition(), billboardNoTextureData);
		}
	}

	for(int i = 0; i < enemies.size(); i++)
	{
		if(enemies[i]->IsSceneUpdated())
		{
			enemies[i]->Render(modelMatrix, sceneTopDownCamera.ResolveCamPosition(), billboardNoTextureData);
			enemies[i]->Render(modelMatrix, materialBlockIndex, sceneGamma, litData, textureData, interpolation);
			enemies[i]->Render(modelMatrix, materialBlockIndex, sceneGamma, litData, interpolation);
			enemies[i]->Render(modelMatrix, materialBlockIndex, sceneGamma, litData, litTextureData, interpolation);
		}
	}

	int sizeLights = lights.size();
	for(int i = 0; i < sizeLights; i++)
	{
		lights[i].Render(modelMatrix, litData, lightUniformBuffer);
		lights[i].Render(modelMatrix, litTextureData, lightUniformBuffer);
	}	

	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		suns[i]->Render(modelMatrix, materialBlockIndex, sceneGamma, 
						litData, unLitData, simpleData, textureData, litTextureData,
						interpolation);
	}
}
void Scene::RenderCurrentLayout()
{
	FontProgData fontData = shaderManager.GetFontProgData();
	SimpleProgData simpleData = shaderManager.GetSimpleNoUBProgData();
	TextureProgData textureData = shaderManager.GetTextureProgData();

	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = sceneLayouts.begin();
		iter != sceneLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->Draw(fontData, simpleData, textureData);
			break;
		}
	}
}

void Scene::UpdateScene()
{
	if(isSpawning)
	{
		SpawnEnemies();
	}
	
	for(int i = 0; i < 20; i++)
	{
		if(explosion[i].IsActive())
		{
			explosion[i].Update();
		}
		if(explosion[i].IsDead())
		{
			explosion[i].Init();			
		}
	}

	// Should be in the OnEvent function.
	if(!suns.empty())
	{
		if(suns.front()->GetHealth() <= 0)
		{
			explosion[lastUsedExplosion].SetPosition(glm::vec3());
			explosion[lastUsedExplosion].Init();
			explosion[lastUsedExplosion].Activate();
			lastUsedExplosion++;
			if(lastUsedExplosion >= 20)
			{
				lastUsedExplosion = 0;
			}

			sceneMusic.Play(MUSIC_GAME_OVER, CHANNEL_GAME);

			suns.front()->RemoveSatellites();
			suns.pop_back();
			lights.pop_back(); // got to find the light connected to the sun
							   // even if I remove the light the ship is still lit
		}
	}

	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		suns[i]->Update();

		// TODO: should send an event from the sun instead.
		char matter[10];
		itoa(suns[i]->GetCurrentResource(), matter, 10);
		std::string labelText = "Matter: ";
		labelText.append(matter);
		GetLayout(LAYOUT_IN_GAME)->GetControl("labelMatter")->ChangeText(labelText);
	}
	
	for(std::vector<std::shared_ptr<Enemy>>::iterator iter = enemies.begin();
		iter != enemies.end();
		)
	{
		if((*iter)->IsSceneUpdated())
		{
			if(!suns.empty())
			{
				(*iter)->Update(false, *suns.front().get());
			}
			else
			{
				(*iter)->Update(true);
			}
			
			// Should be done by a collision system
			float distanceBetweenEnemyAndSceneOrigin = glm::length(glm::vec3() - (*iter)->GetPosition());
			if(distanceBetweenEnemyAndSceneOrigin > enemyDestructionRadius)
			{
				/*
				EventArg enemyOutOfBoundsEventArg[1];
				enemyOutOfBoundsEventArg[0].argType = "what_event";
				enemyOutOfBoundsEventArg[0].argument.varType = TYPE_STRING;
				strcpy(enemyOutOfBoundsEventArg[0].argument.varString, "destroy");
				Event enemyOutOfBoundsEvent = Event(1, EVENT_TYPE_OTHER, enemyOutOfBoundsEventArg);

				(*iter)->OnEvent(enemyOutOfBoundsEvent);
				*/
				enemies.erase(iter);
				break;
			}
		}
		if((*iter)->IsDestroyed())
		{
			srand(time(0));
			int explosionIndices[3] = { MUSIC_EXPLOSION, MUSIC_EXPLOSION_2, MUSIC_EXPLOSION_3 };
			SoundType explType = SoundType(explosionIndices[rand() % 3]);

			sceneMusic.Play(explType, CHANNEL_GAME); // WARN: has some delay but it is acceptable

			explosion[lastUsedExplosion].SetPosition((*iter)->position);
			//explosion[lastUsedExplosion].Init();
			explosion[lastUsedExplosion].Activate();
			lastUsedExplosion++;
			if(lastUsedExplosion >= 20)
			{
				lastUsedExplosion = 0;
			}
			
			EventArg enemyKilledEventArg[2];
			enemyKilledEventArg[0].argType = "what_event";
			enemyKilledEventArg[0].argument.varType = TYPE_STRING;
			strcpy(enemyKilledEventArg[0].argument.varString, "enemyGainedResource");
			enemyKilledEventArg[1].argType = "how_much";
			enemyKilledEventArg[1].argument.varType = TYPE_INTEGER;
			enemyKilledEventArg[1].argument.varInteger = (*iter)->GetResourceGivenOnKill();
			Event enemyKilledEvent = Event(2, EVENT_TYPE_OTHER, enemyKilledEventArg);

			suns[0]->OnEvent(enemyKilledEvent);

			enemies.erase(iter);
			break;
		}
		++iter;
	}
	
	if(!suns.empty())
	{
		std::vector<std::shared_ptr<Skill>> skills = suns[0]->GetAllSkills();
		int sizeSkills = skills.size();
		for(int skillIndex = 0; skillIndex < sizeSkills; skillIndex++)
		{
			if(skills[skillIndex]->GetSkillType() == "aoeSkill" || 
			   skills[skillIndex]->GetSkillType() == "burnSkill")
			{
				glm::vec4 mousePos_atZ = 
					sceneMouse.GetPositionAtDimension(currentDisplayData.windowWidth, 
													  currentDisplayData.windowHeight,
													  currentDisplayData.projectionMatrix, 
													  currentDisplayData.modelMatrix, 
													  glm::vec4(sceneTopDownCamera.ResolveCamPosition(), 1.0f), 
													  glm::comp::Y);

				skills[skillIndex]->SetParameter(PARAM_POSITION, 
												 mousePos_atZ.swizzle(glm::comp::X, glm::comp::Y, glm::comp::Z));
			}
			// TODO:
			// Maybe a design flaw - http://stackoverflow.com/a/307793/628873
			if(skills[skillIndex]->GetSkillType() == "passiveAOESkill")
			{
				for(int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++)
				{
					if(skills[skillIndex]->IsIntersectingObject(enemies[enemyIndex]->GetPosition()))
					{
						Event skillEvent = skills[skillIndex]->GetGeneratedEvent("timeended");
						if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
						{
							enemies[enemyIndex]->OnEvent(skillEvent);
							skills[skillIndex]->RemoveGeneratedEvent("timeended");
						}
					}
				}
			}

			if(skills[skillIndex]->GetSkillType() == "burnSkill" && 
			   skills[skillIndex]->IsDeployed() == true)
			{
				for(int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++)
				{
					if(skills[skillIndex]->IsIntersectingObject(enemies[enemyIndex]->GetPosition()))
					{
						Event skillEvent = skills[skillIndex]->GetGeneratedEvent("timeended");
						if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
						{
							enemies[enemyIndex]->OnEvent(skillEvent);
							skills[skillIndex]->RemoveGeneratedEvent("timeended");
						}
					}
				}
			}

			if(skills[skillIndex]->GetSkillType() == "sunNovaSkill" ||
			   skills[skillIndex]->GetSkillType() == "satChainSkill" ||
			   skills[skillIndex]->GetSkillType() == "satFrostNova")
			{
				for(int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++)
				{
					if(skills[skillIndex]->IsIntersectingObject(enemies[enemyIndex]->GetPosition()))
					{
						if(skills[skillIndex]->GetSkillType() == "satFrostNova")
						{
							Event skillEvent = skills[skillIndex]->GetGeneratedEvent("stunskilldeployed");
							if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
							{
								enemies[enemyIndex]->OnEvent(skillEvent);
							}
						}
						else
						{
							//Event skillEvent = skills[skillIndex]->GetGeneratedEvent("skilldeployed");
							if(skills[skillIndex]->IsStarted()/*skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY*/)
							{
								enemies[enemyIndex]->health -= skills[skillIndex]->GetDamage();
								//enemies[enemyIndex]->OnEvent(skillEvent);
							}
						}
					}

					if(skills[skillIndex]->GetSkillType() == "satFrostNova")
					{			
						Event skillEvent = skills[skillIndex]->GetGeneratedEvent("stuntimeended");
						
						if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
						{
							enemies[enemyIndex]->OnEvent(skillEvent);
							skills[skillIndex]->RemoveGeneratedEvent("stuntimeended");
						}
					}
				}
			}

			if(skills[skillIndex]->GetSkillType() == "satShieldSkill")
			{
				for(int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++)
				{
					if(skills[skillIndex]->IsIntersectingObject(enemies[enemyIndex]->GetPosition()))
					{
						Event skillEvent = skills[skillIndex]->GetGeneratedEvent("shieldskilldeployed");
						if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
						{
							enemies[enemyIndex]->OnEvent(skillEvent);
							skills[skillIndex]->OnEvent(Event(EVENT_TYPE_ATTACKED));
						}
					}
				}
			}
		}
	}
	else if(suns.empty())
	{
		sceneFusionInput.Clear();
	}
}
void Scene::UpdateFusion(char key, Event &returnedFusionEvent)
{
	returnedFusionEvent = sceneFusionInput.Update(key);
	if(returnedFusionEvent.GetType() != EVENT_TYPE_EMPTY)
	{
		OnEvent(returnedFusionEvent);
	}
}
void Scene::UpdateCurrentLayout(int windowWidth, int windowHeight)
{
	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = sceneLayouts.begin();
		iter != sceneLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->Update(windowWidth, windowHeight);
			break;
		}
	}
}

void Scene::AddFusionSequence(std::string sequenceName, 
							  char buttonA, char buttonB, char buttonC)
{
	sceneFusionInput.AddSequence(sequenceName, buttonA, buttonB, buttonC);
}

void Scene::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ON_CLICK:
		if(strcmp(_event.GetArgument("object").varString, "sun") == 0)
		{
			sceneMusic.Play(MUSIC_ON_SUN_CLICK, CHANNEL_INTERACTION);
		}
		if(strcmp(_event.GetArgument("object").varString, "exitButton") == 0)
		{
			this->SetLayout(LAYOUT_MENU, true);
			this->SetLayout(LAYOUT_IN_GAME, false);
		}
		if(strcmp(_event.GetArgument("object").varString, "newGameButton") == 0)
		{
			this->SetLayout(LAYOUT_MENU, false);
			this->SetLayout(LAYOUT_IN_GAME, true);

			EventArg inGameEventArg[1];
			inGameEventArg[0].argType = "command";
			inGameEventArg[0].argument.varType = TYPE_STRING;
			strcpy(inGameEventArg[0].argument.varString, "playBackgroundMusic");
			Event inGameEvent = Event(1, EVENT_TYPE_OTHER, inGameEventArg);

			OnEvent(inGameEvent);
		}
		if(strcmp(_event.GetArgument("object").varString, "saveGameButton") == 0)
		{
			this->SetLayout(LAYOUT_SAVE_GAME, true);
			this->SetLayout(LAYOUT_MENU, false);
		}
		if(strcmp(_event.GetArgument("object").varString, "printCmd") == 0)
		{
			std::printf("%s", 
				this->GetLayout(LAYOUT_MENU)->GetControl("sample")->GetContent().c_str());
			
			this->GetLayout(LAYOUT_MENU)->GetControl("sample")->Clear();
		}
		if(strcmp(_event.GetArgument("object").varString, "applyInput") == 0)
		{
			if(!suns.empty())
			{
				ProcessVariablesTweak(this->GetLayout(LAYOUT_IN_GAME)->GetControl("varInput")->GetContent());
			}

			this->GetLayout(LAYOUT_IN_GAME)->GetControl("varInput")->Clear();
			this->GetLayout(LAYOUT_IN_GAME)->GetControl("varInput")->Deactivate();
		}
		if(strcmp(_event.GetArgument("object").varString, "quitGameButton") == 0)
		{
			// Add clean-up if necessary.
			exit(EXIT_SUCCESS);
		}
		if(strcmp(_event.GetArgument("object").varString, "deploySkill") == 0)
		{
			if(!suns.empty())
			{
				std::vector<std::shared_ptr<Skill>> skills = suns[0]->GetAllSkills();
				if(!enemies.empty() && !skills.empty())
				{
					for(int i = 0; i < enemies.size(); i++)
					{
						for(int skillIndex = 0; skillIndex < skills.size(); skillIndex++)
						{
							Event skillEvent = skills[skillIndex]->GetGeneratedEvent("skilldeployed");
							//skills[skillIndex]->RemoveGeneratedEvent("skilldeployed");

							if(skills[skillIndex]->IsIntersectingObject(enemies[i]->GetPosition()) &&
							   skills[skillIndex]->GetSkillType() != "burnSkill")
							{
								enemies[i]->health -= skills[skillIndex]->GetDamage();
								if(skills[skillIndex]->GetSkillType() == "aoeSkill")
								{
									sceneMusic.Play(MUSIC_AOE, CHANNEL_GAME);
								}
								//enemies[i]->OnEvent(skillEvent);
							}
						}
					}
				}
			}
		}
		break;
	case EVENT_TYPE_SPACE_BTN_CLICK:
		if(strcmp(_event.GetArgument("command").varString, "playBackgroundMusic") == 0)
		{
			sceneMusic.Play(MUSIC_BACKGROUND);
		}
		break;
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("command").varString, "playMenuMusic") == 0)
		{
			//sceneMusic.Stop();
			sceneMusic.Play(MUSIC_MENU, CHANNEL_MASTER); // TODO: The channel should correspond to the music
		}
		if(strcmp(_event.GetArgument("command").varString, "playBackgroundMusic") == 0)
		{
			sceneMusic.Stop(CHANNEL_MASTER);
			sceneMusic.Play(MUSIC_BACKGROUND, CHANNEL_MASTER); // TODO: The channel should correspond to the music
		}
		if(strcmp(_event.GetArgument("what_event").varString, "fusion_seq") == 0)
		{
			sceneMusic.Play(MUSIC_FUSION, CHANNEL_GAME); // TODO: The channel should correspond to the music

			if(strcmp(_event.GetArgument("buttons").varString, 
					  sceneFusionInput.GetSequenceButtons("fireSatellite").c_str()) == 0)
			{
				if(HasSuns())
				{
					suns[0]->AddSatellite(/*"mesh-files/UnitSphere.xml"*/"../data/mesh-files/fire_planet.obj", 
										  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
										  SatelliteType(SATELLITE_FIRE));
				}
			}
			if(strcmp(_event.GetArgument("buttons").varString, 
					  sceneFusionInput.GetSequenceButtons("waterSatellite").c_str()) == 0)
			{
				if(HasSuns())
				{
					suns[0]->AddSatellite(/*"mesh-files/UnitSphere.xml"*/"../data/mesh-files/water_planet.obj", 
										  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
										  SatelliteType(SATELLITE_WATER));
				}
			}
			if(strcmp(_event.GetArgument("buttons").varString, 
					  sceneFusionInput.GetSequenceButtons("earthSatellite").c_str()) == 0)
			{
				if(HasSuns())
				{
					suns[0]->AddSatellite(/*"mesh-files/UnitSphere.xml"*/"../data/mesh-files/earth_planet.obj", 
										  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
										  SatelliteType(SATELLITE_EARTH));
				}
			}
			if(strcmp(_event.GetArgument("buttons").varString,
			   sceneFusionInput.GetSequenceButtons("airSatellite").c_str()) == 0)
			{
				if(HasSuns())
				{
					suns[0]->AddSatellite(/*"mesh-files/UnitSphere.xml"*/"../data/mesh-files/air_planet.obj",
										  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
										  SatelliteType(SATELLITE_AIR));
				}
			}

			std::vector<std::shared_ptr<Skill>> skills = suns[0]->GetAllSkills();
			for(int i = 0; i < skills.size(); i++)
			{
				skills[i]->OnEvent(_event);
				std::printf("Fusion called: %s\n", _event.GetArgument("buttons").varString);
				if(skills[i]->IsStarted())
				{/*
					EventArg skillDeployedEventArg[2];
					skillDeployedEventArg[0].argType = "what_event";
					skillDeployedEventArg[0].argument.varType = TYPE_STRING;
					strcpy(skillDeployedEventArg[0].argument.varString, "skillDeployed");
					skillDeployedEventArg[1].argType = "how_much";
					skillDeployedEventArg[1].argument.varType = TYPE_INTEGER;
					skillDeployedEventArg[1].argument.varInteger = skills[i]->GetApplyCost();
					Event skillDeployedEvent = Event(2, EVENT_TYPE_OTHER, skillDeployedEventArg);*/
					Event skillDeployedEvent = skills[i]->GetGeneratedEvent("skilldeployed");
					if(skillDeployedEvent.GetType() != EVENT_TYPE_EMPTY)
					{
						suns[0]->OnEvent(skillDeployedEvent);
					}
					skills[i]->RemoveGeneratedEvent("skilldeployed");
					
					// simple fix for hard problems... FOR HOW LONG?!
					Event sunResourceEvent = suns[0]->GetGeneratedEvent("insufRes");
					if(sunResourceEvent.GetType() != EVENT_TYPE_EMPTY)
					{
						skills[i]->isStarted = false;
						suns[0]->RemoveGeneratedEvent("insufRes");
					}
					else if(skillDeployedEvent.GetType() != EVENT_TYPE_EMPTY)
					{						
						if(skills[i]->GetSkillType() == "sunNovaSkill") 
						{
							sceneMusic.Play(MUSIC_SUN_NOVA, CHANNEL_GAME);
						}
						if(skills[i]->GetSkillType() == "satFrostNova")
						{
							sceneMusic.Play(MUSIC_FROST_NOVA, CHANNEL_GAME);
						}
					}
				}
			}
		}

		if(strcmp(_event.GetArgument("what_event").varString, "skillHov") == 0)
		{
			if(IsLayoutOn(LAYOUT_IN_GAME))
			{
				std::string toolTipText = "";
				SkillType hoveredSkillType = 
					GetSkillTypeByUpgradeButton(_event.GetArgument("index").varInteger, 
												_event.GetArgument("satType").varInteger);

				toolTipText = skillsStats[hoveredSkillType].skillToolTipText;

				glm::vec2 mousePosition = 
					glm::vec2(sceneMouse.GetCurrentPosition().x, 
							  currentDisplayData.windowHeight - sceneMouse.GetCurrentPosition().y);

				GetLayout(LAYOUT_IN_GAME)->GetControl("labelToolTip")->SetPosition(mousePosition);
				GetLayout(LAYOUT_IN_GAME)->GetControl("labelToolTip")->ChangeText(toolTipText);
				GetLayout(LAYOUT_IN_GAME)->GetControl("labelToolTip")->SetIsVisible(true);
			}
			//std::printf("skill hov");
		}
		
		if(strcmp(_event.GetArgument("what_event").varString, "skillUpgr") == 0)
		{
			if(_event.GetArgument("satType").varInteger == -1)
			{
				std::vector<std::shared_ptr<Skill>> sunSkills = suns[0]->GetSunSkills();
				for(int i = 0; i < sunSkills.size(); i++)
				{
					if(sunSkills[i]->boxIndexForUpgrade == _event.GetArgument("index").varInteger)
					{
						if(sunSkills[i]->isResearched)
						{
							return;
						}
						if(sunSkills[i]->skillResearchCost > suns[0]->currentResource)
						{
							std::printf("not enough resource");
							return;
						}
						sunSkills[i]->isResearched = true;
						suns[0]->currentResource -= sunSkills[i]->skillResearchCost;
						suns[0]->OnEvent(_event);
						return;
					}
				}
			}
			else 
			{
				std::vector<std::shared_ptr<Skill>> satSkills = 
					suns[0]->GetSatelliteSkills(SatelliteType(_event.GetArgument("satType").varInteger));

				for(int i = 0; i < satSkills.size(); i++)
				{
					if(satSkills[i]->boxIndexForUpgrade == _event.GetArgument("index").varInteger)
					{
						if(satSkills[i]->isResearched)
						{
							return;
						}
						if(satSkills[i]->skillResearchCost > suns[0]->currentResource)
						{
							std::printf("not enough resource\n");
							return;
						}
						satSkills[i]->isResearched = true;
						suns[0]->currentResource -= satSkills[i]->skillResearchCost;
						suns[0]->OnEvent(_event);
						return;
					}
				}
			}
			//std::printf("%i\n", _event.GetArgument("index").varInteger);
		}
		break;
	default:
		// ERR: Invalid event type when spawning sats
		HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
		break;
	};
}

void Scene::StartScene()
{
	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		std::vector<std::shared_ptr<CelestialBody>> sunSatellites = suns[i]->GetSatellites();
		for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin();
			iter != sunSatellites.end(); ++iter)
		{
			(*iter)->Start();
		}
	}
}
void Scene::StopScene()
{
	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		std::vector<std::shared_ptr<CelestialBody>> sunSatellites = suns[i]->GetSatellites();
		for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin();
			iter != sunSatellites.end(); ++iter)
		{
			(*iter)->Stop();
		}
	}
}


void Scene::AddSunLight(const SunLight &newSunLight)
{
	lights.push_back(newSunLight);
}
void Scene::AddSun(const std::shared_ptr<CelestialBody> newSun)
{
	suns.push_back(newSun);
	//InitSkillStats();
}
void Scene::AddEnemy(const std::shared_ptr<Enemy> newEnemy)
{
	enemies.push_back(newEnemy);
}

void Scene::AddLayouts(const std::map<LayoutType, std::shared_ptr<Layout>> &newLayouts)
{
	sceneLayouts.insert(newLayouts.begin(), newLayouts.end());
}
void Scene::AddLayout(const std::shared_ptr<Layout> newLayout)
{
	sceneLayouts.insert(std::make_pair(newLayout->GetLayoutType(), newLayout));
}
void Scene::AddLayout(LayoutType layoutType, LayoutInfo layoutInfo)
{
	sceneLayouts.insert(std::make_pair(layoutType, std::shared_ptr<Layout>(new Layout(layoutType, layoutInfo))));
}
std::shared_ptr<Layout> Scene::GetLayout(LayoutType layoutType)
{
	if(this->HasLayout(layoutType))
	{
		return sceneLayouts[layoutType];
	}

	std::string errorMessage = "no such layout ";
	switch(layoutType)
	{
	case LAYOUT_IN_GAME:
		errorMessage += "LAYOUT_IN_GAME";
		break;
	case LAYOUT_LOAD_GAME:
		errorMessage += "LAYOUT_LOAD_GAME";
		break;
	case LAYOUT_MENU:
		errorMessage += "LAYOUT_MENU";
		break;
	case LAYOUT_NEW_GAME:
		errorMessage += "LAYOUT_NEW_GAME";
		break;
	case LAYOUT_OPTIONS:
		errorMessage += "LAYOUT_OPTIONS";
		break;
	case LAYOUT_OTHER:
		errorMessage += "LAYOUT_OTHER";
		break;
	case LAYOUT_SAVE_GAME:
		errorMessage += "LAYOUT_SAVE_GAME";
		break;
	default:
		HandleUnexpectedError("unexpected \'default\' reached", __LINE__, __FILE__);
		break;
	}
	HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
}
bool Scene::HasLayout(LayoutType layoutType)
{
	return sceneLayouts.find(layoutType) != sceneLayouts.end();
}
bool Scene::IsLayoutOn(LayoutType layoutType)
{
	if(this->HasLayout(layoutType))
	{
		return sceneLayouts[layoutType]->IsSet();
	}
	return false;
}
void Scene::SetLayout(LayoutType layoutType, bool isSet)
{
	if(this->HasLayout(layoutType))
	{
		sceneLayouts[layoutType]->Set(isSet);
	}
	else
	{
		std::string errorMessage = "no such layout ";
		switch(layoutType)
		{
		case LAYOUT_IN_GAME:
			errorMessage += "LAYOUT_IN_GAME";
			break;
		case LAYOUT_LOAD_GAME:
			errorMessage += "LAYOUT_LOAD_GAME";
			break;
		case LAYOUT_MENU:
			errorMessage += "LAYOUT_MENU";
			break;
		case LAYOUT_NEW_GAME:
			errorMessage += "LAYOUT_NEW_GAME";
			break;
		case LAYOUT_OPTIONS:
			errorMessage += "LAYOUT_OPTIONS";
			break;
		case LAYOUT_OTHER:
			errorMessage += "LAYOUT_OTHER";
			break;
		case LAYOUT_SAVE_GAME:
			errorMessage += "LAYOUT_SAVE_GAME";
			break;
		default:
			HandleUnexpectedError("unexpected \'default\' reached", __LINE__, __FILE__);
			break;
		}
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}
}


void Scene::SetMusic(const std::string &fileName, SoundType soundType)
{
	sceneMusic.SetFileForPlay(fileName, soundType);
}
void Scene::SetMusicVolume(float volume, ChannelType chType)
{
	sceneMusic.SetVolume(volume, chType);
}

void Scene::SetDisplayData(const DisplayData &newDisplayData)
{
	currentDisplayData.gamma = newDisplayData.gamma;
	currentDisplayData.modelMatrix = newDisplayData.modelMatrix;
	currentDisplayData.projectionMatrix = newDisplayData.projectionMatrix;
	currentDisplayData.mousePosition = newDisplayData.mousePosition;
	currentDisplayData.windowHeight = newDisplayData.windowHeight;
	currentDisplayData.windowWidth = newDisplayData.windowWidth;
	currentDisplayData.zFar = newDisplayData.zFar;
	currentDisplayData.zNear = newDisplayData.zNear;
}

void Scene::PlayMusic(SoundType soundType)
{
	sceneMusic.Play(soundType);
}

void Scene::SetTopDownCamera(const TopDownCamera &newCamera)
{
	sceneTopDownCamera = newCamera;
}
void Scene::SetMouse(const Mouse &newMouse)
{
	sceneMouse = newMouse;
}

void Scene::SetGamma(float newSceneGamma)
{
	sceneGamma = newSceneGamma;
}

void Scene::SetFusion(const FusionInput &newFusionInput)
{
	sceneFusionInput = newFusionInput;
}

void Scene::SetExplosion(const ExplosionEmitter &newExplosionEmitter)
{
	for(int i = 0; i < 20; i++)
	{	
		explosion.push_back(newExplosionEmitter);
	}
}

void Scene::SetLayoutPreset(LayoutPreset layoutPreset)
{
	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = sceneLayouts.begin();
		iter != sceneLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->SetCurrentPreset(layoutPreset);
			break;
		}
	}
}

TopDownCamera &Scene::GetTopDownCamera()
{
	return sceneTopDownCamera;
}
Mouse &Scene::GetMouse()
{
	return sceneMouse;
}


std::shared_ptr<CelestialBody> Scene::GetSun()
{
	return suns.front(); // replace 0 with an index
}
SunLight Scene::GetSunLight()
{
	return lights.front();
}

SkillType Scene::GetSkillTypeByFusionCombination(char fusionA, char fusionB, char fusionC)
{
	std::string buttons = "";
	buttons += fusionA;
	buttons += fusionB;
	buttons += fusionC;

	std::string skillName = sceneFusionInput.GetSequenceName(buttons);
	if(skillName == "aoeSkill")
	{
		return SkillType::SKILL_TYPE_AOE;
	}
	if(skillName == "passiveAoeSkill")
	{
		return SkillType::SKILL_TYPE_PASSIVE_AOE;
	}
	if(skillName == "sunNovaSkill")
	{
		return SkillType::SKILL_TYPE_SUN_NOVA; 
	}
	if(skillName == "satFrostNova")
	{
		return SkillType::SKILL_TYPE_SAT_FROSTNOVA;
	}
	if(skillName == "satShieldSkill")
	{
		return SkillType::SKILL_TYPE_SAT_SHIELD;
	}
	if(skillName == "burnSkill")
	{
		return SkillType::SKILL_TYPE_BURN;
	}

	return SkillType(-1);
}

SkillType Scene::GetSkillTypeByUpgradeButton(int buttonIndex, int bodyType)
{
	switch(bodyType)
	{
	case -1:
		if(buttonIndex == 0)
		{
			return SkillType::SKILL_TYPE_PASSIVE_AOE;
		}
		if(buttonIndex == 1)
		{
			return SkillType::SKILL_TYPE_BURN;
		}
		if(buttonIndex == 2)
		{
			return SkillType::SKILL_TYPE_SUN_NOVA;
		}
		if(buttonIndex == 3)
		{
			return SkillType::SKILL_TYPE_AOE;
		}
		break;
	case SATELLITE_AIR:
		if(buttonIndex == 0)
		{
			return SkillType::SKILL_TYPE_SAT_PASSIVE_AOE;
		}
		break;
	case SATELLITE_EARTH:
		if(buttonIndex == 0)
		{
			return SkillType::SKILL_TYPE_SAT_SHIELD;
		}
		break;
	case SATELLITE_FIRE:
		if(buttonIndex == 0)
		{
			return SkillType::SKILL_TYPE_SAT_CHAIN;
		}
		break;
	case SATELLITE_WATER:
		if(buttonIndex == 0)
		{
			return SkillType::SKILL_TYPE_SAT_FROSTNOVA;
		}
		break;
	default:
		break;
	}

	return SkillType(-1);
}

std::string Scene::GetCurrentFusionInputSequence()
{
	return sceneFusionInput.GetCurrentInputSequence();
}

bool Scene::HasSuns()
{
	if(!suns.empty())
		return true;
	return false;
}