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


GUILoader::GUILoader(const std::string &fileName,
					 int windowWidth, int windowHeight)
{
	std::string line;
	std::ifstream data(fileName);

	layouts.clear();


	LayoutType layoutType;
	glm::vec4 layoutColor;
	int layoutIsSet = 0;


	std::string controlType;
	char controlName[30];
	char controlText[30];
	int controlHasBackground = 0;
	int controlCurrPreset = 0;
	float textBoxWidth = 0.0f;
	int textBoxCharWidth = 0;
	// PresetAttributes controlPresets[3]; !!!
	glm::vec2 controlPosition;
	int controlTextSize = 0;
	LayoutType toLayout;


	if(data.is_open())
	{	
		//getline(data, line);
		while(!data.eof())
		{
			getline(data, line);
			char tag[30];
			sscanf(line.c_str(), "%s ", &tag);

			if(strcmp(tag, "layout") == 0)
			{
				line.erase(0, 6);
				line[0] = ' ';
				sscanf(line.c_str(), "%i %f %f %f %f %i", &layoutType,
														  &layoutColor.x,
														  &layoutColor.y,
														  &layoutColor.z,
														  &layoutColor.w,
														  &layoutIsSet);

				LayoutInfo layoutInfo;
				layoutInfo.backgroundColor = layoutColor;

				std::shared_ptr<Layout> layout = std::shared_ptr<Layout>(new Layout(layoutType, layoutInfo));

				layouts.insert(std::make_pair(layoutType, layout));

				layouts[layoutType]->Set(layoutIsSet);
			}
			if(strcmp(tag, "button") == 0)
			{
				line.erase(0, 6);
				line[0] = ' ';
				sscanf(line.c_str(), "%s %s %i %i %i %f %f %i", &controlName,
																&controlText,
																&controlHasBackground,
																&toLayout,
																&controlCurrPreset,
																&controlPosition.x,
																&controlPosition.y,
																&controlTextSize);						

				//std::printf("%i ", controlPresets[controlCurrPreset].textSize);

				std::shared_ptr<Button> button = std::shared_ptr<Button>(new Button(LayoutPreset(controlCurrPreset),
																					controlName, controlText,
																					controlPosition,
																					controlTextSize,
																					controlHasBackground));

				button->Init("../data/fonts/AGENCYR.TTF", 
							 windowWidth, windowHeight);

				layouts[toLayout]->AddControl(button);
			}
			if(strcmp(tag, "textbox") == 0)
			{
				line.erase(0, 7);
				line[0] = ' ';
				sscanf(line.c_str(), "%s %f %i %i %i %i %f %f %i", &controlName,
																   &textBoxWidth,
																   &textBoxCharWidth,
																   &controlHasBackground,
																   &toLayout,
																   &controlCurrPreset,
																   &controlPosition.x,
																   &controlPosition.y,
																   &controlTextSize);

				std::shared_ptr<TextBox> textBox = std::shared_ptr<TextBox>(new TextBox(textBoxWidth, textBoxCharWidth,
																						LayoutPreset(controlCurrPreset),
																						controlName, "",
																						controlPosition, 
																						controlTextSize,
																						controlHasBackground));
				textBox->Init("../data/fonts/AGENCYR.TTF", 
							  windowWidth, windowHeight);

				layouts[toLayout]->AddControl(textBox);
			}
			if(strcmp(tag, "label") == 0)
			{
				line.erase(0, 5);
				line[0] = ' ';
				sscanf(line.c_str(), "%s %s %i %i %i %f %f %i", &controlName,
																&controlText,																
																&controlHasBackground,	
																&toLayout,															
																&controlCurrPreset,
																&controlPosition.x,
																&controlPosition.y,
																&controlTextSize);

				std::shared_ptr<Label> label = std::shared_ptr<Label>(new Label(LayoutPreset(controlCurrPreset),
																				controlName, controlText,
																				controlPosition,
																				controlTextSize,
																				controlHasBackground));
				label->Init("../data/fonts/AGENCYR.TTF", 
							windowWidth, windowHeight);

				layouts[toLayout]->AddControl(label);

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
	// TODO: Add error check.

	return layouts[layoutType];

	//std::printf("No such layout! in GUILoader::GetLayout");
}