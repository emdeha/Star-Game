#include "stdafx.h"
#include "ShaderManager.h"

#pragma warning(push, 0)
#include <sstream>

#include "../framework/framework.h"
#pragma warning(pop)

#include "../framework/ErrorAPI.h"


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
	if (foundProgram == loadedPrograms.end())
	{
		std::ostringstream errorMsg;
		errorMsg << "No such program: " << (int)programType << ".\n";
		HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
		ProgramData wrongData;
		wrongData.programId = FE_PROGRAM_BAD;
		return wrongData;
	}
	return (*foundProgram).second;
}

unsigned int ShaderManager::GetUniformBuffer(UniformBufferType ubType) const
{
	auto foundUB = uniformBuffers.find(ubType);
	if (foundUB == uniformBuffers.end())
	{
		std::ostringstream errorMsg;
		errorMsg << "No such uniform buffer: " << (int)ubType << ".\n";
		HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
		return 0;
	}
	return (*foundUB).second;
}
void ShaderManager::SetUniformBuffer(UniformBufferType ubType, unsigned int index) 
{
	auto foundUB = uniformBuffers.find(ubType);
	if (foundUB == uniformBuffers.end())
	{
		std::ostringstream errorMsg;
		errorMsg << "No such uniform buffer: " << (int)ubType << ".\n";
		HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
		return;
	}
	foundUB->second = index;
}

int ShaderManager::GetBlockIndex(BlockType blockType) const
{
	auto foundBlock = blockIndices.find(blockType);
	if (foundBlock == blockIndices.end())
	{
		std::ostringstream errorMsg;
		errorMsg << "No such block type: " << (int)blockType << ".\n";
		HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
		return -1;
	}
	return (*foundBlock).second;
}