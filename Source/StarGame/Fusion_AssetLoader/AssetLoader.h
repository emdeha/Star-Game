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


#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H


#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "../AssetLoader/Texture.h"
#include "../StarGame/AudioEnums.h"
#include "../GUISystem/GUISystem.h"
#include "../assimp/include/assimp/Importer.hpp"
#include "../assimp/include/assimp/scene.h"
#include "../assimp/include/assimp/postprocess.h"
#include "yaml-cpp/yaml.h"


namespace FusionEngine
{
	class IAssetObject
	{
	public:
		IAssetObject() {}
	    virtual ~IAssetObject() {}
	};


	template<class LoadedObject>
	class ITypeLoader
	{
	public:
		virtual LoadedObject Load(const std::string &type, const std::string &name) = 0;
	};


	template<class ObjectToLoad>
	class AssetLoader
	{
	private:
		std::map<std::string, ITypeLoader<ObjectToLoad>*> loaders; 

	public:
		AssetLoader() {}
		
		void RegisterType(const std::string &type, ITypeLoader<ObjectToLoad>* loader);
		ObjectToLoad LoadAssetObject(const std::string &type, const std::string &name);
	};

	template<class ObjectToLoad>
	void AssetLoader<ObjectToLoad>::RegisterType(const std::string &type, ITypeLoader<ObjectToLoad>* loader)
	{
		loaders.insert(std::make_pair<std::string, ITypeLoader<ObjectToLoad>*>(type, loader));
	}

	template<class ObjectToLoad>
	ObjectToLoad AssetLoader<ObjectToLoad>::LoadAssetObject(const std::string &type, const std::string &name)
	{
		auto loader = loaders[type];

		return loader->Load(type, name); 
	}

	/////////////////////
	///  Mesh Loader  ///
	/////////////////////
	struct CVertex
	{
		glm::vec3 position;
		glm::vec2 textureCoord;
		glm::vec3 normal;
		
		CVertex() {}

		CVertex(const glm::vec3 &newPosition,
			   const glm::vec2 &newTextureCoord,
			   const glm::vec3 &newNormal)
		{
			position = newPosition;
			textureCoord = newTextureCoord;
			normal = newNormal;
		}
	};

	struct MeshEntry
	{
		std::vector<CVertex> vertices;
		std::vector<unsigned int> indices;

		unsigned int materialIndex;

		MeshEntry() {}
		MeshEntry(std::vector<CVertex> newVertices,
				  std::vector<unsigned int> newIndices,
				  unsigned int newMaterialIndex)
		{
			vertices = newVertices;
			indices = newIndices;
			materialIndex = newMaterialIndex;
		}
	};

	class MeshAssetObject : public IAssetObject
	{
	private:
		std::vector<MeshEntry> meshEntries;
		std::vector<std::shared_ptr<Texture2D>> textures;

	public:
		MeshAssetObject() {}

		void AddEntry(const MeshEntry &newMeshEntry);
		void AddTexture(const std::shared_ptr<Texture2D>& newTexture);


		const std::vector<MeshEntry> GetMeshEntries();
		const std::vector<std::shared_ptr<Texture2D>> GetTextures();
	};

	class MeshLoader : public ITypeLoader<MeshAssetObject>
	{
	private:
		void InitFromScene(const aiScene *scene, MeshAssetObject &loadedMesh);
		void InitMesh(unsigned int index, const aiMesh *mesh, MeshAssetObject &loadedMesh);
		void InitMaterials(const aiScene *scene, MeshAssetObject &loadedMesh);

	public:
		MeshAssetObject Load(const std::string &type, const std::string &name);
	};


	/////////////////////
	/// Image Loader  ///
	/////////////////////
	class ImageAssetObject : public IAssetObject
	{
	private:
		std::shared_ptr<Texture2D> loadedTexture;

	public:
		ImageAssetObject() {}
		ImageAssetObject(const std::shared_ptr<Texture2D>& newLoadedTexture)
		{
			loadedTexture = newLoadedTexture;
		}

		const std::shared_ptr<Texture2D> GetLoadedTexture();
	};

	class ImageLoader : public ITypeLoader<ImageAssetObject>
	{
	private:
	public:
		ImageAssetObject Load(const std::string &type, const std::string &name);
	};


	//////////////////////
	///  Audio Loader  ///
	//////////////////////
	struct CAudioFile
	{
		std::string path;
		SoundType soundType;
		bool isLooping;
	};

	struct CAudioData
	{
		ChannelType channel;
		float channelVolume;
		std::vector<CAudioFile> audioFiles;
	};

	class AudioAssetObject : public IAssetObject
	{
	private:
		std::vector<std::pair<ChannelType, CAudioData>> loadedAudio;

	public:
		AudioAssetObject() {}
		AudioAssetObject(const std::vector<std::pair<ChannelType, CAudioData>> &newLoadedAudio) 
		{
			loadedAudio = newLoadedAudio;
		}

		const std::vector<std::pair<ChannelType, CAudioData>> GetAllLoadedAudios();
	};

	class CAudioLoader : public ITypeLoader<AudioAssetObject>
	{
	private:
	public:
		AudioAssetObject Load(const std::string &type, const std::string &name);
	};


	////////////////////
	///  GUI Loader  ///
	////////////////////
	struct CLayoutData
	{
		LayoutType layoutType;
		glm::vec4 layoutColor;
		int layoutIsSet;
		std::string layoutBackgroundImageFileName;
	};

	struct CControlData
	{
		std::string name;
		std::string text; 
		std::string leftBackgroundImage;
		std::string rightBackgroundImage;
		std::string middleBackgroundImage;

		glm::vec4 fontColor;
		glm::vec4 margins;
		glm::vec2 position;
		glm::vec2 percentagedPosition;
		int textSize;

		bool hasBackground;
		bool isVisible;
		bool isUsingPercentage;
			
		float textBoxWidth;

		float imageBoxWidth;
		float imageBoxHeight;

		LayoutType toLayout;


		std::string onHoverText;
		std::string onHoverFont;
		std::string onHoverLeftBackgroundImage;
		std::string onHoverRightBackgroundImage;
		std::string onHoverMiddleBackgroundImage;
		glm::vec4 onHoverFontColor;
	};

	class GUIAssetObject : public IAssetObject
	{
	private:
		std::map<LayoutType, std::shared_ptr<Layout>> layouts;

	public:
		GUIAssetObject() {}
		GUIAssetObject(const std::map<LayoutType, std::shared_ptr<Layout>> &loadedLayouts)
		{
			layouts = loadedLayouts;
		}

		std::shared_ptr<Layout> GetLayout(LayoutType layoutType);
		const std::map<LayoutType, std::shared_ptr<Layout>> &GetAllLoadedLayouts();
	};

	class CGUILoader : public ITypeLoader<GUIAssetObject>
	{
	private:
	public:
		GUIAssetObject Load(const std::string &type, const std::string &name);
	};


	/////////////////////////
	///  TweakVar Loader  ///
	/////////////////////////
	struct CTweakVarData
	{
		enum VarType
		{
			TYPE_TWEAK_FLOAT,
			TYPE_TWEAK_INT,
			TYPE_TWEAK_STRING,
		};

		int itemIndex; // for indexed commands. -1 for non-existence.
		VarType currentType;

		union 
		{
			float varFloat;
			int varInt;
			char varString[50];
		};

		CTweakVarData() : itemIndex(-1) {}
	};

	class TweakVarAssetObject : public IAssetObject
	{
	private:
		std::vector<std::pair<std::string, CTweakVarData>> loadedTweaks;

	public:
		TweakVarAssetObject() {}
		TweakVarAssetObject(const std::vector<std::pair<std::string, CTweakVarData>> &newLoadedTweaks)
		{
			loadedTweaks = newLoadedTweaks;
		}

		const std::vector<std::pair<std::string, CTweakVarData>> GetAllLoadedVars();
	};

	class TweakVarLoader : public ITypeLoader<TweakVarAssetObject>
	{
	private:
		std::pair<std::string, CTweakVarData> ToInt(const std::string &command, int value, int enumIndex = -999);
		std::pair<std::string, CTweakVarData> ToFloat(const std::string &command, float value, int enumIndex = -999);
		std::pair<std::string, CTweakVarData> ToString(const std::string &command, const std::string &value, int enumIndex = -999);

	public:
		TweakVarAssetObject Load(const std::string &type, const std::string &name);
	};
}


#endif