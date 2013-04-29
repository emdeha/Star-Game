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


Control::Control()
{
	name = "";
	text = "";
	windowHeight = 0;
	windowWidth = 0;
	position = glm::vec2();
	isActive = false;
	isVisible = false;
	isUsingPercentage = false;
	hasBackground = false;
}

Control::Control(const std::string &newName, const std::string &newText,
				 glm::vec4 newFontColor, glm::vec2 newPosition, glm::vec4 newMargins,
				 int newTextSize,
				 bool newHasBackground, bool newIsVisible, bool newIsUsingPercentage,
				 glm::vec2 newPercentagedPosition)
{
	name = newName;
	text = newText;
	fontColor = newFontColor;
	position = newPosition;
	textSize = newTextSize;
	hasBackground = newHasBackground;
	isVisible = newIsVisible;
	isUsingPercentage = newIsUsingPercentage;
	percentagedPosition = newPercentagedPosition;
	margins = newMargins;
	textPosition = position;
}

void Control::Init(const std::string &fontName, const std::string &bckgTextureFileName,
			       int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;
	
	textToDisplay = Text(fontName.c_str());
	textToDisplay.Init(windowWidth, windowHeight);

	controlSquare = 
		Utility::Primitives::Square(glm::vec4(1.0f),
									glm::vec3(position, 0.0f), 
									0.0f, (float)textSize, true);
	controlSquare.Init(windowWidth, windowHeight);
	
	if(hasBackground)
	{
		controlBackground = 
			Utility::Primitives::Sprite(glm::vec3(), glm::vec4(1.0f), 0.0f, 0.0f, false);
		controlBackground.Init(bckgTextureFileName, newWindowWidth, newWindowHeight);
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
	textToDisplay.ComputeTextDimensions(text.c_str(), textPosition, textSize);

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

void Control::Draw(const FontProgData &fontData, const TextureProgData &textureData)
{
	if(isVisible)
	{
		if(hasBackground)
		{
			glutil::MatrixStack identityMatStack;
			identityMatStack.SetIdentity();

			controlBackground.Draw(identityMatStack, textureData);
		}

		textToDisplay.Print(text.c_str(), fontData, position, fontColor, textSize);	
	}
}

void Control::SetPosition(glm::vec2 newPosition, bool newIsUsingPercentage)
{
	position = newPosition;
	isUsingPercentage = newIsUsingPercentage;
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

std::string Control::GetContent()
{
	return text;
}
bool Control::IsActive()
{
	return isActive;
}
void Control::Deactivate()
{
	isActive = false;
}
void Control::SetIsVisible(bool newIsVisible)
{
	isVisible = newIsVisible;
}
void Control::SetText(const std::string &newText)
{
	text = newText;
}
std::string Control::GetName()
{
	return name;
}











TextControl::TextControl()
{
	name = "";
	text = "";

	windowWidth = 0;
	windowHeight = 0;

	isActive = false;
}

TextControl::TextControl(LayoutPreset newCurrentPreset,
						 const std::string &newName, const std::string &newText,
						 glm::vec2 newPosition, int newTextSize,
						 bool newHasBackground, bool newIsVisible)
{
	name = newName;
	text = newText;

	hasBackground = newHasBackground;
	
	currentPreset = newCurrentPreset;

	presets[currentPreset].position = newPosition;
	presets[currentPreset].textSize = newTextSize;

	presets[currentPreset].bottomTextMargin = 0.0f;
	presets[currentPreset].bottomTextMargin_percent = 0.0f;
	presets[currentPreset].topTextMargin = 0.0f;
	presets[currentPreset].topTextMargin_percent = 0.0f;
	presets[currentPreset].leftTextMargin = 0.0f;
	presets[currentPreset].leftTextMargin_percent = 0.0f;
	presets[currentPreset].rightTextMargin = 0.0f;
	presets[currentPreset].rightTextMargin_percent = 0.0f;

	isActive = false;

	isVisible = newIsVisible; // TODO: Should also disable button events

	controlSquare =
		Utility::Primitives::Square(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
									glm::vec3(newPosition, 0.0f),
									0.0f, (float)newTextSize, true);
}

TextControl::TextControl(LayoutPreset newCurrentPreset,
						 const std::string &newName, 
						 glm::vec2 newPosition, bool newIsVisible)
{
	name = newName;
	currentPreset = newCurrentPreset;
	presets[currentPreset].position = newPosition;
	isActive = false;
	isVisible = newIsVisible;
	controlSquare = 
		Utility::Primitives::Square(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
									glm::vec3(newPosition, 0.0f),
									0.0f, 0.0f, true);
}

void TextControl::Init(const std::string &fontName,
					   int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	textToDisplay = Text(fontName.c_str());
	textToDisplay.Init(windowWidth, windowHeight);

	controlSquare.Init(windowWidth, windowHeight);
	
	ComputeNewAttributes();
}

void TextControl::SetPosition(glm::vec2 newPosition)
{
	presets[SMALL].position = newPosition;
	presets[MEDIUM].position = newPosition;
	presets[BIG].position = newPosition;
	ComputeNewAttributes();
}

void TextControl::SetIsVisible(bool newIsVisible)
{
	isVisible = newIsVisible;
}

void TextControl::ComputeNewAttributes()
{
	textToDisplay.ComputeTextDimensions(text.c_str(), presets[currentPreset].position, 
													  presets[currentPreset].textSize);

	if(hasBackground)
	{
		/// Not tested!!!

		glm::vec2 boxMaxCorner;
		glm::vec2 boxMinCorner;

		boxMinCorner.y = textToDisplay.GetTextMinHeight() - presets[currentPreset].bottomTextMargin;
		boxMaxCorner.y = textToDisplay.GetTextMaxHeight() + presets[currentPreset].topTextMargin;

		boxMinCorner.x = textToDisplay.GetTextMinWidth() - presets[currentPreset].leftTextMargin;
		boxMaxCorner.x = textToDisplay.GetTextMaxWidth() + presets[currentPreset].rightTextMargin;

		controlSquare.SetPosition(glm::vec3(boxMinCorner, 0.0f));
		//controlSquare.SetHeight
		/*
		bufferData[0] = boxMaxCorner.x; bufferData[1] = boxMaxCorner.y;
		bufferData[2] = 0.0f; bufferData[3] = 1.0f;
		bufferData[4] = boxMaxCorner.x; bufferData[5] = boxMinCorner.y; 
		bufferData[6] = 0.0f; bufferData[7] = 1.0f;
		bufferData[8] = boxMinCorner.x; bufferData[9] = boxMaxCorner.y; 
		bufferData[10] = 0.0f; bufferData[11] = 1.0f;

		bufferData[12] = boxMaxCorner.x; bufferData[13] = boxMinCorner.y; 
		bufferData[14] = 0.0f; bufferData[15] = 1.0f;
		bufferData[16] = boxMinCorner.x; bufferData[17] = boxMinCorner.y; 
		bufferData[18] = 0.0f; bufferData[19] = 1.0f;
		bufferData[20] = boxMinCorner.x; bufferData[21] = boxMaxCorner.y; 
		bufferData[22] = 0.0f; bufferData[23] = 1.0f;
		*/
	}
	else
	{
		glm::vec2 boxMinCorner;
		glm::vec2 boxMaxCorner;

		boxMinCorner.y = presets[currentPreset].position.y;
		boxMaxCorner.y = textToDisplay.GetTextMaxHeight();
		boxMinCorner.x = presets[currentPreset].position.x;
		boxMaxCorner.x = textToDisplay.GetTextMaxWidth();

		controlSquare.SetPosition(glm::vec3(boxMinCorner, 0.0f));
		controlSquare.SetHeight(textToDisplay.GetTextMaxHeight());
		controlSquare.SetWidth(textToDisplay.GetTextMaxWidth());
	}
}

void TextControl::Draw(const FontProgData &fontData, const SimpleProgData &simpleData)
{
	if(isVisible)
	{
		if(hasBackground)
		{
			glutil::MatrixStack identityMatStack;
			identityMatStack.SetIdentity();

			controlSquare.Draw(identityMatStack, simpleData);
		}

		textToDisplay.Print(text.c_str(), fontData, 
							presets[currentPreset].position, 
							glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
							presets[currentPreset].textSize);
	}
}

void TextControl::Update(int newWindowWidth, int newWindowHeight)
{
	if(isVisible)
	{
		windowWidth = newWindowWidth;
		windowHeight = newWindowHeight;

		textToDisplay.UpdateWindowDimensions(windowWidth, windowHeight);

		ComputeNewAttributes();
	}
}

std::string TextControl::GetName()
{
	return name;
}

void TextControl::AddPreset(LayoutPreset newPreset,
						    int newTextSize,
						    glm::vec2 newPosition,
						    float newBottomTextMargin_percent, 
						    float newTopTextMargin_percent,
						    float newRightTextMargin_percent,
						    float newLeftTextMargin_percent)
{
	if(hasBackground)
	{
		presets[newPreset].bottomTextMargin = 
			((newBottomTextMargin_percent * windowHeight) / 100.0f) / windowHeight;
		presets[newPreset].topTextMargin = 
			((newTopTextMargin_percent * windowHeight) / 100.0f) / windowHeight;
		presets[newPreset].rightTextMargin = 
			((newRightTextMargin_percent * windowHeight) / 100.0f) / windowHeight;
		presets[newPreset].leftTextMargin = 
			((newLeftTextMargin_percent * windowHeight) / 100.0f) / windowHeight;

		presets[newPreset].bottomTextMargin_percent = newBottomTextMargin_percent;
		presets[newPreset].topTextMargin_percent = newTopTextMargin_percent;
		presets[newPreset].rightTextMargin_percent = newRightTextMargin_percent;
		presets[newPreset].leftTextMargin_percent = newLeftTextMargin_percent;
	}

	presets[newPreset].textSize = newTextSize;
	presets[newPreset].position = newPosition;
}

void TextControl::SetPreset(LayoutPreset newCurrentPreset)
{
	if(presets[newCurrentPreset].textSize > 0)
	{
		currentPreset = newCurrentPreset;
		
		ComputeNewAttributes();
	}
	else
	{
		for(int i = 0; i < PRESETS_COUNT; i++)
		{
			if(presets[i].textSize > 0)
			{
				currentPreset = LayoutPreset(i);
			}
		}
	}
}

bool TextControl::IsMouseOn(glm::vec2 mouseCoordinates_windowSpace)
{
	if(isVisible)
	{
		mouseCoordinates_windowSpace.y = windowHeight - mouseCoordinates_windowSpace.y;

		float minHeight = controlSquare.GetPosition().y;
		float minWidth = controlSquare.GetPosition().x;

		float maxHeight = windowHeight - controlSquare.GetHeight();
		float maxWidth = windowWidth - controlSquare.GetWidth();


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

bool TextControl::IsActive()
{
	return isActive;
}

void TextControl::Deactivate()
{
	isActive = false;
}