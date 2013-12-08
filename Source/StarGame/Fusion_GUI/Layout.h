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


#ifndef FE_LAYOUT_H
#define FE_LAYOUT_H


#pragma warning(push, 0)
#include <vector>
#include <memory>
#pragma warning(pop)

#include "Control.h"


namespace FusionEngine
{
	enum LayoutType
	{
		FE_LAYOUT_BAD = -1,

		FE_LAYOUT_MENU = 0,
		FE_LAYOUT_GAME,
		FE_LAYOUT_SAVE,
		FE_LAYOUT_LOAD,
		FE_LAYOUT_OPTIONS,
		FE_LAYOUT_QUIT,
	};


	class Layout : public IEventListener
	{
	private:
		LayoutType type;
		bool isSet;
		
		std::vector<std::shared_ptr<Control>> controls;

		Sprite2D background;
		bool isBackgroundSet;

	public:
		Layout(LayoutType newType)
			: type(newType), isSet(false), isBackgroundSet(false),
			  background(glm::vec2(), 0.0f, 0.0f) {}
		Layout(LayoutType newType, unsigned short backgroundWidth, unsigned short backgroundHeight)
			: type(newType), isSet(false), isBackgroundSet(false),
			  background(glm::vec2(backgroundWidth, backgroundHeight), backgroundWidth, backgroundHeight) {}

		void Init(EventManager &eventManager);

		void AddControl(const std::shared_ptr<Control> newControl);
		std::vector<std::shared_ptr<Control>> GetControls() const;

		void Draw(glutil::MatrixStack &modelMatrix) const;

	public:
		bool IsSet() const;
		void Set(bool newIsSet);
		bool HasActiveTextBox() const;
		void SetBackgroundSprite(const std::string &spriteFile);

		void DeactivateAllControls();

		virtual bool HandleEvent(const IEventData &eventData);
	};
}


#endif