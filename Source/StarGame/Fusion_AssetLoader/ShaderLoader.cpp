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


ProgramType ShaderLoader::GetProgramTypeFromString(const std::string &strId)
{
	return FE_PROGRAM_BAD;
}

std::pair<BlockType, int> ShaderLoader::GetBlockPairFromString(const std::string &strId)
{
	if (strId == "MATERIAL")
	{
		return std::make_pair(BT_MATERIAL, 0);
	}
	if (strId == "LIGHT")
	{
		return std::make_pair(BT_LIGHT, 1);
	}
	if (strId == "PROJECTION")
	{
		return std::make_pair(BT_PROJECTION, 2);
	}
	if (strId == "ORTHOGRAPHIC")
	{
		return std::make_pair(BT_ORTHOGRAPHIC, 3);
	}

	return std::make_pair(BT_BAD, -1);
}

std::pair<UniformBufferType, unsigned int> ShaderLoader::GetUBPairFromString(const std::string &strId)
{
	if (strId == "LIGHT")
	{
		return std::make_pair(UBT_LIGHT, 0);
	}
	if (strId == "PROJECTION")
	{
		return std::make_pair(UBT_PROJECTION, 0);
	}
	if (strId == "ORTHOGRAPHIC")
	{
		return std::make_pair(UBT_ORTHOGRAPHIC, 0);
	}

	return std::make_pair(UBT_BAD, -1);
}

UniformType GetUniformTypeFromString(const std::string &strId)
{
	return FE_UNIFORM_BAD;
}

AttribType GetAttribTypeFromString(const std::string &strId)
{
	return FE_ATTRIB_BAD;
}


ShaderAssetObject ShaderLoader::Load(const std::string &type, const std::string &name)
{
	YAML::Node shaderConfig = YAML::LoadFile("../data/" + type + "/" + name);

	std::map<UniformBufferType, unsigned int> uniformBuffers;
	std::map<BlockType, int> blockIndices;
	std::map<ProgramType, ProgramData> programs;

	if (shaderConfig)
	{
		for (auto shaderNode = shaderConfig.begin(); shaderNode != shaderConfig.end(); ++shaderNode)
		{
			if (shaderConfig["block-indices"])
			{
				for (auto blockIdx = shaderConfig["block-indices"].begin();
					 blockIdx != shaderConfig["block-indices"].end(); ++blockIdx) 
				{
					std::string blockStr = blockIdx->as<std::string>();
					blockIndices.insert(GetBlockPairFromString(blockStr));
				}
			}
			
			if (shaderConfig["uniform-buffers"])
			{
				for (auto ubIdx = shaderConfig["uniform-buffers"].begin();
					 ubIdx != shaderConfig["uniform-buffers"].end(); ++ubIdx)
				{
					std::string ubStr = ubIdx->as<std::string>();
					uniformBuffers.insert(GetUBPairFromString(ubStr));
				}
			}

			if (shaderConfig["programs"])
			{
			}
		}
	}

	return ShaderAssetObject(programs, uniformBuffers, blockIndices);
}