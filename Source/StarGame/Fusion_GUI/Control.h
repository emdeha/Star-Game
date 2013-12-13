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


#ifndef FE_CONTROL_H
#define FE_CONTROL_H


#pragma warning(push, 0)
#include "../glsdk/glm/glm.hpp"
#pragma warning(pop)

#include "../Fusion_EntitySystem/EventManager.h"
#include "../Fusion_Renderer/Sprite.h"
#include "../Fusion_GUI/Text.h"


namespace FusionEngine
{
	enum RelativityOption
	{
		FE_RELATIVE_TOP_LEFT,
		FE_RELATIVE_TOP_RIGHT,
		FE_RELATIVE_BOTTOM_LEFT,
		FE_RELATIVE_BOTTOM_RIGHT,
		FE_RELATIVE_CENTER_BOTTOM,
		FE_RELATIVE_CENTER_TOP,

		FE_RELATIVE_BAD = -1,
	};

	class Control;

	// TODO: Maybe not a good decision
	typedef void (*EventHandlingFunc)(Control*, const IEventData&);

	class Control : public IEventListener
	{
	protected:
		EventHandlingFunc handleOnKeyPress;

		std::string name; // TODO: integer ID in the future

		glm::ivec2 initialPosition;
		glm::ivec2 position;

		RelativityOption currentRelativity;

		unsigned short width;
		unsigned short height;

		unsigned short windowWidth;
		unsigned short windowHeight;

		Sprite2D background;
		bool hasBackground;
		bool isVisible;
		bool isActive;

	public:
		virtual ~Control() {}

		Control(const std::string &newName,
				glm::ivec2 newPosition,
				unsigned short newWidth, unsigned short newHeight,
				unsigned short newWindowWidth, unsigned short newWindowHeight)
			: name(newName), position(newPosition), initialPosition(newPosition),
			  currentRelativity(FE_RELATIVE_TOP_RIGHT), width(newWidth), height(newHeight), 
			  windowWidth(newWindowWidth), windowHeight(newWindowHeight),
			  isVisible(true), hasBackground(false), handleOnKeyPress(nullptr) {}

		virtual void Init(EventManager &eventManager);

		virtual void Draw(glutil::MatrixStack &modelMatrix) const;

		bool IsMouseOn(glm::ivec2 mouseCoordinates_windowSpace) const;

		virtual bool HandleEvent(const IEventData &eventData);

		virtual bool IsTextBox() { return false; }
		virtual bool IsImageBox() { return false; }
		virtual bool IsButton() { return false; }
		virtual bool IsLabel() { return false; }

	public:
		std::string GetName() const;

		void SetVisibility(bool newIsVisible);
		bool IsVisible() const;
		void SetActive(bool newIsActive);
		bool IsActive() const;

		void SetOnKeyPressedHandler(EventHandlingFunc onKeyPressedHandler);
		void SetRelativity(RelativityOption relativeTo);
		void SetBackground(const std::string &backgroundFileName);

		void ChangeBackgroundImage(const std::string &newImageFile);
	};

	
	class TextControl : public Control
	{
	protected:
		EventHandlingFunc handleOnClick;
		
		Text text;

		glm::ivec2 margins;

		std::string textFont;
		std::string textString;
		glm::vec4 textColor;
		glm::vec2 textPosition;
		int textSize;

	public:
		TextControl(const std::string &newName, glm::ivec2 newPosition,
					unsigned short newWidth, unsigned short newHeight,
					unsigned short newWindowWidth, unsigned short newWindowHeight,
					glm::ivec2 newMargins)
				: Control(newName, newPosition, newWidth, newHeight, 
						  newWindowWidth, newWindowHeight),
				  margins(newMargins), handleOnClick(nullptr), 
				  textString(""), textFont(""), textColor(-1.0f), 
				  textPosition(-1.0f), textSize(-1) {}

		void SetOnClickHandler(EventHandlingFunc onClickHandler);
		void SetTextProperties(const std::string &newTextFont, const std::string &newTextString,
							   glm::vec4 newTextColor, unsigned short newTextSize);

		std::string GetText() const;

		virtual void Init(EventManager &eventManager);

		virtual void Draw(glutil::MatrixStack &modelMatrix) const;

		virtual bool HandleEvent(const IEventData &eventData);
	};


	class Label : public TextControl
	{
	public:
		Label(const std::string &newName, glm::ivec2 newPosition,
			  unsigned short newWidth, unsigned short newHeight, 
			  unsigned short newWindowWidth, unsigned short newWindowHeight,
			  glm::ivec2 newMargins)
				: TextControl(newName, newPosition, newWidth, newHeight,
				  newWindowWidth, newWindowHeight, newMargins) {}

		virtual bool HandleEvent(const IEventData &eventData);

		virtual bool IsLabel() { return true; }
	};

	
	class Button : public TextControl
	{
	public:
		Button(const std::string &newName, glm::ivec2 newPosition,
			   unsigned short newWidth, unsigned short newHeight, 
			   unsigned short newWindowWidth, unsigned short newWindowHeight,
			   glm::ivec2 newMargins)
				: TextControl(newName, newPosition, newWidth, newHeight,
				  newWindowWidth, newWindowHeight, newMargins) {}

		virtual bool HandleEvent(const IEventData &eventData);

		virtual bool IsButton() { return true; }
	};


	class TextBox : public TextControl
	{
	private:
		std::string visibleText;
		float maxWidth;

	public:
		TextBox(const std::string &newName, glm::ivec2 newPosition,
				unsigned short newWidth, unsigned short newHeight, 
				unsigned short newWindowWidth, unsigned short newWindowHeight,
				glm::ivec2 newMargins, float newMaxWidth)
				: TextControl(newName, newPosition, newWidth, newHeight,
							  newWindowWidth, newWindowHeight, newMargins),
				  visibleText(""), maxWidth(newMaxWidth) {}

		virtual void Init(EventManager &eventManager);

		virtual bool HandleEvent(const IEventData &eventData);

		virtual bool IsTextBox() { return true; }
	};


	class ImageBox : public Control
	{
	public:
		ImageBox(const std::string &newName,
				 glm::ivec2 newPosition,
				 unsigned short newWidth, unsigned short newHeight,
				 unsigned short newWindowWidth, unsigned short newWindowHeight)
				 : Control(newName, newPosition, newWidth, newHeight, 
						   newWindowWidth, newWindowHeight) {}

		virtual void Init(EventManager &eventManager);

		virtual bool HandleEvent(const IEventData &eventData);

		virtual bool IsImageBox() { return true; }
	};
}


#endif