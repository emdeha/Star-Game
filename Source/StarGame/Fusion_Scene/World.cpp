#include "stdafx.h"
#include "World.h"

#include "../Fusion_EntitySystem/EntityEvents.h"
#include "../Fusion_GUI/ControlEvents.h"

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
	std::shared_ptr<TextControl> newGame = 
		std::shared_ptr<TextControl>(new TextControl("newGame", glm::ivec2(100, 260), 
											 200, 50, displayData.windowWidth, displayData.windowHeight, 
											 glm::ivec2(5, 5)));
	newGame->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	newGame->SetTextProperties("../data/fonts/AGENCYR.TTF", "New Game", glm::vec4(1.0f), 24);
	newGame->SetOnClickHandler(NewGameHandler);
	newGame->SetBackground("../data/images/b-middle-section.png");
	newGame->Init(eventManager);
	std::shared_ptr<TextControl> loadGame =
		std::shared_ptr<TextControl>(new TextControl("loadGame", glm::ivec2(100, 200),
											 200, 50, displayData.windowWidth, displayData.windowHeight,
											 glm::ivec2(5, 5)));
	loadGame->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	loadGame->SetTextProperties("../data/fonts/AGENCYR.TTF", "Load Game", glm::vec4(1.0f), 24);
	loadGame->SetOnClickHandler(LoadGameHandler);
	loadGame->Init(eventManager);
	std::shared_ptr<TextControl> saveGame =
		std::shared_ptr<TextControl>(new TextControl("saveGame", glm::ivec2(100, 140),
											 200, 50, displayData.windowWidth, displayData.windowHeight,
											 glm::ivec2(5, 5)));
	saveGame->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	saveGame->SetTextProperties("../data/fonts/AGENCYR.TTF", "Save Game", glm::vec4(1.0f), 24);
	saveGame->SetOnClickHandler(SaveGameHandler);
	saveGame->Init(eventManager);
	std::shared_ptr<TextControl> options = 
		std::shared_ptr<TextControl>(new TextControl("options", glm::ivec2(100, 80),
											 200, 50, displayData.windowWidth, displayData.windowHeight,
											 glm::ivec2(5, 5)));
	options->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	options->SetTextProperties("../data/fonts/AGENCYR.TTF", "Options", glm::vec4(1.0f), 24);
	options->SetOnClickHandler(OptionsHandler);
	options->Init(eventManager);
	std::shared_ptr<TextControl> quit =
		std::shared_ptr<TextControl>(new TextControl("quit", glm::ivec2(100, 20),
											 200, 50, displayData.windowWidth, displayData.windowHeight,
											 glm::ivec2(5, 5)));
	quit->SetRelativity(FE_RELATIVE_BOTTOM_LEFT);
	quit->SetTextProperties("../data/fonts/AGENCYR.TTF", "Quit", glm::vec4(1.0f), 24);
	quit->SetOnClickHandler(QuitHandler);
	quit->Init(eventManager);
	std::shared_ptr<TextBox> testTextBox = 
		std::shared_ptr<TextBox>(new TextBox("sampleTextBox", glm::ivec2(200, 70),
											 100, 50, displayData.windowWidth, displayData.windowHeight,
											 glm::ivec2(5, 10), 80.0f));
	testTextBox->SetRelativity(FE_RELATIVE_TOP_RIGHT);
	testTextBox->SetTextProperties("../data/fonts/AGENCYR.TTF", "", glm::vec4(1.0f), 24);
	testTextBox->SetBackground("../data/images/fusion-empty.jpg");
	testTextBox->SetOnClickHandler(TextBoxClickHandler);
	testTextBox->Init(eventManager);
	std::shared_ptr<ImageBox> testImageBox =
		std::shared_ptr<ImageBox>(new ImageBox("sampleImageBox", glm::ivec2(-25, 10),
											   50, 50, displayData.windowWidth, displayData.windowHeight));
	testImageBox->SetRelativity(FE_RELATIVE_CENTER_BOTTOM);
	testImageBox->SetBackground("../data/images/fusion-catalyst.jpg");
	testImageBox->Init(eventManager);
	testLayout->AddControl(newGame);
	testLayout->AddControl(loadGame);
	testLayout->AddControl(saveGame);
	testLayout->AddControl(options);
	testLayout->AddControl(quit);
	testLayout->AddControl(testTextBox);
	testLayout->AddControl(testImageBox);

	std::shared_ptr<Layout> saveLayout = 
		std::shared_ptr<Layout>(new Layout(FE_LAYOUT_SAVE));
	saveLayout->Init(eventManager);
	std::shared_ptr<TextControl> inSave =
		std::shared_ptr<TextControl>(new TextControl("inSave", glm::ivec2(-50, 350),
								 100, 50, displayData.windowWidth, displayData.windowHeight,
								 glm::ivec2(5, 10)));
	inSave->SetRelativity(FE_RELATIVE_CENTER_BOTTOM);
	inSave->SetTextProperties("../data/fonts/AGENCYR.TTF", "IN SAVE", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 24);
	inSave->SetOnClickHandler(BackHandler);
	inSave->Init(eventManager);
	saveLayout->AddControl(inSave);
	std::shared_ptr<Layout> loadLayout = 
		std::shared_ptr<Layout>(new Layout(FE_LAYOUT_LOAD));
	loadLayout->Init(eventManager);
	std::shared_ptr<TextControl> inLoad =
		std::shared_ptr<TextControl>(new TextControl("inLoad", glm::ivec2(-50, 350),
								 100, 50, displayData.windowWidth, displayData.windowHeight,
								 glm::ivec2(5, 10)));
	inLoad->SetRelativity(FE_RELATIVE_CENTER_BOTTOM);
	inLoad->SetTextProperties("../data/fonts/AGENCYR.TTF", "IN LOAD", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 24);
	inLoad->SetOnClickHandler(BackHandler);
	inLoad->Init(eventManager);
	loadLayout->AddControl(inLoad);
	std::shared_ptr<Layout> optionsLayout =
		std::shared_ptr<Layout>(new Layout(FE_LAYOUT_OPTIONS));
	optionsLayout->Init(eventManager);
	std::shared_ptr<TextControl> inOptions =
		std::shared_ptr<TextControl>(new TextControl("inOptions", glm::ivec2(-50, 350),
								 100, 50, displayData.windowWidth, displayData.windowHeight,
								 glm::ivec2(5, 10)));
	inOptions->SetRelativity(FE_RELATIVE_CENTER_BOTTOM);
	inOptions->SetTextProperties("../data/fonts/AGENCYR.TTF", "IN OPTIONS", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 24);
	inOptions->SetOnClickHandler(BackHandler);
	inOptions->Init(eventManager);
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

Layout &World::GetCurrentLayout()
{
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		if ((*layout).second->IsSet())
		{
			return *(*layout).second.get();
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
	}

	return false;
}