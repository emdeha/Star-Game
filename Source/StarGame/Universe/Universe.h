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

/// \file Universe.h
/// \brief Contains the class which wraps the whole game world.

#ifndef UNIVERSE_H
#define UNIVERSE_H


#include <vector>
#include "../Entities/Lights.h"
#include "../Entities/PlanetBodies.h"
#include "../Audio/Audio.h"
#include "../GUISystem/GUISystem.h"

#include "../framework/EventSystem.h"


/// \class Universe
/// \brief This class updates, renders and maintains the game world a.k.a. Universe.
class Universe
{
private:
	std::vector<SunLight> lights; ///< Holds the world's lights.
	std::vector<std::shared_ptr<Sun>> suns; ///< Holds the world's suns.
	// std::vector<std::shared_ptr<Enemies>> enemies;

	std::map<LayoutType, std::shared_ptr<Layout>> universeLayouts;

	Audio universeMusic;

	float universeGamma;

public:
	Universe();

	/// \fn UpdateUniverse
	/// \brief Goes through the game objects and calls their update functions.
	void UpdateUniverse();

	void OnEvent(Event &_event);

	/// \fn RenderUniverse
	/// \brief Goes through the game objects and calls their render functions.
	void RenderUniverse(glutil::MatrixStack &modelMatrix, 
						GLuint materialBlockIndex, GLuint lightUniformBuffer,
						const LitProgData &litData, 
						const UnlitProgData &unLitData, 
						const SimpleProgData &interpData);

	void RenderCurrentLayout(const FontProgData &fontData,
							 const SimpleProgData &simpleData);
	void UpdateCurrentLayout(int windowWidth, int windowHeight);


	void StopUniverse();
	void StartUniverse();
	
	
	void AddSunLight(const SunLight &newSunLight);
	void AddSun(const std::shared_ptr<Sun> newSun);


	void AddLayout(const std::shared_ptr<Layout> newLayout);
	void AddLayout(LayoutType layoutType, LayoutInfo layoutInfo);
	std::shared_ptr<Layout> GetLayout(LayoutType layoutType);
	bool HasLayout(LayoutType layoutType);
	bool IsLayoutOn(LayoutType layoutType);

	void SetLayout(LayoutType layoutType, bool isSet);
	void SetLayoutPreset(LayoutPreset layoutPreset);

	void SetMusic(const std::string &fileName, SoundTypes soundType);
	void SetMusicVolume(float volume, ChannelType chType);
	void PlayMusic(SoundTypes soundType);

	
	void SetGamma(float newUniverseGamma);


	~Universe();

	// TODO: Add Load function
};

#endif