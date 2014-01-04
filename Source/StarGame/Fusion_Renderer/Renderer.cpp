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

#include "../Fusion_Entities/Components.h"
#include "../Fusion_Scene/World.h"


using namespace FusionEngine;


static void GenerateUniformBuffers(int &materialBlockSize, glm::vec4 diffuseColor, GLuint &materialUniformBuffer)
{
	Material material;
	material.diffuseColor = glm::vec4(1.0f);
	material.specularColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	material.shininessFactor = 0.3f;

	int uniformBufferAlignSize = 0;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);

	materialBlockSize += uniformBufferAlignSize - 
		(materialBlockSize % uniformBufferAlignSize);

	glGenBuffers(1, &materialUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, materialUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, &material, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void Renderer::SubscribeForRendering(const std::string &id, const MeshAssetObject &mesh)
{
	RenderComponent *meshRender = 
		static_cast<RenderComponent*>(GetWorld().GetComponentForObject(id, FE_COMPONENT_RENDER).get());

	glGenVertexArrays(1, &meshRender->vao);
	glBindVertexArray(meshRender->vao);
	
	modelToCameraMatrixUniform.insert(std::make_pair(id, 
		glGetUniformLocation(meshRender->shaderProgramID, "modelToCameraMatrix")));
	colorUniform.insert(std::make_pair(id,
		glGetUniformLocation(meshRender->shaderProgramID, "color")));
	normalModelToCameraMatrixUniform.insert(std::make_pair(id,
		glGetUniformLocation(meshRender->shaderProgramID, "normalModelToCameraMatrix")));

	positionAttrib.insert(std::make_pair(id,
		glGetAttribLocation(meshRender->shaderProgramID, "position")));
	textureAttrib.insert(std::make_pair(id,
		glGetAttribLocation(meshRender->shaderProgramID, "texCoord")));
	normalAttrib.insert(std::make_pair(id,
		glGetAttribLocation(meshRender->shaderProgramID, "normal")));


    auto meshEntries = mesh.GetMeshEntries();
	for(auto meshEntry = meshEntries.begin(); meshEntry != meshEntries.end(); ++meshEntry)
    {
		if(meshRender->renderType == RenderComponent::FE_RENDERER_LIT)
        {
			int materialUBSize = 0;
			GenerateUniformBuffers(materialUBSize, glm::vec4(1.0f), meshRender->materialUniformBuffer);
			glBindBufferRange(GL_UNIFORM_BUFFER, meshEntry->get()->materialIndex, 
							  meshRender->materialUniformBuffer, 0, materialUBSize);
		}

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
	subscribedMeshes.insert(std::make_pair(id, mesh));
}
void Renderer::UnsubscribeForRendering(/*unsigned int*/const std::string &id)
{
	auto meshForUnsubscription = subscribedMeshes.find(id);
	if (meshForUnsubscription != subscribedMeshes.end())
	{
		subscribedMeshes.erase(meshForUnsubscription);
	}
	else
	{
		std::ostringstream errorMsg;
		errorMsg << "Cannot find mesh with id: " << id;
		HandleUnexpectedError(errorMsg.str(), __LINE__, __FILE__);
	}
}


void Renderer::Render(glutil::MatrixStack &modelMatrix) const
{
	glFrontFace(GL_CCW);

	for (auto subscribedMesh = subscribedMeshes.begin(); subscribedMesh != subscribedMeshes.end(); ++subscribedMesh)
    {
		RenderComponent *meshRender = 
			static_cast<RenderComponent*>(GetWorld().
				GetComponentForObject((*subscribedMesh).first, FE_COMPONENT_RENDER).get());

		glBindVertexArray(meshRender->vao);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		if (meshRender->renderType == RenderComponent::FE_RENDERER_LIT)
		{
			glEnableVertexAttribArray(2);
		}

		
		std::vector<std::shared_ptr<MeshEntry>> entries = subscribedMesh->second.GetMeshEntries();
        for(std::vector<std::shared_ptr<MeshEntry>>::const_iterator entry = entries.begin(); 
			entry != entries.end(); ++entry)
        {
			glUseProgram(meshRender->shaderProgramID); 
		
            glutil::PushStack push(modelMatrix);
			
			
			TransformComponent *meshTransform =
				static_cast<TransformComponent*>(GetWorld().
					GetComponentForObject((*subscribedMesh).first, FE_COMPONENT_TRANSFORM).get());

			modelMatrix.Translate(meshTransform->position);
			modelMatrix.RotateX(meshTransform->rotation.x);
			modelMatrix.RotateY(meshTransform->rotation.y);
			modelMatrix.RotateZ(meshTransform->rotation.z);
			modelMatrix.Scale(meshTransform->scale);

			glUniformMatrix4fv((*modelToCameraMatrixUniform.find(subscribedMesh->first)).second,
							   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
			glUniform4f((*colorUniform.find(subscribedMesh->first)).second, 
						1.0f, 1.0f, 1.0f, 1.0f);

			if(meshRender->renderType == RenderComponent::FE_RENDERER_LIT)
            {
				glBindBufferRange(GL_UNIFORM_BUFFER, entry->get()->materialIndex, 
								  meshRender->materialUniformBuffer, 0, sizeof(Material));

				glm::mat3 normMatrix(modelMatrix.Top());
				normMatrix = glm::transpose(glm::inverse(normMatrix));

				glUniformMatrix3fv((*normalModelToCameraMatrixUniform.find(subscribedMesh->first)).second, 
								   1, GL_FALSE, glm::value_ptr(normMatrix));
            }

            glBindBuffer(GL_ARRAY_BUFFER, entry->get()->vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FusionEngine::Vertex), 0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FusionEngine::Vertex), (const GLvoid*)12);
			if (meshRender->renderType == RenderComponent::FE_RENDERER_LIT)
			{
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FusionEngine::Vertex), (const GLvoid*)20);
			}

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entry->get()->indexBuffer);
			
			if(entry->get()->materialIndex < subscribedMesh->second.GetTextures().size() &&
			   subscribedMesh->second.GetTextures()[entry->get()->materialIndex])
            {
				subscribedMesh->second.GetTextures()[entry->get()->materialIndex]->Bind(GL_TEXTURE0);
            }
			

			glDrawElements(GL_TRIANGLES, entry->get()->indicesCount, GL_UNSIGNED_INT, 0);
			
            if(meshRender->renderType == RenderComponent::FE_RENDERER_LIT)
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