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

// TODO: Should be 'SimpleProgData' or something like that
struct SimpleProgData
{
	GLuint theProgram;

	GLuint modelToCameraMatrixUnif;
	GLuint colorUnif;

	GLuint positionAttrib;
};

struct FontProgData
{
	GLuint theProgram;

	GLuint textureUnif;
	GLuint colorUnif;

	GLuint positionAttrib;
	GLuint textureCoordAttrib;
};


#endif