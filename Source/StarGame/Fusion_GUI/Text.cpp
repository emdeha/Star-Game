#include "stdafx.h"
#include "Text.h"

#include "../Fusion_Scene/World.h"
#include "../framework/ErrorAPI.h"

#pragma warning(push, 1)
#include "../glsdk/glm/gtc/type_ptr.hpp"
#pragma warning(pop)


using namespace FusionEngine;

Text::Text(const std::string &fontName, const std::string &newText,
		   glm::vec2 newPosition, glm::vec4 newColor, unsigned short newFontSize)
		   : position(newPosition), color(newColor), fontSize(newFontSize), text(newText), 
		     hasRenderData(true), textWidth(0.0f)
{
	if (FT_Init_FreeType(&ft))
	{
		HandleUnexpectedError("could not initialize FreeType library", __LINE__, __FILE__);
		return;
	}

	if (FT_New_Face(ft, fontName.c_str(), 0, &fontFace))
	{
		std::string errorMessage = "could not open font file ";
		errorMessage += fontName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return;
	}
}

void Text::Init(unsigned short newWindowWidth, unsigned short newWindowHeight)
{
	FusionEngine::ProgramData fontProgramData = GetWorld().GetShaderManager().GetProgram(FE_PROGRAM_FONT);
	fontProgram = fontProgramData.programId;
	colorUnif = fontProgramData.GetUniform(FE_UNIFORM_FONT_COLOR);
	fontTextureUnif = fontProgramData.GetUniform(FE_UNIFORM_FONT_TEXTURE);
	positionAttrib = fontProgramData.GetAttrib(FE_ATTRIB_POSITION);

	glUseProgram(fontProgram);

	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	// TODO: Use it later
	// FT_Set_Char_Size(fontFace, 0, fontSize * 64, windowWidth, windowHeight);

	std::vector<GLfloat> textBufferData;
	glm::vec2 unmodifiedPosition = position;

	FT_Set_Pixel_Sizes(fontFace, 0, fontSize);
	glm::vec4 finalCoords;
	for (const char *p = text.c_str(); *p; p++)
	{
		if (FT_Load_Char(fontFace, *p, FT_LOAD_RENDER))
		{
			continue;
		}

		GLuint textureObject;
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &textureObject);
		glBindTexture(GL_TEXTURE_2D, textureObject);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows,
					 0, GL_RED, GL_UNSIGNED_BYTE, fontFace->glyph->bitmap.buffer);

		textureObjects.push_back(textureObject);

		finalCoords.x = (position.x - fontFace->glyph->bitmap_left);
		finalCoords.y = (position.y - fontFace->glyph->bitmap_top);
		finalCoords.z = -(float)fontFace->glyph->bitmap.width;
		finalCoords.w = -(float)fontFace->glyph->bitmap.rows;

		glm::vec2 firstVertex = glm::vec2(finalCoords.x, finalCoords.y);
		glm::vec2 secondVertex = glm::vec2(finalCoords.x + finalCoords.z, finalCoords.y);
		glm::vec2 thirdVertex = glm::vec2(finalCoords.x, finalCoords.y - finalCoords.w);
		glm::vec2 fourthVertex = glm::vec2(finalCoords.x + finalCoords.z, finalCoords.y - finalCoords.w);

		textBufferData.push_back(firstVertex.x); textBufferData.push_back(firstVertex.y);
		textBufferData.push_back(0.0f); textBufferData.push_back(0.0f);

		textBufferData.push_back(secondVertex.x); textBufferData.push_back(secondVertex.y);
		textBufferData.push_back(1.0f); textBufferData.push_back(0.0f);
		
		textBufferData.push_back(thirdVertex.x); textBufferData.push_back(thirdVertex.y);
		textBufferData.push_back(0.0f); textBufferData.push_back(1.0f);

		textBufferData.push_back(fourthVertex.x); textBufferData.push_back(fourthVertex.y);
		textBufferData.push_back(1.0f); textBufferData.push_back(1.0f);

		position.x -= (fontFace->glyph->advance.x >> 6);
		position.y -= (fontFace->glyph->advance.y >> 6);
	}

	glUseProgram(0);

	if (textBufferData.size() > 0)
	{
		textWidth = finalCoords.x + finalCoords.z - unmodifiedPosition.x;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vertexBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * textBufferData.size(), 
					 &textBufferData[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		hasRenderData = false;
	}
}

void Text::SetPosition(glm::vec2 newPosition, unsigned short newWindowWidth, unsigned short newWindowHeight)
{
	position = newPosition;
	std::vector<GLfloat> textBufferData;

	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	// TODO: Use it later
	// FT_Set_Char_Size(fontFace, 0, fontSize * 64, windowWidth, windowHeight);
	glm::vec2 unmodifiedPosition = position;

	FT_Set_Pixel_Sizes(fontFace, 0, fontSize);
	glm::vec4 finalCoords;
	for (const char *p = text.c_str(); *p; p++)
	{
		if (FT_Load_Char(fontFace, *p, FT_LOAD_RENDER))
		{
			continue;
		}

		finalCoords.x = (position.x - fontFace->glyph->bitmap_left);
		finalCoords.y = (position.y - fontFace->glyph->bitmap_top);
		finalCoords.z = -(float)fontFace->glyph->bitmap.width;
		finalCoords.w = -(float)fontFace->glyph->bitmap.rows;

		glm::vec2 firstVertex = glm::vec2(finalCoords.x, finalCoords.y);
		glm::vec2 secondVertex = glm::vec2(finalCoords.x + finalCoords.z, finalCoords.y);
		glm::vec2 thirdVertex = glm::vec2(finalCoords.x, finalCoords.y - finalCoords.w);
		glm::vec2 fourthVertex = glm::vec2(finalCoords.x + finalCoords.z, finalCoords.y - finalCoords.w);

		textBufferData.push_back(firstVertex.x); textBufferData.push_back(firstVertex.y);
		textBufferData.push_back(0.0f); textBufferData.push_back(0.0f);

		textBufferData.push_back(secondVertex.x); textBufferData.push_back(secondVertex.y);
		textBufferData.push_back(1.0f); textBufferData.push_back(0.0f);
		
		textBufferData.push_back(thirdVertex.x); textBufferData.push_back(thirdVertex.y);
		textBufferData.push_back(0.0f); textBufferData.push_back(1.0f);

		textBufferData.push_back(fourthVertex.x); textBufferData.push_back(fourthVertex.y);
		textBufferData.push_back(1.0f); textBufferData.push_back(1.0f);

		position.x -= (fontFace->glyph->advance.x >> 6);
		position.y -= (fontFace->glyph->advance.y >> 6);
	}

	if (textBufferData.size() > 0)
	{
		textWidth = finalCoords.x + finalCoords.z - unmodifiedPosition.x;

		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * textBufferData.size(), &textBufferData[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		hasRenderData = false;
	}
}

void Text::CalculateTextWidth(const std::string &textString)
{
	glm::vec2 unmodifiedPosition = position;

	FT_Set_Pixel_Sizes(fontFace, 0, fontSize);
	glm::vec4 finalCoords;
	for (const char *p = textString.c_str(); *p; p++)
	{
		if (FT_Load_Char(fontFace, *p, FT_LOAD_RENDER))
		{
			continue;
		}

		finalCoords.x = (position.x - fontFace->glyph->bitmap_left);
		finalCoords.y = (position.y - fontFace->glyph->bitmap_top);
		finalCoords.z = -(float)fontFace->glyph->bitmap.width;
		finalCoords.w = -(float)fontFace->glyph->bitmap.rows;

		position.x -= (fontFace->glyph->advance.x >> 6);
		position.y -= (fontFace->glyph->advance.y >> 6);
	}

	textWidth = finalCoords.x + finalCoords.z - unmodifiedPosition.x;
}

void Text::Draw() const
{
	if (hasRenderData)
	{
		glDisable(GL_CULL_FACE); 
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(fontProgram);
		glBindVertexArray(vao);
		{
			glUniform4fv(colorUnif, 1, glm::value_ptr(color));

			glEnableVertexAttribArray(positionAttrib);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
			glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

			for (size_t i = 0; i < textureObjects.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureObjects[i]);

				glDrawArrays(GL_TRIANGLE_STRIP, 4 * i, 4);
			}
		}
		glDisableVertexAttribArray(positionAttrib);

		glUseProgram(0);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
	}
}

float Text::GetWidth() const
{
	return textWidth;
}