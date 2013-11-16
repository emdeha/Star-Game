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


#ifndef FE_PROGRAM_DATA_H
#define FE_PROGRAM_DATA_H


#include <map>

#include "../glsdk/glload/gl_3_3.h"

#include "ShaderEnums.h"


namespace FusionEngine
{
	struct ProgramData
	{
		GLuint programId;

		std::map<UniformType, GLuint> uniforms;
		std::map<AttribType, GLuint> attribs;

		GLuint GetUniform(UniformType uniformType) const
		{
			auto foundUniform = uniforms.find(uniformType);
			return (*foundUniform).second;
		}
		GLuint GetAttrib(AttribType attribType) const
		{
			auto foundAttrib = attribs.find(attribType);
			return (*foundAttrib).second;
		}
	};
}


#endif