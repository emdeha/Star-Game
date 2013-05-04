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



// Maybe the music should be contained in the layout.


/// \ingroup module_StarGame

/// \file Scene.h
/// \brief Contains the class which wraps the whole game world.

#ifndef SCENE_H
#define SCENE_H


#include <vector>
#include "../Entities/Lights.h"
#include "../Entities/PlanetBodies.h"
#include "../Entities/Enemy.h"
#include "../Entities/Skills.h"
#include "../Fusion/FusionInput.h"
#include "../Mouse/Mouse.h"
#include "../Camera/TopDownCamera.h"
#include "../Audio/Audio.h"
#include "../GUISystem/GUISystem.h"
#include "../StarGame/DisplayData.h"
#include "../StarGame/ShaderManager.h"
#include "../AssetLoader/TweakableVarsLoader.h"
#include "../AssetLoader/AudioLoader.h"

#include "../framework/EventSystem.h"

#include "../AssetLoader/yaml-cpp/yaml.h"
#include <fstream>


struct EnemyStats
{
	float spawnRangeInnerRad;
	float spawnRangeOuterRad;
	float speed;
	float lineOfSight;
	int damage;
	int health;
	int resourceGivenOnKill;
	int swarmersCount;
	float swarmersAttackTime_secs;
	float projectileSpeed;
	float fastSuicideBomberChargeSpeed;
	int deployUnitsCount;
	int deployUnitsLife;
	int deployUnitsResourceGivenOnKill;
	float deployUnitsSpeed;
	float deployUnitsLineOfSight;
	float deployUnitsProjSpeed;
};


struct SpawnData
{
	Framework::Timer waveSpawnTimer;
	float timeDecrement_secs;
	float initialSpawnTime_secs;
	float endSpawnTime_secs;
	float startingSpawnTime_secs;
	int currentEnemyCount; // used as an index which fibonacci number to find
	int maxEnemyCount;
	int startingEnemyCount;
};


/// \class Scene
/// \brief This class updates, renders and maintains the game world.
class Scene
{
private:	
	typedef void (*EventHandlerFunction)(Scene &, Control *);
	/*
	std::vector<std::pair<std::string, EventHandlerFunction>> eventHandlers;
	*/

	std::vector<SunLight> lights; ///< Holds the world's lights.

	std::vector<std::shared_ptr<CelestialBody>> suns;
	std::vector<std::shared_ptr<Enemy>> enemies;

	std::vector<ExplosionEmitter> explosion;
	int lastUsedExplosion;

	std::map<LayoutType, std::shared_ptr<Layout>> sceneLayouts;

	SpawnData spawnData;
	EnemyStats enemyStats[ENEMY_TYPE_COUNT];
	SkillsStats skillsStats[SKILL_TYPE_COUNT];
	FusionInput sceneFusionInput;
	Mouse sceneMouse;
	TopDownCamera sceneTopDownCamera;
	Audio sceneMusic;

	ShaderManager shaderManager;
	DisplayData currentDisplayData;

	float sceneGamma;
	float enemyDestructionRadius;

	bool isSpawning;
	bool isPaused;

	//std::vector<std::pair<std::string, TweakVarData>> tweakVarsData;

private:
	void SpawnSwarm();
	void SpawnSpaceship();
	void SpawnFastSuicideBomber();
	void SpawnAsteroid();
	void SpawnMothership();
	void SpawnEnemies();

	void SaveVariablesTweak();

	void InitEnemyStats();
	void InitSkillStats();

public:
	Scene() {};
	Scene(float newSceneGamma,
		  float newInitialSpawnTime_secs, float newEndSpawnTime_secs, float newTimeDecrement_secs,
		  int newCurrentEnemyCount, int newMaxEnemyCount, float newEnemyDestructionRadius);

	ShaderManager &GetShaderManager();

	/// \fn UpdateScene
	/// \brief Goes through the game objects and calls their update functions.
	void UpdateScene();

	void UpdateFusion(char key, Event &returnedFusionEvent);

	void OnEvent(Event &_event);

	/// \fn RenderScene
	/// \brief Goes through the game objects and calls their render functions.
	void RenderScene(glutil::MatrixStack &modelMatrix, float interpolation);

	void RenderCurrentLayout();
	void UpdateCurrentLayout(int windowWidth, int windowHeight);


	void StopScene();
	void StartScene();

	void ResetScene();
	
	void AddFusionSequence(std::string sequenceName, 
						   char buttonA, char buttonB, char buttonC);

	void AddSunLight(const SunLight &newSunLight);
	
	void AddSun(const std::shared_ptr<CelestialBody> newSun);
	
	void AddEnemy(const std::shared_ptr<Enemy> newEnemy);

	
	void AddEventHandler(const std::string &name, const std::string &controlName, 
						 LayoutType layoutToAddTo, EventHandlerFunction handler);
	void CallEventHandler(const std::string &name, LayoutType layoutToCallIt, Scene &scene, Control *control);
	
	
	void AddLayouts(const std::map<LayoutType, std::shared_ptr<Layout>> &newLayouts);
	void AddLayout(const std::shared_ptr<Layout> newLayout);
	void AddLayout(LayoutType layoutType, glm::vec4 layoutBacgkroundColor);
	std::shared_ptr<Layout> GetLayout(LayoutType layoutType);
	bool HasLayout(LayoutType layoutType);
	bool IsLayoutOn(LayoutType layoutType);

	void SetLayout(LayoutType layoutType, bool isSet);
	void SetLayoutPreset(LayoutPreset layoutPreset);

	void SetMusic(const std::string &fileName, SoundType soundType);
	void SetMusicVolume(float volume, ChannelType chType);

	void SetDisplayData(const DisplayData &newDisplayData);

	void SetExplosion(const ExplosionEmitter &newExplosionEmitter);

	void PlayMusic(SoundType soundType, ChannelType channel);
	void StopMusic(ChannelType channel);

	void SetTopDownCamera(const TopDownCamera &newCamera);
	void SetMouse(const Mouse &newMouse);

	void SetGamma(float newSceneGamma);

	void SetFusion(const FusionInput &newFusionInput);

	
	TopDownCamera &GetTopDownCamera();
	Mouse &GetMouse();


	std::shared_ptr<CelestialBody> GetSun();

	SunLight GetSunLight();
	SkillType GetSkillTypeByFusionCombination(char fusionA, char fusionB, char fusionC);
	SkillType GetSkillTypeByUpgradeButton(int buttonIndex, int bodyType);
	std::string GetCurrentFusionInputSequence();

	bool HasSuns();
	bool IsPaused();
	void SetPause(bool newIsPaused);
	
	void InitTweakableVariables(bool isLoadedFromConfig = false,
								const std::string &configFileName = "");
	void LoadAudio(const std::string &configFileName);

	void LoadGame(const std::string &saveGameFileName);
	void SaveGame(const std::string &saveGameFileName);

	void ProcessVariablesTweak(const std::string &command);
};


/*
class EventHandler
{
public:
	virtual void HandleEvent(Scene &scene, Control *control) = 0;
};

class HandleTestEvent : public EventHandler
{
public:
	void HandleEvent(Scene &scene, Control *control);
};*/


#endif