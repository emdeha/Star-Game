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

	std::map<UniformBufferType, unsigned int> uniformBuffers;
	std::map<BlockType, int> blockIndices;
	std::map<ProgramType, ProgramData> programs;

	if (shaderConfig)
	{
		if (shaderConfig["block-indices"])
		{
			for (auto blockIdx = shaderConfig["block-indices"].begin(); 
				 blockIdx != shaderConfig["block-indices"].end(); ++blockIdx)
			{
				if (blockIdx->second.as<std::string>() == "MATERIAL")
				{
					blockIndices.insert(std::make_pair(BT_MATERIAL, 0));
				}
				if (blockIdx->second.as<std::string>() == "LIGHT")
				{
					blockIndices.insert(std::make_pair(BT_LIGHT, 1));
				}
				if (blockIdx->second.as<std::string>() == "PROJECTION")
				{
					blockIndices.insert(std::make_pair(BT_PROJECTION, 2));
				}
				if (blockIdx->second.as<std::string>() == "ORTHOGRAPHIC")
				{
					blockIndices.insert(std::make_pair(BT_ORTHOGRAPHIC, 3));
				}
			}
		}
		
		if (shaderConfig["uniform-buffers"])
		{
			for (auto ub = shaderConfig["uniform-buffers"].begin();
				 ub != shaderConfig["uniform-buffers"].end(); ++ub)
			{
				if (ub->second.as<std::string>() == "LIGHT")
				{
					uniformBuffers.insert(std::make_pair(UBT_LIGHT, 0));
				}
				if (ub->second.as<std::string>() == "PROJECTION")
				{
					uniformBuffers.insert(std::make_pair(UBT_PROJECTION, 0));
				}
				if (ub->second.as<std::string>() == "ORTHOGRAPHIC")
				{
					uniformBuffers.insert(std::make_pair(UBT_ORTHOGRAPHIC, 0));
				}
			}
		}

		if (shaderConfig["programs"])
		{
		}
	}

	return ShaderAssetObject(programs, uniformBuffers, blockIndices);
}