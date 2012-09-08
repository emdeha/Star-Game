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


#ifndef GUI_SYSTEM_H
#define GUI_SYSTEM_H

#include <vector>
#include <string>

#include <glm\glm.hpp>

#include <glload/gl_3_3.h>
#include <glutil/glutil.h>

#include "../framework/framework.h"
#include "../framework/Utility.h"
#include "../framework/Text.h"
#include "../framework/EventSystem.h"

#include "../ProgramData/ProgramData.h"

enum LayoutType
{
	LAYOUT_MENU,
	LAYOUT_IN_GAME,

	LAYOUT_SUB_NEW_GAME,
	LAYOUT_SUB_LOAD_GAME,
	LAYOUT_SUB_OPTIONS,
};

enum LayoutPreset
{
	SMALL,
	MEDIUM,
	BIG,

	PRESETS_COUNT,
};

struct LayoutInfo
{
	int textSize;
	
	glm::vec4 backgroundColor;
};

class Button;

class Layout
{
private:
	LayoutType layoutType;
	LayoutInfo layoutInfo;

	std::vector<Button> buttonControls;
	std::vector<Layout> subLayouts;

	bool isSet;

public:
	Layout();
	Layout(LayoutType newLayoutType, LayoutInfo newLayoutInfo);

	void AddButtonControl(const Button &newButtonControl);
	void AddSubLayout(const Layout &newSubLayout);

	Button GetButtonControl(const std::string &buttonName);

	void Draw(const FontProgData &fontData, const SimpleProgData &simpleData);
	void Update(int windowWidth, int windowHeight);

	LayoutType GetLayoutType();

	bool IsSet();
	void Set(bool newIsSet);

	void SetCurrentPreset(LayoutPreset newCurrentPreset);
};


struct ButtonPresetAttributes
{
	glm::vec2 position;

	float topTextMargin_percent;
	float bottomTextMargin_percent;
	float leftTextMargin_percent;
	float rightTextMargin_percent;

	float topTextMargin;
	float bottomTextMargin;
	float leftTextMargin;
	float rightTextMargin;

	int textSize;
};

// The window dimensions should later be bound only to the 'Layout' class.

class Button
{
private:
	Text textTitle;

	GLfloat bufferData[24];

	std::string name;
	std::string title;
	
	
	float textMinHeight;
	float textMaxHeight;
	float textMinWidth;
	float textMaxWidth;


	LayoutPreset currentPreset;

	ButtonPresetAttributes presets[3];


	int windowWidth;
	int windowHeight;

	GLuint vbo;
	GLuint vao;

public:
	Button();


	// Maybe this constructor is not needed.
	/*Button(const std::string &newName, const std::string &newTitle,
		   glm::vec2 newPosition,
		   float newTopTextMargin, float newBottomTextMargin, 
		   float newRightTextMargin, float newLeftTextMargin,
		   int newTextSize);*/


	Button(const std::string &newName, const std::string &newTitle,
		   glm::vec2 newPosition,
		   float buttonTopMargin_percent, float buttonBottomMargin_percent,
		   float buttonLeftMargin_percent, float buttonRightMargin_percent,
		   int newTextSize, 
		   int newWindowWidth, int newWindowHeight,
		   LayoutPreset newCurrentPreset);

	void Init(const std::string fontName,
			  int newWindowWidth, int newWindowHeight);

	bool IsMouseOn(glm::vec2 mouseCoordinates);

	void OnEvent(Event &_event);

	void Draw(const FontProgData &fontData, const SimpleProgData &simpleData);
	void Update(int newWindowWidth, int newWindowHeight);

	void ComputeNewAttributes();


	std::string GetName();

	void SetTextSize(int newTextSize);
	void SetNewMargins(float buttonTopMargin_percent, float buttonBottomMargin_percent,
					   float buttonLeftMargin_percent, float buttonRightMargin_percent);
	void SetNewPosition(glm::vec2 newPosition);

	void AddPreset(LayoutPreset newPreset,
				   float newBottomTextMargin_percent, 
				   float newTopTextMargin_percent,
				   float newRightTextMargin_percent,
				   float newLeftTextMargin_percent,
				   int newTextSize,
				   glm::vec2 newPosition);
	void SetPreset(LayoutPreset newCurrentPreset);
};

#endif