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


void TextBox::ComputeNewAttributes()
{
	if(isUsingPercentage)
	{
		position = glm::vec2((percentagedPosition.x / 100) * windowWidth,
							 (percentagedPosition.y / 100) * windowHeight);
	}
	textToDisplay.ComputeTextDimensions("|", glm::vec2(), textSize);

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
		controlSquare.SetWidth(maxWidth + margins.w);
		controlSquare.SetHeight(height);
		
		controlBackground.Update(maxWidth + margins.w, height, boxMinCorner);
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
		controlSquare.SetWidth(maxWidth);
	}
}

void TextBox::InputChar(char ch)
{
	// 8 represents the ASCII code of BACKSPACE
	if((int)ch == 8)
	{
		text.pop_back();
		int newTextLength = text.length() - visibleText.length();
		if(newTextLength >= 0)
		{
			for(int i = visibleText.length() - 1; i >= 1; i--)
			{
				visibleText[i] = visibleText[i - 1];
			}
			visibleText[0] = text[newTextLength];
		}
		else
		{
			visibleText.pop_back();
		}
	}
	else
	{
		text += ch;
		visibleText.push_back(ch);
	}
	
	textToDisplay.ComputeTextDimensions(visibleText.c_str(), glm::vec2(), textSize);
	float width = fabsf(textToDisplay.GetTextMinWidth()) + textToDisplay.GetTextMaxWidth() + margins.w + margins.z;

	while(width > maxWidth && (int)ch != 8)
	{
		textToDisplay.ComputeTextDimensions(visibleText.c_str(), glm::vec2(), textSize);
		width = fabsf(textToDisplay.GetTextMinWidth()) + textToDisplay.GetTextMaxWidth() + margins.w + margins.z;
		
		visibleText.erase(visibleText.begin());
	}
}

std::string TextBox::GetType()
{
	return "TextBox";
}