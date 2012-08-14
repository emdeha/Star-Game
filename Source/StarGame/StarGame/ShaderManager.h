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


#include <map>
#include <string>

#include "../ProgramData/ProgramData.h"


enum BlockType
{
	BT_MATERIAL,
	BT_LIGHT, 
	BT_PROJECTION,
};

enum UniformBufferType
{
	UBT_PROJECTION,
	UBT_LIGHT,
};


class ShaderManager
{
private:
	std::map<BlockType, int> blockIndices;
	std::map<UniformBufferType, unsigned int> uniformBuffers; 
	
	LitProgData litData;
	UnlitProgData unlitData;
	SimpleProgData simpleData;
	FontProgData fontData;

public:
	ShaderManager();

	void LoadLitProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadUnlitProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadSimpleProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadFontProgram(const std::string &vertexShader, const std::string &fragmentShader);

	LitProgData GetLitProgData();
	UnlitProgData GetUnlitProgData();
	SimpleProgData GetSimpleProgData();
	FontProgData GetFontProgData();

	const int GetBlockIndex(BlockType);
	unsigned int GetUniformBuffer(UniformBufferType);
	void SetUniformBuffer(UniformBufferType, unsigned int value);
};



#endif