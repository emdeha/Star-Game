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


#ifndef FE_TEXTURE_H
#define FE_TEXTURE_H


#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glutil/glutil.h"
#include "../glsdk/glload/gll.hpp"

#include "../freeimage/FreeImage.h"


class Texture2D
{
private: 
	GLuint textureObject;

public:
	Texture2D();

	// store = GL_RGB, uploadFormat = GL_BGR, componentType = GL_UNSIGNED_BYTE
	bool Load(const std::string &fileName,
			  GLuint store, GLuint uploadFormat, GLuint componentType, bool repeat);

	void Bind(GLenum textureUnit);
	void Unbind();
};


#endif