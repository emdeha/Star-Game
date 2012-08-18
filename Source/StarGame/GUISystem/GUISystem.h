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

struct LayoutInfo
{
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

	void AddButtonControl(const Button &newControl);
	void AddSubLayout(const Layout &newSubLayout);

	void Draw();

	LayoutType GetLayoutType();

	bool IsSet();
	void Set(bool newIsSet);
};

// The window dimensions should later be bound only to the 'Layout' class.

class Button
{
private:
	Text textTitle;

	std::string name;
	std::string title;

	glm::vec2 position;


	float textMinHeight;
	float textMaxHeight;
	float textMinWidth;
	float textMaxWidth;


	float topTextMargin;
	float bottomTextMargin;
	float leftTextMargin;
	float rightTextMargin;

	int textSize;

	int windowWidth;
	int windowHeight;

	GLuint vbo;
	GLuint vao;

public:
	Button();
	Button(const std::string &newName, const std::string &newTitle,
		   glm::vec2 newPosition,
		   float newTopTextMargin, float newBottomTextMargin, 
		   float newRightTextMargin, float newLeftTextMargin,
		   int newTextSize);

	void Init(const std::string fontName,
			  int newWindowWidth, int newWindowHeight);

	bool IsMouseOn(glm::vec2 mouseCoordinates);

	void OnEvent(Event &_event);

	void Draw(FontProgData fontData, SimpleProgData simpleData);
	void Update(int newWindowWidth, int newWindowHeight);
};

#endif