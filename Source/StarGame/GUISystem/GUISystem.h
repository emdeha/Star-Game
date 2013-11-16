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

#include "../glsdk/glm/glm.hpp"

#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glutil/glutil.h"

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
	LAYOUT_ALL,
};

enum LayoutPreset
{
	SMALL,
	MEDIUM,
	BIG,

	PRESETS_COUNT,
};

class TextControl;
class Control;
class Scene;

// The window dimensions should later be bound only to the 'Layout' class.

class Layout
{
private:
	typedef void (*EventHandlerFunction)(Scene &, Control *);
	std::vector<std::pair<std::string, EventHandlerFunction>> eventHandlers;

	LayoutType layoutType;

	Utility::Primitives::Sprite backgroundSprite;
	glm::vec4 backgroundColor;

	std::vector<std::shared_ptr<Control>> controls;
	std::vector<std::shared_ptr<Layout>> subLayouts;

	bool isSet;
	bool hasBackground;

public:
	Layout();
	Layout(LayoutType newLayoutType, glm::vec4 newBackgroundColor);


	//void AddControl(const std::shared_ptr<TextControl> newControl);
	void AddControl(const std::shared_ptr<Control> newControl);
	void AddSubLayout(const std::shared_ptr<Layout> newSubLayout);
	void AddEventHandler(const std::string &name, const std::string &eventType, 
						 const std::string &controlName, const std::string &controlType,
						 EventHandlerFunction handler, char key = ' ');
	void CallEventHandler(const std::string &name, Scene &scene, Control *control);

	void SetBackgroundImage(float width, float height, 
							const std::string &backgroundImageFileName);

	std::shared_ptr<Control> GetControl(const std::string &controlName);
	std::vector<std::shared_ptr<Control>>& GetControls();

	void Draw(); 
	void Update(int windowWidth, int windowHeight);

	void HandleClickedControls(bool isRightButtonclicked,
							   glm::ivec2 mouseCoordinates_windowSpace,
							   Scene &scene);
	void HandleHoveredControls(glm::ivec2 mouseCoordinates_windowSpace, Scene &scene);
	void HandleKeyPress(Scene &scene, char key, const std::string &explicitControlName = "");
	std::shared_ptr<Control> GetClickedControl(glm::ivec2 mouseCoordinates_windowSpace);

	LayoutType GetLayoutType();
	void DeactivateAllControls();

	bool IsSet();
	bool IsControl(const std::string &controlName);
	void Set(bool newIsSet);

	void SetCurrentPreset(LayoutPreset newCurrentPreset);


	std::shared_ptr<Control> GetActiveControl();
	bool HasActiveControl();
};

struct HoveredProperties
{
	std::string font;
	std::string text;
	std::string backgroundLeftImage;
	std::string backgroundRightImage;
	std::string backgroundMiddleImage;

	glm::vec4 textColor;
};

class Control
{
protected:
	Text textToDisplay;

	std::string name;
	std::string text;
	std::string visibleText;
	std::string fontName;

	std::string bckgLeftImage;
	std::string bckgRightImage;
	std::string bckgMiddleImage;

	std::string onClickEventName;
	std::string onHoverEventName;
	std::vector<std::string> onKeyPressEventName;
	// add other event names

	glm::vec4 fontColor;
	glm::vec4 margins; // margins; x - bottom, y - top, z - left, w - right
	glm::vec2 position;
	glm::vec2 percentagedPosition;
	int textSize;

	bool isVisible;
	bool isActive;
	bool isUsingPercentage;
	bool hasBackground;
	bool isHovered;
	bool hasHoveredSoon;
	bool hasUnhoveredSoon;

	Utility::Primitives::ComplexSprite controlBackground;
	Utility::Primitives::Square controlSquare; // TODO: Replace with rectangle geometry primitive

	int windowWidth;
	int windowHeight;

	HoveredProperties onHoverProps;

public:
	Control();

	Control(const std::string &newName, const std::string &newText,
			glm::vec4 newFontColor, glm::vec2 newPosition, glm::vec4 newMargins,
			int newTextSize,
			bool newHasBackground, bool newIsVisible, bool newIsUsingPercentage,
			HoveredProperties newOnHoverProps,
			glm::vec2 newPercentagedPosition = glm::vec2());

	void Init(const std::string &newFontName, 
			  const std::string &bckgLeftTextureFileName,
			  const std::string &bckgRightTextureFileName,
			  const std::string &bckgMiddleTextureFileName,
			  int newWindowWidth, int newWindowHeight);

	std::string GetContent();

	bool IsActive();

	void SetIsVisible(bool newIsVisible);
	void SetText(const std::string &newText);
	void SetIsActive(bool newIsActive);

	std::string GetName();

	bool IsMouseOn(glm::ivec2 mouseCoordinates_windowSpace);

	void ClearContent();

	void SubscribeForEvent(const std::string &eventName);
	void SubscribeForHoverEvent(const std::string &eventName);
	void SubscribeForKeyPressEvent(const std::string &eventName, char key);
	std::string GetSubscribedEvent();
	std::string GetSubscribedHoverEvent();
	std::string GetSubscribedKeyPressEvent(char key);

	void SetIsHovered(bool newIsHovered);


	virtual void Draw(const FontProgData &fontData, const TextureProgData &textureData);

	virtual void ComputeNewAttributes();
	virtual void Update(int newWindowWidth, int newWindowHeight);

	virtual bool IsImageBox() { return false; }

	// maybe it is not necessary to be virtual
	virtual void SetPosition(glm::vec2 newPosition, bool newIsUsingPercentage); 

	// TODO: Only for ImageBox, maybe there's a better design decision
	virtual void SetTexture(const std::string &textureName) { }

	virtual std::string GetType() { return ""; }

	virtual void InputChar(char ch) {}
};


class Button : public Control
{
public: 
	Button() : Control() {}

	Button(const std::string &newName, const std::string &newText,
		   glm::vec4 newFontColor, glm::vec2 newPosition, glm::vec4 newMargins,
		   int newTextSize,
		   bool newHasBackground, bool newIsVisible, bool newIsUsingPercentage,
		   HoveredProperties newOnHoverProps,
		   glm::vec2 newPercentagedPosition = glm::vec2()) 
		   : Control(newName, newText,
					 newFontColor, newPosition, newMargins,
					 newTextSize, 
					 newHasBackground, newIsVisible, newIsUsingPercentage,
				     newOnHoverProps,
					 newPercentagedPosition) {};

	std::string GetType();
};


class Label : public Control
{
public:
	Label() : Control() {}

	Label(const std::string &newName, const std::string &newText,
		  glm::vec4 newFontColor, glm::vec2 newPosition, glm::vec4 newMargins,
		  int newTextSize,
		  bool newHasBackground, bool newIsVisible, bool newIsUsingPercentage,
		  HoveredProperties newOnHoverProps,
		  glm::vec2 newPercentagedPosition = glm::vec2()) 
		   : Control(newName, newText,
					 newFontColor, newPosition, newMargins,
					 newTextSize, 
					 newHasBackground, newIsVisible, newIsUsingPercentage,
					 newOnHoverProps,
					 newPercentagedPosition) {};

	std::string GetType();
};

class TextBox : public Control
{
private:
	float maxWidth;

public:
	TextBox() : Control()
	{
	}
	TextBox(float newMaxWidth,
		    const std::string &newName, const std::string &newText,
		    glm::vec4 newFontColor, glm::vec2 newPosition, glm::vec4 newMargins,
		    int newTextSize,
		    bool newHasBackground, bool newIsVisible, bool newIsUsingPercentage,
			HoveredProperties newOnHoverProps,
		    glm::vec2 newPercentagedPosition = glm::vec2()) 
		    : Control(newName, newText,
					  newFontColor, newPosition, newMargins,
					  newTextSize, 
					  newHasBackground, newIsVisible, newIsUsingPercentage,
					  newOnHoverProps,
					  newPercentagedPosition) 
	{
		maxWidth = newMaxWidth;
		controlSquare.SetWidth(maxWidth);
	}

	void ComputeNewAttributes();

	void InputChar(char ch);
	std::string GetType();
};

class ImageBox : public Control
{
private:
	float width;
	float height;

	Utility::Primitives::Sprite image;

public:
	ImageBox() : Control()
	{
		width = 0;
		height = 0;
	}
	ImageBox(float newWidth, float newHeight,
			 const std::string &newName, const std::string &newText,
		     glm::vec4 newFontColor, glm::vec2 newPosition, glm::vec4 newMargins,
		     int newTextSize,
		     bool newHasBackground, bool newIsVisible, bool newIsUsingPercentage,
			 HoveredProperties newOnHoverProps,
		     glm::vec2 newPercentagedPosition = glm::vec2()) 
		     : Control(newName, newText,
					   newFontColor, newPosition, newMargins,
					   newTextSize, 
					   newHasBackground, newIsVisible, newIsUsingPercentage,
					   newOnHoverProps,
					   newPercentagedPosition) 
	{
		width = newWidth;
		height = newHeight;
	}

	void Init(const std::string &imageFileName, 
			  int windowWidth, int windowHeight);

	void ComputeNewAttributes();

	void Update(int newWindowWidth, int newWindowHeight);
	// Adding the font data for later usage.
	void Draw(const FontProgData &fontData, const TextureProgData &textureData);

	void SetTexture(const std::string &textureName);

	std::string GetType();
};


#endif