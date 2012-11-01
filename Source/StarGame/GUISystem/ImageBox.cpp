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


ImageBox::ImageBox()
{
	name = "";

	isActive = false;
}
ImageBox::ImageBox(LayoutPreset newCurrentPreset,
			       const std::string &newName, 
				   glm::vec2 newPosition,
				   float newWidth, float newHeight)
{
	currentPreset = newCurrentPreset;
	presetPosition[currentPreset] = newPosition;
	width = newWidth;
	height = newHeight;
	name = newName;
	isActive = false;
}

void ImageBox::Init()
{
	bufferData[0] = presetPosition[currentPreset].x - width; 
	bufferData[1] = presetPosition[currentPreset].y - height; 
	bufferData[2] = 0.0f;
	bufferData[3] = presetPosition[currentPreset].x -  width; 
	bufferData[4] = presetPosition[currentPreset].y; 
	bufferData[5] = 0.0f; 
	bufferData[6] = presetPosition[currentPreset].x;
	bufferData[7] = presetPosition[currentPreset].y - height; 
	bufferData[8] = 0.0f; 

	bufferData[9] = presetPosition[currentPreset].x - width; 
	bufferData[10] = presetPosition[currentPreset].y;
	bufferData[11] = 0.0f;
	bufferData[12] = presetPosition[currentPreset].x; 
	bufferData[13] = presetPosition[currentPreset].y;
	bufferData[14] = 0.0f; 
	bufferData[15] = presetPosition[currentPreset].x; 
	bufferData[16] = presetPosition[currentPreset].y - height;
	bufferData[17] = 0.0f;


	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);	
}

void ImageBox::Draw(const SimpleProgData &simpleData)
{
	glUseProgram(simpleData.theProgram);
	glBindVertexArray(vao);
	{
		glUniform4f(simpleData.colorUnif, 1.0f, 0.0f, 0.0f, 1.0f);
		glUniformMatrix4fv(simpleData.modelToCameraMatrixUnif, 
						   1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

		glEnableVertexAttribArray(simpleData.positionAttrib);		
		glVertexAttribPointer(simpleData.positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);		
		glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_STATIC_DRAW);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}