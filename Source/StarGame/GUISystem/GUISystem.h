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
#include "../AssetLoader/Texture.h"



enum LayoutType
{
	LAYOUT_MENU,
	LAYOUT_IN_GAME,

	LAYOUT_SAVE_GAME,
	LAYOUT_NEW_GAME,
	LAYOUT_LOAD_GAME,
	LAYOUT_OPTIONS,

	LAYOUT_OTHER,
};

enum LayoutPreset
{
	SMALL,
	MEDIUM,
	BIG,

	PRESETS_COUNT,
};

class TextControl;

// The window dimensions should later be bound only to the 'Layout' class.

class Layout
{
private:
	LayoutType layoutType;

	Utility::Primitives::Sprite backgroundSprite;
	glm::vec4 backgroundColor;

	std::vector<std::shared_ptr<TextControl>> controls;
	std::vector<std::shared_ptr<Layout>> subLayouts;

	bool isSet;
	bool hasBackground;

public:
	Layout();
	Layout(LayoutType newLayoutType, glm::vec4 newBackgroundColor);


	void AddControl(const std::shared_ptr<TextControl> newControl);
	void AddSubLayout(const std::shared_ptr<Layout> newSubLayout);

	void SetBackgroundImage(float width, float height, 
							const std::string &backgroundImageFileName);

	std::shared_ptr<TextControl> GetControl(const std::string &controlName);

	void Draw(const FontProgData &fontData, 
			  const SimpleProgData &simpleData,
			  const TextureProgData &textureData);
	void Update(int windowWidth, int windowHeight);

	LayoutType GetLayoutType();

	bool IsSet();
	bool IsControl(const std::string &controlName);
	void Set(bool newIsSet);

	void SetCurrentPreset(LayoutPreset newCurrentPreset);

	
	std::shared_ptr<TextControl> GetActiveControl();
	bool HasActiveControl();
};


class Control
{
protected:
	Text textToDisplay;

	std::string name;
	std::string text;

	glm::vec4 fontColor;
	glm::vec4 margins; // margins; x - bottom, y - top, z - left, w - right
	glm::vec2 position;
	glm::vec2 textPosition;
	glm::vec2 percentagedPosition;
	int textSize;

	bool isVisible;
	bool isActive;
	bool isUsingPercentage;
	bool hasBackground;

	Utility::Primitives::Sprite controlBackground;
	Utility::Primitives::Square controlSquare; // TODO: Replace with rectangle geometry primitive

	int windowWidth;
	int windowHeight;

public:
	Control();

	Control(const std::string &newName, const std::string &newText,
			glm::vec4 newFontColor, glm::vec2 newPosition, glm::vec4 newMargins,
			int newTextSize,
			bool newHasBackground, bool newIsVisible, bool newIsUsingPercentage,
			glm::vec2 newPercentagedPosition = glm::vec2());

	void Init(const std::string &fontName, const std::string &bckgTextureFileName,
			  int newWindowWidth, int newWindowHeight);

	std::string GetContent();

	bool IsActive();
	void Deactivate();

	void SetIsVisible(bool newIsVisible);
	void SetText(const std::string &newText);

	std::string GetName();


	virtual void Draw(const FontProgData &fontData, const TextureProgData &textureData);

	virtual void ComputeNewAttributes();
	virtual void Update(int newWindowWidth, int newWindowHeight);

	virtual bool IsImageBox() { return false; }

	// maybe it is not necessary to be virtual
	virtual void SetPosition(glm::vec2 newPosition, bool newIsUsingPercentage); 
};

































struct PresetAttributes
{
	glm::vec2 position;
	int textSize;

	float topTextMargin_percent;
	float bottomTextMargin_percent;
	float leftTextMargin_percent;
	float rightTextMargin_percent;

	float topTextMargin;
	float bottomTextMargin;
	float leftTextMargin;
	float rightTextMargin;
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

	bool isVisible;
	
	Utility::Primitives::Square controlSquare;

	
	bool hasBackground;
	bool isActive;

public:
	TextControl(); //v

	TextControl(LayoutPreset newCurrentPreset,
				const std::string &newName, const std::string &newText,
				glm::vec2 newPosition, int newTextSize,
				bool newHasBackground, bool newIsVisible); //v

	TextControl(LayoutPreset newCurrentPreset,
				const std::string &newName, 
				glm::vec2 newPosition, bool newIsVisible); //v

	void Init(const std::string &fontName, 
			  int newWindowWidth, int newWindowHeight); //v
	virtual void Init() {}; //v

	virtual void Draw(const FontProgData &fontData, const SimpleProgData &simpleData); //v
	virtual void Draw(const FontProgData &fontData, const TextureProgData &textureData) {}; //v
	virtual void Draw(const TextureProgData &textureData) {}; //v

	virtual void ComputeNewAttributes(); //v
	virtual void Update(int newWindowWidth, int newWindowHeight); //v

	virtual void OnEvent(Event &_event) = 0;

	virtual bool IsMouseOn(glm::vec2 mouseCoordinates_windowSpace);

	virtual void InputChar(char ch) {}
	virtual void Clear() {}
	virtual std::string GetContent() { return ""; }
	virtual void SetPosition(glm::vec2 newPosition); //v
	virtual void SetIsVisible(bool newIsVisible); //v
	virtual void ChangeText(const std::string &newText) { text = newText; } //v

	virtual bool IsImageBox() { return false; } //v
	virtual bool IsHintBox() { return false; } //v
	bool IsActive(); //v
	void Deactivate(); //v

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


class ImageBox : public TextControl
{
private:
	LayoutPreset currentPreset;
	glm::vec2 presetPosition[3];

	float width; 
	float height;
	
	int index; // TODO: Only for the fusion box. Should refactor.

	std::string name;

	std::string fusionTextures[4]; // TODO: Only for fusion. Should refactor.

	bool isActive;

	Utility::Primitives::Sprite boxSprite;

public:
	ImageBox() : TextControl() 
	{
		name = "";

		isActive = false;

		index = 0;
	}
	ImageBox(LayoutPreset newCurrentPreset,
			 const std::string &newName, 
			 glm::vec2 newPosition,
			 float newWidth, float newHeight,
			 int newIndex,
			 bool newIsVisible) 
			 : TextControl(newCurrentPreset, newName, newPosition, newIsVisible) 
	{
		width = newWidth;
		height = newHeight;
		name = newName;
		isActive = false;

		index = newIndex;

		boxSprite = 
			Utility::Primitives::Sprite(glm::vec3(newPosition, 0.0f), 
										glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
										newWidth, newHeight, false);
	}

	void SetTextures(std::string textures[]);
	void SetPosition(glm::vec2 newPosition);

	void Init();

	void OnEvent(Event &_event);

	void AddPreset(LayoutPreset newPreset, glm::vec2 newPosition);

	void Update(int newWindowWidth, int newWindowHeight);
	void Draw(const TextureProgData &textureData);

	bool IsImageBox()
	{
		return true;
	}
};


class FusionHint : public TextControl
{
private:
	LayoutPreset currentPreset;
	glm::vec3 presetPosition[3];

	float  width;
	float height;

	std::string name;
	
	std::vector<std::string> skillTextures; 
	std::vector<std::string> skillDescriptions;

	bool isActive;
	bool isVisible;

	Utility::Primitives::Sprite boxSprite;

public:
	FusionHint() : TextControl()
	{
		name = "";
		isActive = false;
		isVisible = true;
	}
	FusionHint(LayoutPreset newCurrentPreset,
			   const std::string &newName,
			   glm::vec2 newPosition, 
			   float newWidth, float newHeight,
			   int textSize, bool newIsVisible)
			   : TextControl(newCurrentPreset, newName, "", newPosition, textSize, false, newIsVisible)
	{
		currentPreset = newCurrentPreset;
		width = newWidth;
		height = newHeight;
		name = newName;
		isActive = false;
		isVisible = true;

		boxSprite = 
			Utility::Primitives::Sprite(glm::vec3(newPosition, 0.0f),
										glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
										newWidth, newHeight, false);
	}

	void SetTextures(std::vector<std::string> textures);
	void SetDescriptions(std::vector<std::string> descriptions);
	void SetPosition(glm::vec2 newPosition);

	void Init();

	void OnEvent(Event &_event);

	void AddPreset(LayoutPreset newPreset, glm::vec2 newPosition);

	void Update(int newWindowWidth, int newWindowHeight);
	void Draw(const FontProgData &fontData, const TextureProgData &textureData);

	bool IsHintBox()
	{
		return true;
	}
};


class Button : public TextControl
{
public:
	Button() : TextControl() {}

	Button(LayoutPreset newCurrentPreset,
		   const std::string &newName, const std::string &newText,
		   glm::vec2 newPosition, int newTextSize,
		   bool newHasBackground, bool newIsVisible) 
		   : TextControl(newCurrentPreset, 
						 newName, newText,
						 newPosition, newTextSize,
						 newHasBackground, newIsVisible) {};

	void OnEvent(Event &_event);
};


class Label : public TextControl
{
public:
	Label() : TextControl() {}

	Label(LayoutPreset newCurrentPreset,
		  const std::string &newName, const std::string &newText,
		  glm::vec2 newPosition, int newTextSize,
		  bool newHasBackground, bool newIsVisible) 
		  : TextControl(newCurrentPreset, 
						newName, newText,
						newPosition, newTextSize,
						newHasBackground, newIsVisible) {};

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
		    bool newHasBackground, bool newIsVisible) 
		    : TextControl(newCurrentPreset, 
						  newName, newText,
						  newPosition, newTextSize,
						  newHasBackground, newIsVisible)
	{
		visibleText = "";

		controlSquare.SetWidth(maxWidth);
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