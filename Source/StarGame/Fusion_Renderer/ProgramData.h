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


#pragma warning(push, 1)
#include <map>
#include <sstream>

#include "../glsdk/glload/gl_3_3.h"
#pragma warning(pop)

#include "../framework/ErrorAPI.h"
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
			if (foundUniform == uniforms.end())
			{
				std::ostringstream errorMsg;
				errorMsg << "No such uniform: " << (int)uniformType << ".\n";
				HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
				return 0;
			}
			return (*foundUniform).second;
		}
		GLuint GetAttrib(AttribType attribType) const
		{
			auto foundAttrib = attribs.find(attribType);
			if (foundAttrib == attribs.end())
			{
				std::ostringstream errorMsg;
				errorMsg << "No such attrib: " << (int)attribType << ".\n";
				HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
				return 0;
			}
			return (*foundAttrib).second;
		}
	};
}


#endif