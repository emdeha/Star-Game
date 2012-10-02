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

	LayoutType toLayout;
};


GUILoader::GUILoader(const std::string &fileName,
					 int windowWidth, int windowHeight)
{
	std::string line;
	std::ifstream data(fileName);

	layouts.clear();


	LayoutData layoutData;
	ControlData controlData;

	std::string fontDir = "../data/fonts/";
	char fontFile[30];


	if(data.is_open())
	{	
		//getline(data, line);
		while(!data.eof())
		{
			getline(data, line);
			char tag[30];
			sscanf(line.c_str(), "%s ", &tag);

			if(strcmp(tag, "mainfont") == 0)
			{
				line.erase(0, 8);
				line[0] = ' ';
				sscanf(line.c_str(), "%s ", &fontFile);

				fontDir += fontFile;
			}
			if(strcmp(tag, "layout") == 0)
			{
				line.erase(0, 6);
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
			if(strcmp(tag, "button") == 0)
			{
				line.erase(0, 6);
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
				line.erase(0, 7);
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
				line.erase(0, 5);
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
	}
	else
	{
		// TODO: Better error handling.
		std::printf("Unable to open file.\n");
		return;
	}

	return;
}

std::shared_ptr<Layout> GUILoader::GetLayout(LayoutType layoutType)
{
	if(layouts.find(layoutType) == layouts.end())
	{
		// TODO: Better error handling.
		std::printf("No such layout\n");
		return false;
	}

	return layouts[layoutType];
}

const std::map<LayoutType, std::shared_ptr<Layout>> &GUILoader::GetAllLoadedLayouts()
{
	return layouts;
}