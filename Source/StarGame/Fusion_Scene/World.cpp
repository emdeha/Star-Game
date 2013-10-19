#include "stdafx.h"
#include "World.h"

#include "../AssetLoader/GUILoader.h"


using namespace FusionEngine;

World::~World()
{
}

void World::Load(const std::string &guiLayoutFile)
{
	FusionEngine::AssetLoader<FusionEngine::GUIAssetObject> guiLoader;
	guiLoader.RegisterType("gui", new FusionEngine::GUILoader());
	FusionEngine::GUIAssetObject loadedGUI = guiLoader.LoadAssetObject("gui", "guiLayoutFile");
	guiLayouts = loadedGUI.GetAllLoadedLayouts();
}

void World::Render() 
{
	// Render GUI
	FontProgData fontData = shaderManager.GetFontProgData();
	SimpleProgData simpleData = shaderManager.GetSimpleNoUBProgData();
	TextureProgData textureData = shaderManager.GetTextureProgData();
	
	for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
	{
		(*layout).second->Draw(fontData, simpleData, textureData);
	}

	// Render Lights
	sunLight.Render(displayData.modelMatrix,
					shaderManager.GetLitProgData(), shaderManager.GetUniformBuffer(UBT_LIGHT));
	
	// Render Models
	renderer.Render(displayData.modelMatrix);
}