#include "stdafx.h"
#include "AssetLoader.h"

#include "../framework/ErrorAPI.h"
#include "../Fusion_Scene/World.h"
#include "../Fusion_GUI/ControlEvents.h"


using namespace FusionEngine;


std::shared_ptr<Layout> GUIAssetObject::GetLayout(LayoutType layoutType) const
{
	if(layouts.find(layoutType) == layouts.end())
    {
        std::string errorMessage = "no layout of type ";
        switch(layoutType)
        {
        case FE_LAYOUT_GAME:
            errorMessage += "LAYOUT_IN_GAME";
            break;
        case FE_LAYOUT_LOAD:
            errorMessage += "LAYOUT_LOAD_GAME";
            break;
        case FE_LAYOUT_MENU:
            errorMessage += "LAYOUT_MENU";
            break;
        case FE_LAYOUT_OPTIONS:
            errorMessage += "LAYOUT_OPTIONS";
            break;
        case FE_LAYOUT_SAVE:
            errorMessage += "LAYOUT_SAVE_GAME";
            break;
        default:
            HandleUnexpectedError("unexpected \'default\' reached", __LINE__, __FILE__);
            break;
        }
        HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
    }

    return layouts.at(layoutType);
}

const std::map<LayoutType, std::shared_ptr<Layout>> &GUIAssetObject::GetAllLoadedLayouts() const
{
	return layouts;
}


RelativityOption GetControlRelativity(const std::string &relativityString)
{
	if (relativityString == "BOTTOM_LEFT")
	{
		return FE_RELATIVE_BOTTOM_LEFT;
	}
	else if (relativityString == "BOTTOM_RIGHT")
	{
		return FE_RELATIVE_BOTTOM_RIGHT;
	}
	else if (relativityString == "CENTER_TOP")
	{
		return FE_RELATIVE_CENTER_TOP;
	}
	else if (relativityString == "CENTER_BOTTOM")
	{
		return FE_RELATIVE_CENTER_BOTTOM;
	}
	else if (relativityString == "TOP_RIGHT")
	{
		return FE_RELATIVE_TOP_RIGHT;
	}
	else if (relativityString == "TOP_LEFT")
	{
		return FE_RELATIVE_TOP_LEFT;
	}

	return FE_RELATIVE_BAD;
}

LayoutType GetLayoutType(const std::string &layoutTypeString)
{
	if (layoutTypeString == "layout-game")
	{
		return FE_LAYOUT_GAME;
	}
	else if (layoutTypeString == "layout-menu")
	{
		return FE_LAYOUT_MENU;
	}
	else if (layoutTypeString == "layout-save-game")
	{
		return FE_LAYOUT_SAVE;
	}
	else if (layoutTypeString == "layout-load-game")
	{
		return FE_LAYOUT_LOAD;
	}
	else if(layoutTypeString == "layout-options")
	{
		return FE_LAYOUT_OPTIONS;
	}

	return FE_LAYOUT_BAD;
}

EventHandlingFunc GetEventHandlerForControl(const std::string &eventHandlerType)
{
	if (eventHandlerType == "TEXT_BOX_ON_CLICK")
	{
		return Click_TextBox;
	}
	else if (eventHandlerType == "NEW_GAME_ON_CLICK")
	{
		return Click_NewGame;
	}
	else if (eventHandlerType == "RESUME_GAME_ON_CLICK")
	{
		return Click_ResumeGame;
	}
	else if (eventHandlerType == "SAVE_GAME_ON_CLICK")
	{
		return Click_SaveGame;
	}
	else if (eventHandlerType == "LOAD_GAME_ON_CLICK")
	{
		return Click_LoadGame;
	}
	else if (eventHandlerType == "OPTIONS_ON_CLICK")
	{
		return Click_Options;
	}
	else if (eventHandlerType == "BACK_BTN_ON_CLICK")
	{
		return Click_Back;
	}
	else if (eventHandlerType == "APPLY_INPUT_ON_CLICK")
	{
		return Click_Apply;
	}
	else if (eventHandlerType == "QUIT_ON_CLICK")
	{
		return Click_Quit;
	}
	else if (eventHandlerType == "FUSION_ON_KEY")
	{
		return KeyPressed_FusionImageBox;
	}
	else if (eventHandlerType == "GAME_LAYOUT_ON_KEY")
	{
		return KeyPressed_GameLayout;
	}

	std::string errorMessage = "not a valid event handling function ";
	errorMessage += eventHandlerType;
	HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	return nullptr;
}

GUIAssetObject GUILoader::Load(const std::string &type, const std::string &name)
{
	// GENERAL SETTINGS
	// TODO: Figure out how to pass them
	int windowWidth = GetWorld().GetDisplayData().windowWidth;
	int windowHeight = GetWorld().GetDisplayData().windowHeight;


	std::string fontsDir = "";
    std::string texturesDir = "";
    std::string defaultFont = "";

	// LAYOUT SETTINGS
	LayoutType currentLayoutType;
	bool isLayoutSet;

	// CONTROL SETTINGS
	std::string controlName;
	std::string controlType;
	std::string controlText;
	std::string controlBackgroundImage;
	glm::vec4 controlFontColor;
	glm::ivec2 controlDimensions;
	glm::ivec2 controlMargins;
	bool controlIsVisible;
	bool controlHasBackground;
	RelativityOption controlRelativity;
	glm::ivec2 controlPosition;
	unsigned short controlTextSize;
	float controlTextBoxMaxWidth;
	std::map<size_t, std::string> preloadedBackgroundsNames;

	EventHandlingFunc onClickHandler;
	EventHandlingFunc onKeyPressedHandler;

	//////////////////////////////

    YAML::Node guiData = YAML::LoadFile("../data/" + type + "/" + name);

	std::map<LayoutType, std::shared_ptr<Layout>> layouts;
	if (!guiData["details"])
	{
		HandleUnexpectedError("No \'details\' node in GUI config", __LINE__, __FILE__);
		return layouts; 
	}
    fontsDir = guiData["details"]["fonts-dir"].as<std::string>();
    texturesDir = guiData["details"]["textures-dir"].as<std::string>();
    defaultFont = guiData["details"]["default-font"].as<std::string>();
    
    for (YAML::Node::const_iterator guiNode = guiData.begin();
         guiNode != guiData.end(); ++guiNode)
    {
        if (guiNode->first.as<std::string>() != "details")
        {
			std::string layoutType = guiNode->first.as<std::string>();
			currentLayoutType = GetLayoutType(layoutType);
			if (currentLayoutType == FE_LAYOUT_BAD)
			{
				HandleUnexpectedError("BAD LAYOUT CAUGHT!!!", __LINE__, __FILE__);
				return layouts;
			}
        
            isLayoutSet = guiNode->second["is-active"].as<bool>();

            std::shared_ptr<Layout> newLayout =
                std::shared_ptr<Layout>(new Layout(currentLayoutType, windowWidth, windowHeight));
			newLayout->Init(GetWorld().GetEventManager());
			newLayout->Set(isLayoutSet);
			std::string layoutBackgroundFile = "";
            if (guiNode->second["background-image"])
            {
                layoutBackgroundFile = texturesDir;
                layoutBackgroundFile += guiNode->second["background-image"].as<std::string>();
				newLayout->SetBackgroundSprite(layoutBackgroundFile);
            }
			
			if (guiNode->second["event-handlers"])
			{
				for (auto eventHandler = guiNode->second["event-handlers"].begin();
						eventHandler != guiNode->second["event-handlers"].end(); ++eventHandler)
				{
					if (eventHandler->first.as<std::string>() == "OnKey")
					{
						newLayout->SetOnKeyPressedHandler(GetEventHandlerForControl(
																eventHandler->second.as<std::string>()));
					}
				}
			}

            if (guiNode->second["controls"])
            {
                for (YAML::Node::const_iterator control = guiNode->second["controls"].begin();
                     control != guiNode->second["controls"].end(); ++control)
                {
					controlName = control->second["name"].as<std::string>();
					controlHasBackground = control->second["has-background"].as<bool>();
					if (controlHasBackground)
					{
						controlBackgroundImage = control->second["background"].as<std::string>();
					}
					controlDimensions = glm::ivec2(control->second["width-height"][0].as<int>(),
												   control->second["width-height"][1].as<int>());
					controlIsVisible = control->second["is-visible"].as<bool>();
					controlRelativity = GetControlRelativity(control->second["relativity"].as<std::string>());
					if (controlRelativity == FE_RELATIVE_BAD)
					{
						HandleUnexpectedError("BAD RELATIVITY CAUGHT!!!", __LINE__, __FILE__);
						return layouts;
					}
					controlPosition = glm::ivec2(control->second["position"][0].as<int>(),
												 control->second["position"][1].as<int>());

					if (control->second["preloaded-backgrounds"])
					{
						size_t index = 1;
						for (auto background = control->second["preloaded-backgrounds"].begin();
							 background != control->second["preloaded-backgrounds"].end(); ++background)
						{
							preloadedBackgroundsNames.insert(std::make_pair(
															 index, 
															 texturesDir + background->as<std::string>()));
							index++;
						}
					}

					if (control->second["event-handlers"])
					{
						for (auto eventHandler = control->second["event-handlers"].begin();
							 eventHandler != control->second["event-handlers"].end(); ++eventHandler)
						{
							if (eventHandler->first.as<std::string>() == "OnClick")
							{
								onClickHandler =
									GetEventHandlerForControl(
										eventHandler->second.as<std::string>());
							}
							if (eventHandler->first.as<std::string>() == "OnKey")
							{
								onKeyPressedHandler = 
									GetEventHandlerForControl(
										eventHandler->second.as<std::string>());
							}
						}
					}
					
					std::string controlType = control->second["control-type"].as<std::string>();
                    if (controlType == "label" || controlType == "button" || controlType == "textBox")
                    {					
						controlTextSize = control->second["text-size"].as<unsigned short>();
						controlText = control->second["text"].as<std::string>();
						if (controlHasBackground)
						{
							controlMargins = glm::ivec2(control->second["margins"][0].as<int>(),
														control->second["margins"][1].as<int>());
						}
						controlFontColor = glm::vec4(control->second["font-color"][0].as<float>(),
													 control->second["font-color"][1].as<float>(),
													 control->second["font-color"][2].as<float>(),
													 control->second["font-color"][3].as<float>());
						if (controlType == "textBox")
						{
							controlTextBoxMaxWidth = control->second["text-max-width"].as<float>();
						}
                    }

					std::string controlBackgroundImageDir = texturesDir + controlBackgroundImage;
					if (controlType == "label")
					{
						std::shared_ptr<Label> newLabel = std::shared_ptr<Label>(new Label(
								controlName, controlPosition, controlDimensions.x, controlDimensions.y,
								windowWidth, windowHeight, controlMargins)
								);
						newLabel->SetRelativity(controlRelativity);
						newLabel->SetTextProperties(fontsDir + defaultFont, controlText, 
													controlFontColor, controlTextSize);
						if (controlHasBackground)
						{
							newLabel->SetBackground(controlBackgroundImageDir);
						}
						if (preloadedBackgroundsNames.size() > 0)
						{
							newLabel->AddPreloadedBackgrounds(preloadedBackgroundsNames);
						}
						newLabel->SetVisibility(controlIsVisible);
						newLabel->Init(GetWorld().GetEventManager());

						newLayout->AddControl(newLabel);
					}
					else if (controlType == "button")
					{
						std::shared_ptr<Button> newButton = std::shared_ptr<Button>(new Button(
								controlName, controlPosition, controlDimensions.x, controlDimensions.y,
								windowWidth, windowHeight, controlMargins)
								);
						newButton->SetRelativity(controlRelativity);
						newButton->SetTextProperties(fontsDir + defaultFont, controlText, 
													 controlFontColor, controlTextSize);
						if (controlHasBackground)
						{
							newButton->SetBackground(controlBackgroundImageDir);
						}
						if (preloadedBackgroundsNames.size() > 0)
						{
							newButton->AddPreloadedBackgrounds(preloadedBackgroundsNames);
						}
						newButton->SetVisibility(controlIsVisible);
						if (onClickHandler)
						{
							newButton->SetOnClickHandler(onClickHandler);
						}
						newButton->Init(GetWorld().GetEventManager());

						newLayout->AddControl(newButton);
					}
					else if (controlType == "textBox")
					{
						std::shared_ptr<TextBox> newTextBox = std::shared_ptr<TextBox>(new TextBox(
								controlName, controlPosition, controlDimensions.x, controlDimensions.y,
								windowWidth, windowHeight, controlMargins, controlTextBoxMaxWidth)
								);
						newTextBox->SetRelativity(controlRelativity);
						newTextBox->SetTextProperties(fontsDir + defaultFont, controlText, 
													  controlFontColor, controlTextSize);
						if (controlHasBackground)
						{
							newTextBox->SetBackground(controlBackgroundImageDir);
						}
						if (preloadedBackgroundsNames.size() > 0)
						{
							newTextBox->AddPreloadedBackgrounds(preloadedBackgroundsNames);
						}
						newTextBox->SetVisibility(controlIsVisible);
						if (onClickHandler)
						{
							newTextBox->SetOnClickHandler(onClickHandler);
						}
						newTextBox->Init(GetWorld().GetEventManager());

						newLayout->AddControl(newTextBox);
					}
					else if (controlType == "imageBox")
					{
						std::shared_ptr<ImageBox> newImageBox = std::shared_ptr<ImageBox>(new ImageBox(
								controlName, controlPosition, controlDimensions.x, controlDimensions.y,
								windowWidth, windowHeight)
								);
						newImageBox->SetRelativity(controlRelativity);
						if (controlHasBackground)
						{
							newImageBox->SetBackground(controlBackgroundImageDir);
						}
						if (preloadedBackgroundsNames.size() > 0)
						{
							newImageBox->AddPreloadedBackgrounds(preloadedBackgroundsNames);
						}
						newImageBox->SetVisibility(controlIsVisible);
						if (onKeyPressedHandler)
						{
							newImageBox->SetOnKeyPressedHandler(onKeyPressedHandler);
						}
						newImageBox->Init(GetWorld().GetEventManager());

						newLayout->AddControl(newImageBox);
					}
                }
            }

            layouts.insert(std::make_pair(currentLayoutType, newLayout));
        }
    }

	return layouts;
}