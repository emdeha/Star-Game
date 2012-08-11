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


#include "stdafx.h"
#include "GUISystem.h"


Layout::Layout()
{
	layoutType = LAYOUT_IN_GAME;
	layoutInfo.backgroundColor = glm::vec4();

//	controls.resize(0);
	subLayouts.resize(0);

	isSet = false;
}
Layout::Layout(LayoutType newLayoutType, LayoutInfo newLayoutInfo)
{
	layoutType = newLayoutType;
	layoutInfo = newLayoutInfo;

//	controls.resize(0);
	subLayouts.resize(0);

	isSet = true;
}

void Layout::Draw()
{
	glm::vec4 backgroundColor = layoutInfo.backgroundColor;
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

	for(std::vector<Button>::iterator iter = buttonControls.begin();
		iter != buttonControls.end(); ++iter)
	{
	//	(*iter).Draw();
	}
	for(std::vector<Layout>::iterator iter = subLayouts.begin();
		iter != subLayouts.end();
		++iter)
	{
		if((*iter).IsSet())
		{
			(*iter).Draw();
		}
	}
}


LayoutType Layout::GetLayoutType()
{
	return layoutType;
}
inline bool Layout::IsSet()
{
	return isSet;
}
inline void Layout::Set(bool newIsSet)
{
	isSet = newIsSet;
}