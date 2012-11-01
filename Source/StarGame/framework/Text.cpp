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


#include "Text.h"
#include <glm/gtc/type_ptr.hpp>


Text::Text()
{	
	isBold = false;
}

Text::Text(const char *fontName/*, const char *fontName_bold*/)
{
	if(FT_Init_FreeType(&ft))
	{
		std::printf("Could not initialize the FreeType library.\n");
		return;
	}

	if(FT_New_Face(ft, fontName, 0, &fontFace))
	{
		std::printf("Could not open font file.\n");
		return;
	}	
	if(FT_New_Face(ft, "../data/fonts/AGENCYB.TTF", 0, &fontFace_bold))
	{
		std::printf("Could not open font file.\n");
		return;
	}

	isBold = false;

	textMinWidth = 999.0f;
	textMaxWidth = -999.0f;
	textMinHeight = 999.0f;
	textMaxHeight = -999.0f;
}


void Text::Init(int newWindowWidth, int newWindowHeight)
{
	glGenBuffers(1, &vbo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;
}

void Text::UpdateWindowDimensions(int newWindowWidth, int newWindowHeight)
{
	windowHeight = newWindowHeight;
	windowWidth = newWindowWidth;

	textMinWidth = 9999.0f;
	textMaxWidth = -9999.0f;
	textMinHeight = 9999.0f;
	textMaxHeight = -9999.0f;
}

void Text::Print(const char *text, const FontProgData &fontData,
				 glm::vec2 position,
				 glm::vec4 color,
				 int fontSize)
{
	FT_Face fontFaceToUse = fontFace;
	if(isBold)
	{
		fontFaceToUse = fontFace_bold;
	}
	else
	{
		fontFaceToUse = fontFace;
	}

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(fontData.theProgram);
	glBindVertexArray(vao);

	FT_Set_Pixel_Sizes(fontFaceToUse, 0, fontSize);
	glUniform4fv(fontData.colorUnif, 1, glm::value_ptr(color));

	GLuint texture;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(fontData.textureUnif, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	for(const char *p = text; *p; p++)
	{
		if(FT_Load_Char(fontFaceToUse, *p, FT_LOAD_RENDER))
		{
			continue;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontFaceToUse->glyph->bitmap.width, fontFaceToUse->glyph->bitmap.rows,
					 0, GL_RED, GL_UNSIGNED_BYTE, fontFaceToUse->glyph->bitmap.buffer);

		glm::vec4 finalCoordinates;
		finalCoordinates.x = (position.x + fontFaceToUse->glyph->bitmap_left);
		finalCoordinates.y = (position.y + fontFaceToUse->glyph->bitmap_top);

		finalCoordinates.z = fontFaceToUse->glyph->bitmap.width;
		finalCoordinates.w = fontFaceToUse->glyph->bitmap.rows;
		

		glm::vec2 firstVertex = glm::vec2(finalCoordinates.x,
										  finalCoordinates.y);
		glm::vec2 secondVertex = glm::vec2(finalCoordinates.x + finalCoordinates.z,
										   finalCoordinates.y);
		glm::vec2 thirdVertex = glm::vec2(finalCoordinates.x,
										  finalCoordinates.y - finalCoordinates.w);
		glm::vec2 fourthVertex = glm::vec2(finalCoordinates.x + finalCoordinates.z,
										   finalCoordinates.y - finalCoordinates.w);

		const GLfloat bufferData[16] = 
		{
			windowWidth - firstVertex.x, windowHeight - firstVertex.y, 0.0f, 0.0f,
			windowWidth - secondVertex.x, windowHeight - secondVertex.y, 1.0f, 0.0f,
			windowWidth - thirdVertex.x, windowHeight - thirdVertex.y, 0.0f, 1.0f,
			windowWidth - fourthVertex.x, windowHeight - fourthVertex.y, 1.0f, 1.0f,
		};

		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(fontData.positionAttrib);	
		glVertexAttribPointer(fontData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		position.x += (fontFaceToUse->glyph->advance.x >> 6);
		position.y += (fontFaceToUse->glyph->advance.y >> 6);
	}

	glDisableVertexAttribArray(fontData.positionAttrib);
	glDeleteTextures(1, &texture);

	glUseProgram(0);

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void Text::ComputeTextDimensions(const char *text, glm::vec2 position, int fontSize)
{
	FT_Face fontFaceToUse = fontFace;
	if(isBold)
	{
		fontFaceToUse = fontFace_bold;
	}
	else
	{
		fontFaceToUse = fontFace;
	}


	FT_Set_Pixel_Sizes(fontFaceToUse, 0, fontSize);
	
	glm::vec4 finalCoordinates;

	for(const char *p = text; *p; p++)
	{
		if(FT_Load_Char(fontFaceToUse, *p, FT_LOAD_RENDER))
		{
			continue;
		}

		finalCoordinates.x = position.x + fontFaceToUse->glyph->bitmap_left;
		finalCoordinates.y = position.y + fontFaceToUse->glyph->bitmap_top;

		finalCoordinates.z = fontFaceToUse->glyph->bitmap.width;
		finalCoordinates.w = fontFaceToUse->glyph->bitmap.rows;

		position.x += (fontFaceToUse->glyph->advance.x >> 6);
		position.y += (fontFaceToUse->glyph->advance.y >> 6);

		glm::vec2 minVertex;
		minVertex.x = (finalCoordinates.x );
		minVertex.y = (finalCoordinates.y - finalCoordinates.w);

		glm::vec2 maxVertex;
		maxVertex.y = (finalCoordinates.y);


		if(minVertex.y < textMinHeight)
			textMinHeight = minVertex.y;
		if(maxVertex.y >= textMaxHeight)
			textMaxHeight = maxVertex.y;

		if(minVertex.x < textMinWidth)
			textMinWidth = minVertex.x;
		if(maxVertex.x > textMaxWidth)
			textMaxWidth = maxVertex.x;
	}

	textMaxWidth = windowWidth - (finalCoordinates.x + finalCoordinates.z);
	textMaxHeight = windowHeight - textMaxHeight;

	textMinWidth = windowWidth - textMinWidth;
	textMinHeight = windowHeight - textMinHeight;
};


/// This function is not tested!!!
glm::vec2 Text::ComputeCharacterDimensions(char ch, glm::vec2 position, int fontSize)
{
	FT_Face fontFaceToUse = fontFace;
	if(isBold)
	{
		fontFaceToUse = fontFace_bold;
	}
	else
	{
		fontFaceToUse = fontFace;
	}

	FT_Set_Pixel_Sizes(fontFaceToUse, 0, fontSize);

	FT_Load_Char(fontFaceToUse, ch, FT_LOAD_RENDER);

	glm::vec2 maxCorner;
	maxCorner.x = windowWidth - (position.x + fontFaceToUse->glyph->bitmap_left + 
								 fontFaceToUse->glyph->bitmap.width);
	maxCorner.y = windowHeight - (position.y + fontFaceToUse->glyph->bitmap_top);

	return maxCorner;
}