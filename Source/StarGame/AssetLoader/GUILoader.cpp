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
	

	LayoutType layoutType;
	glm::vec4 layoutColor;
	int layoutIsSet;

	std::string controlType;
	std::string controlName;
	std::string controlText;
	int controlHasBackground;
	LayoutPreset controlCurrPreset;
	PresetAttributes controlPresets[3];
	LayoutType toLayout;


	if(data.is_open())
	{	
		while(!data.eof())
		{
			getline(data, line);
			char tag[30];
			sscanf_s(line.c_str(), "%s ", &tag);

			if(strcmp(tag, "layout") == 0)
			{
				getline(data, line);
				sscanf_s(line.c_str(), "%s ", &tag);
			
				if(strcmp(tag, "type") == 0)
				{
					line.erase(0, 4);
					line[0] = ' ';
					sscanf_s(line.c_str(), "%i ", &layoutType);
					getline(data, line);
					sscanf_s(line.c_str(), "%s ", &tag);
				}
				if(strcmp(tag, "color") == 0)
				{
					line.erase(0, 5);
					line[0] = ' ';
					sscanf_s(line.c_str(), "%f %f %f %f ", &layoutColor.x,
														   &layoutColor.y,
														   &layoutColor.z,
														   &layoutColor.w);
					getline(data, line);
					sscanf_s(line.c_str(), "%s ", &tag);
				}
				if(strcmp(tag, "isset") == 0)
				{
					line.erase(0, 5);
					line[0] = ' ';
					sscanf_s(line.c_str(), "%i ", &layoutIsSet);
					getline(data, line);
					sscanf_s(line.c_str(), "%s ", &tag);
				}
				if(strcmp(tag, "end") == 0)
				{
					break;
				}
			}
			if(strcmp(tag, "control") == 0)
			{
				getline(data, line);
			
				if(strcmp(tag, "type") == 0)
				{
					line.erase(0, 4);
					line[0] = ' ';
					sscanf_s(line.c_str(), "%s ", &controlType);
					getline(data, line);
					sscanf_s(line.c_str(), "%s ", &tag);
				}
				if(strcmp(tag, "name") == 0)
				{
					line.erase(0, 4);
					line[0] = ' ';
					sscanf_s(line.c_str(), "%s ", &controlName);
					getline(data, line);
					sscanf_s(line.c_str(), "%s ", &tag);
				}
				if(strcmp(tag, "text") == 0)
				{
					line.erase(0, 4);
					line[0] = ' ';
					sscanf_s(line.c_str(), "%s ", &controlText);
					getline(data, line);
					sscanf_s(line.c_str(), "%s ", &tag);
				}
				if(strcmp(tag, "hasbckg") == 0)
				{
					line.erase(0, 7);
					line[0] = ' ';
					sscanf_s(line.c_str(), "%i ", &controlHasBackground);
					getline(data, line);
					sscanf_s(line.c_str(), "%s ", &tag);
					if(controlHasBackground == 1)
					{
						// add implementation for color getting
					}
				}
				if(strcmp(tag, "preset") == 0)
				{
					line.erase(0, 6);
					line[0] = ' ';
					sscanf_s(line.c_str(), "%i ", &controlCurrPreset);
					getline(data, line);
					switch(controlCurrPreset)
					{
					case SMALL:
						sscanf_s(line.c_str(), "%s ", &tag);
						if(strcmp(tag, "position") == 0)
						{
							line.erase(0, 8);
							line[0] = ' ';
							sscanf_s(line.c_str(), "%f %f ", &controlPresets[SMALL].position.x,
															 &controlPresets[SMALL].position.y);
							getline(data, line);
							sscanf_s(line.c_str(), "%s ", &tag);
						}
						if(strcmp(tag, "textsize") == 0)
						{
							line.erase(0, 8);
							line[0] = ' ';
							sscanf_s(line.c_str(), "%i ", &controlPresets[SMALL].textSize);

							getline(data, line);
							sscanf_s(line.c_str(), "%s ", &tag);
						}
						if(strcmp(tag, "end") == 0)
						{
							break;
						}
					case MEDIUM:
						sscanf_s(line.c_str(), "%s ", &tag);
						if(strcmp(tag, "position") == 0)
						{
							line.erase(0, 8);
							line[0] = ' ';
							sscanf_s(line.c_str(), "%f %f ", &controlPresets[MEDIUM].position.x,
															 &controlPresets[MEDIUM].position.y);
							getline(data, line);
							sscanf_s(line.c_str(), "%s ", &tag);
						}
						if(strcmp(tag, "textsize") == 0)
						{
							line.erase(0, 8);
							line[0] = ' ';
							sscanf_s(line.c_str(), "%i ", &controlPresets[MEDIUM].textSize);

							getline(data, line);
							sscanf_s(line.c_str(), "%s ", &tag);
						}
						if(strcmp(tag, "end") == 0)
						{
							break;
						}
					case BIG:
						sscanf_s(line.c_str(), "%s ", &tag);
						if(strcmp(tag, "position") == 0)
						{
							line.erase(0, 8);
							line[0] = ' ';
							sscanf_s(line.c_str(), "%f %f ", &controlPresets[BIG].position.x,
															 &controlPresets[BIG].position.y);
							getline(data, line);
							sscanf_s(line.c_str(), "%s ", &tag);
						}
						if(strcmp(tag, "textsize") == 0)
						{
							line.erase(0, 8);
							line[0] = ' ';
							sscanf_s(line.c_str(), "%i ", &controlPresets[BIG].textSize);

							getline(data, line);
							sscanf_s(line.c_str(), "%s ", &tag);
						}
						if(strcmp(tag, "end") == 0)
						{
							break;
						}
					}
				}
				if(strcmp(tag, "tolayout") == 0)
				{
					line.erase(0, 8);
					line[0] = ' ';
					sscanf_s(line.c_str(), "%i ", &toLayout);

					getline(data, line);
					sscanf_s(line.c_str(), "%s ", &tag);
				}
				if(strcmp(tag, "end") == 0)
				{
					break;
				}
			}
			if(strcmp(tag, "endfile") == 0)
			{

			}
		}
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
	for(std::vector<std::shared_ptr<Layout>>::iterator iter = layouts.begin();
		iter != layouts.end(); ++iter)
	{
		if((*iter)->GetLayoutType() == layoutType)
		{
			return (*iter);
		}
	}

	std::printf("No such layout! in GUILoader::GetLayout");
}