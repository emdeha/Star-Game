#include "stdafx.h"
#include "World.h"

#include "../Fusion_EntitySystem/EntityEvents.h"

using namespace FusionEngine;


World::World()
{
	eventManager.AddListener(this, FusionEngine::EVENT_ON_RESHAPE);
	eventManager.AddListener(this, FusionEngine::EVENT_ON_CLICK);
}

World::~World()
{
}

void World::Load(const std::string &guiLayoutFile)
{
	FusionEngine::AssetLoader<FusionEngine::GUIAssetObject> guiLoader;
	guiLoader.RegisterType("loader-files", new FusionEngine::GUILoader());
	FusionEngine::GUIAssetObject loadedGUI = guiLoader.LoadAssetObject("loader-files", guiLayoutFile);
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
		if ((*layout).second->IsSet())
		{
			(*layout).second->Draw(fontData, simpleData, textureData);
		}
	}

	// Render Lights
	// TODO: Render relative to layout
	sunLight.Render(displayData.modelMatrix,
					shaderManager.GetLitProgData(), shaderManager.GetUniformBuffer(UBT_LIGHT));
	
	// Render Models
	// TODO: Render relative to layout
	renderer.Render(displayData.modelMatrix);
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
			for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
			{
				if((*layout).second->IsSet())
				{
					(*layout).second->Update(data.windowWidth, data.windowHeight);
				}
			}
		}
		break;
	case FusionEngine::EVENT_ON_CLICK:		
		{
			const OnClickEvent &data = static_cast<const OnClickEvent&>(eventData);
			if (data.isLeftButtonDown)
			{
				if (data.objectId == "newGame")
				{
					SetLayout(LAYOUT_IN_GAME);
				}
				else if (data.objectId == "resumeGame")
				{
					std::printf("\n\n\n CLICKED ON RESUME_GAME \n\n\n");
				}
				// ...
			}
		}
		break;
	}

	return false;
}