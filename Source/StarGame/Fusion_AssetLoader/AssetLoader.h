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


#include "../glsdk/glm/glm.hpp"

#include "../AssetLoader/Texture.h"
#include "../StarGame/AudioEnums.h"
#include "../GUISystem/GUISystem.h"
#include "../assimp/include/assimp/Importer.hpp"
#include "../assimp/include/assimp/scene.h"
#include "../assimp/include/assimp/postprocess.h"
#include "yaml-cpp/yaml.h"

#include "../Fusion_Renderer/ShaderEnums.h"


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
		if(loaders.find(type) == loaders.end())
		{
			loaders.insert(std::make_pair<const std::string&, ITypeLoader<ObjectToLoad>*&>(type, loader));
		}
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
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 textureCoord;
		glm::vec3 normal;
		
		Vertex() {}

		Vertex(const glm::vec3 &newPosition,
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
		GLuint vertexBuffer;
		GLuint indexBuffer;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		unsigned int materialIndex;
		unsigned int indicesCount;

		MeshEntry() {}
		MeshEntry(unsigned int newMaterialIndex)
		{
			materialIndex = newMaterialIndex;
			vertexBuffer = -1;
			indexBuffer = -1;
			indicesCount = 0;
		}
		~MeshEntry();

		void Init(const std::vector<Vertex> &newVertices,
				  const std::vector<unsigned int> &newIndices);
	};

	class MeshAssetObject : public IAssetObject
	{
	private:
		std::vector<std::shared_ptr<MeshEntry>> meshEntries;
		std::vector<std::shared_ptr<Texture2D>> textures;

	public:
		GLuint vao;

    public:
		MeshAssetObject() {}

		void AddEntry(const std::shared_ptr<MeshEntry>& newMeshEntry);
		void AddTexture(const std::shared_ptr<Texture2D>& newTexture);


		const std::vector<std::shared_ptr<MeshEntry>> GetMeshEntries() const;
		const std::vector<std::shared_ptr<Texture2D>> GetTextures() const;
	};

	class MeshLoader : public ITypeLoader<MeshAssetObject>
	{
	private:
		GLuint vao;

		void InitFromScene(const aiScene *scene, MeshAssetObject &loadedMesh, const std::string &name);
		void InitMesh(unsigned int index, const aiMesh *mesh, MeshAssetObject &loadedMesh);
		void InitMaterials(const aiScene *scene, MeshAssetObject &loadedMesh, const std::string &name);

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

		const std::shared_ptr<Texture2D> GetLoadedTexture() const;
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
	struct AudioFile
	{
		std::string path;
		SoundType soundType;
		bool isLooping;
	};

	struct AudioData
	{
		ChannelType channel;
		float channelVolume;
		std::vector<AudioFile> audioFiles;
	};

	class AudioAssetObject : public IAssetObject
	{
	private:
		std::vector<std::pair<ChannelType, AudioData>> loadedAudio;

	public:
		AudioAssetObject() {}
		AudioAssetObject(const std::vector<std::pair<ChannelType, AudioData>> &newLoadedAudio) 
		{
			loadedAudio = newLoadedAudio;
		}

		const std::vector<std::pair<ChannelType, AudioData>> GetAllLoadedAudios() const;
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
	struct LayoutData
	{
		LayoutType layoutType;
		glm::vec4 layoutColor;
		int layoutIsSet;
		std::string layoutBackgroundImageFileName;
	};

	struct ControlData
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

		std::shared_ptr<Layout> GetLayout(LayoutType layoutType) const;
		const std::map<LayoutType, std::shared_ptr<Layout>> &GetAllLoadedLayouts() const;
	};

	class GUILoader : public ITypeLoader<GUIAssetObject>
	{
	private:
	public:
		GUIAssetObject Load(const std::string &type, const std::string &name);
	};


	/////////////////////////
	///  TweakVar Loader  ///
	/////////////////////////
	struct TweakVarData
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

		TweakVarData() : itemIndex(-1) {}
	};

	class TweakVarAssetObject : public IAssetObject
	{
	private:
		std::vector<std::pair<std::string, TweakVarData>> loadedTweaks;

	public:
		TweakVarAssetObject() {}
		TweakVarAssetObject(const std::vector<std::pair<std::string, TweakVarData>> &newLoadedTweaks)
		{
			loadedTweaks = newLoadedTweaks;
		}

		const std::vector<std::pair<std::string, TweakVarData>> GetAllLoadedVars() const;
	};

	class TweakVarLoader : public ITypeLoader<TweakVarAssetObject>
	{
	private:
		std::pair<std::string, TweakVarData> ToInt(const std::string &command, int value, int enumIndex = -999) const;
		std::pair<std::string, TweakVarData> ToFloat(const std::string &command, float value, int enumIndex = -999) const;
		std::pair<std::string, TweakVarData> ToString(const std::string &command, const std::string &value, int enumIndex = -999) const;

	public:
		TweakVarAssetObject Load(const std::string &type, const std::string &name);
	};

	/////////////////////
	//  Shader Loader  //
	/////////////////////
	struct ProgramData
	{
		GLuint programId;

		std::map<UniformType, GLuint> uniforms;
		std::map<AttribType, GLuint> attribs;
	};

	class ShaderAssetObject : public IAssetObject
	{
	private:
		std::map<ProgramType, ProgramData> loadedPrograms;

		std::map<UniformBufferType, unsigned int> uniformBuffers;
		std::map<BlockType, int> blockIndices;

	public:
		ShaderAssetObject() {}
		ShaderAssetObject(const std::map<ProgramType, ProgramData> &newLoadedPrograms,
						  const std::map<UniformBufferType, unsigned int> &newUniformBuffers,
						  const std::map<BlockType, int> &newBlockIndices)
			: loadedPrograms(newLoadedPrograms), 
			  uniformBuffers(newUniformBuffers), blockIndices(newBlockIndices) {}

		unsigned int GetUniformBuffer(UniformBufferType ubType) const;
		int GetLoadedBlockIndex(BlockType blockType) const;
		
		ProgramData GetLoadedProgram(ProgramType progType) const;
		const std::map<ProgramType, ProgramData> &GetAllLoadedPrograms() const;
	};

	class ShaderLoader : public ITypeLoader<ShaderAssetObject>
	{
	private:
		ProgramType GetProgramTypeFromString(const std::string &strId) const;

		std::pair<BlockType, int> GetBlockPairFromString(const std::string &strId) const;
		std::pair<UniformBufferType, unsigned int> GetUBPairFromString(const std::string &strId) const;
		
		UniformType GetUniformTypeFromString(const std::string &strId) const;
		AttribType GetAttribTypeFromString(const std::string &strId) const;

	public:
		ShaderAssetObject Load(const std::string &type, const std::string &name);
	};
}


#endif