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
		glutil::MatrixStack identityMatStack;
		identityMatStack.SetIdentity();

		controlSquare.Draw(identityMatStack, simpleData);
		/*
		glUseProgram(simpleData.theProgram);
		glBindVertexArray(vao);
		{
			glUniform4f(simpleData.colorUnif, 1.0f, 1.0f, 0.0f, 1.0f);

			glUniformMatrix4fv(simpleData.modelToCameraMatrixUnif, 1, GL_FALSE, 
							   glm::value_ptr(glm::mat4(1.0f)));

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glEnableVertexAttribArray(simpleData.positionAttrib);
			glVertexAttribPointer(simpleData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

			glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);
		glUseProgram(0);
		*/
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
		glm::vec4 position = glm::vec4(presets[currentPreset].position, 0.0f, 1.0f);

		controlSquare.SetPosition(position);
		controlSquare.SetHeight(presets[currentPreset].textSize);

		/*glm::vec2 maxCorner;
		glm::vec2 minCorner;

		//maxCorner = glm::vec2(controlSquare.GetMaxCorner().x,
		//					  controlSquare.GetMaxCorner().y) + 
		//					  presets[currentPreset].position;
		//maxCorner.y += presets[currentPreset].textSize;
		minCorner = presets[currentPreset].position;

		maxCorner.x = windowWidth - maxCorner.x;
		maxCorner.y = windowHeight - maxCorner.y;
		minCorner.x = windowWidth - minCorner.x;
		minCorner.y = windowHeight - minCorner.y;


		controlSquare.SetHeight(controlSquare.GetHeight() + presets[currentPreset].textSize);
		controlSquare.SetPosition(glm::vec4(minCorner, 0.0f, 1.0f));*/
		/*
		maxCorner = boxMaxCorner + presets[currentPreset].position;
		maxCorner.y += presets[currentPreset].textSize; // Make it a member of the class
		minCorner = presets[currentPreset].position;


		maxCorner.x = windowWidth - maxCorner.x;
		maxCorner.y = windowHeight - maxCorner.y;
		minCorner.x = windowWidth - minCorner.x;
		minCorner.y = windowHeight - minCorner.y;
		*/
		/*
		bufferData[0] = maxCorner.x; bufferData[1] = maxCorner.y;
		bufferData[2] = 0.0f; bufferData[3] = 1.0f;
		bufferData[4] = maxCorner.x; bufferData[5] = minCorner.y; 
		bufferData[6] = 0.0f; bufferData[7] = 1.0f;
		bufferData[8] = minCorner.x; bufferData[9] = maxCorner.y; 
		bufferData[10] = 0.0f; bufferData[11] = 1.0f;

		bufferData[12] = maxCorner.x; bufferData[13] = minCorner.y; 
		bufferData[14] = 0.0f; bufferData[15] = 1.0f;
		bufferData[16] = minCorner.x; bufferData[17] = minCorner.y; 
		bufferData[18] = 0.0f; bufferData[19] = 1.0f;
		bufferData[20] = minCorner.x; bufferData[21] = maxCorner.y; 
		bufferData[22] = 0.0f; bufferData[23] = 1.0f;
		*/
	}
	else
	{
		glm::vec2 boxMaxCorner;
		boxMaxCorner.y = presets[currentPreset].textSize;

		controlSquare.SetHeight(presets[currentPreset].textSize);
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

bool TextBox::IsMouseOn(glm::vec2 mouseCoordinates_windowSpace)
{
	mouseCoordinates_windowSpace.y = windowHeight - mouseCoordinates_windowSpace.y;


	float minHeight = controlSquare.GetPosition().y;
	float minWidth = controlSquare.GetPosition().x;

	float maxHeight = minHeight + controlSquare.GetHeight();
	float maxWidth = minWidth + controlSquare.GetWidth();

	/*
	float minHeight = presets[currentPreset].position.y;
	float minWidth = presets[currentPreset].position.x;

	float maxHeight = minHeight + presets[currentPreset].textSize;
	float maxWidth = minWidth + boxMaxCorner.x;
	*/

	if(mouseCoordinates_windowSpace.y > minHeight &&
	   mouseCoordinates_windowSpace.x > minWidth &&
	   mouseCoordinates_windowSpace.y < maxHeight &&
	   mouseCoordinates_windowSpace.x < maxWidth)
	{
		return true;
	}
	else return false;
}