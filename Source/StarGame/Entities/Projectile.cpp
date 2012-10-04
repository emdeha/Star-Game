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
#include "Enemy.h"


static void GenerateUniformBuffers(int &materialBlockSize, 
								   glm::vec4 diffuseColor,
								   GLuint &materialUniformBuffer)
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


Projectile::Projectile(glm::vec3 newPosition, glm::vec3 newVelocity,
					   int newDamage)
{
	position = newPosition;
	velocity = newVelocity;
	damage = newDamage;

	isDestroyed = false;
}

void Projectile::LoadMesh(const std::string &meshFile)
{
	try
	{
		std::auto_ptr<Framework::Mesh> newMesh(new Framework::Mesh(meshFile));
		mesh = newMesh;
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}

	GenerateUniformBuffers(materialBlockSize, glm::vec4(0.0f), materialUniformBuffer);
}

void Projectile::LoadMesh(const std::auto_ptr<Framework::Mesh> newMesh)
{
	mesh = std::auto_ptr<Framework::Mesh>(new Framework::Mesh(*newMesh.get()));

	GenerateUniformBuffers(materialBlockSize, glm::vec4(0.0f), materialUniformBuffer);
}

void Projectile::Update(Sun &sun)
{
	if(!isDestroyed)
	{
		position += velocity;

		if(IsTargetHit(glm::vec3(sun.GetPosition()), sun.GetRadius()))
		{
			std::printf("%f, %f, %f", sun.GetPosition().x, sun.GetPosition().y, sun.GetPosition().z);

			this->OnTargetHit(sun);
		}
	}
}

void Projectile::Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
						float gamma, 
						const LitProgData &litData)
{
	{
		glutil::PushStack push(modelMatrix);

		modelMatrix.Translate(position);
		modelMatrix.Scale(0.1f);

		glBindBufferRange(GL_UNIFORM_BUFFER, materialBlockIndex, materialUniformBuffer,
						  0, sizeof(MaterialBlock));

		glm::mat3 normMatrix(modelMatrix.Top());
		normMatrix = glm::transpose(glm::inverse(normMatrix));

		glUseProgram(litData.theProgram);
		glUniformMatrix4fv(litData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniformMatrix3fv(litData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));

		mesh->Render("lit");

		glUseProgram(0);

		glBindBufferBase(GL_UNIFORM_BUFFER, materialBlockIndex, 0);
	}
}

bool Projectile::IsTargetHit(glm::vec3 targetPosition, float targetRadius)
{
	glm::vec3 distance = position - targetPosition;
	float distanceLength = 
		distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;

	if(distanceLength <= targetRadius * targetRadius)
	{
		return true;
	}
	else return false;
}

void Projectile::OnTargetHit(Sun &sun)
{
	EventArg sunHitEventArg[1];
	sunHitEventArg[0].argType = "damage";
	sunHitEventArg[0].argument.varType = TYPE_INTEGER;
	sunHitEventArg[0].argument.varInteger = damage;
	Event sunHitEvent(1, EVENT_TYPE_ATTACKED, sunHitEventArg);

	sun.OnEvent(sunHitEvent);

	
	mesh->DeleteObjects();
	isDestroyed = true;
}