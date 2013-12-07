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

void NewGameHandler()
{
	GetWorld().SetLayout(FE_LAYOUT_GAME);
}
void SaveGameHandler()
{
	GetWorld().SetLayout(FE_LAYOUT_SAVE);
}
void LoadGameHandler()
{
	GetWorld().SetLayout(FE_LAYOUT_LOAD);
}
void OptionsHandler()
{
	GetWorld().SetLayout(FE_LAYOUT_OPTIONS);
}
void BackHandler()
{
	GetWorld().SetLayout(FE_LAYOUT_MENU);
}
void QuitHandler()
{
	exit(0);
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
	std::shared_ptr<Layout> testLayout =
		std::shared_ptr<Layout> (new Layout(FE_LAYOUT_MENU, displayData.windowWidth, displayData.windowHeight));
	testLayout->Init(eventManager);
	testLayout->SetBackgroundSprite("../data/images/background.png");
	std::shared_ptr<Control> newGame = 
		std::shared_ptr<Control>(new Control("newGame", glm::ivec2(100, 260), glm::ivec2(5, 5), 
											 200, 50, displayData.windowWidth, displayData.windowHeight));
	newGame->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	newGame->SetTextProperties("../data/fonts/AGENCYR.TTF", "New Game", glm::vec4(1.0f), 24);
	newGame->SetOnClickHandler(NewGameHandler);
	newGame->Init("../data/images/b-middle-section.jpg", eventManager);
	std::shared_ptr<Control> loadGame =
		std::shared_ptr<Control>(new Control("loadGame", glm::ivec2(100, 200), glm::ivec2(5, 5), 
											 200, 50, displayData.windowWidth, displayData.windowHeight));
	loadGame->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	loadGame->SetTextProperties("../data/fonts/AGENCYR.TTF", "Load Game", glm::vec4(1.0f), 24);
	loadGame->SetOnClickHandler(LoadGameHandler);
	loadGame->Init("../data/images/b-middle-section.jpg", eventManager);
	std::shared_ptr<Control> saveGame =
		std::shared_ptr<Control>(new Control("saveGame", glm::ivec2(100, 140), glm::ivec2(5, 5), 
											 200, 50, displayData.windowWidth, displayData.windowHeight));
	saveGame->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	saveGame->SetTextProperties("../data/fonts/AGENCYR.TTF", "Save Game", glm::vec4(1.0f), 24);
	saveGame->SetOnClickHandler(SaveGameHandler);
	saveGame->Init("../data/images/b-middle-section.jpg", eventManager);
	std::shared_ptr<Control> options = 
		std::shared_ptr<Control>(new Control("options", glm::ivec2(100, 80), glm::ivec2(5, 5), 
											 200, 50, displayData.windowWidth, displayData.windowHeight));
	options->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	options->SetTextProperties("../data/fonts/AGENCYR.TTF", "Options", glm::vec4(1.0f), 24);
	options->SetOnClickHandler(OptionsHandler);
	options->Init("../data/images/b-middle-section.jpg", eventManager);
	std::shared_ptr<Control> quit =
		std::shared_ptr<Control>(new Control("quit", glm::ivec2(100, 20), glm::ivec2(5, 5), 
											 200, 50, displayData.windowWidth, displayData.windowHeight));
	quit->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	quit->SetTextProperties("../data/fonts/AGENCYR.TTF", "Quit", glm::vec4(1.0f), 24);
	quit->SetOnClickHandler(QuitHandler);
	quit->Init("../data/images/b-middle-section.jpg", eventManager);
	testLayout->AddControl(newGame);
	testLayout->AddControl(loadGame);
	testLayout->AddControl(saveGame);
	testLayout->AddControl(options);
	testLayout->AddControl(quit);

	std::shared_ptr<Layout> saveLayout = 
		std::shared_ptr<Layout>(new Layout(FE_LAYOUT_SAVE));
	saveLayout->Init(eventManager);
	std::shared_ptr<Control> inSave =
		std::shared_ptr<Control>(new Control("inSave", glm::ivec2(-50, 350), glm::ivec2(5, 10),
								 100, 50, displayData.windowWidth, displayData.windowHeight));
	inSave->SetRelativity(FE_RELATIVE_CENTER_BOTTOM);
	inSave->SetTextProperties("../data/fonts/AGENCYR.TTF", "IN SAVE", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 24);
	inSave->SetOnClickHandler(BackHandler);
	inSave->Init("../data/images/b-middle-section.jpg", eventManager);
	saveLayout->AddControl(inSave);
	std::shared_ptr<Layout> loadLayout = 
		std::shared_ptr<Layout>(new Layout(FE_LAYOUT_LOAD));
	loadLayout->Init(eventManager);
	std::shared_ptr<Control> inLoad =
		std::shared_ptr<Control>(new Control("inLoad", glm::ivec2(-50, 350), glm::ivec2(5, 10),
								 100, 50, displayData.windowWidth, displayData.windowHeight));
	inLoad->SetRelativity(FE_RELATIVE_CENTER_BOTTOM);
	inLoad->SetTextProperties("../data/fonts/AGENCYR.TTF", "IN LOAD", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 24);
	inLoad->SetOnClickHandler(BackHandler);
	inLoad->Init("../data/images/b-middle-section.jpg", eventManager);
	loadLayout->AddControl(inLoad);
	std::shared_ptr<Layout> optionsLayout =
		std::shared_ptr<Layout>(new Layout(FE_LAYOUT_OPTIONS));
	optionsLayout->Init(eventManager);
	std::shared_ptr<Control> inOptions =
		std::shared_ptr<Control>(new Control("inOptions", glm::ivec2(-50, 350), glm::ivec2(5, 10),
								 100, 50, displayData.windowWidth, displayData.windowHeight));
	inOptions->SetRelativity(FE_RELATIVE_CENTER_BOTTOM);
	inOptions->SetTextProperties("../data/fonts/AGENCYR.TTF", "IN OPTIONS", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 24);
	inOptions->SetOnClickHandler(BackHandler);
	inOptions->Init("../data/images/b-middle-section.jpg", eventManager);
	optionsLayout->AddControl(inOptions);
	std::shared_ptr<Layout> inGameLayout = 
		std::shared_ptr<Layout>(new Layout(FE_LAYOUT_GAME));
	inGameLayout->Init(eventManager);
	guiLayouts.insert(std::make_pair(FE_LAYOUT_MENU, testLayout));
	guiLayouts.insert(std::make_pair(FE_LAYOUT_GAME, inGameLayout));
	guiLayouts.insert(std::make_pair(FE_LAYOUT_SAVE, saveLayout));
	guiLayouts.insert(std::make_pair(FE_LAYOUT_LOAD, loadLayout));
	guiLayouts.insert(std::make_pair(FE_LAYOUT_OPTIONS, optionsLayout));

	SetLayout(FE_LAYOUT_MENU);


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
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).second->IsSet())
		{
			(*layout).second->Draw(displayData.modelMatrix);
			
			if ((*layout).first == FE_LAYOUT_GAME)
			{
				sunLight.Render(displayData.modelMatrix, shaderManager);
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