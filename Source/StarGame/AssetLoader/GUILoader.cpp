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
	std::string layoutBackgroundImageFileName;
};

struct ControlData
{
	std::string name;
	std::string text; 
	std::string backgroundImage;

	glm::vec4 fontColor;
	glm::vec4 margins;
	glm::vec2 position;
	glm::vec2 percentagedPosition;
	int textSize;

	bool hasBackground;
	bool isVisible;
	bool isUsingPercentage;
		
	float textBoxWidth;

	float imageBoxWidth;
	float imageBoxHeight;

	LayoutType toLayout;
};


GUILoader::GUILoader(const std::string &fileName,
					 int windowWidth, int windowHeight)
{
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
		else if(guiNode->first.as<std::string>() == "layout-load-game")
		{
			layoutData.layoutType = LAYOUT_LOAD_GAME;
		}
		else if(guiNode->first.as<std::string>() == "layout-options")
		{
			layoutData.layoutType = LAYOUT_OPTIONS;
		}
		
		if(guiNode->first.as<std::string>() != "details")
		{
			layoutData.layoutColor = glm::vec4(guiNode->second["background-color"][0].as<float>(),
											   guiNode->second["background-color"][1].as<float>(), 
											   guiNode->second["background-color"][2].as<float>(), 
											   guiNode->second["background-color"][3].as<float>());
			layoutData.layoutIsSet = guiNode->second["is-active"].as<bool>();

			std::shared_ptr<Layout> newLayout =
				std::shared_ptr<Layout>(new Layout(layoutData.layoutType, layoutData.layoutColor));
			layouts.insert(std::make_pair(layoutData.layoutType, newLayout));

			layouts[layoutData.layoutType]->Set(layoutData.layoutIsSet);
			if(guiNode->second["background-image"].IsDefined())
			{
				layoutData.layoutBackgroundImageFileName = texturesDir;
				layoutData.layoutBackgroundImageFileName += guiNode->second["background-image"].as<std::string>();
				layouts[layoutData.layoutType]->SetBackgroundImage(1280, 768, layoutData.layoutBackgroundImageFileName);
			}

			if(guiNode->second["controls"])
			{
				ControlData controlData;
				for(YAML::Node::const_iterator control = guiNode->second["controls"].begin();
					control != guiNode->second["controls"].end(); ++control)
				{
					if(control->second["control-type"].as<std::string>() == "label")
					{					
						controlData.name = control->second["name"].as<std::string>();
						controlData.text = control->second["text"].as<std::string>();
						controlData.hasBackground = control->second["has-background"].as<bool>();
						controlData.isVisible = control->second["is-visible"].as<bool>();
						controlData.isUsingPercentage = control->second["is-using-percentage"].as<bool>();
						controlData.fontColor = glm::vec4(control->second["font-color"][0].as<float>(),
														  control->second["font-color"][1].as<float>(),
														  control->second["font-color"][2].as<float>(),
														  control->second["font-color"][3].as<float>());
						controlData.textSize = control->second["text-size"].as<int>();
						controlData.backgroundImage = "";
						if(controlData.hasBackground)
						{
							controlData.margins = glm::vec4(control->second["margins"][0].as<float>(),
															control->second["margins"][1].as<float>(),
															control->second["margins"][2].as<float>(),
															control->second["margins"][3].as<float>());
							controlData.backgroundImage = texturesDir;
							controlData.backgroundImage += control->second["background-image"].as<std::string>();
						}
						controlData.percentagedPosition = glm::vec2();
						controlData.position = glm::vec2();
						if(controlData.isUsingPercentage)
						{
							controlData.percentagedPosition = 
								glm::vec2(control->second["percentaged-pos"][0].as<float>(),
										  control->second["percentaged-pos"][1].as<float>());
						}
						else
						{
							controlData.position = glm::vec2(control->second["position"][0].as<float>(),
															 control->second["position"][1].as<float>());
						}

						std::shared_ptr<Label> label =
							std::shared_ptr<Label>(new Label(controlData.name, controlData.text,
															 controlData.fontColor, controlData.position,
															 controlData.margins, controlData.textSize,
															 controlData.hasBackground, controlData.isVisible,
															 controlData.isUsingPercentage, 
															 controlData.percentagedPosition));
						label->Init(fontsDir + defaultFont, controlData.backgroundImage,
									windowWidth, windowHeight);

						newLayout->AddControl(label);
					}
					else if(control->second["control-type"].as<std::string>() == "button")
					{
						controlData.name = control->second["name"].as<std::string>();
						controlData.text = control->second["text"].as<std::string>();
						controlData.hasBackground = control->second["has-background"].as<bool>();
						controlData.isVisible = control->second["is-visible"].as<bool>();
						controlData.isUsingPercentage = control->second["is-using-percentage"].as<bool>();
						controlData.fontColor = glm::vec4(control->second["font-color"][0].as<float>(),
														  control->second["font-color"][1].as<float>(),
														  control->second["font-color"][2].as<float>(),
														  control->second["font-color"][3].as<float>());
						controlData.textSize = control->second["text-size"].as<int>();
						controlData.backgroundImage = "";
						if(controlData.hasBackground)
						{
							controlData.margins = glm::vec4(control->second["margins"][0].as<float>(),
															control->second["margins"][1].as<float>(),
															control->second["margins"][2].as<float>(),
															control->second["margins"][3].as<float>());
							controlData.backgroundImage = texturesDir;
							controlData.backgroundImage += control->second["background-image"].as<std::string>();
						}
						controlData.percentagedPosition = glm::vec2();
						controlData.position = glm::vec2();
						if(controlData.isUsingPercentage)
						{
							controlData.percentagedPosition = 
								glm::vec2(control->second["percentaged-pos"][0].as<float>(),
										  control->second["percentaged-pos"][1].as<float>());
						}
						else
						{
							controlData.position = glm::vec2(control->second["position"][0].as<float>(),
															 control->second["position"][1].as<float>());
						}

						std::shared_ptr<Button> button = 
							std::shared_ptr<Button>(new Button(controlData.name, controlData.text,
															   controlData.fontColor, controlData.position,
															   controlData.margins, controlData.textSize,
															   controlData.hasBackground, controlData.isVisible,
															   controlData.isUsingPercentage, 
															   controlData.percentagedPosition));
						button->Init(fontsDir + defaultFont, controlData.backgroundImage,
								     windowWidth, windowHeight);

						newLayout->AddControl(button);
					}
					else if(control->second["control-type"].as<std::string>() == "imageBox")
					{
						controlData.name = control->second["name"].as<std::string>();
						//controlData.text = control->second["text"].as<std::string>();
					    //controlData.hasBackground = control->second["has-background"].as<bool>();
						controlData.isVisible = control->second["is-visible"].as<bool>();
						controlData.isUsingPercentage = control->second["is-using-percentage"].as<bool>();
						//controlData.fontColor = glm::vec4(control->second["font-color"][0].as<float>(),
						//		    						control->second["font-color"][1].as<float>(),
						//								    control->second["font-color"][2].as<float>(),
						//								    control->second["font-color"][3].as<float>());
						//controlData.textSize = control->second["text-size"].as<int>();
						//controlData.backgroundImage = "";
						//if(controlData.hasBackground)
						//{
						//	controlData.margins = glm::vec4(control->second["margins"][0].as<float>(),
						//									control->second["margins"][1].as<float>(),
						//									control->second["margins"][2].as<float>(),
						//									control->second["margins"][3].as<float>());
						//	controlData.backgroundImage = texturesDir;
						//	controlData.backgroundImage += control->second["background-image"].as<std::string>();
						//}
						controlData.imageBoxHeight = control->second["height"].as<float>();
						controlData.imageBoxWidth = control->second["width"].as<float>();
						controlData.backgroundImage = texturesDir;
						controlData.backgroundImage += control->second["image"].as<std::string>();
						controlData.percentagedPosition = glm::vec2();
						controlData.position = glm::vec2();
						if(controlData.isUsingPercentage)
						{
							controlData.percentagedPosition = 
								glm::vec2(control->second["percentaged-pos"][0].as<float>(),
										  control->second["percentaged-pos"][1].as<float>());
						}
						else
						{
							controlData.position = glm::vec2(control->second["position"][0].as<float>(),
															 control->second["position"][1].as<float>());
						}

						std::shared_ptr<ImageBox> imageBox = 
							std::shared_ptr<ImageBox>(new ImageBox(controlData.imageBoxWidth, controlData.imageBoxHeight,
																   controlData.name, controlData.text,
																   controlData.fontColor, controlData.position,
																   controlData.margins, controlData.textSize,
																   controlData.hasBackground, controlData.isVisible,
																   controlData.isUsingPercentage, 
																   controlData.percentagedPosition));
						imageBox->Init(controlData.backgroundImage, windowWidth, windowHeight);

						newLayout->AddControl(imageBox);
					}
					else if(control->second["control-type"].as<std::string>() == "textBox")
					{
						controlData.name = control->second["name"].as<std::string>();
						//controlData.text = control->second["text"].as<std::string>();
						controlData.text = "";
						controlData.hasBackground = control->second["has-background"].as<bool>();
						controlData.isVisible = control->second["is-visible"].as<bool>();
						controlData.isUsingPercentage = control->second["is-using-percentage"].as<bool>();
						controlData.fontColor = glm::vec4(control->second["font-color"][0].as<float>(),
														  control->second["font-color"][1].as<float>(),
														  control->second["font-color"][2].as<float>(),
														  control->second["font-color"][3].as<float>());
						controlData.textSize = control->second["text-size"].as<int>();
						controlData.textBoxWidth = control->second["width"].as<float>();
						controlData.backgroundImage = "";
						if(controlData.hasBackground)
						{
							controlData.margins = glm::vec4(control->second["margins"][0].as<float>(),
															control->second["margins"][1].as<float>(),
															control->second["margins"][2].as<float>(),
															control->second["margins"][3].as<float>());
							controlData.backgroundImage = texturesDir;
							controlData.backgroundImage += control->second["background-image"].as<std::string>();
						}
						controlData.percentagedPosition = glm::vec2();
						controlData.position = glm::vec2();
						if(controlData.isUsingPercentage)
						{
							controlData.percentagedPosition = 
								glm::vec2(control->second["percentaged-pos"][0].as<float>(),
										  control->second["percentaged-pos"][1].as<float>());
						}
						else
						{
							controlData.position = glm::vec2(control->second["position"][0].as<float>(),
															 control->second["position"][1].as<float>());
						}

						std::shared_ptr<TextBox> textBox = 
							std::shared_ptr<TextBox>(new TextBox(controlData.textBoxWidth,
																 controlData.name, controlData.text,
															     controlData.fontColor, controlData.position,
															     controlData.margins, controlData.textSize,
															     controlData.hasBackground, controlData.isVisible,
															     controlData.isUsingPercentage, 
															     controlData.percentagedPosition));
						textBox->Init(fontsDir + defaultFont, controlData.backgroundImage,
								      windowWidth, windowHeight);

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