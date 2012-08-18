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


//GLfloat bufferData[24];

// TODO: Make data-driven

Button::Button()
{
	name = "";
	title = "";
	position = glm::vec2();

	textMinHeight = 0.0f;
	textMaxHeight = 0.0f;
	textMinWidth = 0.0f;
	textMaxWidth = 0.0f;

	topTextMargin = 0.0f;
	bottomTextMargin = 0.0f;
	leftTextMargin = 0.0f;
	rightTextMargin = 0.0f;

	textSize = 0;

	windowWidth = 0;
	windowHeight = 0;
}
Button::Button(const std::string &newName, const std::string &newTitle,
			   glm::vec2 newPosition,
			   float newTopTextMargin, float newBottomTextMargin, 
			   float newRightTextMargin, float newLeftTextMargin,
			   int newTextSize)
{
	name = newName;
	title = newTitle;
	position = newPosition;

	topTextMargin = newTopTextMargin;
	bottomTextMargin = newBottomTextMargin;
	leftTextMargin = newLeftTextMargin;
	rightTextMargin = newRightTextMargin;

	textSize = newTextSize;

	windowWidth = 0;
	windowHeight = 0;
}

void Button::Init(const std::string fontName,
				  int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	textTitle = Text(fontName.c_str());
	textTitle.Init(newWindowWidth, newWindowHeight);

	glGenBuffers(1, &vbo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
			
	
	textTitle.ComputeTextDimensions(title.c_str(), position, textSize);

	textMinHeight = textTitle.GetTextMinHeight() - bottomTextMargin;
	textMaxHeight = textTitle.GetTextMaxHeight() + topTextMargin;

	textMinWidth = textTitle.GetTextMinWidth() - leftTextMargin;
	textMaxWidth = textTitle.GetTextMaxWidth() + rightTextMargin;

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

void Button::Draw(const FontProgData &fontData, const SimpleProgData &simpleData)
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

	textTitle.Print(title.c_str(), fontData, position, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), textSize);
}

void Button::Update(int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	textTitle.UpdateWindowDimensions(windowWidth, windowHeight);
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
	}
}

std::string Button::GetName()
{
	return name;
}