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

	index = 0;

	texture = std::shared_ptr<Texture>(new Texture(GL_TEXTURE_2D, "../data/images/fusion-empty.png"));
}
ImageBox::ImageBox(LayoutPreset newCurrentPreset,
			       const std::string &newName, 
				   glm::vec2 newPosition,
				   float newWidth, float newHeight,
				   int newIndex)
{
	currentPreset = newCurrentPreset;
	presetPosition[currentPreset] = newPosition;
	width = newWidth;
	height = newHeight;
	name = newName;
	isActive = false;

	index = newIndex;

	texture = std::shared_ptr<Texture>(new Texture(GL_TEXTURE_2D, "../data/images/fusion-empty.png"));
}

void ImageBox::Init()
{
	vertexData[0] = presetPosition[currentPreset].x; 
	vertexData[1] = presetPosition[currentPreset].y - height; 
	vertexData[2] = 0.0f; vertexData[3] = 1.0f;

	vertexData[4] = presetPosition[currentPreset].x - width; 
	vertexData[5] = presetPosition[currentPreset].y - height; 
	vertexData[6] = 0.0f; vertexData[7] = 1.0f;

	vertexData[8] = presetPosition[currentPreset].x - width;
	vertexData[9] = presetPosition[currentPreset].y; 
	vertexData[10] = 0.0f; vertexData[11] = 1.0f;

	vertexData[12] = presetPosition[currentPreset].x; 
	vertexData[13] = presetPosition[currentPreset].y;
	vertexData[14] = 0.0f; vertexData[15] = 1.0f;


	textureCoordsData[0] = 0.0f; textureCoordsData[1] = 1.0f;
	textureCoordsData[2] = 1.0f; textureCoordsData[3] = 1.0f;
	textureCoordsData[4] = 1.0f; textureCoordsData[5] = 0.0f;
	textureCoordsData[6] = 0.0f; textureCoordsData[7] = 0.0f;


	indicesData[0] = 0; indicesData[1] = 1; indicesData[2] = 2;
	indicesData[3] = 2; indicesData[4] = 3; indicesData[5] = 0;

	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);	


	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glGenBuffers(1, &textureCoordsBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordsData), textureCoordsData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &indicesBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesData), indicesData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	

	if(!texture->Load())
	{
		std::printf("Error loading texture");
		return;
	}
}


void ImageBox::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("what_event").varString, "fusion_button") == 0)
		{
			std::string button = _event.GetArgument("what_button").varString;

			if(_event.GetArgument("button_pos").varInteger == index)
			{
				if(strcmp(button.c_str(), "q") == 0)
				{
					texture = std::shared_ptr<Texture>(new Texture(GL_TEXTURE_2D, "../data/images/fusion-q.png"));
					if(!texture->Load())
					{
						std::printf("Error loading texture");
						return;
					}
				}
				if(strcmp(button.c_str(), "w") == 0)
				{
					texture = std::shared_ptr<Texture>(new Texture(GL_TEXTURE_2D, "../data/images/fusion-w.png"));
					if(!texture->Load())
					{
						std::printf("Error loading texture");
						return;
					}
				}
				if(strcmp(button.c_str(), "e") == 0)
				{
					texture = std::shared_ptr<Texture>(new Texture(GL_TEXTURE_2D, "../data/images/fusion-e.png"));
					if(!texture->Load())
					{
						std::printf("Error loading texture");
						return;
					}
				}
			}
		}
		if(strcmp(_event.GetArgument("what_event").varString, "fusion_seq") == 0)
		{
			texture = std::shared_ptr<Texture>(new Texture(GL_TEXTURE_2D, "../data/images/fusion-empty.png"));
			if(!texture->Load())
			{
				std::printf("Error loading texture");
				return;
			}
		}
		break;
	}
}


void ImageBox::Draw(const TextureProgData &textureData)
{
	glUseProgram(textureData.theProgram);
	glBindVertexArray(vao);
	{
		glUniformMatrix4fv(textureData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));


		glEnableVertexAttribArray(textureData.positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glVertexAttribPointer(textureData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(textureData.texturePosAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
		glVertexAttribPointer(textureData.texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

		
		texture->Bind(GL_TEXTURE0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glDisableVertexAttribArray(textureData.positionAttrib);
		glDisableVertexAttribArray(textureData.texturePosAttrib);
	}
	glBindVertexArray(0);
	glUseProgram(0);
}