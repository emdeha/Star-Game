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

	LAYOUT_OTHER,
	/*LAYOUT_SUB_NEW_GAME,
	LAYOUT_SUB_SAVE_GAME,
	LAYOUT_SUB_LOAD_GAME,
	LAYOUT_SUB_OPTIONS,*/
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

/*class Button;
class Label;
class TextBox;*/

class TextControl;

// The window dimensions should later be bound only to the 'Layout' class.

class Layout
{
private:
	LayoutType layoutType;
	LayoutInfo layoutInfo;

	//std::vector<std::shared_ptr<Button>> buttonControls;
	//std::vector<std::shared_ptr<Label>> labelControls;
	//std::vector<std::shared_ptr<TextBox>> textBoxControls;

	std::vector<std::shared_ptr<TextControl>> controls;
	std::vector<std::shared_ptr<Layout>> subLayouts;

	bool isSet;

public:
	Layout();
	Layout(LayoutType newLayoutType, LayoutInfo newLayoutInfo);

	//void AddButtonControl(std::shared_ptr<Button> newButtonControl);
	//void AddLabelControl(std::shared_ptr<Label> newLabelControl);
	//void AddTextBoxControl(std::shared_ptr<TextBox> newTextBoxControl);

	void AddControl(std::shared_ptr<TextControl> newControl);
	void AddSubLayout(std::shared_ptr<Layout> newSubLayout);

	//std::shared_ptr<Button> GetButtonControl(const std::string &buttonName);
	//std::shared_ptr<TextBox> GetTextBoxControl(const std::string &textBoxName);

	std::shared_ptr<TextControl> GetControl(const std::string &controlName);

	void Draw(const FontProgData &fontData, const SimpleProgData &simpleData);
	void Update(int windowWidth, int windowHeight);

	LayoutType GetLayoutType();

	bool IsSet();
	void Set(bool newIsSet);

	void SetCurrentPreset(LayoutPreset newCurrentPreset);

	
	std::shared_ptr<TextControl> GetActiveControl();
	bool HasActiveControl();
	//std::shared_ptr<TextBox> GetActiveTextBox();
	//bool HasActiveTextBox();
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


class ImageBox
{
private:
	LayoutPreset currentPreset;
	glm::vec2 presetPosition[3];

	float width; 
	float height;
	
	std::string name;

	bool isActive;

	GLfloat bufferData[18];
	GLuint vbo;
	GLuint vao;

	GLuint imageID;

public:
	ImageBox();
	ImageBox(LayoutPreset newCurrentPreset,
			 const std::string &newName, 
			 glm::vec2 newPosition,
			 float newWidth, float newHeight);

	void Init();

public:
	void Draw(const SimpleProgData &simpleData);
};


class TextControl
{
protected:
	LayoutPreset currentPreset;
	PresetAttributes presets[3];

	Text textToDisplay;

	std::string name;
	std::string text;

	int windowWidth;
	int windowHeight;

	
	glm::vec2 boxMinCorner;
	glm::vec2 boxMaxCorner;

	
	bool hasBackground;
	bool isActive;


	GLfloat bufferData[24];
	GLuint vbo;
	GLuint vao;

public:
	TextControl();

	TextControl(LayoutPreset newCurrentPreset,
				const std::string &newName, const std::string &newText,
				glm::vec2 newPosition, int newTextSize,
				bool newHasBackground);

	void Init(const std::string &fontName, 
			  int newWindowWidth, int newWindowHeight);
		
	virtual void Draw(const FontProgData &fontData, const SimpleProgData &simpleData);

	virtual void ComputeNewAttributes();
	virtual void Update(int newWindowWidth, int newWindowHeight);

	virtual void OnEvent(Event &_event) = 0;

	virtual bool IsMouseOn(glm::vec2 mouseCoordinates_windowSpace);

	virtual void InputChar(char ch) {}
	virtual void Clear() {}
	virtual std::string GetContent() { return ""; }

	bool IsActive();

public:
	std::string GetName();

	void AddPreset(LayoutPreset newPreset,
				   int newTextSize,
				   glm::vec2 newPosition,
				   float newBottomTextMargin_percent = 0.0f, 
				   float newTopTextMargin_percent = 0.0f,
				   float newRightTextMargin_percent = 0.0f,
				   float newLeftTextMargin_percent = 0.0f);
	void SetPreset(LayoutPreset newCurrentPreset);
};


class Button : public TextControl
{
public:
	Button() : TextControl() {}

	Button(LayoutPreset newCurrentPreset,
		   const std::string &newName, const std::string &newText,
		   glm::vec2 newPosition, int newTextSize,
		   bool newHasBackground) 
		   : TextControl(newCurrentPreset, 
						 newName, newText,
						 newPosition, newTextSize,
						 newHasBackground) {};

	void OnEvent(Event &_event);
};


class Label : public TextControl
{
public:
	Label() : TextControl() {}

	Label(LayoutPreset newCurrentPreset,
		  const std::string &newName, const std::string &newText,
		  glm::vec2 newPosition, int newTextSize,
		  bool newHasBackground) 
		  : TextControl(newCurrentPreset, 
						newName, newText,
						newPosition, newTextSize,
						newHasBackground) {};

	void OnEvent(Event &_event);
};


class TextBox : public TextControl
{
private:
	std::string visibleText;

	int maxNumberChars;

	//bool isActive;

public:
	TextBox() : TextControl() 
	{
		visibleText = "";
	}

	TextBox(float maxWidth, int newMaxNumberChars,
			LayoutPreset newCurrentPreset,
		    const std::string &newName, const std::string &newText,
		    glm::vec2 newPosition, int newTextSize,
		    bool newHasBackground) 
		    : TextControl(newCurrentPreset, 
						  newName, newText,
						  newPosition, newTextSize,
						  newHasBackground)
	{
		visibleText = "";

		boxMaxCorner.x = maxWidth;
		maxNumberChars = newMaxNumberChars;
		//isActive = false;
	}

	
	void Draw(const FontProgData &fontData, const SimpleProgData &simpleData);

	void ComputeNewAttributes();
	void Update(int newWindowWidth, int newWindowHeight);

	void InputChar(char ch);
	void Clear();

	void OnEvent(Event &_event);

	std::string GetContent();
	void SetContent(std::string newInputText);

	bool IsMouseOn(glm::vec2 mouseCoordinates_windowSpace);
};

#endif