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


#ifndef WORLD_H
#define WORLD_H


#include "../Fusion_EntitySystem/EventManager.h"
#include "../Fusion_Renderer/Renderer.h"
#include "../Mouse/Mouse.h" // TODO: Replace with Fusion_Mouse
#include "../Camera/TopDownCamera.h" // TODO: Replace with Fusion_Camera
#include "../StarGame/ShaderManager.h" // TODO: Replace with Fusion_ShaderManager
#include "../Entities/Lights.h" // TODO: Refactor
#include "../GUISystem/GUISystem.h"

#include <memory>
#include <map>


namespace FusionEngine
{
	struct DisplayData
	{
		float gamma;

		float zNear;
		float zFar;

		glm::mat4 projectionMatrix;
		glutil::MatrixStack modelMatrix;

		int windowWidth;
		int windowHeight;

		glm::ivec2 mousePosition;

		DisplayData()
		{
			gamma = 2.2f;

			zNear = 1.0f;
			zFar = 1000.0f;
		}
	};

	class World : public IEventListener
	{
	private:
		typedef std::map<LayoutType, std::shared_ptr<Layout>> LayoutsMap;
		
		EventManager eventManager;
		ShaderManager shaderManager;
		Renderer renderer;
		Mouse mouse;
		DisplayData displayData;
		TopDownCamera camera;
		SunLight sunLight;
		LayoutsMap guiLayouts;

	private:
		World();
		World(const World&);
		World& operator=(const World&);
		~World();

	public:
		float interpolation;


		static World& GetWorld();

		EventManager& GetEventManager();
		ShaderManager& GetShaderManager();
		Renderer& GetRenderer();
		Mouse& GetMouse();
		DisplayData& GetDisplayData();
		TopDownCamera& GetCamera();
		SunLight& GetSunLight();
		LayoutsMap& GetLayouts();

	public:
		// TODO: Later - a generic loading method
		void Load(const std::string &guiLayoutFile); 

		void Render();

		// WARN: If there's no layout of this type, all layouts will be unset!!!
		void SetLayout(LayoutType layoutToSet);

		virtual bool HandleEvent(const IEventData &eventData);
	};

	inline World& World::GetWorld()
	{
		static World worldInstance;
		return worldInstance;
	}

	inline EventManager& World::GetEventManager()
	{
		return eventManager;
	}
	inline Renderer& World::GetRenderer()
	{
		return renderer;
	}
	inline Mouse& World::GetMouse()
	{
		return mouse;
	}
	inline DisplayData& World::GetDisplayData()
	{
		return displayData;
	}
	inline TopDownCamera& World::GetCamera()
	{
		return camera;
	}
	inline ShaderManager& World::GetShaderManager()
	{
		return shaderManager;
	}
	inline SunLight& World::GetSunLight()
	{
		return sunLight;
	}
	inline World::LayoutsMap& World::GetLayouts()
	{
		return guiLayouts;
	}
}

inline FusionEngine::World& GetWorld()
{
	return FusionEngine::World::GetWorld();
}


#endif