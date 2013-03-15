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
	std::string line;
	std::ifstream data(fileName);

	if(!data.is_open())
	{
		std::string errorMessage = "cannot open gui config file ";
		errorMessage += fileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}

	layouts.clear();


	LayoutData layoutData;
	ControlData controlData;

	std::string fontDir = "../data/fonts/";
	std::string texturesDir = "../data/images/";
	char fontFile[30];
	char textureFile[4][30];

	while(data)
	{
		getline(data, line);
		char tag[30];
		sscanf(line.c_str(), "%s ", &tag);

		if(strcmp(tag, "mainfont") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(tag));
			line[0] = ' ';
			sscanf(line.c_str(), "%s ", &fontFile);

			fontDir += fontFile;
		}
		if(strcmp(tag, "fusiontextures") == 0)
		{
			for(int i = 0; i < 4; i++)
			{
				controlData.fusionTextures[i] = texturesDir;
			}

			line.erase(line.begin(), line.begin() + strlen(tag));
			line[0] = ' ';
			sscanf(line.c_str(), "%s %s %s %s ", &textureFile[0],
													&textureFile[1],
													&textureFile[2],
													&textureFile[3]);

			for(int i = 0; i < 4; i++)
			{
				controlData.fusionTextures[i] += textureFile[i];
			}
		}
		if(strcmp(tag, "skillData") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(tag));
			line[0] = ' ';
			int skillDataCount = 0;
			sscanf(line.c_str(), "%i ", &skillDataCount);

			char chCount[3];
			itoa(skillDataCount, chCount, 10);
			line.erase(line.begin(), line.begin() + strlen(chCount) + 1);

			controlData.skillTextures.resize(skillDataCount);
			controlData.skillDescriptions.resize(skillDataCount);
			for(int i = 0; i < skillDataCount; i++)
			{
				char extractedTexture[30];
				sscanf(line.c_str(), "%s ", &extractedTexture);
				controlData.skillTextures[i] = texturesDir;
				controlData.skillTextures[i] += extractedTexture;
				line.erase(line.begin(), line.begin() + strlen(extractedTexture) + 1);
				char extractedDescription[30];
				sscanf(line.c_str(), "%s ", &extractedDescription);
				controlData.skillDescriptions[i] = extractedDescription;
				line.erase(line.begin(), line.begin() + strlen(extractedDescription) + 1);
			}
		}
		if(strcmp(tag, "layout") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(tag));
			line[0] = ' ';
			sscanf(line.c_str(), "%i %f %f %f %f %i", 
				&layoutData.layoutType,
				&layoutData.layoutColor.x,
				&layoutData.layoutColor.y,
				&layoutData.layoutColor.z,
				&layoutData.layoutColor.w,
				&layoutData.layoutIsSet);

			LayoutInfo layoutInfo;
			layoutInfo.backgroundColor = layoutData.layoutColor;

			std::shared_ptr<Layout> layout = 
				std::shared_ptr<Layout>(new Layout(layoutData.layoutType, layoutInfo));

			layouts.insert(std::make_pair(layoutData.layoutType, layout));

			layouts[layoutData.layoutType]->Set(layoutData.layoutIsSet);
		}
		if(strcmp(tag, "imagebox") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(tag));
			line[0] = ' ';
			sscanf(line.c_str(), "%s %i %f %f %f %f %f %f %f %f %i",
				&controlData.controlName,
				&controlData.toLayout,
				&controlData.controlPresets[SMALL].position.x,
				&controlData.controlPresets[SMALL].position.y,
				&controlData.controlPresets[MEDIUM].position.x,
				&controlData.controlPresets[MEDIUM].position.y,
				&controlData.controlPresets[BIG].position.x,
				&controlData.controlPresets[BIG].position.y,
				&controlData.imageBoxWidth,
				&controlData.imageBoxHeight,
				&controlData.imageBoxIndex);

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
			layouts[controlData.toLayout]->AddControl(imageBox);
		}
		if(strcmp(tag, "hintBox") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(tag)); 
			line[0] = ' ';
			sscanf(line.c_str(), "%s %i %f %f %i %f %f %i %f %f %i %f %f",
				&controlData.controlName,
				&controlData.toLayout,
				&controlData.controlPresets[SMALL].position.x,
				&controlData.controlPresets[SMALL].position.y,
				&controlData.controlPresets[SMALL].textSize,
				&controlData.controlPresets[MEDIUM].position.x,
				&controlData.controlPresets[MEDIUM].position.y,
				&controlData.controlPresets[MEDIUM].textSize,
				&controlData.controlPresets[BIG].position.x,
				&controlData.controlPresets[BIG].position.y,
				&controlData.controlPresets[BIG].textSize,
				&controlData.imageBoxWidth,
				&controlData.imageBoxHeight);

			std::shared_ptr<FusionHint> hintBox =
				std::shared_ptr<FusionHint>(new FusionHint(SMALL,
															controlData.controlName,
															controlData.controlPresets[SMALL].position,
															controlData.imageBoxWidth,
															controlData.imageBoxHeight,
															controlData.controlPresets[SMALL].textSize));
				
			hintBox->SetTextures(controlData.skillTextures);
			hintBox->SetDescriptions(controlData.skillDescriptions);
			hintBox->Init();
			hintBox->AddPreset(MEDIUM, controlData.controlPresets[MEDIUM].position);
			hintBox->AddPreset(BIG, controlData.controlPresets[BIG].position);
			layouts[controlData.toLayout]->AddControl(hintBox);
		}
		if(strcmp(tag, "button") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(tag));
			line[0] = ' ';
			sscanf(line.c_str(), "%s %s %i %i %f %f %i %f %f %i %f %f %i", 
				&controlData.controlName,
				&controlData.controlText,
				&controlData.controlHasBackground,
				&controlData.toLayout,
				&controlData.controlPresets[SMALL].position.x,
				&controlData.controlPresets[SMALL].position.y,
				&controlData.controlPresets[SMALL].textSize,
				&controlData.controlPresets[MEDIUM].position.x,
				&controlData.controlPresets[MEDIUM].position.y,
				&controlData.controlPresets[MEDIUM].textSize,
				&controlData.controlPresets[BIG].position.x,
				&controlData.controlPresets[BIG].position.y,
				&controlData.controlPresets[BIG].textSize);

			std::shared_ptr<Button> button = 
				std::shared_ptr<Button>(new Button(LayoutPreset(SMALL),
													controlData.controlName, controlData.controlText,
													controlData.controlPresets[SMALL].position,
													controlData.controlPresets[SMALL].textSize,
													controlData.controlHasBackground));
				
			button->Init(fontDir, 
							windowWidth, windowHeight);

			button->AddPreset(MEDIUM, 
								controlData.controlPresets[MEDIUM].textSize, 
								controlData.controlPresets[MEDIUM].position);

			button->AddPreset(BIG,
								controlData.controlPresets[BIG].textSize, 
								controlData.controlPresets[BIG].position);

			layouts[controlData.toLayout]->AddControl(button);
		}
		if(strcmp(tag, "textbox") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(tag));
			line[0] = ' ';
			sscanf(line.c_str(), "%s %f %i %i %i %f %f %i %f %f %i %f %f %i", 
				&controlData.controlName,
				&controlData.textBoxWidth,
				&controlData.textBoxCharWidth,
				&controlData.controlHasBackground,
				&controlData.toLayout,
				&controlData.controlPresets[SMALL].position.x,
				&controlData.controlPresets[SMALL].position.y,
				&controlData.controlPresets[SMALL].textSize,
				&controlData.controlPresets[MEDIUM].position.x,
				&controlData.controlPresets[MEDIUM].position.y,
				&controlData.controlPresets[MEDIUM].textSize,
				&controlData.controlPresets[BIG].position.x,
				&controlData.controlPresets[BIG].position.y,
				&controlData.controlPresets[BIG].textSize);
				
			std::shared_ptr<TextBox> textBox = 
				std::shared_ptr<TextBox>(new TextBox(controlData.textBoxWidth, 
														controlData.textBoxCharWidth,
														LayoutPreset(SMALL),
														controlData.controlName, "",
														controlData.controlPresets[SMALL].position, 
														controlData.controlPresets[SMALL].textSize,
														controlData.controlHasBackground));

			textBox->Init(fontDir, 
							windowWidth, windowHeight);

			textBox->AddPreset(MEDIUM,
								controlData.controlPresets[MEDIUM].textSize,
								controlData.controlPresets[MEDIUM].position);

			textBox->AddPreset(BIG,
								controlData.controlPresets[BIG].textSize,
								controlData.controlPresets[BIG].position);

			layouts[controlData.toLayout]->AddControl(textBox);
		}
		if(strcmp(tag, "label") == 0)
		{
			line.erase(line.begin(), line.begin() + strlen(tag));
			line[0] = ' ';
			sscanf(line.c_str(), "%s %s %i %i %f %f %i %f %f %i %f %f %i",
				&controlData.controlName,
				&controlData.controlText,
				&controlData.controlHasBackground,
				&controlData.toLayout,
				&controlData.controlPresets[SMALL].position.x,
				&controlData.controlPresets[SMALL].position.y,
				&controlData.controlPresets[SMALL].textSize,
				&controlData.controlPresets[MEDIUM].position.x,
				&controlData.controlPresets[MEDIUM].position.y,
				&controlData.controlPresets[MEDIUM].textSize,
				&controlData.controlPresets[BIG].position.x,
				&controlData.controlPresets[BIG].position.y,
				&controlData.controlPresets[BIG].textSize);


			std::shared_ptr<Label> label = 
				std::shared_ptr<Label>(new Label(LayoutPreset(SMALL),
													controlData.controlName, 
													controlData.controlText,
													controlData.controlPresets[SMALL].position,
													controlData.controlPresets[SMALL].textSize,
													controlData.controlHasBackground));

			label->Init(fontDir, 
						windowWidth, windowHeight);

			label->AddPreset(MEDIUM,
								controlData.controlPresets[MEDIUM].textSize,
								controlData.controlPresets[MEDIUM].position);

			label->AddPreset(BIG,
								controlData.controlPresets[BIG].textSize,
								controlData.controlPresets[BIG].position);

			layouts[controlData.toLayout]->AddControl(label);
		}
		if(strcmp(tag, "**") == 0) // file comments
		{
			continue;
		}
		if(strcmp(tag, "endfile") == 0)
		{
			data.close();
			return;
		}
	}
	data.close();

	return;
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