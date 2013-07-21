//Copyright (C) 2010-2012 by Jason L. McKesson
//This file is licensed under the MIT License.


#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <string.h>
#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glload/gll.hpp"
#include "../glsdk/glutil/Shader.h"
#include "../glsdk/freeglut/include/GL/freeglut.h"
#include "framework.h"
#include "directories.h"

#ifdef LOAD_X11
#define APIENTRY
#endif

namespace Framework
{
	GLuint LoadShader(GLenum eShaderType, const std::string &strShaderFilename)
	{
		std::string strFilename = FindFileOrThrow(strShaderFilename);
		std::ifstream shaderFile(strFilename.c_str());
		std::stringstream shaderData;
		shaderData << shaderFile.rdbuf();
		shaderFile.close();

		try
		{
			return glutil::CompileShader(eShaderType, shaderData.str());
		}
		catch(std::exception &e)
		{
			fprintf(stderr, "%s\n", e.what());
			throw;
		}
	}

	GLuint CreateProgram(const std::vector<GLuint> &shaderList)
	{
		try
		{
			GLuint prog = glutil::LinkProgram(shaderList);
			std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
			return prog;
		}
		catch(std::exception &e)
		{
			std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
			fprintf(stderr, "%s\n", e.what());
			throw;
		}
	}

	float DegToRad(float fAngDeg)
	{
		const float fDegToRad = 3.14159f * 2.0f / 360.0f;
		return fAngDeg * fDegToRad;
	}

	std::string FindFileOrThrow( const std::string &strBasename )
	{
		std::string strFilename = LOCAL_FILE_DIR + strBasename;
		std::ifstream testFile(strFilename.c_str());
		if(testFile.is_open())
			return strFilename;

		
		strFilename = GLOBAL_FILE_DIR + strBasename;
		testFile.open(strFilename.c_str());
		if(testFile.is_open())
			return strFilename;

		throw std::runtime_error("Could not find the file " + strBasename);
	}
}