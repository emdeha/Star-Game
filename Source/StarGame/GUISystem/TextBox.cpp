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

	textToDisplay.Print(visibleText.c_str(), fontData,
					    presets[currentPreset].position,
					    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
					    presets[currentPreset].textSize);
}


void TextBox::ComputeNewAttributes()
{
	textToDisplay.ComputeTextDimensions("|", presets[currentPreset].position,
										presets[currentPreset].textSize);

	if(hasBackground)
	{
		boxMaxCorner.y = textToDisplay.GetTextMaxHeight();

		bufferData[0] = boxMaxCorner.x; bufferData[1] = boxMaxCorner.y;
		bufferData[2] = 0.0f; bufferData[3] = 1.0f;
		bufferData[4] = boxMaxCorner.x; bufferData[5] = presets[currentPreset].position.y; 
		bufferData[6] = 0.0f; bufferData[7] = 1.0f;
		bufferData[8] = presets[currentPreset].position.x; bufferData[9] = boxMaxCorner.y; 
		bufferData[10] = 0.0f; bufferData[11] = 1.0f;

		bufferData[12] = boxMaxCorner.x; bufferData[13] = presets[currentPreset].position.y; 
		bufferData[14] = 0.0f; bufferData[15] = 1.0f;
		bufferData[16] = presets[currentPreset].position.x; bufferData[17] = presets[currentPreset].position.y; 
		bufferData[18] = 0.0f; bufferData[19] = 1.0f;
		bufferData[20] = presets[currentPreset].position.x; bufferData[21] = boxMaxCorner.y; 
		bufferData[22] = 0.0f; bufferData[23] = 1.0f;
	}
	else
	{
		boxMaxCorner.y = textToDisplay.GetTextMaxHeight();
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

/*bool TextBox::IsActive()
{
	return isActive;
}*/

bool TextBox::IsMouseOn(glm::vec2 mousePosition_clipSpace)
{
	float minHeight = presets[currentPreset].position.y;
	float minWidth = presets[currentPreset].position.x;

	if(mousePosition_clipSpace.y > minHeight &&
	   mousePosition_clipSpace.x > minWidth &&
	   mousePosition_clipSpace.y < boxMaxCorner.y &&
	   mousePosition_clipSpace.x < boxMaxCorner.x)
	{
		return true;
	}
	else return false;
}