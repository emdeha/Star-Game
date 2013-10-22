#include "stdafx.h"
#include "ShaderManager.h"

#include "../framework/framework.h"


using namespace FusionEngine;


void ShaderManager::LoadProgram(ProgramType programType,
								const std::string &vertexShader, const std::string &fragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, vertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, fragmentShader));

	Program loadedProgram;
	loadedProgram.programHandle = Framework::CreateProgram(shaderList);

	loadedPrograms.insert(std::make_pair(programType, loadedProgram));
}

Program ShaderManager::GetProgram(ProgramType programType)
{
	return loadedPrograms[programType];
}