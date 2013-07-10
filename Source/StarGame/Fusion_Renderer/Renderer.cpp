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

void Renderer::SubscribeForRendering(EntityManager *manager, Entity *entity)
{
	ComponentMapper<Mesh> meshData = manager->GetComponentList(entity, CT_MESH);

	glGenVertexArrays(1, &meshData[0]->mesh.vao);
	glBindVertexArray(meshData[0]->mesh.vao);


    std::vector<std::shared_ptr<MeshEntry>> meshEntries = meshData[0]->mesh.mesh.GetMeshEntries();
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
    }

	glBindVertexArray(0);
	subscribedMeshes.push_back(std::make_pair<unsigned int, MeshData>(entity->GetIndex(), meshData[0]->mesh));
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
    {
		glBindVertexArray(subscribedMesh->second.vao);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		
		
		std::vector<std::shared_ptr<MeshEntry>> entries = subscribedMesh->second.mesh.GetMeshEntries();
        for(std::vector<std::shared_ptr<MeshEntry>>::const_iterator entry = entries.begin(); 
			entry != entries.end(); ++entry)
        {
			glUseProgram(subscribedMesh->second.shaderProgram); 
		
            glutil::PushStack push(modelMatrix);

			ComponentMapper<Transform> transformData = manager->GetComponentList(subscribedMesh->first, CT_TRANSFORM);
			modelMatrix.Translate(transformData[0]->position);
			modelMatrix.RotateX(transformData[0]->rotation.x);
			modelMatrix.RotateY(transformData[0]->rotation.y);
			modelMatrix.RotateZ(transformData[0]->rotation.z);
			modelMatrix.Scale(transformData[0]->scale);


			glUniformMatrix4fv(glGetUniformLocation(subscribedMesh->second.shaderProgram, "modelToCameraMatrix"),
                1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
			glUniform4f(glGetUniformLocation(subscribedMesh->second.shaderProgram, "color"),
				        1.0f, 1.0f, 1.0f, 1.0f);

			
            glBindBuffer(GL_ARRAY_BUFFER, entry->get()->vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FusionEngine::Vertex), 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FusionEngine::Vertex), (const GLvoid*)12);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FusionEngine::Vertex), (const GLvoid*)20);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entry->get()->indexBuffer);
			
			if(entry->get()->materialIndex < subscribedMesh->second.mesh.GetTextures().size() &&
			   subscribedMesh->second.mesh.GetTextures()[entry->get()->materialIndex])
            {
				subscribedMesh->second.mesh.GetTextures()[entry->get()->materialIndex]->Bind(GL_TEXTURE0);
            }
			

			glDrawElements(GL_TRIANGLES, entry->get()->indicesCount, GL_UNSIGNED_INT, 0);

			glUseProgram(0);
        }

        
        glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
    }

	glBindVertexArray(0);
	glFrontFace(GL_CW);
}