#include "stdafx.h"
#include "Texture.h"


#include "../framework/ErrorAPI.h"


Texture2D::Texture2D()
{
#ifdef FREEIMAGE_LIB
	FreeImage_Initialize();
#endif
}

bool Texture2D::Load(const std::string &fileName, 
                     GLuint storeFormat, GLuint uploadFormat, GLuint componentType,
                     bool repeat)
{
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(0);
    BYTE *bits(0);

    unsigned int width(0), height(0);

    fif = FreeImage_GetFileType(fileName.c_str(), 0);

    if(fif == FIF_UNKNOWN)
    {
        fif = FreeImage_GetFIFFromFilename(fileName.c_str());
    }
    if(fif == FIF_UNKNOWN)
    {
		std::string errorMsg = "Image file format unknown - ";
		errorMsg.append(fileName);
		HandleUnexpectedError(errorMsg, __LINE__, __FILE__);
        return false;
    }

    if(FreeImage_FIFSupportsReading(fif))
    {
        dib = FreeImage_Load(fif, fileName.c_str());
    }

    if(!dib)
    {
		std::string errorMsg = "Could not load image - ";
		errorMsg.append(fileName);
		HandleUnexpectedError(errorMsg, __LINE__, __FILE__);
        return false;
    }

    bits = FreeImage_GetBits(dib);
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);

    if((bits == 0) || (width == 0) || (height == 0))
    {
		std::string errorMsg = "Incorrect image bits, width or height - ";
		errorMsg.append(fileName);
		HandleUnexpectedError(errorMsg, __LINE__, __FILE__);
        return false;
    }


    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &textureObject);
    glBindTexture(GL_TEXTURE_2D, textureObject);

    glTexImage2D(GL_TEXTURE_2D, // texture type
                 0, 
                 storeFormat, // how OpenGL will store the texture data
                 width, // width
                 height, // height
                 0, // must always be 0
                 uploadFormat, // we are uploading four components to the texture
                 componentType, // each component is stored in a single byte
                 bits); // the texture's data


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void Texture2D::Bind(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureObject);
}

void Texture2D::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}