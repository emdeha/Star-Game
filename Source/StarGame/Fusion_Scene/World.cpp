#include "stdafx.h"
#include "World.h"

#include "../Fusion_EventManager/EntityEvents.h"
#include "../Fusion_GUI/ControlEvents.h"
#include "../Fusion_Entities/CelestialBody.h"

using namespace FusionEngine;

#define FAST_LOAD

World::World()
{
	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);
}

World::~World()
{
}

void World::Load(const std::string &guiLayoutFile,
				 const std::string &audioFile,
				 const std::string &shaderDataFile)
{
	// Load Shader Programs
	FusionEngine::AssetLoader<FusionEngine::ShaderAssetObject> shaderLoader;
	shaderLoader.RegisterType("loader-files", new FusionEngine::ShaderLoader());
	FusionEngine::ShaderAssetObject loadedShaders = shaderLoader.LoadAssetObject("loader-files", shaderDataFile);
	shaderManager.Load(loadedShaders.GetAllLoadedPrograms(),
					   loadedShaders.GetAllLoadedUniformBuffers(),
					   loadedShaders.GetAllLoadedBlockIndices());

	// Load GUI
	FusionEngine::AssetLoader<FusionEngine::GUIAssetObject> guiLoader;
	guiLoader.RegisterType("loader-files", new FusionEngine::GUILoader());
	FusionEngine::GUIAssetObject loadedGUI = guiLoader.LoadAssetObject("loader-files", guiLayoutFile);
	guiLayouts = loadedGUI.GetAllLoadedLayouts();

#ifndef FAST_LOAD
	// Load Audio
	FusionEngine::AssetLoader<FusionEngine::AudioAssetObject> audioLoader;
	audioLoader.RegisterType("loader-files", new FusionEngine::CAudioLoader());
	FusionEngine::AudioAssetObject loadedAudio = audioLoader.LoadAssetObject("loader-files", audioFile);
	auto loadedAudios = loadedAudio.GetAllLoadedAudios();
	
	std::string path = "../data/music/"; // TODO: Get from config
	for (auto loadedAudio = loadedAudios.begin(); loadedAudio != loadedAudios.end(); ++loadedAudio)
	{
		for (auto audioFile = loadedAudio->second.audioFiles.begin(); 
			 audioFile != loadedAudio->second.audioFiles.end(); ++audioFile)
		{
			path += audioFile->path;
			audio.SetFileForPlay(path, audioFile->soundType, audioFile->isLooping);
			audio.SetVolume(loadedAudio->second.channelVolume, loadedAudio->second.channel);
			path = "../data/music/";
		}
	}
#endif
	
	// Load Cheats
}

void World::ReloadGUI(const std::string &guiLayoutFile)
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		(*layout).second->RemoveListeners(eventManager);
	}
	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);

	guiLayouts.clear();

	FusionEngine::AssetLoader<FusionEngine::GUIAssetObject> guiLoader;
	guiLoader.RegisterType("loader-files", new FusionEngine::GUILoader());
	FusionEngine::GUIAssetObject loadedGUI = guiLoader.LoadAssetObject("loader-files", guiLayoutFile);
	guiLayouts = loadedGUI.GetAllLoadedLayouts();
}

void World::Update()
{
	sun->Update();
	for (auto enemy = enemies.begin(); enemy != enemies.end(); ++enemy)
	{
		(*enemy)->Update();
	}
}

void World::Render()
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).second->IsSet())
		{
			(*layout).second->Draw(displayData.modelMatrix);
			
			if ((*layout).first == FE_LAYOUT_GAME)
			{
				//sunLight.Render(displayData.modelMatrix, shaderManager);
				renderer.Render(displayData.modelMatrix);
			}
		}
	}
}

void World::SetLayout(LayoutType layoutToSet)
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).first == layoutToSet)
		{
			(*layout).second->Set(true);
		}
		else 
		{
			(*layout).second->Set(false);
		}
	}
}

Layout *World::GetLayout(LayoutType layoutType) const
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).second->GetType() == layoutType)
		{
			return (*layout).second.get();
		}
	}

	std::ostringstream errorMsg;
	errorMsg << "no such layout: " << (int)layoutType;
	HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	return nullptr;
}

Layout *World::GetCurrentLayout() const
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).second->IsSet())
		{
			return (*layout).second.get();
		}
	}

	HandleUnexpectedError("no current layout", __LINE__, __FILE__);
	return nullptr;
}

unsigned int World::GetCurrentFusionInputIndex() const
{
	return fusionInput->GetCurrentInputIndex();
}

std::string World::GetFusionInputValidButtons() const
{
	return fusionInput->GetValidInputButtons();
}
char World::GetFusionInputSequenceEndButton() const
{
	return fusionInput->GetSequenceEndButton();
}

void World::SetFusionInput(char sequenceEndButton,
						   char validButtonA, char validButtonB, char validButtonC)
{
	fusionInput = std::unique_ptr<FusionInput>(
		new FusionInput(sequenceEndButton, validButtonA, validButtonB, validButtonC,
					    eventManager));
}

void World::AddFusionSequence(const std::string &sequenceName,
							  char buttonA, char buttonB, char buttonC)
{
	fusionInput->AddSequence(sequenceName, buttonA, buttonB, buttonC);
}
void World::AddFusionSequence(const std::string &sequenceName,
							  const std::string &sequenceButtons)
{
	AddFusionSequence(sequenceName, sequenceButtons[0], sequenceButtons[1], sequenceButtons[2]);
}

std::string World::GetActiveSkillName() const
{
	if (fusionInput->GetCurrentInputSequence().length() > 0)
	{
			return fusionInput->GetSequenceName(fusionInput->GetCurrentInputSequence());
	}
	else return "";
}

std::shared_ptr<IComponent> World::GetComponentForObject(const std::string &id, ComponentType componentID)
{
	if (id == "sun")
	{
			return sun->GetComponent(componentID);
	}
	else
	{
			auto sunSats = sun->GetSatellites();
			for (auto sat = sunSats.begin(); sat != sunSats.end(); ++sat)
			{
					if ((*sat)->GetID() == id)
					{
							return (*sat)->GetComponent(componentID);
					}
			}

			auto allSkills = sun->GetAllSkills();
			for (auto skill = allSkills.begin(); skill != allSkills.end(); ++skill)
			{
					if ((*skill)->GetID() == id)
					{
							return (*skill)->GetComponent(componentID);
					}
			}

			for (auto enemy = enemies.begin(); enemy != enemies.end(); ++enemy)
			{
					if ((*enemy)->GetID() == id)
					{
							return (*enemy)->GetComponent(componentID);
					}
			}
	}

	std::ostringstream errorMsg;
	errorMsg << "Object with id: " << id << " not found";
	HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	return nullptr;
}

bool World::HandleEvent(const IEventData &eventData)
{
	EventType type = eventData.GetType();
	switch(type)
	{
	case FusionEngine::EVENT_ON_RESHAPE:
		{
			const OnReshapeEvent &data = static_cast<const OnReshapeEvent&>(eventData);
			displayData.windowHeight = data.windowHeight;
			displayData.windowWidth = data.windowWidth;
		}
		break;
	}

	return false;
}