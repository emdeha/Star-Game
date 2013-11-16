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


#ifndef FE_SHADER_MANAGER_H
#define FE_SHADER_MANAGER_H


#include <map>
#include <string>

#include "../glsdk/glload/gl_3_3.h"

#include "ProgramData.h"

namespace FusionEngine
{
	class ShaderManager
	{
	private:
		typedef std::map<ProgramType, ProgramData> ProgramMap;
		typedef std::map<UniformBufferType, unsigned int> UBTMap;
		typedef std::map<BlockType, int> BlockMap;

		ProgramMap loadedPrograms;

		UBTMap uniformBuffers;
		BlockMap blockIndices;

	public:
		ShaderManager() {};

		void Load(ProgramMap newLoadedPrograms,
				  UBTMap newUniformBuffers, BlockMap newBlockIndices);

		void LoadProgram(ProgramType programType, 
						 const std::string &vertexShader, const std::string &fragmentShader);

		ProgramData GetProgram(ProgramType programType) const;

		unsigned int GetUniformBuffer(UniformBufferType ubType) const;
		void SetUniformBuffer(UniformBufferType ubType, unsigned int index);

		int GetBlockIndex(BlockType blockType) const;
	};
}


#endif