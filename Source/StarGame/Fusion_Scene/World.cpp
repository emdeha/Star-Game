#include "stdafx.h"
#include "World.h"

#include "../Fusion_EntitySystem/EntityEvents.h"

using namespace FusionEngine;

#define FAST_LOAD

World::World()
{
	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);
	eventManager.AddListener(this, FusionEngine::EVENT_ON_CLICK);
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
	/*
	FusionEngine::AssetLoader<FusionEngine::GUIAssetObject> guiLoader;
	guiLoader.RegisterType("loader-files", new FusionEngine::GUILoader());
	FusionEngine::GUIAssetObject loadedGUI = guiLoader.LoadAssetObject("loader-files", guiLayoutFile);
	guiLayouts = loadedGUI.GetAllLoadedLayouts();
	*/
	Layout testLayout = Layout(FE_LAYOUT_MENU);
	std::shared_ptr<Control> testControl = 
		std::shared_ptr<Control>(new Control(glm::ivec2(30, 330), 100, 10, displayData.windowWidth, displayData.windowHeight));
	testControl->SetRelativity(FE_RELATIVE_TOP_LEFT);
	testControl->Init("../data/images/b-middle-section.jpg", eventManager);
	std::shared_ptr<Control> testControlTwo =
		std::shared_ptr<Control>(new Control(glm::ivec2(130, 360), 100, 30, displayData.windowWidth, displayData.windowHeight));
	testControlTwo->Init("../data/images/b-middle-section.jpg", eventManager);
	std::shared_ptr<Control> centeredControl = 
		std::shared_ptr<Control>(new Control(glm::ivec2(-20, 320), 40, 20, displayData.windowWidth, displayData.windowHeight));
	centeredControl->SetRelativity(FE_RELATIVE_CENTER_TOP);
	centeredControl->Init("../data/images/b-middle-section.jpg", eventManager);
	testLayout.AddControl(testControl);
	testLayout.AddControl(testControlTwo);
	testLayout.AddControl(centeredControl);
	guiLayouts.insert(std::make_pair(FE_LAYOUT_MENU, testLayout));


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

void World::Render()
{
	// Render GUI
	/*
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).second->IsSet())
		{
			(*layout).second->Draw(shaderManager);
		}
	}
	*/
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		(*layout).second.Draw(displayData.modelMatrix);
	}

	// Render Lights
	// TODO: Render relative to layout
	sunLight.Render(displayData.modelMatrix, shaderManager);
	
	// Render Models
	// TODO: Render relative to layout
	renderer.Render(displayData.modelMatrix);
}

void World::SetLayout(LayoutType layoutToSet)
{
	/*
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).first == layoutToSet)
		{
			(*layout).second->Set(true);
			(*layout).second->Update(displayData.windowWidth, displayData.windowHeight);
		}
		else 
		{
			(*layout).second->Set(false);
		}
	}
	*/
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
			/*
			for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
			{
				if((*layout).second->IsSet())
				{
					(*layout).second->Update(data.windowWidth, data.windowHeight);
				}
			}
			*/
		}
		break;
	case FusionEngine::EVENT_ON_CLICK:		
		{
			const OnClickEvent &data = static_cast<const OnClickEvent&>(eventData);
			if (data.isLeftButtonDown)
			{
				/*
				if (data.objectId == "newGame")
				{
					SetLayout(LAYOUT_IN_GAME);
					audio.Play(MUSIC_BACKGROUND);
				}
				else if (data.objectId == "resumeGame")
				{
					SetLayout(LAYOUT_IN_GAME);
				}
				else if (data.objectId == "saveGame")
				{
					SetLayout(LAYOUT_SAVE_GAME);
				}
				else if (data.objectId == "loadGame")
				{
					SetLayout(LAYOUT_LOAD_GAME);
				}
				else if (data.objectId == "options")
				{
					SetLayout(LAYOUT_OPTIONS);
				}
				else if (data.objectId == "quitGame")
				{
					exit(EXIT_SUCCESS);
				}
				else if (data.objectId == "backBtn")
				{
					SetLayout(LAYOUT_MENU);
				}
				*/
			}
		}
		break;
	}

	return false;
}