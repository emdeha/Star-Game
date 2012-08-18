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

}

Text::Text(const char *fontName)
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

	textMinWidth = 3.0f;
	textMaxWidth = 0.0f;
	textMinHeight = 3.0f;
	textMaxHeight = 0.0f;
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
}

void Text::Print(const char *text, const FontProgData &fontData,
				 glm::vec2 position,
				 glm::vec4 color,
				 int fontSize)
{	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(fontData.theProgram);
	glBindVertexArray(vao);

	FT_Set_Pixel_Sizes(fontFace, 0, fontSize);
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
		if(FT_Load_Char(fontFace, *p, FT_LOAD_RENDER))
		{
			continue;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows,
					 0, GL_RED, GL_UNSIGNED_BYTE, fontFace->glyph->bitmap.buffer);

		glm::vec4 finalCoordinates;
		glm::vec2 scale(2.0f / windowWidth, 2.0f / windowHeight);
		finalCoordinates.x = position.x + fontFace->glyph->bitmap_left * scale.x;
		finalCoordinates.y = -position.y - fontFace->glyph->bitmap_top * scale.y;

		finalCoordinates.z = fontFace->glyph->bitmap.width * scale.x;
		finalCoordinates.w = fontFace->glyph->bitmap.rows * scale.y;

		const GLfloat bufferData[16] = 
		{
			finalCoordinates.x,						 -finalCoordinates.y,					   0.0f, 0.0f,
			finalCoordinates.x + finalCoordinates.z, -finalCoordinates.y,					   1.0f, 0.0f,
			finalCoordinates.x,						 -finalCoordinates.y - finalCoordinates.w, 0.0f, 1.0f,
			finalCoordinates.x + finalCoordinates.z, -finalCoordinates.y - finalCoordinates.w, 1.0f, 1.0f,
		};
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(fontData.positionAttrib);	
		glVertexAttribPointer(fontData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		position.x += (fontFace->glyph->advance.x >> 6) * scale.x;
		position.y += (fontFace->glyph->advance.y >> 6) * scale.y;
	}

	glDisableVertexAttribArray(fontData.positionAttrib);
	glDeleteTextures(1, &texture);

	glUseProgram(0);

	glDisable(GL_BLEND);
}


void Text::ComputeTextDimensions(const char *text, glm::vec2 position, int fontSize)
{
	FT_Set_Pixel_Sizes(fontFace, 0, fontSize);

	for(const char *p = text; *p; p++)
	{
		if(FT_Load_Char(fontFace, *p, FT_LOAD_RENDER))
		{
			continue;
		}

		glm::vec4 finalCoordinates = glm::vec4();
		glm::vec2 scale(2.0f / windowWidth, 2.0f / windowHeight);
		finalCoordinates.x = position.x + fontFace->glyph->bitmap_left * scale.x;
		finalCoordinates.y = -position.y - fontFace->glyph->bitmap_top * scale.y;

		finalCoordinates.z = fontFace->glyph->bitmap.width * scale.x;
		finalCoordinates.w = fontFace->glyph->bitmap.rows * scale.y;

		position.x += (fontFace->glyph->advance.x >> 6) * scale.x;
		position.y += (fontFace->glyph->advance.y >> 6) * scale.y;

		if(textMinHeight > -finalCoordinates.y - finalCoordinates.w)
			textMinHeight = -finalCoordinates.y - finalCoordinates.w;
		if(textMaxHeight < -finalCoordinates.y)
			textMaxHeight = -finalCoordinates.y;

		if(textMaxWidth < finalCoordinates.x + finalCoordinates.z)
			textMaxWidth = finalCoordinates.x + finalCoordinates.z;
		if(textMinWidth > finalCoordinates.x)
			textMinWidth = finalCoordinates.x;
	}
};