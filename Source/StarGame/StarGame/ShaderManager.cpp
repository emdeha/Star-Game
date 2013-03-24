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
#include "../framework/ErrorAPI.h"


// TODO: Check if each uniform and attribute location is gathered correctly


Program::Program()
{
	data.theProgram = 0;
	data.uniformData.clear();
	data.attributeData.clear();
}

void Program::AddShader(GLenum shaderType, const std::string &shaderFileName)
{
	data.shaderNames.insert(std::make_pair(shaderType, shaderFileName));
}

void Program::AddDataElement(ProgramDataType whatData, 
							 const std::string &dataElementName)
{
	switch(whatData)
	{
	case PDT_UNIFORM:
		data.uniformData.insert(std::make_pair(dataElementName, 0));
		break;
	case PDT_ATTRIBUTE:
		data.attributeData.insert(std::make_pair(dataElementName, 0));
		break;
	default:
		// Handle errors.
		return;
	}
}

void Program::SetData(ProgramDataType whatData,
					  const std::map<std::string, GLuint> &newData)
{
	switch(whatData)
	{
	case PDT_UNIFORM:
		data.uniformData.insert(newData.begin(), newData.end());
		break;
	case PDT_ATTRIBUTE:
		data.attributeData.insert(newData.begin(), newData.end());
		break;
	default:
		// Handle errors.
		return;
	}
}

std::map<std::string, GLuint> Program::GetData()
{
	typedef std::map<std::string, GLuint> DataMap;
	DataMap allData;

	allData.insert(data.uniformData.begin(), data.uniformData.end());
	for(DataMap::iterator iter = data.attributeData.begin();
		iter != data.attributeData.end();
		++iter)
	{
		allData.insert(std::make_pair(iter->first, iter->second));
	}

	return allData;
}

void Program::BuildProgram()
{
	std::vector<GLuint> shaderList;

	for(std::map<GLenum, std::string>::iterator iter = data.shaderNames.begin();
		iter != data.shaderNames.end();
		++iter)
	{
		shaderList.push_back(Framework::LoadShader(iter->first, iter->second.c_str()));
	}
			
	data.theProgram = Framework::CreateProgram(shaderList);


	for(std::map<std::string, GLuint>::iterator iter = data.uniformData.begin();
		iter != data.uniformData.end();
		++iter)
	{
		iter->second = glGetUniformLocation(data.theProgram, iter->first.c_str());
	}

	for(std::map<std::string, GLuint>::iterator iter = data.attributeData.begin();
		iter != data.attributeData.end();
		++iter)
	{
		iter->second = glGetAttribLocation(data.theProgram, iter->first.c_str());
	}
}



ShaderProgramContainer::ShaderProgramContainer()
{
	shaderPrograms.clear();
}


void ShaderProgramContainer::AddProgram(const std::string &newProgramName, const Program &newProgram)
{
	shaderPrograms.insert(std::make_pair(newProgramName, newProgram));
}


void ShaderProgramContainer::LoadProgram(const std::string &programName)
{
	// TODO: Check if the program exists.

	shaderPrograms[programName].BuildProgram();
}

Program ShaderProgramContainer::GetProgram(const std::string &programName)
{
	return shaderPrograms[programName];
}

void ShaderManager::LoadProgram(const std::string &vertexShader, const std::string &fragmentShader,
								const std::string &programDataType)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	programDatas[programDataType].theProgram = Framework::CreateProgram(shaderList);
}

void ShaderManager::AddAttrubute(const std::string &programDataType, 
								 const std::string &attributeName, const std::string &attributeNameInShader)
{
	GLuint attribLocation = 
		glGetAttribLocation(programDatas[programDataType].theProgram, attributeNameInShader.c_str());

	programDatas[programDataType].attributes.insert(std::make_pair(attributeName, attribLocation));
}
void ShaderManager::AddUniform(const std::string &programDataType, 
							   const std::string &uniformName, 
							   const std::string &uniformNameInShader)
{
	GLuint uniformLocation =
		glGetUniformLocation(programDatas[programDataType].theProgram, uniformNameInShader.c_str());

	programDatas[programDataType].attributes.insert(std::make_pair(uniformName, uniformLocation));
}
void ShaderManager::AddUniformBlock(const std::string &programDataType, 
									const std::string &uniformBlockName, 
									const std::string &uniformBlockNameInShader,
									BlockType blockType)
{
	GLuint uniformBlockLocation =
		glGetUniformBlockIndex(programDatas[programDataType].theProgram, uniformBlockNameInShader.c_str());

	programDatas[programDataType].attributes.insert(std::make_pair(uniformBlockName, uniformBlockLocation));

	glUniformBlockBinding(programDatas[programDataType].theProgram, uniformBlockLocation, blockType);
}






ShaderManager::ShaderManager()
{
	blockIndices.insert(std::make_pair(BT_MATERIAL, 0));
	blockIndices.insert(std::make_pair(BT_LIGHT, 1));
	blockIndices.insert(std::make_pair(BT_PROJECTION, 2));
	blockIndices.insert(std::make_pair(BT_ORTHOGRAPHIC, 3));

	uniformBuffers.insert(std::make_pair(UBT_PROJECTION, 0));
	uniformBuffers.insert(std::make_pair(UBT_ORTHOGRAPHIC, 0));
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

void ShaderManager::LoadLitTextureProgram(const std::string &vertexShader,
										  const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	litTextureData.theProgram = Framework::CreateProgram(shaderList);
	litTextureData.modelToCameraMatrixUnif = glGetUniformLocation(litTextureData.theProgram, "modelToCameraMatrix");
	litTextureData.lightIntensityUnif = glGetUniformLocation(litTextureData.theProgram, "lightIntensity");
	litTextureData.normalModelToCameraMatrixUnif = glGetUniformLocation(litTextureData.theProgram, "normalModelToCameraMatrix");
	litTextureData.cameraSpaceLightPosUnif = glGetUniformLocation(litTextureData.theProgram, "cameraSpaceLightPos");
	litTextureData.textureUnif = glGetUniformLocation(litTextureData.theProgram, "_sampler");

	litTextureData.positionAttrib = glGetAttribLocation(litTextureData.theProgram, "position");
	litTextureData.normalAttrib = glGetAttribLocation(litTextureData.theProgram, "normal");
	litTextureData.textureCoordAttrib = glGetAttribLocation(litTextureData.theProgram, "texCoord");

	GLuint materialBlock = glGetUniformBlockIndex(litTextureData.theProgram, "Material");
	GLuint lightBlock = glGetUniformBlockIndex(litTextureData.theProgram, "Light");
	GLuint projectionBlock = glGetUniformBlockIndex(litTextureData.theProgram, "Projection");

	if(materialBlock != GL_INVALID_INDEX)
		glUniformBlockBinding(litTextureData.theProgram, materialBlock, blockIndices[BT_MATERIAL]);

	glUniformBlockBinding(litTextureData.theProgram, lightBlock, blockIndices[BT_LIGHT]);
	glUniformBlockBinding(litTextureData.theProgram, projectionBlock, blockIndices[BT_PROJECTION]);
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
	//simpleData.projectionMatrixUnif = glGetUniformLocation(simpleData.theProgram, "cameraToClipMatrix");

	simpleData.positionAttrib = glGetAttribLocation(simpleData.theProgram, "position");


	GLuint projectionBlock = glGetUniformBlockIndex(simpleData.theProgram, "Projection");	
	glUniformBlockBinding(simpleData.theProgram, projectionBlock, blockIndices[BT_PROJECTION]);
}

void ShaderManager::LoadSimpleNoUBProgram(const std::string &vertexShader, 
										  const std::string &fragmentShader)	
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	simpleNoUBData.theProgram = Framework::CreateProgram(shaderList);
	simpleNoUBData.modelToCameraMatrixUnif = 
		glGetUniformLocation(simpleNoUBData.theProgram, "modelToCameraMatrix");
	simpleNoUBData.colorUnif = 
		glGetUniformLocation(simpleNoUBData.theProgram, "color");
	simpleNoUBData.projectionMatrixUnif = 
		glGetUniformLocation(simpleNoUBData.theProgram, "cameraToClipMatrix");

	simpleNoUBData.positionAttrib =
		glGetAttribLocation(simpleNoUBData.theProgram, "position");


	GLuint projectionBlock = glGetUniformBlockIndex(simpleNoUBData.theProgram, "OrthographicProjection");
	glUniformBlockBinding(simpleNoUBData.theProgram, projectionBlock, blockIndices[BT_ORTHOGRAPHIC]);
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

	fontData.projectionMatrixUnif = glGetUniformLocation(fontData.theProgram, "projection");

	fontData.positionAttrib = glGetAttribLocation(fontData.theProgram, "position");


	GLuint projectionBlock = glGetUniformBlockIndex(fontData.theProgram, "OrthographicProjection");
	glUniformBlockBinding(fontData.theProgram, projectionBlock, blockIndices[BT_ORTHOGRAPHIC]);
	//fontData.textureCoordAttrib = glGetAttribLocation(fontData.theProgram, "textureCoordinates");
}

void ShaderManager::LoadSimpleTextureProgData(const std::string &vertexShader,
											  const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	simpleTextureProgData.theProgram = Framework::CreateProgram(shaderList);
	simpleTextureProgData.textureUnif = 
		glGetUniformLocation(simpleTextureProgData.theProgram, "colorTexture");
	simpleTextureProgData.modelToCameraMatrixUnif =
		glGetUniformLocation(simpleTextureProgData.theProgram, "modelToCameraMatrix");
	simpleTextureProgData.colorUnif = 
		glGetUniformLocation(simpleTextureProgData.theProgram, "color");

	GLuint projectionBlock = 
		glGetUniformBlockIndex(simpleTextureProgData.theProgram, "Projection");
	glUniformBlockBinding(simpleTextureProgData.theProgram, projectionBlock, blockIndices[BT_PROJECTION]); 

	simpleTextureProgData.positionAttrib = 
		glGetAttribLocation(simpleTextureProgData.theProgram, "position");
	simpleTextureProgData.normalAttrib =
		glGetAttribLocation(simpleTextureProgData.theProgram, "normal");
	simpleTextureProgData.textureCoordAttrib =
		glGetAttribLocation(simpleTextureProgData.theProgram, "texCoord");
}

void ShaderManager::LoadTextureProgData(const std::string &vertexShader, 
										const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	textureProgData.theProgram = Framework::CreateProgram(shaderList);


	textureProgData.modelToCameraMatrixUnif = 
		glGetUniformLocation(textureProgData.theProgram, "modelToCameraMatrix");
	textureProgData.projectionMatrixUnif = 
		glGetUniformLocation(textureProgData.theProgram, "cameraToClipMatrix");
	textureProgData.colorTextureUnif =
		glGetUniformLocation(textureProgData.theProgram, "_sampler");

	textureProgData.colorUnif =
		glGetUniformLocation(textureProgData.theProgram, "color");

	textureProgData.positionAttrib = 
		glGetAttribLocation(textureProgData.theProgram, "position");
	textureProgData.texturePosAttrib =
		glGetAttribLocation(textureProgData.theProgram, "texCoord");


	GLuint projectionBlock = glGetUniformBlockIndex(textureProgData.theProgram, "OrthographicProjection");
	glUniformBlockBinding(textureProgData.theProgram, projectionBlock, blockIndices[BT_ORTHOGRAPHIC]);
}

void ShaderManager::LoadPerspectiveTextureProgData(const std::string &vertexShader, 
												   const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	perspTextureProgData.theProgram = Framework::CreateProgram(shaderList);


	perspTextureProgData.modelToCameraMatrixUnif = 
		glGetUniformLocation(perspTextureProgData.theProgram, "modelToCameraMatrix");
	perspTextureProgData.projectionMatrixUnif = 
		glGetUniformLocation(perspTextureProgData.theProgram, "cameraToClipMatrix");
	perspTextureProgData.colorTextureUnif =
		glGetUniformLocation(perspTextureProgData.theProgram, "_sampler");
	perspTextureProgData.colorUnif =
		glGetUniformLocation(perspTextureProgData.theProgram, "color");

	perspTextureProgData.positionAttrib = 
		glGetAttribLocation(perspTextureProgData.theProgram, "position");
	perspTextureProgData.texturePosAttrib =
		glGetAttribLocation(perspTextureProgData.theProgram, "texCoord");


	GLuint projectionBlock = glGetUniformBlockIndex(perspTextureProgData.theProgram, "Projection");
	glUniformBlockBinding(perspTextureProgData.theProgram, projectionBlock, blockIndices[BT_PROJECTION]);
}

void ShaderManager::LoadBillboardProgDataNoTexture(const std::string &vertexShader,
												   const std::string &geometryShader,
												   const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_GEOMETRY_SHADER, geometryShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	billboardProgDataNoTexture.theProgram = Framework::CreateProgram(shaderList);


	billboardProgDataNoTexture.cameraPositionUnif = 
		glGetUniformLocation(billboardProgDataNoTexture.theProgram, "cameraPosition");
	billboardProgDataNoTexture.cameraToClipMatrixUnif =
		glGetUniformLocation(billboardProgDataNoTexture.theProgram, "cameraToClipMatrix");
	billboardProgDataNoTexture.modelToCameraMatrixUnif =
		glGetUniformLocation(billboardProgDataNoTexture.theProgram, "modelToCameraMatrix");
	billboardProgDataNoTexture.colorUnif = 
		glGetUniformLocation(billboardProgDataNoTexture.theProgram, "color");
	billboardProgDataNoTexture.billboardSizeUnif =
		glGetUniformLocation(billboardProgDataNoTexture.theProgram, "billboardSize");
	billboardProgDataNoTexture.deltaPositionUnif =
		glGetUniformLocation(billboardProgDataNoTexture.theProgram, "deltaPosition");

	billboardProgDataNoTexture.positionAttrib = 
		glGetAttribLocation(billboardProgDataNoTexture.theProgram, "position");
}

void ShaderManager::LoadBillboardProgData(const std::string &vertexShader,
										  const std::string &geometryShader,
										  const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_GEOMETRY_SHADER, geometryShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	billboardProgData.theProgram = Framework::CreateProgram(shaderList);


	billboardProgData.cameraPositionUnif = 
		glGetUniformLocation(billboardProgData.theProgram, "cameraPosition");
	billboardProgData.cameraToClipMatrixUnif = 
		glGetUniformLocation(billboardProgData.theProgram, "cameraToClipMatrix");
	billboardProgData.modelToCameraMatrixUnif = 
		glGetUniformLocation(billboardProgData.theProgram, "modelToCameraMatrix");
	billboardProgData.samplerUnif = 
		glGetUniformLocation(billboardProgData.theProgram, "_sampler");
	billboardProgData.billboardSizeUnif =
		glGetUniformLocation(billboardProgData.theProgram, "billboardSize");
	billboardProgData.deltaPositionUnif =
		glGetUniformLocation(billboardProgData.theProgram, "deltaPosition");
	billboardProgData.colorUnif = 
		glGetUniformLocation(billboardProgData.theProgram, "color");
	
	billboardProgData.positionAttrib =
		glGetAttribLocation(billboardProgData.theProgram, "position");
	billboardProgData.textureCoordAttrib =
		glGetAttribLocation(billboardProgData.theProgram, "texCoord");
}

void ShaderManager::LoadParticleProgData(const std::string &vertexShader, 
										 const std::string &geometryShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_GEOMETRY_SHADER, geometryShader));

	particleProgData.theProgram = Framework::CreateProgram(shaderList);


	particleProgData.deltaTime_millisecondsUnif = 
		glGetUniformLocation(particleProgData.theProgram, "deltaTime_milliseconds");
	particleProgData.launcherLifetimeUnif = 
		glGetUniformLocation(particleProgData.theProgram, "launcherLifetime");
	particleProgData.randomTextureUnif = 
		glGetUniformLocation(particleProgData.theProgram, "randomDirections");
	particleProgData.secondaryShellLifetimeUnif = 
		glGetUniformLocation(particleProgData.theProgram, "secondaryShellLifetime");
	particleProgData.shellLifetimeUnif = 
		glGetUniformLocation(particleProgData.theProgram, "shellLifetime");
	particleProgData.timeUnif = 
		glGetUniformLocation(particleProgData.theProgram, "time");

	particleProgData.particleAgeAttrib = 
		glGetAttribLocation(particleProgData.theProgram, "age");
	particleProgData.particlePositionAttrib = 
		glGetAttribLocation(particleProgData.theProgram, "position");
	particleProgData.particleTypeAttrib = 
		glGetAttribLocation(particleProgData.theProgram, "type");
	particleProgData.particleVelocityAttrib = 
		glGetAttribLocation(particleProgData.theProgram, "velocity");
}

void ShaderManager::LoadSpriteParticleProgData(const std::string &vertexShader, const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	spriteParticleProgData.theProgram = Framework::CreateProgram(shaderList);

	spriteParticleProgData.deltaPositionUnif = 
		glGetUniformLocation(spriteParticleProgData.theProgram, "deltaPosition");
	spriteParticleProgData.cameraToClipMatrixUnif = 
		glGetUniformLocation(spriteParticleProgData.theProgram, "cameraToClipMatrix");
	spriteParticleProgData.colorUnif = 
		glGetUniformLocation(spriteParticleProgData.theProgram, "color");
	spriteParticleProgData.modelToCameraMatrixUnif =
		glGetUniformLocation(spriteParticleProgData.theProgram, "modelToCameraMatrix");
	spriteParticleProgData.samplerUnif = 
		glGetUniformLocation(spriteParticleProgData.theProgram, "_sampler");

	spriteParticleProgData.positionAttrib = 
		glGetAttribLocation(spriteParticleProgData.theProgram, "position");
	spriteParticleProgData.texCoordAttrib =
		glGetAttribLocation(spriteParticleProgData.theProgram, "texCoord");

	GLuint projectionBlock = glGetUniformBlockIndex(spriteParticleProgData.theProgram, "Projection");
	glUniformBlockBinding(spriteParticleProgData.theProgram, projectionBlock, blockIndices[BT_PROJECTION]);
}


LitProgData ShaderManager::GetLitProgData()
{
	return litData;
}
LitTextureProgData ShaderManager::GetLitTextureProgData()
{
	return litTextureData;
}
UnlitProgData ShaderManager::GetUnlitProgData()
{
	return unlitData;
}
SimpleProgData ShaderManager::GetSimpleProgData()
{
	return simpleData;
}
SimpleProgData ShaderManager::GetSimpleNoUBProgData()
{
	return simpleNoUBData;
}
FontProgData ShaderManager::GetFontProgData()
{
	return fontData;
}
SimpleTextureProgData ShaderManager::GetSimpleTextureProgData()
{
	return simpleTextureProgData;
}
TextureProgData ShaderManager::GetTextureProgData()
{
	return textureProgData;
}
TextureProgData ShaderManager::GetPerspectiveTextureProgData()
{
	return perspTextureProgData;
}
BillboardProgDataNoTexture ShaderManager::GetBillboardProgDataNoTexture()
{
	return billboardProgDataNoTexture;
}
BillboardProgData ShaderManager::GetBillboardProgData()
{
	return billboardProgData;
}
ParticleProgData ShaderManager::GetParticleProgData()
{
	return particleProgData;
}
SpriteParticleProgData ShaderManager::GetSpriteParticleProgData()
{
	return spriteParticleProgData;
}


const int ShaderManager::GetBlockIndex(BlockType blockType)
{
	if(blockIndices.find(blockType) != blockIndices.end())
		return blockIndices[blockType];

	else 
	{
		HandleUnexpectedError("the block doesn\'t exist", __LINE__, __FILE__);
		//std::printf("The block doesn't exist\n");
		return 0;
	}
}
unsigned int ShaderManager::GetUniformBuffer(UniformBufferType uniformBufferType)
{
	if(uniformBuffers.find(uniformBufferType) != uniformBuffers.end())
		return uniformBuffers[uniformBufferType];

	else
	{
		HandleUnexpectedError("the uniform buffer doesn\'t exist", __LINE__, __FILE__);
		//std::printf("The uniform buffer doesn't exist\n");
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
		HandleUnexpectedError("the uniform buffe doesn\'t exist", __LINE__, __FILE__);
		//std::printf("The uniform buffer doesn't exist\n");
		return;
	}
}