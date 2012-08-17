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


Button::Button()
{
	name = "";
	title = "";
	position = glm::vec2();
}
Button::Button(const std::string &newName, const std::string &newTitle,
			   glm::vec2 newPosition)
{
	name = newName;
	title = newTitle;
	position = newPosition;
}

void Button::Init(const std::string fontName,
				  int windowWidth, int windowHeight)
{
	textTitle = Text(fontName.c_str());
	textTitle.Init(windowWidth, windowHeight);

	glGenBuffers(1, &vbo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

// TODO: Fix the bug with the text on-top of the button background.

void Button::Draw(FontProgData fontData, SimpleProgData simpleData)
{	
	textTitle.Print(title.c_str(), fontData, position, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 48);
	
	float textMinHeight = textTitle.GetTextMinHeight();
	float textMaxHeight = textTitle.GetTextMaxHeight();

	float textMinWidth = textTitle.GetTextMinWidth();
	float textMaxWidth = textTitle.GetTextMaxWidth();

	const GLfloat bufferData[] =
	{
		textMaxWidth, textMaxHeight, 0.01f, 1.0f,
		textMaxWidth, textMinHeight, 0.01f, 1.0f,
		textMinWidth, textMaxHeight, 0.01f, 1.0f,
		
		textMaxWidth, textMinHeight, 0.01f, 1.0f,
		textMinWidth, textMinHeight, 0.01f, 1.0f,
		textMinWidth, textMaxHeight, 0.01f, 1.0f,
	};
	
	//std::printf("%u\n", simpleData.theProgram);
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