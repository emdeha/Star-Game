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

#define ASCII_CHAR_TABLE_SIZE 255


Control::Control()
{
	name = "";
	text = "";
	onClickEventName = "";
	onHoverEventName = "";
	onKeyPressEventName.resize(ASCII_CHAR_TABLE_SIZE);
	windowHeight = 0;
	windowWidth = 0;
	position = glm::vec2();
	isActive = false;
	isVisible = false;
	isUsingPercentage = false;
	hasBackground = false;
	isHovered = false;
	hasHoveredSoon = false;
	hasUnhoveredSoon = true;

	onHoverProps.text = "";
	onHoverProps.backgroundLeftImage = "";
	onHoverProps.backgroundRightImage = "";
	onHoverProps.backgroundMiddleImage = "";
	onHoverProps.font = "";
	onHoverProps.textColor = glm::vec4(1.0f);
}

Control::Control(const std::string &newName, const std::string &newText,
				 glm::vec4 newFontColor, glm::vec2 newPosition, glm::vec4 newMargins,
				 int newTextSize,
				 bool newHasBackground, bool newIsVisible, bool newIsUsingPercentage,
				 HoveredProperties newOnHoverProps,
				 glm::vec2 newPercentagedPosition)
{
	name = newName;
	text = newText;
	onClickEventName = "";
	onHoverEventName = "";
	onKeyPressEventName.resize(ASCII_CHAR_TABLE_SIZE);
	visibleText = text;
	fontColor = newFontColor;
	position = newPosition;
	textSize = newTextSize;
	hasBackground = newHasBackground;
	isVisible = newIsVisible;
	isUsingPercentage = newIsUsingPercentage;
	isActive = false;
	isHovered = false;
	hasHoveredSoon = false;
	hasUnhoveredSoon = true;
	percentagedPosition = newPercentagedPosition;
	margins = newMargins;

	onHoverProps.text = newOnHoverProps.text;
	onHoverProps.backgroundLeftImage = newOnHoverProps.backgroundLeftImage;
	onHoverProps.backgroundRightImage = newOnHoverProps.backgroundRightImage;
	onHoverProps.backgroundMiddleImage = newOnHoverProps.backgroundMiddleImage;
	onHoverProps.font = newOnHoverProps.font;
	onHoverProps.textColor = newOnHoverProps.textColor;
}

void Control::Init(const std::string &newFontName, 
				   const std::string &bckgLeftTextureFileName,
				   const std::string &bckgRightTextureFileName,
				   const std::string &bckgMiddleTextureFileName,
			       int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;
	
	fontName = newFontName;

	textToDisplay = Text(fontName.c_str());
	textToDisplay.Init(windowWidth, windowHeight);

	controlSquare = 
		Utility::Primitives::Square(glm::vec4(1.0f),
									glm::vec3(position, 0.0f), 
									0.0f, (float)textSize, true);
	controlSquare.Init(windowWidth, windowHeight);

	bckgLeftImage = bckgLeftTextureFileName;
	bckgRightImage = bckgRightTextureFileName;
	bckgMiddleImage = bckgMiddleTextureFileName;
	
	if(hasBackground)
	{
		controlBackground = 
			Utility::Primitives::ComplexSprite(glm::vec3(), glm::vec4(1.0f), margins, 0.0f, 0.0f, false);
		controlBackground.Init(bckgLeftImage,
							   bckgRightImage,
							   bckgMiddleImage,
							   newWindowWidth, newWindowHeight);
	}

	ComputeNewAttributes();
}

void Control::ComputeNewAttributes()
{	
	if(isUsingPercentage)
	{
		position = glm::vec2((percentagedPosition.x / 100) * windowWidth,
							 (percentagedPosition.y / 100) * windowHeight);
	}
	textToDisplay.ComputeTextDimensions(text.c_str(), glm::vec2(), textSize);

	if(hasBackground)
	{
		glm::vec2 boxMinCorner;
		glm::vec2 boxMaxCorner;

		boxMinCorner.y = windowHeight - position.y + fabsf(textToDisplay.GetTextMinHeight()) + margins.y;
		boxMaxCorner.y = windowHeight - position.y + textToDisplay.GetTextMaxHeight() + margins.x;
		boxMinCorner.x = windowWidth - position.x + fabsf(textToDisplay.GetTextMinWidth()) + margins.z;
		boxMaxCorner.x = windowWidth - position.x + textToDisplay.GetTextMaxWidth() + margins.w;

		float width = fabsf(textToDisplay.GetTextMinWidth()) + textToDisplay.GetTextMaxWidth() + margins.w + margins.z;
		float height = fabsf(textToDisplay.GetTextMinHeight()) + textToDisplay.GetTextMaxHeight() + margins.x + margins.y;
		
		controlSquare.SetPosition(glm::vec3(boxMinCorner, 0.0f));
		controlSquare.SetWidth(width);
		controlSquare.SetHeight(height);
		
		controlBackground.Update(width, height, boxMinCorner);
	}
	else
	{
		glm::vec2 boxMinCorner;
		glm::vec2 boxMaxCorner;

		boxMinCorner.y = windowHeight - position.y;
		boxMaxCorner.y = windowHeight - textToDisplay.GetTextMaxHeight();
		boxMinCorner.x = windowWidth - position.x;
		boxMaxCorner.x = windowWidth - textToDisplay.GetTextMaxWidth();

		controlSquare.SetPosition(glm::vec3(boxMinCorner, 0.0f));
		controlSquare.SetHeight(textToDisplay.GetTextMaxHeight());
		controlSquare.SetWidth(textToDisplay.GetTextMaxWidth());
	}
}

void Control::Update(int newWindowWidth, int newWindowHeight)
{
	if(isVisible)
	{
		windowWidth = newWindowWidth;
		windowHeight = newWindowHeight;

		textToDisplay.UpdateWindowDimensions(windowWidth, windowHeight);

		ComputeNewAttributes();
	}
}

void Control::Draw(FusionEngine::ShaderManager shaderManager)
{
	if(isVisible)
	{
		if(hasBackground)
		{
			glutil::MatrixStack identityMatStack;
			identityMatStack.SetIdentity();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			controlBackground.Draw(identityMatStack, shaderManager);

			glDisable(GL_BLEND);
		}

		if(!isHovered)
		{
			if(!hasUnhoveredSoon)
			{
				textToDisplay = Text(fontName.c_str());
				textToDisplay.Init(windowWidth, windowHeight);
				if(hasBackground)
                {
					controlBackground = 
						Utility::Primitives::ComplexSprite(glm::vec3(), glm::vec4(1.0f), margins, 0.0f, 0.0f, false);
					controlBackground.Init(bckgLeftImage,
						  				   bckgRightImage, 
										   bckgMiddleImage,  
										   windowWidth, windowHeight);
					ComputeNewAttributes();
                }
				hasHoveredSoon = false;
				hasUnhoveredSoon = true;
			}

			textToDisplay.Print(shaderManager,
								visibleText.c_str(), position, fontColor, textSize);	
		}
		else
		{
			if(!hasHoveredSoon)
			{
				textToDisplay = Text(onHoverProps.font.c_str());
				textToDisplay.Init(windowWidth, windowHeight);
				if(hasBackground)
                {
					controlBackground = 
						Utility::Primitives::ComplexSprite(glm::vec3(), glm::vec4(1.0f), margins, 0.0f, 0.0f, false);
					controlBackground.Init(onHoverProps.backgroundLeftImage, 
										   onHoverProps.backgroundRightImage,
										   onHoverProps.backgroundMiddleImage,
										   windowWidth, windowHeight);
					ComputeNewAttributes();
                }
				hasHoveredSoon = true;
				hasUnhoveredSoon = false;
			}

			std::string textToPrint = "";
			if(onHoverProps.text == "")
			{
				textToPrint = visibleText;
			}
			else
			{
				textToPrint = onHoverProps.text;
			}
			textToDisplay.Print(shaderManager, textToPrint.c_str(), position, 
								onHoverProps.textColor, textSize); 
		}
	}
}

void Control::SubscribeForEvent(const std::string &eventName)
{
	onClickEventName = eventName;
}
void Control::SubscribeForHoverEvent(const std::string &eventName)
{
	onHoverEventName = eventName;
}
void Control::SubscribeForKeyPressEvent(const std::string &eventName, char key)
{
	onKeyPressEventName[(int)key] = eventName;
}
std::string Control::GetSubscribedEvent()
{
	return onClickEventName;
}
std::string Control::GetSubscribedHoverEvent()
{
	return onHoverEventName;
}
std::string Control::GetSubscribedKeyPressEvent(char key)
{
	return onKeyPressEventName[(int)key];
}

void Control::SetIsHovered(bool newIsHovered)
{
	isHovered = newIsHovered;
}

void Control::SetPosition(glm::vec2 newPosition, bool newIsUsingPercentage)
{
	isUsingPercentage = newIsUsingPercentage;
	if(isUsingPercentage)
	{
		percentagedPosition = newPosition;
	}
	else
	{
		position = newPosition;
	}
	ComputeNewAttributes();
}

bool Control::IsMouseOn(glm::ivec2 mouseCoordinates_windowSpace)
{
	if(isVisible)
	{
		mouseCoordinates_windowSpace.x = windowWidth - mouseCoordinates_windowSpace.x;

		float maxHeight = controlSquare.GetPosition().y;
		float maxWidth = controlSquare.GetPosition().x;
		float minHeight = maxHeight - controlSquare.GetHeight();
		float minWidth = maxWidth - controlSquare.GetWidth();

		if(mouseCoordinates_windowSpace.y > minHeight &&
		   mouseCoordinates_windowSpace.x > minWidth &&
		   mouseCoordinates_windowSpace.y < maxHeight &&
		   mouseCoordinates_windowSpace.x < maxWidth)
		{
			return true;
		}
		else return false;
	}
	else return false;
}

void Control::ClearContent()
{
	text = "";
	visibleText = "";
}

std::string Control::GetContent()
{
	return text;
}
bool Control::IsActive()
{
	return isActive;
}
void Control::SetIsActive(bool newIsActive)
{
	isActive = newIsActive;
}
void Control::SetIsVisible(bool newIsVisible)
{
	isVisible = newIsVisible;
}
void Control::SetText(const std::string &newText)
{
	text = newText;
	visibleText = text;
}
std::string Control::GetName()
{
	return name;
}