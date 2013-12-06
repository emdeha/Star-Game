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


#include <vector>
#include <memory>

#include "Control.h"


namespace FusionEngine
{
	enum LayoutType
	{
		FE_LAYOUT_BAD = -1,

		FE_LAYOUT_MENU = 0,
		FE_LAYOUT_GAME,
	};


	class Layout
	{
	private:
		LayoutType type;
		
		std::vector<std::shared_ptr<Control>> controls;

	public:
		Layout(LayoutType newType)
			: type(newType) {}

		void AddControl(const std::shared_ptr<Control> newControl);
		std::vector<std::shared_ptr<Control>> GetControls();

		void Draw(glutil::MatrixStack &modelMatrix);
	};
}


#endif