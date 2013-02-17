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

	controls.resize(0);
	subLayouts.resize(0);

	isSet = false;
}
Layout::Layout(LayoutType newLayoutType, LayoutInfo newLayoutInfo)
{
	layoutType = newLayoutType;

	layoutInfo = newLayoutInfo;

	controls.resize(0);
	subLayouts.resize(0);

	isSet = true;
}

void Layout::Draw(const FontProgData &fontData, const SimpleProgData &simpleData,
				  const TextureProgData &textureData)
{
	glm::vec4 backgroundColor = layoutInfo.backgroundColor;
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);


	for(std::vector<std::shared_ptr<TextControl>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		if((*iter)->IsImageBox())
		{
			(*iter)->Draw(textureData);
		}
		else if((*iter)->IsHintBox())
		{
			(*iter)->Draw(fontData, textureData);
		}
		else
		{
			(*iter)->Draw(fontData, simpleData);
		}
	}
	for(std::vector<std::shared_ptr<Layout>>::iterator iter = subLayouts.begin();
		iter != subLayouts.end();
		++iter)
	{
		if((*iter)->IsSet())
		{
			(*iter)->Draw(fontData, simpleData, textureData);
		}
	}
}

void Layout::Update(int windowWidth, int windowHeight)
{
	for(std::vector<std::shared_ptr<TextControl>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		(*iter)->Update(windowWidth, windowHeight);
	}
	for(std::vector<std::shared_ptr<Layout>>::iterator iter = subLayouts.begin();
		iter != subLayouts.end(); ++iter)
	{
		(*iter)->Update(windowWidth, windowHeight);
	}
}


void Layout::AddControl(std::shared_ptr<TextControl> newControl)
{
	controls.push_back(newControl);
}
void Layout::AddSubLayout(std::shared_ptr<Layout> newSubLayout)
{
	subLayouts.push_back(newSubLayout);
}


std::shared_ptr<TextControl> Layout::GetControl(const std::string &controlName)
{
	for(std::vector<std::shared_ptr<TextControl>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		if((*iter)->GetName() == controlName)
		{
			return (*iter);
		}
	}

	// TODO: Better error handling.
	std::printf("There is no such control\n");
}

LayoutType Layout::GetLayoutType()
{
	return layoutType;
}
bool Layout::IsSet()
{
	return isSet;
}
void Layout::Set(bool newIsSet)
{
	isSet = newIsSet;
}

void Layout::SetCurrentPreset(LayoutPreset newCurrentPreset)
{
	switch(newCurrentPreset)
	{
	case SMALL: 
		for(std::vector<std::shared_ptr<TextControl>>::iterator iter = controls.begin();
			iter != controls.end(); ++iter)
		{
			(*iter)->SetPreset(SMALL);
		}
		break;
	case MEDIUM:
		for(std::vector<std::shared_ptr<TextControl>>::iterator iter = controls.begin();
			iter != controls.end(); ++iter)
		{
			(*iter)->SetPreset(MEDIUM);
		}
		break;
	case BIG:
		for(std::vector<std::shared_ptr<TextControl>>::iterator iter = controls.begin();
			iter != controls.end(); ++iter)
		{
			(*iter)->SetPreset(BIG);
		}
		break;
	}
}


std::shared_ptr<TextControl> Layout::GetActiveControl()
{
	for(std::vector<std::shared_ptr<TextControl>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		if((*iter)->IsActive())
		{
			return (*iter);
		}
	}
}

bool Layout::HasActiveControl()
{
	for(std::vector<std::shared_ptr<TextControl>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		if((*iter)->IsActive())
		{
			return true;
		}
	}

	return false;
}