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

#include "../framework/EventSystem.h"


/// \class Scene
/// \brief This class updates, renders and maintains the game world.
class Scene
{
private:
	std::vector<SunLight> lights; ///< Holds the world's lights.

	std::vector<std::shared_ptr<CelestialBody>> suns;

	std::vector<std::shared_ptr<Spaceship>> spaceships; ///< Holds the world's enemies.
	std::vector<std::shared_ptr<FastSuicideBomber>> fastSuicideBombers;
	std::vector<std::shared_ptr<Swarm>> swarms;
	std::vector<ExplosionEmitter> explosionEmitters;

	std::map<LayoutType, std::shared_ptr<Layout>> sceneLayouts;

	FusionInput sceneFusionInput;
	Mouse sceneMouse;
	TopDownCamera sceneTopDownCamera;
	Audio sceneMusic;

	DisplayData currentDisplayData;

	float sceneGamma;

public:
	Scene() {};
	Scene(float newSceneGamma);

	/// \fn UpdateScene
	/// \brief Goes through the game objects and calls their update functions.
	void UpdateScene();

	void UpdateFusion(char key, Event &returnedFusionEvent);

	void OnEvent(Event &_event);

	/// \fn RenderScene
	/// \brief Goes through the game objects and calls their render functions.
	void RenderScene(glutil::MatrixStack &modelMatrix, 
					 GLuint materialBlockIndex, GLuint lightUniformBuffer,
					 const LitProgData &litData, 
					 const UnlitProgData &unLitData, 
					 const SimpleProgData &interpData,
					 const BillboardProgDataNoTexture &billboardNoTextureData,
					 float interpolation);

	void RenderCurrentLayout(const FontProgData &fontData,
							 const SimpleProgData &simpleData,
							 const TextureProgData &textureData);
	void UpdateCurrentLayout(int windowWidth, int windowHeight);


	void StopScene();
	void StartScene();
	
	void AddFusionSequence(std::string sequenceName, 
						   char buttonA, char buttonB, char buttonC);

	void AddSunLight(const SunLight &newSunLight);
	
	void AddSun(const std::shared_ptr<CelestialBody> newSun);
	
	void AddSpaceship(const std::shared_ptr<Spaceship> newSpaceship);
	void AddFastSuicideBomber(const std::shared_ptr<FastSuicideBomber> newFastSuicideBomber);
	void AddSwarm(const std::shared_ptr<Swarm> newSwarm);

	void AddExplosionEmitter(const ExplosionEmitter &newExplosionEmitter);

	void AddLayouts(const std::map<LayoutType, std::shared_ptr<Layout>> &newLayouts);
	void AddLayout(const std::shared_ptr<Layout> newLayout);
	void AddLayout(LayoutType layoutType, LayoutInfo layoutInfo);
	std::shared_ptr<Layout> GetLayout(LayoutType layoutType);
	bool HasLayout(LayoutType layoutType);
	bool IsLayoutOn(LayoutType layoutType);

	void SetLayout(LayoutType layoutType, bool isSet);
	void SetLayoutPreset(LayoutPreset layoutPreset);

	void SetMusic(const std::string &fileName, SoundTypes soundType);
	void SetMusicVolume(float volume, ChannelType chType);

	void SetDisplayData(const DisplayData &newDisplayData);

	void PlayMusic(SoundTypes soundType);

	void SetTopDownCamera(const TopDownCamera &newCamera);
	void SetMouse(const Mouse &newMouse);

	void SetGamma(float newSceneGamma);

	void SetFusion(const FusionInput &newFusionInput);

	
	TopDownCamera &GetTopDownCamera();
	Mouse &GetMouse();


	std::shared_ptr<CelestialBody> GetSun();

	std::shared_ptr<Spaceship> GetSpaceship();
	std::shared_ptr<Swarm> GetSwarm();
	void DeleteSwarm();
	bool HasSwarms();
	SunLight GetSunLight();

	bool HasSuns();

	// TODO: Add Load function

	// !!! Only for testing purposes !!!
	void GenerateRandomSwarms(int count,
							  const BillboardProgDataNoTexture &progData);
};

#endif