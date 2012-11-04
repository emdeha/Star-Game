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


// TODO: Make it check what kind of file to load.
// TODO: Add a destructor

#include "stdafx.h"
#include "Texture.h"


Texture::Texture(GLenum newTextureTarget, const std::string &newFileName)
{
#ifdef FREEIMAGE_LIB
	FreeImage_Initialize();
#endif

	textureTarget = newTextureTarget;
	fileName = newFileName;
}

bool Texture::Load()
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib(0);
	BYTE *bits(0);

	unsigned int width(0), height(0);

	fif = FreeImage_GetFileType(fileName.c_str(), 0);

	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(fileName.c_str());
	if(fif == FIF_UNKNOWN)
		return false;

	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, fileName.c_str());

	if(!dib)
		return false;

	bits = FreeImage_GetBits(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);

	if((bits == 0) || (width == 0) || (height == 0))
		return false;


	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureObject);
	glBindTexture(textureTarget, textureObject);


	glTexImage2D(textureTarget, 0, GL_RGBA, 
			     width, height, 0, 
				 GL_BGRA, GL_UNSIGNED_BYTE, bits);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	return true;
}

void Texture::Bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(textureTarget, textureObject);
}