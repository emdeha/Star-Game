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

	LAYOUT_SAVE_GAME,
	LAYOUT_NEW_GAME,
	LAYOUT_LOAD_GAME,
	LAYOUT_OPTIONS,

	LAYOUT_SUB_NEW_GAME,
	LAYOUT_SUB_SAVE_GAME,
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
	glm::vec4 backgroundColor;
};

class Button;
class Label;
class TextBox;

class Layout
{
private:
	LayoutType layoutType;
	LayoutInfo layoutInfo;

	std::vector<std::shared_ptr<Button>> buttonControls;
	std::vector<std::shared_ptr<Label>> labelControls;
	std::vector<std::shared_ptr<TextBox>> textBoxControls;
	std::vector<std::shared_ptr<Layout>> subLayouts;

	bool isSet;

public:
	Layout();
	Layout(LayoutType newLayoutType, LayoutInfo newLayoutInfo);

	void AddButtonControl(std::shared_ptr<Button> newButtonControl);
	void AddLabelControl(std::shared_ptr<Label> newLabelControl);
	void AddTextBoxControl(std::shared_ptr<TextBox> newTextBoxControl);
	void AddSubLayout(std::shared_ptr<Layout> newSubLayout);

	std::shared_ptr<Button> GetButtonControl(const std::string &buttonName);
	std::shared_ptr<TextBox> GetTextBoxControl(const std::string &textBoxName);

	void Draw(const FontProgData &fontData, const SimpleProgData &simpleData);
	void Update(int windowWidth, int windowHeight);

	LayoutType GetLayoutType();

	bool IsSet();
	void Set(bool newIsSet);

	void SetCurrentPreset(LayoutPreset newCurrentPreset);


	std::shared_ptr<TextBox> GetActiveTextBox();
	bool HasActiveTextBox();
};


struct PresetAttributes
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
	LayoutPreset currentPreset;
	PresetAttributes presets[3];


	Text textTitle;
	std::string name;
	std::string title;	


	glm::vec4 backgroundColor;
	

	float textMinHeight;
	float textMaxHeight;
	float textMinWidth;
	float textMaxWidth;


	int windowWidth;
	int windowHeight;
		
	bool hasBackground;


	GLfloat bufferData[24];
	GLuint vbo;
	GLuint vao;

public:
	Button();

	Button(const std::string &newName, const std::string &newTitle, 
		   glm::vec2 newPosition,
		   int newTextSize,
		   int newWindowWidth, int newWindowHeight,
		   LayoutPreset newCurrentPreset,
		   bool newHasBackground,
		   glm::vec4 newBackgroundColor = glm::vec4());

	Button(const std::string &newName, const std::string &newTitle,
		   glm::vec2 newPosition,
		   float buttonTopMargin_percent, float buttonBottomMargin_percent,
		   float buttonLeftMargin_percent, float buttonRightMargin_percent,
		   int newTextSize, 
		   int newWindowWidth, int newWindowHeight,
		   LayoutPreset newCurrentPreset,
		   bool newHasBackground,
		   glm::vec4 newBackgroundColor = glm::vec4());

	void Init(const std::string &fontName,
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
	void SetBackgroundColor(glm::vec4 newBackgroundColor);
	void SetFont(bool isBold);

	void AddPreset(LayoutPreset newPreset,
				   float newBottomTextMargin_percent, 
				   float newTopTextMargin_percent,
				   float newRightTextMargin_percent,
				   float newLeftTextMargin_percent,
				   int newTextSize,
				   glm::vec2 newPosition);
	void SetPreset(LayoutPreset newCurrentPreset);
};


class Label
{
private:
	LayoutPreset currentPreset;
	PresetAttributes presets[3];

	Text labelText;
	std::string name;
	std::string text;


	glm::vec4 backgroundColor;


	float textMinHeight;
	float textMaxHeight;
	float textMinWidth;
	float textMaxWidth;


	int windowWidth;
	int windowHeight;

	bool hasBackground;

	GLfloat bufferData[24];
	GLuint vbo;
	GLuint vao;

public:
	Label();

	Label(const std::string &newName, const std::string &newText,
		  glm::vec2 newPosition,
		  int newTextSize,
		  int newWindowWidth, int newWindowHeight,
		  LayoutPreset newCurrentPreset,
		  bool newHasBackground);
	Label(const std::string &newName, const std::string &newText,
		  glm::vec2 newPosition,
		  int newTextSize,
		  float labelTopMargin_percent, float labelBottomMargin_percent,
		  float labelLeftMargin_percent, float labelRightMargin_percent,
		  int newWindowWidth, int newWindowHeight,
		  LayoutPreset newCurrentPreset,
		  bool newHasBackground);


	void Init(const std::string &fontName,
			  int newWindowWidth, int newWindowHeight);

	void Draw(const FontProgData &fontData, const SimpleProgData &simpleData);
	void Update(int newWindowWidth, int newWindowHeight);


	void AddPreset(LayoutPreset newPreset,
				   float newBottomTextMargin_percent, 
				   float newTopTextMargin_percent,
				   float newRightTextMargin_percent,
				   float newLeftTextMargin_percent,
				   int newTextSize,
				   glm::vec2 newPosition);
	void SetPreset(LayoutPreset newCurrentPreset);


	
	void ComputeNewAttributes();
};



class TextBox
{
private:
	LayoutPreset currentPreset;
	PresetAttributes presets[3];

	Text text;

	std::string name;
	std::string inputText;

	std::string visibleText;

	float maxHeight;
	float maxWidth;

	int maxWidthChars;

	int windowWidth;
	int windowHeight;

	bool hasBackground;
	bool isHandlingInput;

	GLfloat bufferData[24];
	GLuint vbo;
	GLuint vao;

public:
	TextBox();

	TextBox(const std::string &newName,
			glm::vec2 newPosition, int newTextSize,
			float width, int newMaxWidthChars,
			int newWindowWidth, int newWindowHeight,
			LayoutPreset newCurrentPreset,
			bool newHasBackground);


	void Init(const std::string &fontName,
		      int newWindowWidth, int newWindowHeight);

	void Draw(const FontProgData &fontData, const SimpleProgData &simpleData);
	void Update(int newWindowWidth, int newWindowHeight);



	void AddPreset(LayoutPreset newPreset,
				   int newTextSize,
				   glm::vec2 newPosition);
	void SetPreset(LayoutPreset newCurrentPreset);

	void InputChar(char ch);
	void Clear();


	
	void ComputeNewAttributes();


	void OnEvent(Event &_event);

	bool IsMouseOn(glm::vec2 mouseCoordinates);

	bool IsHandlingInput()
	{
		return isHandlingInput;
	}


	std::string GetName()
	{
		return name;
	}
	std::string GetContent()
	{
		return inputText;
	}

	void SetInputText(std::string newInputText)
	{
		inputText = newInputText;
	}
};

#endif