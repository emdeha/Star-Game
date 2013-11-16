#include "stdafx.h"
#include "AssetLoader.h"

#include "../framework/ErrorAPI.h"


using namespace FusionEngine;


unsigned int ShaderAssetObject::GetUniformBuffer(UniformBufferType ubType) const
{
	auto foundIter = uniformBuffers.find(ubType);
	return (*foundIter).second;
}
std::map<UniformBufferType, unsigned int> ShaderAssetObject::GetAllLoadedUniformBuffers() const
{
	return uniformBuffers;
}

int ShaderAssetObject::GetLoadedBlockIndex(BlockType blockType) const
{
	auto foundIter = blockIndices.find(blockType);
	return (*foundIter).second;
}
std::map<BlockType, int> ShaderAssetObject::GetAllLoadedBlockIndices() const
{
	return blockIndices;
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


ProgramType ShaderLoader::GetProgramTypeFromString(const std::string &strId) const
{
	if (strId == "LIT")
	{
		return FE_PROGRAM_LIT;
	}
	if (strId == "LIT_TEXTURE")
	{
		return FE_PROGRAM_LIT_TEXTURE;
	}
	if (strId == "UNLIT")
	{
		return FE_PROGRAM_UNLIT;
	}
	if (strId == "SIMPLE")
	{
		return FE_PROGRAM_SIMPLE;
	}
	if (strId == "SIMPLE_NO_UB")
	{
		return FE_PROGRAM_SIMPLE_NO_UB;
	}
	if (strId == "FONT")
	{
		return FE_PROGRAM_FONT;
	}
	if (strId == "SIMPLE_TEXTURE")
	{
		return FE_PROGRAM_SIMPLE_TEXTURE;
	}
	if (strId == "TEXTURE")
	{
		return FE_PROGRAM_TEXTURE;
	}
	if (strId == "TEXTURE_PERSPECTIVE")
	{
		return FE_PROGRAM_TEXTURE_PERSPECTIVE;
	}
	if (strId == "BILLBOARD_NO_TEXTURE")
	{
		return FE_PROGRAM_BILLBOARD_NO_TEXTURE;
	}
	if (strId == "BILLBOARD")
	{
		return FE_PROGRAM_BILLBOARD;
	}
	if (strId == "PARTICLE")
	{
		return FE_PROGRAM_PARTICLE;
	}
	if (strId == "SPRITE_PARTICLE")
	{
		return FE_PROGRAM_SPRITE_PARTICLE;
	}

	return FE_PROGRAM_BAD;
}

std::pair<BlockType, int> ShaderLoader::GetBlockPairFromString(const std::string &strId) const
{
	if (strId == "MATERIAL")
	{
		return std::make_pair(FE_BT_MATERIAL, 0);
	}
	if (strId == "LIGHT")
	{
		return std::make_pair(FE_BT_LIGHT, 1);
	}
	if (strId == "PROJECTION")
	{
		return std::make_pair(FE_BT_PROJECTION, 2);
	}
	if (strId == "ORTHOGRAPHIC")
	{
		return std::make_pair(FE_BT_ORTHOGRAPHIC, 3);
	}

	return std::make_pair(FE_BT_BAD, -1);
}

std::pair<UniformBufferType, unsigned int> ShaderLoader::GetUBPairFromString(const std::string &strId) const
{
	if (strId == "LIGHT")
	{
		return std::make_pair(FE_UBT_LIGHT, 0);
	}
	if (strId == "PROJECTION")
	{
		return std::make_pair(FE_UBT_PROJECTION, 0);
	}
	if (strId == "ORTHOGRAPHIC")
	{
		return std::make_pair(FE_UBT_ORTHOGRAPHIC, 0);
	}

	return std::make_pair(FE_UBT_BAD, -1);
}

UniformType ShaderLoader::GetUniformTypeFromString(const std::string &strId) const
{
	if (strId == "modelToCameraMatrix")
	{
		return FE_UNIFORM_MODEL_TO_CAMERA_MATRIX;
	}
	if (strId == "normalModelToCameraMatrix")
	{
		return 	FE_UNIFORM_NORMAL_MODEL_TO_CAMERA_MATRIX;
	}
	if (strId == "colorTexture")
	{
		return 	FE_UNIFORM_COLOR_TEXTURE;
	}
	if (strId == "color")
	{
		return 	FE_UNIFORM_COLOR;
	}
	if (strId == "objectColor")
	{
		return 	FE_UNIFORM_OBJECT_COLOR;
	}
	if (strId == "cameraToClipMatrix")
	{
		return 	FE_UNIFORM_CAMERA_TO_CLIP_MATRIX;
	}
	if (strId == "fontColor")
	{
		return 	FE_UNIFORM_FONT_COLOR;
	}
	if (strId == "fontTexture")
	{
		return 	FE_UNIFORM_FONT_TEXTURE;
	}
	if (strId == "projection")
	{
		return 	FE_UNIFORM_PROJECTION;
	}
	if (strId == "_sampler")
	{
		return 	FE_UNIFORM__SAMPLER;
	}
	if (strId == "cameraPosition")
	{
		return 	FE_UNIFORM_CAMERA_POSITION;
	}
	if (strId == "deltaTime_milliseconds")
	{
		return 	FE_UNIFORM_DELTA_TIME_MILLISECONDS;
	}
	if (strId == "launcherLifetime")
	{
		return 	FE_UNIFORM_LAUNCHER_LIFETIME;
	}
	if (strId == "randomDirections")
	{
		return 	FE_UNIFORM_RANDOM_DIRECTIONS;
	}
	if (strId == "secondaryShellLifetime")
	{
		return 	FE_UNIFORM_SECONDARY_SHELL_LIFETIME;
	}
	if (strId == "shellLifetime")
	{
		return 	FE_UNIFORM_SHELL_LIFETIME;
	}
	if (strId == "time")
	{
		return 	FE_UNIFORM_TIME;
	}
	if (strId == "deltaPosition")
	{
		return 	FE_UNIFORM_DELTA_POSITION;
	}
	if (strId == "billboardSize")
	{
		return 	FE_UNIFORM_BILLBOARD_SIZE;
	}
	if (strId == "lightIntensity")
	{
		return 	FE_UNIFORM_LIGHT_INTENSITY;
	}
	if (strId == "cameraSpaceLightPos")
	{
		return 	FE_UNIFORM_CAMERA_SPACE_LIGHT_POS;
	}

	return FE_UNIFORM_BAD;
}

AttribType ShaderLoader::GetAttribTypeFromString(const std::string &strId) const
{
	if (strId == "position")
	{
		return FE_ATTRIB_POSITION;
	}
	if (strId == "normal")
	{
		return FE_ATTRIB_NORMAL;
	}
	if (strId == "texCoord")
	{
		return FE_ATTRIB_TEX_COORD;
	}
	if (strId == "age")
	{
		return FE_ATTRIB_AGE;
	}
	if (strId == "type")
	{
		return FE_ATTRIB_TYPE;
	}
	if (strId == "velocity")
	{
		return FE_ATTRIB_VELOCITY;
	}

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
				for (auto programIdx = shaderConfig["programs"].begin();
					 programIdx != shaderConfig["programs"].end(); ++programIdx)
				{
					ProgramType currentProgType = FE_PROGRAM_BAD;
					ProgramData loadedProgramData;
					std::vector<GLuint> shaderList;
					std::string shaderDir = "shaders/";
					if (programIdx->second["id"])
					{
						currentProgType = 
							GetProgramTypeFromString(programIdx->second["id"].as<std::string>());
					}

					if (programIdx->second["vertex-source"])
					{
						std::string shaderFilePath = shaderDir;
						shaderFilePath.append(programIdx->second["vertex-source"].as<std::string>());
						shaderFilePath.append(".vert");
						shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, shaderFilePath)); 
					}
					if (programIdx->second["fragment-source"])
					{
						std::string shaderFilePath = shaderDir;
						shaderFilePath.append(programIdx->second["fragment-source"].as<std::string>());
						shaderFilePath.append(".frag");
						shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, shaderFilePath));
					}
					if (programIdx->second["geom-source"])
					{
						std::string shaderFilePath = shaderDir;
						shaderFilePath.append(programIdx->second["geom-source"].as<std::string>());
						shaderFilePath.append(".geom");
						shaderList.push_back(Framework::LoadShader(GL_GEOMETRY_SHADER, shaderFilePath));
					}
					
					loadedProgramData.programId = Framework::CreateProgram(shaderList);

					if (programIdx->second["uniforms"] && loadedProgramData.programId >= 0)
					{
						for (auto uniformIdx = programIdx->second["uniforms"].begin();
							 uniformIdx != programIdx->second["uniforms"].end(); ++uniformIdx)
						{
							std::string uniformName = uniformIdx->as<std::string>();
							UniformType uniformType = 
								GetUniformTypeFromString(uniformName);
							GLuint uniformID = 
								glGetUniformLocation(loadedProgramData.programId, uniformName.c_str());

							loadedProgramData.uniforms.insert(std::make_pair(uniformType, uniformID));
						}
					}
					if (programIdx->second["attribs"] && loadedProgramData.programId >= 0)
					{
						for (auto attribIdx = programIdx->second["attribs"].begin();
							 attribIdx != programIdx->second["attribs"].end(); ++attribIdx)
						{
							std::string attribName = attribIdx->as<std::string>();
							AttribType attribType =
								GetAttribTypeFromString(attribName);
							GLuint attribID = 
								glGetAttribLocation(loadedProgramData.programId, attribName.c_str());

							loadedProgramData.attribs.insert(std::make_pair(attribType, attribID));
						}
					}
					if (programIdx->second["blocks"] && loadedProgramData.programId >= 0)
					{
						for (auto blockIdx = programIdx->second["blocks"].begin();
							 blockIdx != programIdx->second["blocks"].end(); ++blockIdx)
						{

							std::string blockName = blockIdx->second.as<std::string>();
							std::string blockIndex = blockIdx->first.as<std::string>();
							GLuint block = 
								glGetUniformBlockIndex(loadedProgramData.programId, blockName.c_str());
							int blockBindingPoint = 
								GetBlockPairFromString(blockIndex).second;
							if (block != GL_INVALID_INDEX)
								glUniformBlockBinding(loadedProgramData.programId, block, blockBindingPoint);
						}
					}

					
					programs.insert(std::make_pair(currentProgType, loadedProgramData));
				}
			}
		}
	}

	return ShaderAssetObject(programs, uniformBuffers, blockIndices);
}