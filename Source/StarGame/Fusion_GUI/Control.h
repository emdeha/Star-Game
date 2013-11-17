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


namespace FusionEngine
{
	enum RelativityOptions
	{
		FE_RELATIVE_TOP_LEFT,
		FE_RELATIVE_TOP_RIGHT,
		FE_RELATIVE_BOTTOM_LEFT,
		FE_RELATIVE_BOTTOM_RIGHT,
	};

	class Control : public IEventListener
	{
	private:
		glm::ivec2 position;

		int width;
		int height;

		int windowWidth;
		int windowHeight;

		Sprite2D background;

	public:
		Control(glm::ivec2 newPosition, int newWidth, int newHeight,
				int newWindowWidth, int newWindowHeight)
			: position(newPosition), width(newWidth), height(newHeight), 
			  windowWidth(newWindowWidth), windowHeight(newWindowHeight) {}

		void SetRelativity(RelativityOptions relativeTo);

		void Init(const std::string &backgroundImageFileName,
				  EventManager &eventManager);

		void Draw(glutil::MatrixStack &modelMatrix);

		virtual bool HandleEvent(const IEventData &eventData);
	};
}


#endif