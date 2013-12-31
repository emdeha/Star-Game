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


#ifndef FE_TEXT_H
#define FE_TEXT_H


#include "../Fusion_Renderer/ShaderManager.h"

#pragma warning(push, 1)
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../glsdk/glm/glm.hpp"

#include <string>
#include <vector>
#pragma warning(pop)


namespace FusionEngine
{
	class Text
	{
	private:
		FT_Library ft;
		FT_Face fontFace;

		glm::vec4 color;
		glm::vec2 position;
		unsigned short fontSize;

		unsigned short windowWidth;
		unsigned short windowHeight;

		float textWidth;

		std::string text;
		
	private:
		GLuint vertexBO;
		GLuint vao;
		GLuint fontProgram;
		GLuint colorUnif;
		GLuint fontTextureUnif;
		GLuint positionAttrib;
		std::vector<GLuint> textureObjects;

		bool hasRenderData;

	public:
		Text() {}
		Text(const std::string &fontName, const std::string &newText,
			 glm::vec2 newPosition, glm::vec4 newColor, unsigned short newFontSize);

		void Init(unsigned short newWindowWidth, unsigned short newWindowHeight);
		void SetPosition(glm::vec2 newPosition, unsigned short newWindowWidth, unsigned short newWindowHeight);
		void CalculateTextWidth(const std::string &textString);

		void Draw() const;

	public:
		float GetWidth() const;
	};
}


#endif