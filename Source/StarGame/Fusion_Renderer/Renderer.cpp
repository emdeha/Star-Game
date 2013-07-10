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
#include "Renderer.h"


using namespace FusionEngine;


static void GenerateUniformBuffers(int &materialBlockSize, glm::vec4 diffuseColor, GLuint &materialUniformBuffer)
{
	Material material;
	material.diffuseColor = glm::vec4(1.0f);
	material.specularColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	material.shininessFactor = 0.3f;

	int uniformBufferAlignSize = 0;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);

	materialBlockSize = sizeof(Material);
	materialBlockSize += uniformBufferAlignSize - 
		(materialBlockSize % uniformBufferAlignSize);

	glGenBuffers(1, &materialUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, materialUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, &material, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void Renderer::SubscribeForRendering(EntityManager *manager, Entity *entity)
{
	ComponentMapper<FusionEngine::Render> renderData = manager->GetComponentList(entity, CT_RENDER);

	glGenVertexArrays(1, &renderData[0]->vao);
	glBindVertexArray(renderData[0]->vao);


    std::vector<std::shared_ptr<MeshEntry>> meshEntries = renderData[0]->mesh.GetMeshEntries();
	for(auto meshEntry = meshEntries.begin(); meshEntry != meshEntries.end(); ++meshEntry)
    {
		glGenBuffers(1, &meshEntry->get()->vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, meshEntry->get()->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * meshEntry->get()->vertices.size(), 
									  &meshEntry->get()->vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &meshEntry->get()->indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEntry->get()->indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshEntry->get()->indices.size(), 
											  &meshEntry->get()->indices[0], GL_STATIC_DRAW);

		if(renderData[0]->rendererType == Render::FE_RENDERER_LIT)
        {
			int materialBlockSize = 0;
			GenerateUniformBuffers(materialBlockSize, glm::vec4(1.0f), renderData[0]->materialUniformBuffer);
        }
    }

	glBindVertexArray(0);
	subscribedMeshes.push_back(std::make_pair<unsigned int, MeshAssetObject>(entity->GetIndex(), renderData[0]->mesh));
}
void Renderer::UnsubscribeForRendering(Entity *entity)
{
	for(auto subscribedMesh = subscribedMeshes.begin(); subscribedMesh != subscribedMeshes.end(); ++subscribedMesh)
    {
		if(subscribedMesh->first == entity->GetID())
        {
			subscribedMeshes.erase(subscribedMesh);
			break;
        }
    }
}


void Renderer::Render(glutil::MatrixStack &modelMatrix,
					  EntityManager *manager)
{
	glFrontFace(GL_CCW);

	for(auto subscribedMesh = subscribedMeshes.begin(); subscribedMesh != subscribedMeshes.end(); ++subscribedMesh)
    {		ComponentMapper<FusionEngine::Render> renderData = manager->GetComponentList(subscribedMesh->first, CT_RENDER);

		glBindVertexArray(renderData[0]->vao);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		
		
		
		std::vector<std::shared_ptr<MeshEntry>> entries = subscribedMesh->second.GetMeshEntries();
        for(std::vector<std::shared_ptr<MeshEntry>>::const_iterator entry = entries.begin(); 
			entry != entries.end(); ++entry)
        {
			glUseProgram(renderData[0]->shaderProgram); 
		
            glutil::PushStack push(modelMatrix);


			ComponentMapper<Transform> transformData = manager->GetComponentList(subscribedMesh->first, CT_TRANSFORM);
			modelMatrix.Translate(transformData[0]->position);
			modelMatrix.RotateX(transformData[0]->rotation.x);
			modelMatrix.RotateY(transformData[0]->rotation.y);
			modelMatrix.RotateZ(transformData[0]->rotation.z);
			modelMatrix.Scale(transformData[0]->scale);

			glUniformMatrix4fv(glGetUniformLocation(renderData[0]->shaderProgram, "modelToCameraMatrix"),
                1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
			glUniform4f(glGetUniformLocation(renderData[0]->shaderProgram, "color"),
				        1.0f, 1.0f, 1.0f, 1.0f);

			if(renderData[0]->rendererType == Render::FE_RENDERER_LIT)
            {
				glBindBufferRange(GL_UNIFORM_BUFFER, entry->get()->materialIndex, 
								  renderData[0]->materialUniformBuffer, 0, sizeof(Material));

				glm::mat3 normMatrix(modelMatrix.Top());
				normMatrix = glm::transpose(glm::inverse(normMatrix));

				glUniformMatrix3fv(glGetUniformLocation(renderData[0]->shaderProgram, "normalModelToCameraMatrix"),
								   1, GL_FALSE, glm::value_ptr(normMatrix));
            }


			
            glBindBuffer(GL_ARRAY_BUFFER, entry->get()->vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FusionEngine::Vertex), 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FusionEngine::Vertex), (const GLvoid*)12);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FusionEngine::Vertex), (const GLvoid*)20);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entry->get()->indexBuffer);
			
			if(entry->get()->materialIndex < subscribedMesh->second.GetTextures().size() &&
			   subscribedMesh->second.GetTextures()[entry->get()->materialIndex])
            {
				subscribedMesh->second.GetTextures()[entry->get()->materialIndex]->Bind(GL_TEXTURE0);
            }
			

			glDrawElements(GL_TRIANGLES, entry->get()->indicesCount, GL_UNSIGNED_INT, 0);
			
            if(renderData[0]->rendererType == Render::FE_RENDERER_LIT)
            {
				glBindBufferBase(GL_UNIFORM_BUFFER, entry->get()->materialIndex, 0);
            }
            
            glUseProgram(0);
        }

        
        glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
    }

	glBindVertexArray(0);
	glFrontFace(GL_CW);
}