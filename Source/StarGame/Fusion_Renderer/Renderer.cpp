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

	subscribedMeshes.push_back(std::make_pair<EntityID, MeshData>(entity->GetID(), meshData[0]->mesh));
}
void Renderer::UnsubscribeForRendering(Entity *entity)
{
	for(std::vector<std::pair<EntityID, MeshData>>::iterator subscribedMesh = subscribedMeshes.begin();
		subscribedMesh != subscribedMeshes.end(); ++subscribedMesh)
    {
		if(subscribedMesh->first == entity->GetID())
        {
			subscribedMeshes.erase(subscribedMesh);
			break;
        }
    }
}


void Renderer::Render(glutil::MatrixStack &modelMatrix)
{
	glFrontFace(GL_CCW);

	for(std::vector<std::pair<EntityID, MeshData>>::iterator subscribedMesh = subscribedMeshes.begin();
		subscribedMesh != subscribedMeshes.end(); ++subscribedMesh)
    {
		glBindVertexArray(subscribedMesh->second.vao);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

        glUseProgram(subscribedMesh->second.shaderProgram); 

		
        for(std::vector<MeshEntry>::const_iterator entry = subscribedMesh->second.mesh.GetMeshEntries().begin();
			entry != subscribedMesh->second.mesh.GetMeshEntries().end(); ++entry)
        {
			glutil::PushStack push(modelMatrix);
			GLuint modelToCameraMatrixUnif = 0;
			glGetUniformLocation(modelToCameraMatrixUnif, "modelToCameraMatrix");
			glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));


			glBindBuffer(GL_ARRAY_BUFFER, entry->vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entry->indexBuffer);

			if(entry->materialIndex < subscribedMesh->second.mesh.GetTextures().size() &&
			   subscribedMesh->second.mesh.GetTextures()[entry->materialIndex])
            {
				subscribedMesh->second.mesh.GetTextures()[entry->materialIndex]->Bind(GL_TEXTURE0);
            }

			glDrawElements(GL_TRIANGLES, entry->indicesCount, GL_UNSIGNED_INT, 0);
        }

		
		glUseProgram(0);
        
        glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		
        glBindVertexArray(0);
    }

	glFrontFace(GL_CW);
}