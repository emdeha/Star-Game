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


#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H


#include <map>
#include <vector>
#include <string>

#include "../ProgramData/ProgramData.h"



// TODO: Dynamic enums for shader variables would speed up the 
//		 generic shader manager


#include "../Fusion_Renderer/ShaderEnums.h"


class ShaderManager
{
private:
	std::map<FusionEngine::BlockType, int> blockIndices;
	std::map<FusionEngine::UniformBufferType, unsigned int> uniformBuffers; 

	LitProgData litData;
	LitTextureProgData litTextureData;
	UnlitProgData unlitData;
	SimpleProgData simpleData;
	SimpleProgData simpleNoUBData;
	FontProgData fontData;
	SimpleTextureProgData simpleTextureProgData;
	TextureProgData textureProgData;
	TextureProgData perspTextureProgData;
	BillboardProgDataNoTexture billboardProgDataNoTexture;
	BillboardProgData billboardProgData;
	ParticleProgData particleProgData;
	SpriteParticleProgData spriteParticleProgData;

public:
	ShaderManager();

	void LoadLitProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadLitTextureProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadUnlitProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadSimpleProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadSimpleNoUBProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadFontProgram(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadSimpleTextureProgData(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadTextureProgData(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadPerspectiveTextureProgData(const std::string &vertexShader, const std::string &fragmentShader);
	void LoadBillboardProgDataNoTexture(const std::string &vertexShader,
										const std::string &geometryShader,
										const std::string &fragmentShader);
	void LoadBillboardProgData(const std::string &vertexShader, 
							   const std::string &geometryShader,
							   const std::string &fragmentShader);
	void LoadParticleProgData(const std::string &vertexShader,
							  const std::string &geometryShader);
	void LoadSpriteParticleProgData(const std::string &vertexShader, const std::string &fragmentShader);


	LitProgData GetLitProgData();
	LitTextureProgData GetLitTextureProgData();
	UnlitProgData GetUnlitProgData();
	SimpleProgData GetSimpleProgData();
	SimpleProgData GetSimpleNoUBProgData();
	FontProgData GetFontProgData();
	SimpleTextureProgData GetSimpleTextureProgData();
	TextureProgData GetTextureProgData();
	TextureProgData GetPerspectiveTextureProgData();
	BillboardProgDataNoTexture GetBillboardProgDataNoTexture();
	BillboardProgData GetBillboardProgData();
	ParticleProgData GetParticleProgData();
	SpriteParticleProgData GetSpriteParticleProgData();


	int GetBlockIndex(FusionEngine::BlockType);
	unsigned int GetUniformBuffer(FusionEngine::UniformBufferType);
	void SetUniformBuffer(FusionEngine::UniformBufferType, unsigned int value);
};



#endif