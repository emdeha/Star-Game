#include "stdafx.h"
#include "AssetLoader.h"

#include "../framework/ErrorAPI.h"


using namespace FusionEngine;


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