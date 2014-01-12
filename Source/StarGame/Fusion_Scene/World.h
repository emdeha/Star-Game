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


#ifndef FE_WORLD_H
#define FE_WORLD_H


#include "../Fusion_EventManager/EventManager.h"
#include "../Fusion_Renderer/Renderer.h"
#include "../Fusion_Renderer/ShaderManager.h"
#include "../Fusion_GUI/Layout.h"
#include "../Fusion_Entities/CelestialBody.h"
#include "../Fusion_Entities/Enemy.h"
#include "../Fusion_Entities/Skill.h"
#include "../Fusion_Entities/Light.h"
#pragma warning(push, 1)
#include "../Mouse/Mouse.h" // TODO: Replace with Fusion_Mouse
#include "../Camera/TopDownCamera.h" // TODO: Replace with Fusion_Camera
#include "../Audio/Audio.h" 

#include <memory>
#include <map>
#pragma warning(pop)


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
		Audio audio;
		LayoutsMap guiLayouts;
		std::unique_ptr<FusionInput> fusionInput;
		
		std::shared_ptr<CelestialBody> sun;
		std::vector<std::shared_ptr<Enemy>> enemies;

	private:
		World();
		World(const World&);
		World& operator=(const World&);
		~World();

		void CreateSun();
		void CreateEnemy(const std::string &id, glm::vec3 position);
		void CreateSkill(const std::string &id, const std::string &fusionCombination,
						 bool hasGeneric, int damage, float range, glm::vec3 position,
						 CelestialBodyType bodyToCreate,
						 OnEventFunc onClick, OnEventFunc onFusionCompleted, OnUpdateFunc onUpdate);

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
		Audio& GetAudio();

	public:
		// TODO: Later - a generic loading method
		void Load(const std::string &guiLayoutFile, 
				  const std::string &audioFile,
				  const std::string &shaderDataFile);
		void ReloadGUI(const std::string &guiLayoutFile);

		void Update();
		void Render();

		// WARN: If there's no layout of this type, all layouts will be unset!!!
		void SetLayout(LayoutType layoutToSet);
		Layout *GetLayout(LayoutType layoutType) const;
		Layout *GetCurrentLayout() const;

		//std::vector<std::shared_ptr<Skill>> GetCollidableSkills() const;
		
		unsigned int GetCurrentFusionInputIndex() const;
		void SetFusionInput(char sequenceEndButton,
							char validButtonA, char validButtonB, char validButtonC);
		void AddFusionSequence(const std::string &sequenceName,
							   char buttonA, char buttonB, char buttonC);
		void AddFusionSequence(const std::string &sequenceName,
							   const std::string &sequenceButtons);
		std::string GetFusionInputValidButtons() const;
		char GetFusionInputSequenceEndButton() const;

		std::string GetActiveSkillName() const;

		virtual bool HandleEvent(const IEventData &eventData);


		// Components
		std::shared_ptr<IComponent> GetComponentForObject(const std::string &id, ComponentType componentID);

		std::shared_ptr<CelestialBody> GetSun() { return sun; }
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
	inline Audio& World::GetAudio()
	{
		return audio;
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