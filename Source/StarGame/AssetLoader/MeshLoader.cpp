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


#include "stdafx.h"
#include "MeshLoader.h"
#include "../Entities/MaterialBlock.h"
#include "../framework/ErrorAPI.h"


#define DIR "../data/mesh-files/"


const std::string ExtractDirectory(const std::string &fileName)
{
	std::string::size_type slashIndex = fileName.find_last_of("/");
	std::string directory;

	if(slashIndex == std::string::npos)
	{
		directory = ".";
	}
	else if(slashIndex == 0)
	{
		directory = "/";
	}
	else
	{
		directory = fileName.substr(0, slashIndex);
	}

	return directory;
}


static void GenerateUniformBuffers(int &materialBlockSize, glm::vec4 diffuseColor, GLuint &materialUniformBuffer)
{
	MaterialBlock material;
	material.diffuseColor = diffuseColor;
	material.specularColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	material.shininessFactor = 0.3f;


	int uniformBufferAlignSize = 0;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);

	materialBlockSize = sizeof(MaterialBlock);
	materialBlockSize += uniformBufferAlignSize -
		(materialBlockSize % uniformBufferAlignSize);

	
	glGenBuffers(1, &materialUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, materialUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, &material, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}



Mesh::MeshEntry::MeshEntry()
{
	vertexBufferObject = -1;
	indexBufferObject = -1;
	indicesCount = 0;
	materialIndex = -1;
}

Mesh::MeshEntry::~MeshEntry()
{
	if(vertexBufferObject != -1)
	{
		glDeleteBuffers(1, &vertexBufferObject);
	}
	if(indexBufferObject != -1)
	{
		glDeleteBuffers(1, &indexBufferObject);
	}
}

void Mesh::MeshEntry::Init(const std::vector<Vertex> &vertices,
						   const std::vector<unsigned int> &indices)
{
	indicesCount = indices.size();

	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &indexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, &indices[0], GL_STATIC_DRAW);
}

Mesh::Mesh() {}

bool Mesh::LoadMesh(const std::string &fileName)
{
	textures.clear();

    const aiScene *scene = 
		aiImportFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if(scene)
	{		
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		return InitFromScene(scene, fileName);
	}
	else
	{
		std::string errorMessage = "cannot parse ";
		errorMessage += fileName;
		errorMessage += ": ";
		errorMessage += aiGetErrorString();
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
		return false;
	}
}

void Mesh::LoadLight()
{	
	GenerateUniformBuffers(materialBlockSize, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), materialUniformBuffer);
}

bool Mesh::InitFromScene(const aiScene *scene, const std::string &fileName)
{
	entries.resize(scene->mNumMeshes);
	textures.resize(scene->mNumMaterials);

	for(unsigned int i = 0; i < entries.size(); i++)
	{
		const aiMesh *mesh = scene->mMeshes[i];
		InitMesh(i, mesh);
	}

	return InitMaterials(scene, fileName);
}

void Mesh::InitMesh(unsigned int index, const aiMesh *mesh)
{
	entries[index].materialIndex = mesh->mMaterialIndex;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const aiVector3D zeroVector(0.0f, 0.0f, 0.0f);

	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// TODO: Use smart ptrs
		const aiVector3D *position = &(mesh->mVertices[i]);
		const aiVector3D *normal = &(mesh->mNormals[i]);
		const aiVector3D *textureCoordinate = mesh->HasTextureCoords(0) ?
			&(mesh->mTextureCoords[0][i]) : &zeroVector;

		Vertex vert(glm::vec3(position->x, position->y, position->z), 
					glm::vec2(textureCoordinate->x, textureCoordinate->y), 
					glm::vec3(normal->x, normal->y, normal->z));
		vertices.push_back(vert);
	}

	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace &face = mesh->mFaces[i];

		//assert(face.mNumIndices == 3);

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	entries[index].Init(vertices, indices);
}

bool Mesh::InitMaterials(const aiScene *scene, const std::string &fileName)
{
	std::string directory = ExtractDirectory(fileName);

	// Initialize the materials.
	for(unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial *material = scene->mMaterials[i];

		textures[i] = NULL;

		if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;

			if(material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				// std::string fullPath = directory + '/' + path.data;
				std::string fullPath = DIR;
				fullPath.append(path.data);
				textures[i] = std::shared_ptr<Texture2D>(new Texture2D());

				if(!textures[i]->Load(fullPath, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE))
				{
					std::printf("Error loading texture '%s'\n", fullPath.c_str());
					return textures[i]->Load("../data/mesh-files/white.png", GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
				}
			}
		}
	

		// Load a white texture in case the model does not include its own texture
		if(!textures[i])
		{
			textures[i] = std::shared_ptr<Texture2D>(new Texture2D());

			std::string path = fileName;
			path.erase(path.end() - 4, path.end());
			path += ".jpg";

			if(!textures[i]->Load(path, GL_RGB, GL_BGR, GL_UNSIGNED_BYTE))
			{
				//HandleUnexpectedError("cannot load texture", __LINE__, __FILE__);
				if(!textures[i]->Load("../data/mesh-files/white.png", GL_RGB, GL_BGR, GL_UNSIGNED_BYTE))
				{
					HandleUnexpectedError("cannot load texture", __LINE__, __FILE__);
				}
				return true;
			}
			return true;
		}
	}
	return false;
}

void Mesh::Render(glutil::MatrixStack &modelMatrix, const SimpleTextureProgData &progData)
{
	glFrontFace(GL_CCW); // Fixes face culling issues with rendering
	
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	for(unsigned int i = 0; i < entries.size(); i++)
	{
		glUseProgram(progData.theProgram);
		
		glutil::PushStack push(modelMatrix);

		glUniformMatrix4fv(progData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		

		glBindBuffer(GL_ARRAY_BUFFER, entries[i].vertexBufferObject);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entries[i].indexBufferObject);

		const unsigned int materialIndex = entries[i].materialIndex;

		if(materialIndex < textures.size() && textures[materialIndex])
		{
			textures[materialIndex]->Bind(GL_TEXTURE0);
		}

		glDrawElements(GL_TRIANGLES, entries[i].indicesCount, GL_UNSIGNED_INT, 0);

		glUseProgram(0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindVertexArray(0);

	glFrontFace(GL_CW);
}

void Mesh::Render(glutil::MatrixStack &modelMatrix, const LitTextureProgData &progData,
				  int materialBlockIndex)
{
	glFrontFace(GL_CCW);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for(unsigned int i = 0; i < entries.size(); i++)
	{
		glUseProgram(progData.theProgram);

		glutil::PushStack push(modelMatrix);

		glBindBufferRange(GL_UNIFORM_BUFFER, materialBlockIndex, materialUniformBuffer, 0, sizeof(MaterialBlock));

		glm::mat3 normMatrix(modelMatrix.Top());
		normMatrix = glm::transpose(glm::inverse(normMatrix));

		glUniformMatrix4fv(progData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniformMatrix3fv(progData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));

		glBindBuffer(GL_ARRAY_BUFFER, entries[i].vertexBufferObject);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entries[i].indexBufferObject);

		const unsigned int materialIndex = entries[i].materialIndex;
		if(materialIndex < textures.size() && textures[materialIndex])
		{
			textures[materialIndex]->Bind(GL_TEXTURE0);
		}

		glDrawElements(GL_TRIANGLES, entries[i].indicesCount, GL_UNSIGNED_INT, 0);

		glBindBufferBase(GL_UNIFORM_BUFFER, materialBlockIndex, 0);

		glUseProgram(0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindVertexArray(0);

	glFrontFace(GL_CW);
}