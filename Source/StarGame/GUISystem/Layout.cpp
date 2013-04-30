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
#include "../framework/ErrorAPI.h"


Layout::Layout()
{
	layoutType = LAYOUT_IN_GAME;

	backgroundColor = glm::vec4();

	controls.resize(0);
	subLayouts.resize(0);

	isSet = false;
	hasBackground = false;

	backgroundSprite = Utility::Primitives::Sprite(glm::vec3(), backgroundColor, 0.0f, 0.0f, false); 
}
Layout::Layout(LayoutType newLayoutType, glm::vec4 newBackgroundColor)
{
	layoutType = newLayoutType;

	 backgroundColor = newBackgroundColor;

	controls.resize(0);
	subLayouts.resize(0);

	isSet = true;
	hasBackground = false;

	backgroundSprite = Utility::Primitives::Sprite(glm::vec3(), backgroundColor, 0.0f, 0.0f, false); 
}

void Layout::Draw(const FontProgData &fontData, 
				  const SimpleProgData &simpleData,
				  const TextureProgData &textureData)
{
	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

	if(hasBackground)
	{
		glutil::MatrixStack identityMatrix;
		identityMatrix.SetIdentity();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		backgroundSprite.Draw(identityMatrix, textureData);

		glDisable(GL_BLEND);
	}

	for(std::vector<std::shared_ptr</*TextControl*/Control>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		/*if((*iter)->IsImageBox())
		{
			(*iter)->Draw(textureData);
		}
		else if((*iter)->IsHintBox())
		{
			(*iter)->Draw(fontData, textureData);
		}
		else
		{*/
			(*iter)->Draw(fontData, textureData);//simpleData);
		//}
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
	for(std::vector<std::shared_ptr<Control/*TextControl*/>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		(*iter)->Update(windowWidth, windowHeight);
	}
	for(std::vector<std::shared_ptr<Layout>>::iterator iter = subLayouts.begin();
		iter != subLayouts.end(); ++iter)
	{
		(*iter)->Update(windowWidth, windowHeight);
	}

	if(hasBackground)
	{
		backgroundSprite.Update(windowWidth, windowHeight);
	}
}

void Layout::HandleClickedControls(bool isRightButtonClicked,
								   glm::ivec2 mouseCoordinates_windowSpace)
{
	for(std::vector<std::shared_ptr<Control>>::iterator control = controls.begin();
		control != controls.end(); ++control)
	{
		if((*control)->IsMouseOn(mouseCoordinates_windowSpace))
		{
			std::printf("Clicked %s\n", (*control)->GetName().c_str());	
		}
	}
}

std::shared_ptr<Control> Layout::GetClickedControl(glm::ivec2 mouseCoordinates_windowSpace)
{
	for(std::vector<std::shared_ptr<Control>>::iterator control = controls.begin();
		control != controls.end(); ++control)
	{
		if((*control)->IsMouseOn(mouseCoordinates_windowSpace))
		{
			return (*control);
		}
	}
}

//void Layout::AddControl(const std::shared_ptr<TextControl> newControl)
void Layout::AddControl(const std::shared_ptr<Control> newControl)
{
	controls.push_back(newControl);
}
void Layout::AddSubLayout(const std::shared_ptr<Layout> newSubLayout)
{
	subLayouts.push_back(newSubLayout);
}

void Layout::SetBackgroundImage(float width, float height, const std::string &backgroundImageFileName)
{
	backgroundSprite =
		Utility::Primitives::Sprite(glm::vec3(1280, 768, 0), backgroundColor, width, height, false);
	backgroundSprite.Init(backgroundImageFileName, 1280, 768);
	hasBackground = true;
}


//std::shared_ptr<TextControl> Layout::GetControl(const std::string &controlName)
std::shared_ptr<Control> Layout::GetControl(const std::string &controlName)
{
	for(std::vector<std::shared_ptr<Control/*TextControl*/>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		if((*iter)->GetName() == controlName)
		{
			return (*iter);
		}
	}
	
	std::string errorMessage = "there is no such control ";
	errorMessage += controlName;
	HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
}

LayoutType Layout::GetLayoutType()
{
	return layoutType;
}
bool Layout::IsSet()
{
	return isSet;
}
bool Layout::IsControl(const std::string &controlName)
{
	for(std::vector<std::shared_ptr</*TextControl*/Control>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		if((*iter)->GetName() == controlName)
		{
			return true;
		}
	}
	return false;
}
void Layout::Set(bool newIsSet)
{
	isSet = newIsSet;
}

void Layout::SetCurrentPreset(LayoutPreset newCurrentPreset)
{
	/*
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
	default:
		HandleUnexpectedError("invalid layout preset", __LINE__, __FILE__);
		break;
	}
	*/
}

//std::shared_ptr<TextControl> Layout::GetActiveControl()
std::shared_ptr<Control> Layout::GetActiveControl()
{
	for(std::vector<std::shared_ptr</*TextControl*/Control>>::iterator iter = controls.begin();
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
	for(std::vector<std::shared_ptr</*TextControl*/Control>>::iterator iter = controls.begin();
		iter != controls.end(); ++iter)
	{
		if((*iter)->IsActive())
		{
			return true;
		}
	}

	return false;
}