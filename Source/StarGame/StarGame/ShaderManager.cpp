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


#include "ShaderManager.h"

#include <vector>
#include <glload/_int_gl_3_3.h>

#include "framework.h"


ShaderManager::ShaderManager()
{
	blockIndices.insert(std::make_pair(BT_MATERIAL, 0));
	blockIndices.insert(std::make_pair(BT_LIGHT, 1));
	blockIndices.insert(std::make_pair(BT_PROJECTION, 2));

	uniformBuffers.insert(std::make_pair(UBT_PROJECTION, 0));
	uniformBuffers.insert(std::make_pair(UBT_LIGHT, 0));
}

void ShaderManager::LoadLitProgram(const std::string &vertexShader, 
								   const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	litData.theProgram = Framework::CreateProgram(shaderList);
	litData.modelToCameraMatrixUnif = glGetUniformLocation(litData.theProgram, "modelToCameraMatrix");
	litData.lightIntensityUnif = glGetUniformLocation(litData.theProgram, "lightIntensity");

	litData.normalModelToCameraMatrixUnif = glGetUniformLocation(litData.theProgram, "normalModelToCameraMatrix");
	litData.cameraSpaceLightPosUnif = glGetUniformLocation(litData.theProgram, "cameraSpaceLightPos");

	GLuint materialBlock = glGetUniformBlockIndex(litData.theProgram, "Material");
	GLuint lightBlock = glGetUniformBlockIndex(litData.theProgram, "Light");
	GLuint projectionBlock = glGetUniformBlockIndex(litData.theProgram, "Projection");

	if(materialBlock != GL_INVALID_INDEX)
		glUniformBlockBinding(litData.theProgram, materialBlock, blockIndices[BT_MATERIAL]);

	glUniformBlockBinding(litData.theProgram, lightBlock, blockIndices[BT_LIGHT]);
	glUniformBlockBinding(litData.theProgram, projectionBlock, blockIndices[BT_PROJECTION]);
}

void ShaderManager::LoadUnlitProgram(const std::string &vertexShader, 
								     const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	unlitData.theProgram = Framework::CreateProgram(shaderList);
	unlitData.modelToCameraMatrixUnif = glGetUniformLocation(unlitData.theProgram, "modelToCameraMatrix");
	unlitData.objectColorUnif = glGetUniformLocation(unlitData.theProgram, "objectColor");

	GLuint projectionBlock = glGetUniformBlockIndex(unlitData.theProgram, "Projection");
	glUniformBlockBinding(unlitData.theProgram, projectionBlock, blockIndices[BT_PROJECTION]);
}

void ShaderManager::LoadSimpleProgram(const std::string &vertexShader, 
								      const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	simpleData.theProgram = Framework::CreateProgram(shaderList);
	simpleData.modelToCameraMatrixUnif = glGetUniformLocation(simpleData.theProgram, "modelToCameraMatrix");
	simpleData.colorUnif = glGetUniformLocation(simpleData.theProgram, "color");

	simpleData.positionAttrib = glGetAttribLocation(simpleData.theProgram, "position");

	GLuint projectionBlock = glGetUniformBlockIndex(simpleData.theProgram, "Projection");	
	glUniformBlockBinding(simpleData.theProgram, projectionBlock, blockIndices[BT_PROJECTION]);
}

void ShaderManager::LoadFontProgram(const std::string &vertexShader, 
								    const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	fontData.theProgram = Framework::CreateProgram(shaderList);
	fontData.colorUnif = glGetUniformLocation(fontData.theProgram, "fontColor");
	fontData.textureUnif = glGetUniformLocation(fontData.theProgram, "fontTexture");

	fontData.positionAttrib = glGetAttribLocation(fontData.theProgram, "position");
	//fontData.textureCoordAttrib = glGetAttribLocation(fontData.theProgram, "textureCoordinates");
}


LitProgData ShaderManager::GetLitProgData()
{
	return litData;
}
UnlitProgData ShaderManager::GetUnlitProgData()
{
	return unlitData;
}
SimpleProgData ShaderManager::GetSimpleProgData()
{
	return simpleData;
}
FontProgData ShaderManager::GetFontProgData()
{
	return fontData;
}


const int ShaderManager::GetBlockIndex(BlockType blockType)
{
	if(blockIndices.find(blockType) != blockIndices.end())
		return blockIndices[blockType];

	else 
	{
		std::printf("The block doesn't exist\n");
		return 0;
	}
}
unsigned int ShaderManager::GetUniformBuffer(UniformBufferType uniformBufferType)
{
	if(uniformBuffers.find(uniformBufferType) != uniformBuffers.end())
		return uniformBuffers[uniformBufferType];

	else
	{
		std::printf("The uniform buffer doesn't exist\n");
		return 0;
	}
}
void ShaderManager::SetUniformBuffer(UniformBufferType uniformBufferType,
									 unsigned int value)
{
	if(uniformBuffers.find(uniformBufferType) != uniformBuffers.end())
		uniformBuffers[uniformBufferType] = value;

	else 
	{
		std::printf("The uniform buffer doesn't exist\n");
		return;
	}
}