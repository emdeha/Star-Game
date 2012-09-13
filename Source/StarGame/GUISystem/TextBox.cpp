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

#include <gl/freeglut.h>


void TextBox::ComputeNewAttributes()
{
	text.ComputeTextDimensions("|", presets[currentPreset].position,
									presets[currentPreset].textSize);

	if(hasBackground)
	{
		maxHeight = text.GetTextMaxHeight();

		bufferData[0] = maxWidth; bufferData[1] = maxHeight;
		bufferData[2] = 0.0f; bufferData[3] = 1.0f;
		bufferData[4] = maxWidth; bufferData[5] = presets[currentPreset].position.y; 
		bufferData[6] = 0.0f; bufferData[7] = 1.0f;
		bufferData[8] = presets[currentPreset].position.x; bufferData[9] = maxHeight; 
		bufferData[10] = 0.0f; bufferData[11] = 1.0f;

		bufferData[12] = maxWidth; bufferData[13] = presets[currentPreset].position.y; 
		bufferData[14] = 0.0f; bufferData[15] = 1.0f;
		bufferData[16] = presets[currentPreset].position.x; bufferData[17] = presets[currentPreset].position.y; 
		bufferData[18] = 0.0f; bufferData[19] = 1.0f;
		bufferData[20] = presets[currentPreset].position.x; bufferData[21] = maxHeight; 
		bufferData[22] = 0.0f; bufferData[23] = 1.0f;
	}
	else
	{
		maxHeight = text.GetTextMaxHeight();
	}
}


TextBox::TextBox()
{
	name = "";
	inputText = "";
	visibleText = "";

	maxHeight = 0.0f;
	maxWidth = 0.0f;

	windowWidth = 0;
	windowHeight = 0;

	hasBackground = false;
	isHandlingInput = false;
}

TextBox::TextBox(const std::string &newName,
				 glm::vec2 newPosition, int newTextSize,
				 float width, int newMaxWidthChars,
				 int newWindowWidth, int newWindowHeight,
				 LayoutPreset newCurrentPreset,
				 bool newHasBackground)
{
	name = newName;
	inputText = "";
	visibleText = "";

	currentPreset = newCurrentPreset;
	presets[currentPreset].position = newPosition;
	presets[currentPreset].textSize = newTextSize;

	maxWidth = width;
	maxWidthChars = newMaxWidthChars;
	maxHeight = 0.0f;

	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	hasBackground = newHasBackground;
	isHandlingInput = false;
}

void TextBox::Init(const std::string &fontName,
				   int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	text = Text(fontName.c_str());
	text.Init(newWindowWidth, newWindowHeight);

	glGenBuffers(1, &vbo);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	ComputeNewAttributes();
}

void TextBox::Draw(const FontProgData &fontData, const SimpleProgData &simpleData)
{
	if(hasBackground)
	{
		glUseProgram(simpleData.theProgram);
		glBindVertexArray(vao);
		{
			glUniform4f(simpleData.colorUnif, 1.0f, 1.0f, 1.0f, 1.0f);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glEnableVertexAttribArray(simpleData.positionAttrib);
			glVertexAttribPointer(simpleData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

			glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_DYNAMIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glDisableVertexAttribArray(0);
		glUseProgram(0);
	}

	text.Print(visibleText.c_str(), fontData,
			   presets[currentPreset].position,
			   glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
			   presets[currentPreset].textSize);
}

void TextBox::Update(int newWindowWidth, int newWindowHeight)
{
	windowHeight = newWindowHeight;
	windowWidth = newWindowWidth;

	text.UpdateWindowDimensions(windowWidth, windowHeight);

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
		inputText.pop_back();
	}
	else
	{
		inputText += ch;
	}

	if(inputText.length() > maxWidthChars)
	{
		if((int)ch != 8)
		{
			for(int i = 0; i < maxWidthChars - 1; i++)
			{
				visibleText[i] = visibleText[i + 1];
			}
			visibleText[maxWidthChars - 1] = ch;
		}
		else
		{
			for(int i = maxWidthChars - 1; i >= 1; i--)
			{
				visibleText[i] = visibleText[i - 1];
			}			
			visibleText[0] = inputText[inputText.length() - maxWidthChars];
		}
	}
	else
	{
		visibleText = inputText;
	}
}

void TextBox::Clear()
{
	inputText = "";
	visibleText = "";
}

bool TextBox::IsMouseOn(glm::vec2 mouseCoordinates)
{
	float minHeight = presets[currentPreset].position.y;
	float minWidth = presets[currentPreset].position.x;

	if(mouseCoordinates.y >  minHeight &&
	   mouseCoordinates.x > minWidth &&
	   mouseCoordinates.y < maxHeight &&
	   mouseCoordinates.x < maxWidth)
	{
		return true;
	}
	else return false;
}

void TextBox::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ON_CLICK:
		isHandlingInput = true;
		break;
	case EVENT_TYPE_UNCLICK:
		isHandlingInput = false;
		break;
	}
}

void TextBox::AddPreset(LayoutPreset newPreset,
						int newTextSize,
						glm::vec2 newPosition)
{
	presets[newPreset].textSize = newTextSize;
	presets[newPreset].position = newPosition;
}

void TextBox::SetPreset(LayoutPreset newCurrentPreset)
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