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


#ifndef FE_SPRITE_H
#define FE_SPRITE_H


#include <memory>

#include "Texture.h"


namespace FusionEngine
{
	class Sprite2D
	{
	private:
		glm::vec2 position;

		float width;
		float height;

		std::shared_ptr<Texture2D> texture;

	private:
		GLuint vertexBO;
		GLuint indexBO;
		GLuint texCoordsBO;
		GLuint vao;

		GLuint programId;
		GLuint modelToCameraMatrixUnif;
		GLuint colorUnif;
		GLuint positionAttrib;
		GLuint texCoordAttrib;

	public:
		Sprite2D() {}

		Sprite2D(glm::vec2 newPosition, float newWidth, float newHeight)
			: position(newPosition), width(newWidth), height(newHeight) 
		{
			texture = std::shared_ptr<Texture2D>(new Texture2D());
		}

		void Init(const std::string &textureFileName);

		void SetPosition(glm::vec2 newPosition);

		// TODO: Should be handled by the Fusion_Renderer
		void Draw(glutil::MatrixStack &modelMatrix);
	};
}


#endif