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

void Button::ComputeNewAttributes()
{
	textTitle.ComputeTextDimensions(title.c_str(), presets[currentPreset].position, 
												   presets[currentPreset].textSize);

	if(hasBackground)
	{
		textMinHeight = textTitle.GetTextMinHeight() - presets[currentPreset].bottomTextMargin;
		textMaxHeight = textTitle.GetTextMaxHeight() + presets[currentPreset].topTextMargin;

		textMinWidth = textTitle.GetTextMinWidth() - presets[currentPreset].leftTextMargin;
		textMaxWidth = textTitle.GetTextMaxWidth() + presets[currentPreset].rightTextMargin;

		bufferData[0] = textMaxWidth; bufferData[1] = textMaxHeight;
		bufferData[2] = 0.0f; bufferData[3] = 1.0f;
		bufferData[4] = textMaxWidth; bufferData[5] = textMinHeight; 
		bufferData[6] = 0.0f; bufferData[7] = 1.0f;
		bufferData[8] = textMinWidth; bufferData[9] = textMaxHeight; 
		bufferData[10] = 0.0f; bufferData[11] = 1.0f;

		bufferData[12] = textMaxWidth; bufferData[13] = textMinHeight; 
		bufferData[14] = 0.0f; bufferData[15] = 1.0f;
		bufferData[16] = textMinWidth; bufferData[17] = textMinHeight; 
		bufferData[18] = 0.0f; bufferData[19] = 1.0f;
		bufferData[20] = textMinWidth; bufferData[21] = textMaxHeight; 
		bufferData[22] = 0.0f; bufferData[23] = 1.0f;
	}
	else
	{
		textMinHeight = textTitle.GetTextMinHeight();
		textMaxHeight = textTitle.GetTextMaxHeight();
		textMinWidth = textTitle.GetTextMinWidth();
		textMaxWidth = textTitle.GetTextMaxWidth();
	}
}

Button::Button()
{
	name = "";
	title = "";

	textMinHeight = 0.0f;
	textMaxHeight = 0.0f;
	textMinWidth = 0.0f;
	textMaxWidth = 0.0f;

	windowWidth = 0;
	windowHeight = 0;

	hasBackground = false;

	backgroundColor = glm::vec4();
}
Button::Button(const std::string &newName, const std::string &newTitle, 
			   glm::vec2 newPosition,
		       int newTextSize,
		       int newWindowWidth, int newWindowHeight,
		       LayoutPreset newCurrentPreset,
		       bool newHasBackground,
			   glm::vec4 newBackgroundColor)
{
	currentPreset = newCurrentPreset;

	name = newName;
	title = newTitle;

	presets[newCurrentPreset].position = newPosition;
	presets[newCurrentPreset].textSize = newTextSize;

	presets[newCurrentPreset].bottomTextMargin = 0.0f;
	presets[newCurrentPreset].bottomTextMargin_percent = 0.0f;
	presets[newCurrentPreset].topTextMargin = 0.0f;
	presets[newCurrentPreset].topTextMargin_percent = 0.0f;
	presets[newCurrentPreset].leftTextMargin = 0.0f;
	presets[newCurrentPreset].leftTextMargin_percent = 0.0f;
	presets[newCurrentPreset].rightTextMargin = 0.0f;
	presets[newCurrentPreset].rightTextMargin_percent = 0.0f;

	hasBackground = newHasBackground;

	backgroundColor = newBackgroundColor;
}
Button::Button(const std::string &newName, const std::string &newTitle,
			   glm::vec2 newPosition,
			   float buttonTopMargin_percent, float buttonBottomMargin_percent,
			   float buttonLeftMargin_percent, float buttonRightMargin_percent,
			   int newTextSize, 
			   int newWindowWidth, int newWindowHeight,
			   LayoutPreset newCurrentPreset,
			   bool newHasBackground,
			   glm::vec4 newBackgroundColor)
{
	// assert(buttonTopMargin_percent > 0.0f);
	// ...
	// ...
	// assert(buttonRightMargin_percent > 0.0f);

	currentPreset = newCurrentPreset;

	name = newName;
	title = newTitle;
	presets[newCurrentPreset].position = newPosition;
	presets[newCurrentPreset].textSize = newTextSize;

	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;


	presets[newCurrentPreset].topTextMargin_percent = buttonTopMargin_percent;
	presets[newCurrentPreset].bottomTextMargin_percent = buttonBottomMargin_percent;
	presets[newCurrentPreset].leftTextMargin_percent = buttonLeftMargin_percent;
	presets[newCurrentPreset].rightTextMargin_percent = buttonRightMargin_percent;


	presets[newCurrentPreset].topTextMargin = 
		((buttonTopMargin_percent * windowHeight) / 100.0f) / windowHeight;
	presets[newCurrentPreset].bottomTextMargin = 
		((buttonBottomMargin_percent * windowHeight) / 100.0f) / windowHeight;
	presets[newCurrentPreset].leftTextMargin = 
		((buttonLeftMargin_percent * windowWidth) / 100.0f) / windowWidth;
	presets[newCurrentPreset].rightTextMargin = 
		((buttonRightMargin_percent * windowWidth) / 100.0f) / windowWidth;

	hasBackground = newHasBackground;

	backgroundColor = newBackgroundColor;
}

void Button::Init(const std::string &fontName,
				  int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	textTitle = Text(fontName.c_str()/*, "../data/fonts/AGENCYB.TTF"*/);
	textTitle.Init(newWindowWidth, newWindowHeight);

	glGenBuffers(1, &vbo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	ComputeNewAttributes();
}

void Button::Draw(const FontProgData &fontData, const SimpleProgData &simpleData)
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
		textTitle.Print(title.c_str(), fontData, 
					presets[currentPreset].position, 
					glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 
					presets[currentPreset].textSize);
}

void Button::Update(int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	textTitle.UpdateWindowDimensions(windowWidth, windowHeight);

	if(hasBackground)
	{
		presets[currentPreset].topTextMargin = 
			((presets[currentPreset].topTextMargin_percent * windowHeight) / 100.0f) / windowHeight;
		presets[currentPreset].bottomTextMargin = 
			((presets[currentPreset].bottomTextMargin_percent * windowHeight) / 100.0f) / windowHeight;
		presets[currentPreset].leftTextMargin = 
			((presets[currentPreset].leftTextMargin_percent * windowWidth) / 100.0f) / windowWidth;
		presets[currentPreset].rightTextMargin = 
			((presets[currentPreset].rightTextMargin_percent * windowWidth) / 100.0f) / windowWidth;
	}

	ComputeNewAttributes();
}

bool Button::IsMouseOn(glm::vec2 mouseCoordinates)
{
	if(mouseCoordinates.y > textMinHeight &&
	   mouseCoordinates.x > textMinWidth &&
	   mouseCoordinates.y < textMaxHeight &&
	   mouseCoordinates.x < textMaxWidth)
	{
		return true;
	}
	else return false;
}

void Button::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ON_CLICK:
		std::printf("Button \'%s\' clicked\n", this->name.c_str());
		break;
	//case EVENT_TYPE_ON_HOVER:
		//textTitle.SetFont(_event.GetArgument("isBold").varBool);
		//break;
	}
}

std::string Button::GetName()
{
	return name;
}

void Button::SetTextSize(int newTextSize)
{
	// assert(textSize > 0);

	presets[currentPreset].textSize = newTextSize;

	ComputeNewAttributes();
}

void Button::SetNewMargins(float buttonTopMargin_percent, float buttonBottomMargin_percent,
				   float buttonLeftMargin_percent, float buttonRightMargin_percent)
{
	if(hasBackground)
	{
		presets[currentPreset].topTextMargin = 
			((presets[currentPreset].topTextMargin_percent * windowHeight) / 100.0f) / windowHeight;
		presets[currentPreset].bottomTextMargin = 
			((presets[currentPreset].bottomTextMargin_percent * windowHeight) / 100.0f) / windowHeight;
		presets[currentPreset].leftTextMargin = 
			((presets[currentPreset].leftTextMargin_percent * windowWidth) / 100.0f) / windowWidth;
		presets[currentPreset].rightTextMargin = 
			((presets[currentPreset].rightTextMargin_percent * windowWidth) / 100.0f) / windowWidth;
	}
	
	ComputeNewAttributes();
}

void Button::SetNewPosition(glm::vec2 newPosition)
{
	presets[currentPreset].position = newPosition;

	ComputeNewAttributes();
}

void Button::SetBackgroundColor(glm::vec4 newBackgroundColor)
{
	backgroundColor = newBackgroundColor;
}


void Button::AddPreset(LayoutPreset newPreset, 
					   float newBottomTextMargin_percent, 
				       float newTopTextMargin_percent,
				       float newRightTextMargin_percent,
				       float newLeftTextMargin_percent,
				       int newTextSize,
				       glm::vec2 newPosition)
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

void Button::SetPreset(LayoutPreset newCurrentPreset)
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

/*void Button::SetFont(bool isBold)
{
	textTitle.SetFont(isBold);
}*/