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


/// \ingroup module_StarGame

/// \file ProgramData.h
/// \brief Contains the structures holding information for various shader programs.


#ifndef PROGRAM_DATA_H
#define PROGRAM_DATA_H


#include <map>
#include <string>

#include <glload/gl_3_3.h>


struct ProgramData
{
	GLuint theProgram;
	// Replace with associative vector
	std::map<std::string, GLuint> uniforms;
	std::map<std::string, GLuint> attributes;

	// Maybe the buffers needn't be stored in the program data
	std::map<std::string, GLuint> uniformBuffers;
};


struct ParticleProgData
{
	GLuint theProgram;

	GLuint deltaTime_millisecondsUnif;
	GLuint randomTextureUnif;
	GLuint timeUnif;
	GLuint launcherLifetimeUnif;
	GLuint shellLifetimeUnif;
	GLuint secondaryShellLifetimeUnif;

	GLuint particleTypeAttrib;
	GLuint particlePositionAttrib;
	GLuint particleVelocityAttrib;
	GLuint particleAgeAttrib;
};

struct BillboardProgData
{
	GLuint theProgram;

	GLuint positionAttrib;
	GLuint textureCoordAttrib;

	GLuint modelToCameraMatrixUnif;
	GLuint cameraToClipMatrixUnif;
	GLuint cameraPositionUnif;
	GLuint samplerUnif;
	GLuint billboardSizeUnif;
};

struct TextureProgData
{
	GLuint theProgram;

	GLuint colorUnif;

	GLuint modelToCameraMatrixUnif;
	GLuint projectionMatrixUnif;

	GLuint colorTextureUnif;

	GLuint positionAttrib;
	GLuint texturePosAttrib;
};

struct LitProgData
{
	GLuint theProgram;

	GLuint modelToCameraMatrixUnif;

	GLuint lightIntensityUnif;

	GLuint normalModelToCameraMatrixUnif;
	GLuint cameraSpaceLightPosUnif;
};

struct UnlitProgData
{
	GLuint theProgram;

	GLuint objectColorUnif;
	GLuint modelToCameraMatrixUnif;
};

struct SimpleProgData
{
	GLuint theProgram;

	GLuint modelToCameraMatrixUnif;
	GLuint projectionMatrixUnif;

	GLuint colorUnif;

	GLuint positionAttrib;
};

struct FontProgData
{
	GLuint theProgram;

	GLuint textureUnif;
	GLuint colorUnif;

	GLuint projectionMatrixUnif;

	GLuint positionAttrib;
	GLuint textureCoordAttrib;
};

struct SimpleTextureProgData
{
	GLuint theProgram;

	GLuint textureUnif;

	GLuint projectionMatrixUnif;
	GLuint modelToCameraMatrixUnif;

	GLuint positionAttrib;
	GLuint textureCoordAttrib;
	GLuint normalAttrib;
};


#endif