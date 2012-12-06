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
#include <vector>
#include <string>

#include "../ProgramData/ProgramData.h"



// TODO: Dynamic enums for shader variables would speed up the 
//		 generic shader manager



enum BlockType
{
	BT_MATERIAL,
	BT_LIGHT, 
	BT_PROJECTION,
	BT_ORTHOGRAPHIC,
};

enum UniformBufferType
{
	UBT_PROJECTION,
	UBT_ORTHOGRAPHIC, 
	UBT_LIGHT,
};


enum ProgramDataType
{
	PDT_UNIFORM,
	PDT_ATTRIBUTE,
};


class Program
{
private:
	struct ProgramData
	{
		GLuint theProgram;

		std::map<GLenum, std::string> shaderNames;

		std::map<std::string, GLuint> uniformData;
		std::map<std::string, GLuint> attributeData;
	};

	ProgramData data;

public:
	Program();
	
	void AddShader(GLenum shaderType, const std::string &shaderFileName);

	void AddDataElement(ProgramDataType whatData,
						const std::string &dataElementName);

	void SetData(ProgramDataType whatData,
				 const std::map<std::string, GLuint> &newData);

	std::map<std::string, GLuint> GetData();
	GLuint GetProgramObject()
	{
		return data.theProgram;
	}

	
	void BuildProgram();
};


class ShaderProgramContainer
{
private:
	std::map<std::string, Program> shaderPrograms;

public:
	ShaderProgramContainer();


	void AddProgram(const std::string &newProgramName, const Program &newProgram);


	void LoadProgram(const std::string &programName);

	Program GetProgram(const std::string &programName);
};





class ShaderManager
{
private:
	std::map<BlockType, int> blockIndices;
	std::map<UniformBufferType, unsigned int> uniformBuffers; 

	std::map<std::string, ProgramData> programDatas;

	LitProgData litData;
	UnlitProgData unlitData;
	SimpleProgData simpleData;
	SimpleProgData simpleNoUBData;
	FontProgData fontData;
	SimpleTextureProgData simpleTextureProgData;
	TextureProgData textureProgData;
	TextureProgData perspTextureProgData;
	BillboardProgDataNoTexture billboardProgDataNoTexture;
	BillboardProgData billboardProgData;
	ParticleProgData particleProgData;

public:
	ShaderManager();

	void LoadProgram(const std::string &vertexShader, const std::string &fragmentShader,
					 const std::string &programDataType);

	void AddUniform(const std::string &programDataType, 
					const std::string &uniformName, 
					const std::string &uniformNameInShader);
	void AddAttrubute(const std::string &programDataType,
					  const std::string &attributeName, 
					  const std::string &attrubuteNameInShader);
	
	void AddUniformBlock(const std::string &programDataType, 
						 const std::string &uniformBlockName, 
						 const std::string &uniformBlockNameInShader,
						 BlockType blockType);
	
	ProgramData GetProgramData(const std::string &programDataType);







	void LoadLitProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadUnlitProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadSimpleProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadSimpleNoUBProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadFontProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadSimpleTextureProgData(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadTextureProgData(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadPerspectiveTextureProgData(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadBillboardProgDataNoTexture(const std::string &vertexShader,
										const std::string &geometryShader,
										const std::string &fragmentShader);
	void LoadBillboardProgData(const std::string &vertexShader, 
							   const std::string &geometryShader,
							   const std::string &fragmentShader);
	void LoadParticleProgData(const std::string &vertexShader,
							  const std::string &geometryShader);


	LitProgData GetLitProgData();
	UnlitProgData GetUnlitProgData();
	SimpleProgData GetSimpleProgData();
	SimpleProgData GetSimpleNoUBProgData();
	FontProgData GetFontProgData();
	SimpleTextureProgData GetSimpleTextureProgData();
	TextureProgData GetTextureProgData();
	TextureProgData GetPerspectiveTextureProgData();
	BillboardProgDataNoTexture GetBillboardProgDataNoTexture();
	BillboardProgData GetBillboardProgData();
	ParticleProgData GetParticleProgData();


	const int GetBlockIndex(BlockType);
	unsigned int GetUniformBuffer(UniformBufferType);
	void SetUniformBuffer(UniformBufferType, unsigned int value);
};



#endif