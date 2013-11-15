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


#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H


#include <unordered_map>
#include <string>

#include "../glsdk/glload/gl_3_3.h"

#include "../Fusion_AssetLoader/AssetLoader.h"
#include "ShaderEnums.h"

namespace FusionEngine
{
	class ShaderManager
	{
	private:
		std::map<ProgramType, ProgramData> loadedPrograms;

		std::map<UniformBufferType, unsigned int> uniformBuffers;
		std::map<BlockType, int> blockIndices;

	public:
		ShaderManager();

		void Load(const ShaderAssetObject &loaderData); // Implement

		void LoadProgram(ProgramType programType, 
						 const std::string &vertexShader, const std::string &fragmentShader);

		ProgramData GetProgram(ProgramType programType);
	};
}


#endif