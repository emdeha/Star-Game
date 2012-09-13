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

	buttonControls.resize(0);
	labelControls.resize(0);
	textBoxControls.resize(0);
	subLayouts.resize(0);

	isSet = false;
}
Layout::Layout(LayoutType newLayoutType, LayoutInfo newLayoutInfo)
{
	layoutType = newLayoutType;

	layoutInfo = newLayoutInfo;

	buttonControls.resize(0);
	labelControls.resize(0);
	textBoxControls.resize(0);
	subLayouts.resize(0);

	isSet = true;
}

void Layout::Draw(const FontProgData &fontData, const SimpleProgData &simpleData)
{
	glm::vec4 backgroundColor = layoutInfo.backgroundColor;
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

	for(std::vector<std::shared_ptr<Button>>::iterator iter = buttonControls.begin();
		iter != buttonControls.end(); ++iter)
	{
		(*iter)->Draw(fontData, simpleData);
	}
	for(std::vector<std::shared_ptr<Label>>::iterator iter = labelControls.begin();
		iter != labelControls.end(); ++iter)
	{
		(*iter)->Draw(fontData, simpleData);
	}
	for(std::vector<std::shared_ptr<TextBox>>::iterator iter = textBoxControls.begin();
		iter != textBoxControls.end(); ++iter)
	{
		(*iter)->Draw(fontData, simpleData);
	}
	for(std::vector<std::shared_ptr<Layout>>::iterator iter = subLayouts.begin();
		iter != subLayouts.end();
		++iter)
	{
		if((*iter)->IsSet())
		{
			(*iter)->Draw(fontData, simpleData);
		}
	}
}

void Layout::Update(int windowWidth, int windowHeight)
{
	for(std::vector<std::shared_ptr<Button>>::iterator iter = buttonControls.begin();
		iter != buttonControls.end(); ++iter)
	{
		(*iter)->Update(windowWidth, windowHeight);
	}
	
	for(std::vector<std::shared_ptr<Label>>::iterator iter = labelControls.begin();
		iter != labelControls.end(); ++iter)
	{
		(*iter)->Update(windowWidth, windowHeight);
	}

	for(std::vector<std::shared_ptr<TextBox>>::iterator iter = textBoxControls.begin();
		iter != textBoxControls.end(); ++iter)
	{
		(*iter)->Update(windowWidth, windowHeight);
	}

	for(std::vector<std::shared_ptr<Layout>>::iterator iter = subLayouts.begin();
		iter != subLayouts.end(); ++iter)
	{
		(*iter)->Update(windowWidth, windowHeight);
	}
}


void Layout::AddButtonControl(std::shared_ptr<Button> newButtonControl)
{
	buttonControls.push_back(newButtonControl);
}
void Layout::AddLabelControl(std::shared_ptr<Label> newLabelControl)
{
	labelControls.push_back(newLabelControl);
}
void Layout::AddTextBoxControl(std::shared_ptr<TextBox> newTextBoxControl)
{
	textBoxControls.push_back(newTextBoxControl);
}
void Layout::AddSubLayout(std::shared_ptr<Layout> newSubLayout)
{
	subLayouts.push_back(newSubLayout);
}


std::shared_ptr<Button> Layout::GetButtonControl(const std::string &buttonName)
{
	for(std::vector<std::shared_ptr<Button>>::iterator iter = buttonControls.begin();
		iter != buttonControls.end(); ++iter)
	{
		if((*iter)->GetName() == buttonName)
		{
			return (*iter);
		}
	}

	// TODO: Better error handling.
	std::printf("There is no such control\n");
}
std::shared_ptr<TextBox> Layout::GetTextBoxControl(const std::string &textBoxName)
{
	for(std::vector<std::shared_ptr<TextBox>>::iterator iter = textBoxControls.begin();
		iter != textBoxControls.end(); ++iter)
	{
		if((*iter)->GetName() == textBoxName)
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
		for(std::vector<std::shared_ptr<Button>>::iterator iter = buttonControls.begin();
			iter != buttonControls.end(); ++iter)
		{
			(*iter)->SetPreset(SMALL);
		}
		for(std::vector<std::shared_ptr<Label>>::iterator iter = labelControls.begin();
			iter != labelControls.end(); ++iter)
		{
			(*iter)->SetPreset(SMALL);
		}
		break;
	case MEDIUM:
		for(std::vector<std::shared_ptr<Button>>::iterator iter = buttonControls.begin();
			iter != buttonControls.end(); ++iter)
		{
			(*iter)->SetPreset(MEDIUM);
		}
		for(std::vector<std::shared_ptr<Label>>::iterator iter = labelControls.begin();
			iter != labelControls.end(); ++iter)
		{
			(*iter)->SetPreset(MEDIUM);
		}
		break;
	case BIG:
		for(std::vector<std::shared_ptr<Button>>::iterator iter = buttonControls.begin();
			iter != buttonControls.end(); ++iter)
		{
			(*iter)->SetPreset(BIG);
		}
		for(std::vector<std::shared_ptr<Label>>::iterator iter = labelControls.begin();
			iter != labelControls.end(); ++iter)
		{
			(*iter)->SetPreset(BIG);
		}
		break;
	}
}


std::shared_ptr<TextBox> Layout::GetActiveTextBox()
{
	for(std::vector<std::shared_ptr<TextBox>>::iterator iter = textBoxControls.begin();
		iter != textBoxControls.end(); ++iter)
	{
		if((*iter)->IsHandlingInput())
		{
			return (*iter);
		}
	}
}

bool Layout::HasActiveTextBox()
{
	for(std::vector<std::shared_ptr<TextBox>>::iterator iter = textBoxControls.begin();
		iter != textBoxControls.end(); ++iter)
	{
		if((*iter)->IsHandlingInput())
		{
			return true;
		}
	}

	return false;
}