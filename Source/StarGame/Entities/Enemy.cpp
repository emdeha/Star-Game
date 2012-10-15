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

Spaceship::Spaceship(glm::vec3 newPosition, 
					 glm::vec3 newDirection, 
					 glm::vec3 newVelocity,
					 float newProjectileSpeed,
					 int newProjectileLifeSpan, int newProjectileDamage)
{
	position = newPosition;
	direction = newDirection;
	velocity = newVelocity;

	health = 100;

	projectileSpeed = newProjectileSpeed;
	projectileLifeSpan = newProjectileLifeSpan;
	projectileDamage = newProjectileDamage;

	projectile = 
		std::unique_ptr<Projectile>(new Projectile(position, 
												   direction * projectileSpeed, 
												   projectileDamage, 
												   projectileLifeSpan));
}

void Spaceship::LoadMesh(const std::string &meshFile)
{
	try
	{
		mesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(meshFile));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}

	GenerateUniformBuffers(materialBlockSize, 
						   glm::vec4(0.21f, 0.42f, 0.34f, 1.0f), 
						   materialUniformBuffer);

	projectile->LoadMesh(meshFile);
}

void Spaceship::Update(bool isSunKilled, Sun &sun)
{
	if(!isSunKilled)
	{
		if(projectile->IsDestroyed())
		{
			glm::vec3 newDirection = glm::vec3();

			if(sun.HasSatellites())
			{
				glm::vec3 satellitePosition = sun.GetOuterSatellite()->GetPosition();
				newDirection = satellitePosition - position;
			}
			else
			{
				newDirection = glm::vec3(sun.GetPosition()) - position;
			}

			newDirection = glm::normalize(newDirection);
			projectile->Recreate(position, 
									newDirection * projectileSpeed, 
									projectileLifeSpan);
		}

		projectile->Update(sun);
	}
}

void Spaceship::Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
					   float gamma, 
					   const LitProgData &litData,
					   float interpolation)
{
	{
		glutil::PushStack push(modelMatrix);


		glm::vec3 viewPosition = position + velocity * interpolation;
		modelMatrix.Translate(position);

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

	if(!projectile->IsDestroyed())
	{
		projectile->Render(modelMatrix, materialBlockIndex, gamma, litData,
						   interpolation);
	}
}

void Spaceship::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_SHOT_FIRED:
		break;
	case EVENT_TYPE_ATTACKED:
		break;
	}
}