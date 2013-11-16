#include "stdafx.h"
#include "ShaderManager.h"

#include "../framework/framework.h"


using namespace FusionEngine;


void ShaderManager::Load(ProgramMap newLoadedPrograms,
						 UBTMap newUniformBuffers, BlockMap newBlockIndices)
{
	loadedPrograms = newLoadedPrograms; 
	uniformBuffers = newUniformBuffers; 
	blockIndices = newBlockIndices;
}

void ShaderManager::LoadProgram(ProgramType programType,
								const std::string &vertexShader, const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	ProgramData loadedProgram;
	loadedProgram.programId = Framework::CreateProgram(shaderList);

	loadedPrograms.insert(std::make_pair(programType, loadedProgram));
}

ProgramData ShaderManager::GetProgram(ProgramType programType) const
{
	auto foundProgram = loadedPrograms.find(programType);
	return (*foundProgram).second;
}

unsigned int ShaderManager::GetUniformBuffer(UniformBufferType ubType) const
{
	auto foundUB = uniformBuffers.find(ubType);
	return (*foundUB).second;
}
void ShaderManager::SetUniformBuffer(UniformBufferType ubType, unsigned int index) 
{
	auto foundUB = uniformBuffers.find(ubType);
	foundUB->second = index;
}

int ShaderManager::GetBlockIndex(BlockType blockType) const
{
	auto foundBlock = blockIndices.find(blockType);
	return (*foundBlock).second;
}