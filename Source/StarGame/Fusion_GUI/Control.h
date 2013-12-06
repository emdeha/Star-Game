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
	private:
		glm::ivec2 initialPosition;
		glm::ivec2 position;
		glm::ivec2 margins;

		RelativityOption currentRelativity;

		int width;
		int height;

		int windowWidth;
		int windowHeight;

		Sprite2D background;
		Text text;

		std::string textFont;
		std::string textString;
		glm::vec4 textColor;
		glm::vec2 textPosition;
		int textSize;

	public:
		Control(glm::ivec2 newPosition, glm::ivec2 newMargins, 
				int newWidth, int newHeight,
				int newWindowWidth, int newWindowHeight)
			: position(newPosition), initialPosition(newPosition), margins(newMargins), 
			  currentRelativity(FE_RELATIVE_TOP_RIGHT),
			  width(newWidth), height(newHeight), 
			  windowWidth(newWindowWidth), windowHeight(newWindowHeight),
			  textString(""), textFont(""), textColor(0.0f), textPosition(0.0f), textSize(-1) {}

		void SetRelativity(RelativityOption relativeTo);
		void SetTextProperties(const std::string &newTextFont, const std::string &newTextString,
							   glm::vec4 newTextColor, int newTextSize);

		void Init(const std::string &backgroundImageFileName,
				  EventManager &eventManager);

		void Draw(glutil::MatrixStack &modelMatrix);

		virtual bool HandleEvent(const IEventData &eventData);
	};
}


#endif