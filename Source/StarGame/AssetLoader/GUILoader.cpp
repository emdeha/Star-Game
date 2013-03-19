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


#include "stdafx.h"
#include "GUILoader.h"
#include "../framework/ErrorAPI.h"

#include <fstream>


struct LayoutData
{
	LayoutType layoutType;
	glm::vec4 layoutColor;
	int layoutIsSet;
};

struct ControlData
{
	char controlName[30];
	char controlText[30]; 
	int controlHasBackground;
	
	PresetAttributes controlPresets[3];

	float textBoxWidth;
	int textBoxCharWidth;

	float imageBoxWidth;
	float imageBoxHeight;

	int imageBoxIndex; /// this is only needed for the fusion. should refactor

	std::string fusionTextures[4];
	std::vector<std::string> skillTextures;
	std::vector<std::string> skillDescriptions;

	LayoutType toLayout;
};


GUILoader::GUILoader(const std::string &fileName,
					 int windowWidth, int windowHeight)
{
	// TODO: Add font loading

	std::string fontsDir = "";
	std::string texturesDir = "";
	std::string defaultFont = "";


	YAML::Node guiData = YAML::LoadFile(fileName);

	LayoutData layoutData;

	fontsDir = guiData["details"]["fonts-dir"].as<std::string>();
	texturesDir = guiData["details"]["textures-dir"].as<std::string>();
	defaultFont = guiData["details"]["default-font"].as<std::string>();

	for(YAML::Node::const_iterator guiNode = guiData.begin();
		guiNode != guiData.end(); ++guiNode)
	{
		/*
		if(guiNode->first.as<std::string>() == "details")
		{
			for(YAML::Node::const_iterator detail = guiNode->second.begin();
				detail != guiNode->second.end(); ++detail)
			{
				if(detail->first.as<std::string>() == "fonts-dir")
				{
					fontsDir = detail->second.as<std::string>();
				}
				else if(detail->first.as<std::string>() == "textures-dir")
				{
					texturesDir = detail->second.as<std::string>();
				}
				else if(detail->first.as<std::string>() == "default-font")
				{
					defaultFont = detail->second.as<std::string>();
				}
			}
		}
		*/

		if(guiNode->first.as<std::string>() == "layout-game")
		{
			layoutData.layoutType = LAYOUT_IN_GAME;
		}
		else if(guiNode->first.as<std::string>() == "layout-menu")
		{
			layoutData.layoutType = LAYOUT_MENU;
		}
		else if(guiNode->first.as<std::string>() == "layout-save-game")
		{
			layoutData.layoutType = LAYOUT_SAVE_GAME;
		}
		
		if(guiNode->first.as<std::string>() != "details")
		{
			layoutData.layoutColor = glm::vec4(guiNode->second["background-color"][0].as<float>(),
											   guiNode->second["background-color"][1].as<float>(), 
											   guiNode->second["background-color"][2].as<float>(), 
											   guiNode->second["background-color"][3].as<float>());
			layoutData.layoutIsSet = guiNode->second["is-active"].as<bool>();

			LayoutInfo layoutInfo;
			layoutInfo.backgroundColor = layoutData.layoutColor;

			std::shared_ptr<Layout> newLayout =
				std::shared_ptr<Layout>(new Layout(layoutData.layoutType, layoutInfo));
			layouts.insert(std::make_pair(layoutData.layoutType, newLayout));

			layouts[layoutData.layoutType]->Set(layoutData.layoutIsSet);

			if(guiNode->second["controls"])
			{
				ControlData controlData;
				for(YAML::Node::const_iterator control = guiNode->second["controls"].begin();
					control != guiNode->second["controls"].end(); ++control)
				{
					if(control->second["control-type"].as<std::string>() == "label")
					{					
						strcpy(controlData.controlName, control->second["name"].as<std::string>().c_str());
						strcpy(controlData.controlText, control->second["text"].as<std::string>().c_str());
						controlData.controlHasBackground = control->second["has-background"].as<bool>();

						int presetID = 0;
						for(YAML::Node::const_iterator preset = control->second["presets"].begin();
							preset != control->second["presets"].end(); ++preset)
						{
							controlData.controlPresets[presetID].position = glm::vec2(preset->second["position"][0].as<int>(),
																					  preset->second["position"][1].as<int>());
							controlData.controlPresets[presetID].textSize = preset->second["text-size"].as<int>();
						
							presetID++;
						}

						std::shared_ptr<Label> label = 
							std::shared_ptr<Label>(new Label(LayoutPreset(SMALL),
															 controlData.controlName, 
															 controlData.controlText,
															 controlData.controlPresets[SMALL].position,
															 controlData.controlPresets[SMALL].textSize,
															 controlData.controlHasBackground));
						label->Init(fontsDir + defaultFont, windowWidth, windowHeight);

						label->AddPreset(MEDIUM, 
										 controlData.controlPresets[MEDIUM].textSize,
										 controlData.controlPresets[MEDIUM].position);
						label->AddPreset(BIG, 
										 controlData.controlPresets[BIG].textSize,
										 controlData.controlPresets[BIG].position);

						newLayout->AddControl(label);
					}
					else if(control->second["control-type"].as<std::string>() == "button")
					{
						strcpy(controlData.controlName, control->second["name"].as<std::string>().c_str());
						strcpy(controlData.controlText, control->second["text"].as<std::string>().c_str());
						controlData.controlHasBackground = control->second["has-background"].as<bool>();

						int presetID = 0;
						for(YAML::Node::const_iterator preset = control->second["presets"].begin();
							preset != control->second["presets"].end(); ++preset)
						{
							controlData.controlPresets[presetID].position = glm::vec2(preset->second["position"][0].as<int>(),
																					  preset->second["position"][1].as<int>());
							controlData.controlPresets[presetID].textSize = preset->second["text-size"].as<int>();

							presetID++;
						}

						std::shared_ptr<Button> button = 
							std::shared_ptr<Button>(new Button(LayoutPreset(SMALL),
															   controlData.controlName, 
															   controlData.controlText,
															   controlData.controlPresets[SMALL].position,
															   controlData.controlPresets[SMALL].textSize,
															   controlData.controlHasBackground));
						button->Init(fontsDir + defaultFont, windowWidth, windowHeight);
					
						button->AddPreset(MEDIUM, 
										  controlData.controlPresets[MEDIUM].textSize,
										  controlData.controlPresets[MEDIUM].position);
						button->AddPreset(BIG, 
										  controlData.controlPresets[BIG].textSize,
										  controlData.controlPresets[BIG].position);

						newLayout->AddControl(button);
					}
					else if(control->second["control-type"].as<std::string>() == "imageBox")
					{
						strcpy(controlData.controlName, control->second["name"].as<std::string>().c_str());
						controlData.imageBoxHeight = control->second["height"].as<int>();
						controlData.imageBoxWidth = control->second["width"].as<int>();
						controlData.imageBoxIndex = control->second["index"].as<int>();

						int presetID = 0;
						for(YAML::Node::const_iterator preset = control->second["presets"].begin();
							preset != control->second["presets"].end(); ++preset)
						{
							controlData.controlPresets[presetID].position = glm::vec2(preset->second["position"][0].as<int>(),
																					  preset->second["position"][1].as<int>());

							presetID++;
						}

						controlData.fusionTextures[0] = texturesDir + control->second["textures"][0].as<std::string>();
						controlData.fusionTextures[1] = texturesDir + control->second["textures"][1].as<std::string>();
						controlData.fusionTextures[2] = texturesDir + control->second["textures"][2].as<std::string>();
						controlData.fusionTextures[3] = texturesDir + control->second["textures"][3].as<std::string>();
					
						std::shared_ptr<ImageBox> imageBox =
							std::shared_ptr<ImageBox>(new ImageBox(LayoutPreset(SMALL),
																   controlData.controlName,
																   controlData.controlPresets[SMALL].position,
																   controlData.imageBoxWidth,
																   controlData.imageBoxHeight,
																   controlData.imageBoxIndex));

						imageBox->SetTextures(controlData.fusionTextures);
						imageBox->Init();
						imageBox->AddPreset(MEDIUM, controlData.controlPresets[MEDIUM].position);
						imageBox->AddPreset(BIG, controlData.controlPresets[BIG].position);
						newLayout->AddControl(imageBox);
					}
					else if(control->second["control-type"].as<std::string>() == "hintBox")
					{
						strcpy(controlData.controlName, control->second["name"].as<std::string>().c_str());
						controlData.imageBoxHeight = control->second["height"].as<int>();
						controlData.imageBoxWidth = control->second["width"].as<int>();

						int presetID = 0;
						for(YAML::Node::const_iterator preset = control->second["presets"].begin();
							preset != control->second["presets"].end(); ++preset)
						{
							controlData.controlPresets[presetID].position = glm::vec2(preset->second["position"][0].as<int>(),
																					  preset->second["position"][1].as<int>());
							controlData.controlPresets[presetID].textSize = preset->second["text-size"].as<int>();

							presetID++;
						}

						for(YAML::Node::const_iterator texture = control->second["textures"].begin();
							texture != control->second["textures"].end(); ++texture)
						{
							controlData.skillTextures.push_back(texturesDir + texture->as<std::string>());
						}
						for(YAML::Node::const_iterator description = control->second["descriptions"].begin();
							description != control->second["descriptions"].end(); ++description)
						{
							controlData.skillDescriptions.push_back(description->as<std::string>());
						}
					
						std::shared_ptr<FusionHint> hintBox =
							std::shared_ptr<FusionHint>(new FusionHint(LayoutPreset(SMALL),
																	   controlData.controlName,
																	   controlData.controlPresets[SMALL].position,
																	   controlData.imageBoxWidth,
																	   controlData.imageBoxHeight,
																	   controlData.imageBoxIndex));

						hintBox->SetTextures(controlData.skillTextures);
						hintBox->SetDescriptions(controlData.skillDescriptions);
						hintBox->Init();
						hintBox->AddPreset(MEDIUM, controlData.controlPresets[MEDIUM].position);
						hintBox->AddPreset(BIG, controlData.controlPresets[BIG].position);
						newLayout->AddControl(hintBox);
					}
					else if(control->second["control-type"].as<std::string>() == "textBox")
					{
						strcpy(controlData.controlName, control->second["name"].as<std::string>().c_str());
						controlData.textBoxWidth = control->second["width"].as<int>();
						controlData.textBoxCharWidth = control->second["char-width"].as<int>();
						controlData.controlHasBackground = control->second["has-background"].as<bool>();

						int presetID = 0;
						for(YAML::Node::const_iterator preset = control->second["presets"].begin();
							preset != control->second["presets"].end(); ++preset)
						{
							controlData.controlPresets[presetID].position = glm::vec2(preset->second["position"][0].as<int>(),
																					  preset->second["position"][1].as<int>());
							controlData.controlPresets[presetID].textSize = preset->second["text-size"].as<int>();

							presetID++;
						}

						std::shared_ptr<TextBox> textBox =
							std::shared_ptr<TextBox>(new TextBox(controlData.textBoxWidth,
																 controlData.textBoxCharWidth,
																 LayoutPreset(SMALL),
																 controlData.controlName, "",
																 controlData.controlPresets[SMALL].position,
																 controlData.controlPresets[SMALL].textSize,
																 controlData.controlHasBackground));
						textBox->Init(fontsDir + defaultFont, windowWidth, windowHeight);

						textBox->AddPreset(MEDIUM, 
										   controlData.controlPresets[MEDIUM].textSize,
										   controlData.controlPresets[MEDIUM].position);
						textBox->AddPreset(BIG, 
										   controlData.controlPresets[BIG].textSize,
										   controlData.controlPresets[BIG].position);

						newLayout->AddControl(textBox);
					}
				}
			}
		}
	}
}

std::shared_ptr<Layout> GUILoader::GetLayout(LayoutType layoutType)
{
	if(layouts.find(layoutType) == layouts.end())
	{
		std::string errorMessage = "no layout of type ";
		switch(layoutType)
		{
		case LAYOUT_IN_GAME:
			errorMessage += "LAYOUT_IN_GAME";
			break;
		case LAYOUT_LOAD_GAME:
			errorMessage += "LAYOUT_LOAD_GAME";
			break;
		case LAYOUT_MENU:
			errorMessage += "LAYOUT_MENU";
			break;
		case LAYOUT_NEW_GAME:
			errorMessage += "LAYOUT_NEW_GAME";
			break;
		case LAYOUT_OPTIONS:
			errorMessage += "LAYOUT_OPTIONS";
			break;
		case LAYOUT_OTHER:
			errorMessage += "LAYOUT_OTHER";
			break;
		case LAYOUT_SAVE_GAME:
			errorMessage += "LAYOUT_SAVE_GAME";
			break;
		default:
			HandleUnexpectedError("unexpected \'default\' reached", __LINE__, __FILE__);
			break;
		}
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}

	return layouts[layoutType];
}

const std::map<LayoutType, std::shared_ptr<Layout>> &GUILoader::GetAllLoadedLayouts()
{
	return layouts;
}