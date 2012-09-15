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


// TODO: Make data-driven
// TODO: function CalculateMarginsByPercentage();


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
						 bool newHasBackground)
{
	name = newName;
	text = newText;

	hasBackground = newHasBackground;

	currentPreset = newCurrentPreset;

	presets[currentPreset].position = newPosition;
	presets[currentPreset].textSize = newTextSize;

	presets[newCurrentPreset].bottomTextMargin = 0.0f;
	presets[newCurrentPreset].bottomTextMargin_percent = 0.0f;
	presets[newCurrentPreset].topTextMargin = 0.0f;
	presets[newCurrentPreset].topTextMargin_percent = 0.0f;
	presets[newCurrentPreset].leftTextMargin = 0.0f;
	presets[newCurrentPreset].leftTextMargin_percent = 0.0f;
	presets[newCurrentPreset].rightTextMargin = 0.0f;
	presets[newCurrentPreset].rightTextMargin_percent = 0.0f;

	isActive = false;
}

void TextControl::Init(const std::string &fontName,
					   int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	textToDisplay = Text(fontName.c_str()/*, "../data/fonts/AGENCYB.TTF"*/);
	textToDisplay.Init(windowWidth, windowHeight);

	glGenBuffers(1, &vbo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	ComputeNewAttributes();
}

void TextControl::ComputeNewAttributes()
{
	textToDisplay.ComputeTextDimensions(text.c_str(), presets[currentPreset].position, 
													  presets[currentPreset].textSize);

	if(hasBackground)
	{
		boxMinCorner.y = textToDisplay.GetTextMinHeight() - presets[currentPreset].bottomTextMargin;
		boxMaxCorner.y = textToDisplay.GetTextMaxHeight() + presets[currentPreset].topTextMargin;

		boxMinCorner.x = textToDisplay.GetTextMinWidth() - presets[currentPreset].leftTextMargin;
		boxMaxCorner.x = textToDisplay.GetTextMaxWidth() + presets[currentPreset].rightTextMargin;

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
	}
	else
	{
		boxMinCorner.y = textToDisplay.GetTextMinHeight();
		boxMaxCorner.y = textToDisplay.GetTextMaxHeight();
		boxMinCorner.x = textToDisplay.GetTextMinWidth();
		boxMaxCorner.x = textToDisplay.GetTextMaxWidth();
	}
}

void TextControl::Draw(const FontProgData &fontData, const SimpleProgData &simpleData)
{
	if(hasBackground)
	{
		glUseProgram(simpleData.theProgram);
		glBindVertexArray(vao);
		{
			glUniform4f(simpleData.colorUnif, 1.0f, 0.0f, 0.0f, 1.0f);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glEnableVertexAttribArray(simpleData.positionAttrib);
			glVertexAttribPointer(simpleData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

			glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_DYNAMIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glDisableVertexAttribArray(0);
		glUseProgram(0);
	}

	textToDisplay.Print(text.c_str(), fontData, 
						presets[currentPreset].position, 
						glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
						presets[currentPreset].textSize);
}

void TextControl::Update(int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	textToDisplay.UpdateWindowDimensions(windowWidth, windowHeight);

	ComputeNewAttributes();
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

bool TextControl::IsMouseOn(glm::vec2 mousePosition_clipSpace)
{
	if(mousePosition_clipSpace.y > boxMinCorner.y &&
	   mousePosition_clipSpace.x > boxMinCorner.x &&
	   mousePosition_clipSpace.y < boxMaxCorner.y &&
	   mousePosition_clipSpace.x < boxMaxCorner.x)
	{
		return true;
	}
	else return false;
}

bool TextControl::IsActive()
{
	return isActive;
}