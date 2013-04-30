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

/*
void TextBox::Draw(const FontProgData &fontData, const TextureProgData &textureData)
{
	if(isVisible)
	{
		if(hasBackground)
		{
			glutil::MatrixStack identityMatStack;
			identityMatStack.SetIdentity();

			controlBackground.Draw(identityMatStack, textureData);
		}

		textToDisplay.Print(visibleText.c_str(), fontData, position, fontColor, textSize);	
	}
}
*/

void TextBox::ComputeNewAttributes()
{
	if(isUsingPercentage)
	{
		position = glm::vec2((percentagedPosition.x / 100) * windowWidth,
							 (percentagedPosition.y / 100) * windowHeight);
	}
	textToDisplay.ComputeTextDimensions("|", position, textSize);

	if(hasBackground)
	{
		glm::vec2 boxMinCorner;
		glm::vec2 boxMaxCorner;

		boxMinCorner.y = windowHeight - position.y + margins.y;
		boxMaxCorner.y = windowHeight - position.y + textToDisplay.GetTextMaxHeight() + margins.x;
		boxMinCorner.x = windowWidth - position.x + margins.z;
		boxMaxCorner.x = windowWidth - position.x + maxWidth + margins.w;

		float width = fabsf(textToDisplay.GetTextMinWidth()) + maxWidth + margins.w + margins.z;
		float height = fabsf(textToDisplay.GetTextMinHeight()) + textToDisplay.GetTextMaxHeight() + margins.x + margins.y;
		
		controlSquare.SetPosition(glm::vec3(boxMinCorner, 0.0f));
		controlSquare.SetWidth(maxWidth);
		controlSquare.SetHeight(textToDisplay.GetTextMaxHeight());
		
		controlBackground.Update(maxWidth, textToDisplay.GetTextMaxHeight(), boxMinCorner);
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

void TextBox::InputChar(char ch)
{
	// 8 represents the ASCII code of BACKSPACE
	if((int)ch == 8)
	{
		text.pop_back();
	}
	else
	{
		text += ch;
	}
	
	// 13 represents the ASCII code of ENTER
	//if((int)ch == 133)
	//{

	//}
	

	if((int)text.length() > maxNumberChars)
	{
		if((int)ch != 8)
		{
			for(int i = 0; i < maxNumberChars - 1; i++)
			{
				visibleText[i] = visibleText[i + 1];
			}
			visibleText[maxNumberChars - 1] = ch;
		}
		else
		{
			for(int i = maxNumberChars - 1; i >= 1; i--)
			{
				visibleText[i] = visibleText[i - 1];
			}			
			visibleText[0] = text[text.length() - maxNumberChars];
		}
	}
	else
	{
		visibleText = text;
	}
}

std::string TextBox::GetType()
{
	return "TextBox";
}

/*
void TextBox::Draw(const FontProgData &fontData, const SimpleProgData &simpleData)
{
	if(hasBackground)
	{
		glutil::MatrixStack identityMatStack;
		identityMatStack.SetIdentity();

		controlSquare.Draw(identityMatStack, simpleData);
	}

	glm::vec2 textPosition = presets[currentPreset].position;

	textToDisplay.Print(visibleText.c_str(), fontData,
						textPosition,
					    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
					    presets[currentPreset].textSize);
}


void TextBox::ComputeNewAttributes()
{
	textToDisplay.ComputeTextDimensions("|", presets[currentPreset].position,
											 presets[currentPreset].textSize);

	if(hasBackground)
	{		
		glm::vec3 position = glm::vec3(presets[currentPreset].position, 0.0f);

		controlSquare.SetPosition(position);
		controlSquare.SetHeight((float)presets[currentPreset].textSize);
	}
	else
	{
		glm::vec2 boxMaxCorner;
		boxMaxCorner.y = (float)presets[currentPreset].textSize;

		controlSquare.SetHeight((float)presets[currentPreset].textSize);
	}
}


void TextBox::Update(int newWindowWidth, int newWindowHeight)
{
	windowHeight = newWindowHeight;
	windowWidth = newWindowWidth;

	textToDisplay.UpdateWindowDimensions(windowWidth, windowHeight);

	ComputeNewAttributes();
}


// TODO: The text doesn't hide well with the max width computed of characters.
//		 Use max width in some units.

// TODO: Figure out how to use the arrow keys.
void TextBox::InputChar(char ch)
{
	// 8 represents the ASCII code of BACKSPACE
	if((int)ch == 8)
	{
		text.pop_back();
	}
	else
	{
		text += ch;
	}
	
	// 13 represents the ASCII code of ENTER
	//if((int)ch == 133)
	//{

	//}
	

	if((int)text.length() > maxNumberChars)
	{
		if((int)ch != 8)
		{
			for(int i = 0; i < maxNumberChars - 1; i++)
			{
				visibleText[i] = visibleText[i + 1];
			}
			visibleText[maxNumberChars - 1] = ch;
		}
		else
		{
			for(int i = maxNumberChars - 1; i >= 1; i--)
			{
				visibleText[i] = visibleText[i - 1];
			}			
			visibleText[0] = text[text.length() - maxNumberChars];
		}
	}
	else
	{
		visibleText = text;
	}
}

void TextBox::Clear()
{
	text = "";
	visibleText = "";
}

void TextBox::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ON_CLICK:
		isActive = true;
		break;
	case EVENT_TYPE_UNCLICK:
		isActive = false;
		break;
	default:
		HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
		break;
	}
}

std::string TextBox::GetContent()
{
	return text;
}
void TextBox::SetContent(std::string newInputText)
{
	text = newInputText;
}

bool TextBox::IsMouseOn(glm::vec2 mouseCoordinates_windowSpace)
{
	if(isVisible)
	{
		mouseCoordinates_windowSpace.y = windowHeight - mouseCoordinates_windowSpace.y;


		float minHeight = controlSquare.GetPosition().y;
		float minWidth = controlSquare.GetPosition().x;

		float maxHeight = minHeight + controlSquare.GetHeight();
		float maxWidth = minWidth + controlSquare.GetWidth();

		if(mouseCoordinates_windowSpace.y > minHeight &&
		   mouseCoordinates_windowSpace.x > minWidth &&
		   mouseCoordinates_windowSpace.y < maxHeight &&
		   mouseCoordinates_windowSpace.x < maxWidth)
		{
			return true;
		}
		else return false;
	}
}
*/