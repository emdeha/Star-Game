#include "stdafx.h"
#include "AssetLoader.h"

#include "../framework/ErrorAPI.h"


using namespace FusionEngine;


const std::shared_ptr<Texture2D> ImageAssetObject::GetLoadedTexture() const
{
	return loadedTexture;
}


ImageAssetObject ImageLoader::Load(const std::string &type, const std::string &name)
{
	std::shared_ptr<Texture2D> loadedTexture;

	loadedTexture = std::shared_ptr<Texture2D>(new Texture2D());
	if(!loadedTexture->Load("../data/" + type + "/" + name, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, false))
    {
		HandleUnexpectedError("cannot load texture " + type + "/" + name, __LINE__, __FILE__);
    }

	return ImageAssetObject(loadedTexture);
}