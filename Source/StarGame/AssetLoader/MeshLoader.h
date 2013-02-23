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



// TODO: Add VAO support.


#ifndef MESH_LOADER_H
#define MESH_LOADER_H


#define INVALID_MATERIAL 0xFFFFFFFF


#include <vector>

#include "..\assimp\include\assimp\cimport.h"
#include "..\assimp\include\assimp\scene.h"
#include "..\assimp\include\assimp\postprocess.h"

#include <glm\glm.hpp>


#include "Texture.h"
#include "../ProgramData/ProgramData.h"


struct Vertex
{
	glm::vec3 position;
	glm::vec2 textureCoordinate;
	glm::vec3 normal;

	Vertex() {};

	Vertex(const glm::vec3 &newPosition,
		   const glm::vec2 &newTextureCoordinate,
		   const glm::vec3 &newNormal)
	{
		position = newPosition;
		textureCoordinate = newTextureCoordinate;
		normal = newNormal;
	}
};


class Mesh
{
private:
	bool InitFromScene(const aiScene *scene, const std::string &fileName);
	void InitMesh(unsigned int index, const aiMesh *mesh);
	bool InitMaterials(const aiScene *scene, const std::string &fileName);

	struct MeshEntry
	{
		MeshEntry();
		~MeshEntry();

		void Init(const std::vector<Vertex> &vertices,
				  const std::vector<unsigned int> &indices);

		GLuint vertexBufferObject;
		GLuint indexBufferObject;
		//GLuint vao;
		unsigned int indicesCount;
		unsigned int materialIndex;
	};

	GLuint vao;
	int materialBlockSize;
	GLuint materialUniformBuffer;

	std::vector<MeshEntry> entries;
	std::vector<std::shared_ptr<Texture2D>> textures;

public:
	Mesh();

	bool LoadMesh(const std::string &fileName);
	void LoadLight();
	void Render(glutil::MatrixStack &modelMatrix, const SimpleTextureProgData &progData);
	void Render(glutil::MatrixStack &modelMatrix, const LitTextureProgData &progData,
				int materialBlockIndex);
};

#endif