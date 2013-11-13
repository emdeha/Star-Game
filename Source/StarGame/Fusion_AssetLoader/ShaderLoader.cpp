#include "stdafx.h"
#include "AssetLoader.h"

#include "../framework/ErrorAPI.h"


using namespace FusionEngine;


unsigned int ShaderAssetObject::GetUniformBuffer(UniformBufferType ubType) const
{
	auto foundIter = uniformBuffers.find(ubType);
	return (*foundIter).second;
}
int ShaderAssetObject::GetLoadedBlockIndex(BlockType blockType) const
{
	auto foundIter = blockIndices.find(blockType);
	return (*foundIter).second;
}

FusionEngine::ProgramData ShaderAssetObject::GetLoadedProgram(ProgramType progType) const
{
	auto foundIter = loadedPrograms.find(progType);
	return (*foundIter).second;
}
const std::map<ProgramType, FusionEngine::ProgramData> &ShaderAssetObject::GetAllLoadedPrograms() const
{
	return loadedPrograms;
}


ShaderAssetObject ShaderLoader::Load(const std::string &type, const std::string &name)
{
	YAML::Node shaderConfig = YAML::LoadFile("../data/" + type + "/" + name);



	if (shaderConfig)
	{
		if (shaderConfig["programs"])
		{
		}
	}
}