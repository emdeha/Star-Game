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


#ifndef TEXTURE_H
#define TEXTURE_H


#include <string>

#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <glload/gll.hpp>

#include <FreeImage.h>


class Texture
{
private:
	GLuint textureObject;
	GLuint samplerObject;

public:
	Texture();
		
	// Maybe the texture loading characteristics must be kept away from the user.
	// Later, I will develop a single format which will have default characteristics.
	bool Load(const std::string &fileName,
			  GLenum textureStorageFormat, 
			  GLenum textureComponents, 
			  GLenum textureComponentsStorageFormat);
	void CreateSamplers();

	void Bind(GLenum textureUnit);
	void Unbind(GLenum textureUnit);
};

#endif