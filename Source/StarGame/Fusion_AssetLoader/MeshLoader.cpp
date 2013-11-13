#include "stdafx.h"
#include "AssetLoader.h"

#include "../framework/ErrorAPI.h"


using namespace FusionEngine;


MeshEntry::~MeshEntry()
{
	if(vertexBuffer != -1)
    {
        glDeleteBuffers(1, &vertexBuffer);
    }
    if(indexBuffer != -1)
    {
        glDeleteBuffers(1, &indexBuffer);
    }
}

void MeshEntry::Init(const std::vector<Vertex> &newVertices,
					 const std::vector<unsigned int> &newIndices)
{
	vertices = newVertices;
	indices = newIndices;

	indicesCount = indices.size();
}


MeshAssetObject MeshLoader::Load(const std::string &type, const std::string &name)
{
	MeshAssetObject loadedMesh;

	Assimp::Importer importer;

	const aiScene *scene = importer.ReadFile("../data/" + type + "/" + name, 
											 aiProcess_Triangulate | aiProcess_GenNormals);

	if(!scene)
    {
		std::string errorMessage = "cannot parse ";
        errorMessage += type + "/" + name; 
        errorMessage += ": ";
        errorMessage += importer.GetErrorString();
        HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
    }
    else 
    {
		InitFromScene(scene, loadedMesh, name);
    }

	return loadedMesh;
}

void MeshLoader::InitFromScene(const aiScene *scene, MeshAssetObject &loadedMesh, const std::string &name)
{
	for(unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
		const aiMesh *mesh = scene->mMeshes[i];
		InitMesh(i, mesh, loadedMesh);
    }

	InitMaterials(scene, loadedMesh, name);
}

void MeshLoader::InitMesh(unsigned int index, const aiMesh *mesh, MeshAssetObject &loadedMesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const aiVector3D zeroVector(0.0f, 0.0f, 0.0f);

	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
		const aiVector3D *position = &(mesh->mVertices[i]);
		const aiVector3D *normal = &(mesh->mNormals[i]);
		const aiVector3D *textureCoordinate = 
			mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zeroVector;

		Vertex vert(glm::vec3(position->x, position->y, position->z),
					glm::vec2(textureCoordinate->x, textureCoordinate->y),
					glm::vec3(normal->x, normal->y, normal->z));
		vertices.push_back(vert);
    }

	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
		const aiFace &face = mesh->mFaces[i];

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
    }

	std::shared_ptr<MeshEntry> newMeshEntry = std::shared_ptr<MeshEntry>(new MeshEntry(mesh->mMaterialIndex));
	newMeshEntry->Init(vertices, indices);

	loadedMesh.AddEntry(newMeshEntry);
}

void MeshLoader::InitMaterials(const aiScene *scene, MeshAssetObject &loadedMesh, const std::string &name)
{
	for(unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
		const aiMaterial *material = scene->mMaterials[i];

		std::shared_ptr<Texture2D> newTexture;

		if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
			aiString path;

			if(material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
				if(!newTexture->Load(path.C_Str(), GL_RGB, GL_RGB, GL_UNSIGNED_BYTE))
                {
					HandleUnexpectedError("loading texture " + std::string(path.C_Str()), __LINE__, __FILE__);
					return;
                }
            }
        }

		// Try finding a texture with the mesh's file name
		if(!newTexture)
        {
			newTexture = std::shared_ptr<Texture2D>(new Texture2D());

			std::string textureName = name;
			textureName.erase(textureName.end() - 4, textureName.end());
			textureName.append(".png");
            
			if(!newTexture->Load(textureName, GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE))
            {
				std::string pathToWhiteTexture = "../data/mesh-files/white.png";
				if(!newTexture->Load(pathToWhiteTexture, GL_RGB, GL_BGR, GL_UNSIGNED_BYTE))
				{
					HandleUnexpectedError("loading DEFAULT texture from path " + pathToWhiteTexture, __LINE__, __FILE__);
					return;
				}
            }
        }

		loadedMesh.AddTexture(newTexture);
    }
}


void MeshAssetObject::AddEntry(const std::shared_ptr<MeshEntry>& newMeshEntry)
{
	meshEntries.push_back(newMeshEntry);
}
void MeshAssetObject::AddTexture(const std::shared_ptr<Texture2D>& newTexture) 
{
	textures.push_back(newTexture);
}

const std::vector<std::shared_ptr<MeshEntry>> MeshAssetObject::GetMeshEntries() const
{
	return meshEntries;
}
const std::vector<std::shared_ptr<Texture2D>> MeshAssetObject::GetTextures() const
{
	return textures;
}