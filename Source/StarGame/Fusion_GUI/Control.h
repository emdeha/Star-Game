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


#include "../glsdk/glm/glm.hpp"

#include "../Fusion_EntitySystem/EventManager.h"
#include "../Fusion_Renderer/Sprite.h"
#include "../Fusion_GUI/Text.h"


namespace FusionEngine
{
	// TODO: Maybe not a good decision
	typedef void (*OnClickHandler)();

	enum RelativityOption
	{
		FE_RELATIVE_TOP_LEFT,
		FE_RELATIVE_TOP_RIGHT,
		FE_RELATIVE_BOTTOM_LEFT,
		FE_RELATIVE_BOTTOM_RIGHT,
		FE_RELATIVE_CENTER_BOTTOM,
		FE_RELATIVE_CENTER_TOP,
	};

	class Control : public IEventListener
	{
	protected:
		//OnClickHandler handleOnClick;

		std::string name; // TODO: integer ID in the future

		glm::ivec2 initialPosition;
		glm::ivec2 position;
		// glm::ivec2 margins;

		RelativityOption currentRelativity;

		int width;
		int height;

		int windowWidth;
		int windowHeight;

		Sprite2D background;
		bool hasBackground;
		bool isVisible;
		//Text text;

		//std::string textFont;
		//std::string textString;
		//glm::vec4 textColor;
		//glm::vec2 textPosition;
		//int textSize;

	public:
		virtual ~Control() {}

		Control(const std::string &newName,
				glm::ivec2 newPosition, //glm::ivec2 newMargins, 
				int newWidth, int newHeight,
				int newWindowWidth, int newWindowHeight)
			: name(newName), //handleOnClick(nullptr),
			  position(newPosition), initialPosition(newPosition), /*margins(newMargins),*/ 
			  currentRelativity(FE_RELATIVE_TOP_RIGHT),
			  width(newWidth), height(newHeight), 
			  windowWidth(newWindowWidth), windowHeight(newWindowHeight),
			  isVisible(true), hasBackground(false) {}
			  //textString(""), textFont(""), textColor(-1.0f), textPosition(-1.0f), textSize(-1) {}

		void SetRelativity(RelativityOption relativeTo);
		void SetVisibility(bool newIsVisible);
		void SetBackground(const std::string &backgroundFileName);
		//void SetTextProperties(const std::string &newTextFont, const std::string &newTextString,
		//					   glm::vec4 newTextColor, int newTextSize);

		//void SetOnClickHandler(OnClickHandler onClickHandler);

		virtual void Init(EventManager &eventManager);

		virtual void Draw(glutil::MatrixStack &modelMatrix);

		bool IsMouseOn(glm::ivec2 mouseCoordinates_windowSpace);

		virtual bool HandleEvent(const IEventData &eventData);

	public:
		std::string GetName();
	};

	
	class TextControl : public Control
	{
	protected:
		OnClickHandler handleOnClick;
		
		Text text;

		glm::ivec2 margins;

		std::string textFont;
		std::string textString;
		glm::vec4 textColor;
		glm::vec2 textPosition;
		int textSize;

	public:
		TextControl(const std::string &newName, glm::ivec2 newPosition,
					int newWidth, int newHeight, int newWindowWidth, int newWindowHeight,
					glm::ivec2 newMargins)
				: Control(newName, newPosition, newWidth, newHeight, 
						  newWindowWidth, newWindowHeight),
				  margins(newMargins), handleOnClick(nullptr), 
				  textString(""), textFont(""), textColor(-1.0f), 
				  textPosition(-1.0f), textSize(-1) {}

		void SetOnClickHandler(OnClickHandler onClickHandler);
		void SetTextProperties(const std::string &newTextFont, const std::string &newTextString,
							   glm::vec4 newTextColor, int newTextSize);

		virtual void Init(EventManager &eventManager);

		virtual void Draw(glutil::MatrixStack &modelMatrix);

		virtual bool HandleEvent(const IEventData &eventData);
	};


	class Label : public TextControl
	{
	public:
		virtual bool HandleEvent(const IEventData &eventData);
	};
}


#endif