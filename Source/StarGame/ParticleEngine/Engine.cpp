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
#include "Engine.h"



ParticleEmitter::ParticleEmitter(glm::vec3 newPosition, int newParticleCount)
{
	position = newPosition;
	particleCount = newParticleCount;

	particles.resize(particleCount);

	vao = 0;
	vertexBO = 0;

	texture = Texture2D();
	texture.Load("../data/images/particle.png");


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, glm::value_ptr(position), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void ParticleEmitter::Init(const BillboardProgData &billboardedProgData)
{
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = position;
		particles[i].velocity = glm::vec3((float)rand() / RAND_MAX * 0.01f,
										  (float)rand() / RAND_MAX * 0.01f,
										  (float)rand() / RAND_MAX * 0.01f);
	}

	glUseProgram(billboardedProgData.theProgram);
	glUniform1f(billboardedProgData.billboardSizeUnif, 0.1f);
	glUseProgram(0);
}


void ParticleEmitter::Update()
{
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position += particles[i].velocity;
	}
}

void ParticleEmitter::Render(glutil::MatrixStack &modelMatrix,
							 glm::vec3 cameraPosition,
							 const BillboardProgData &programData)
{
	glutil::PushStack push(modelMatrix);

	glUseProgram(programData.theProgram);

	
	glUniformMatrix4fv(programData.modelToCameraMatrixUnif,
					   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
	glUniform3f(programData.cameraPositionUnif, 
				cameraPosition.x, cameraPosition.y, cameraPosition.z);

	texture.Bind(GL_TEXTURE0);

	for(int i = 0; i < particleCount; i++)
	{
		glUniform3f(programData.deltaPositionUnif, 
					particles[i].position.x, particles[i].position.y, particles[i].position.z);

		glDrawArrays(GL_POINTS, 0, 1);
	}

	glDisableVertexAttribArray(0);

	glUseProgram(0);
}



SwarmEmitter::SwarmEmitter(glm::vec3 newPosition, int newParticleCount)
{
	position = newPosition;
	particleCount = newParticleCount;

	particles.resize(particleCount);

	vao = 0;
	vertexBO = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, glm::value_ptr(position), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void SwarmEmitter::Init(const BillboardProgDataNoTexture &billboardProgDataNoTexture)
{
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = position + glm::vec3(2.0f * (float) rand() / RAND_MAX - 1.0f, 
													 2.0f * (float) rand() / RAND_MAX - 1.0f, 
													 2.0f * (float) rand() / RAND_MAX - 1.0f);
		particles[i].velocity = (glm::vec3() - position) * 0.0001f;
	}

	glUseProgram(billboardProgDataNoTexture.theProgram);
	glUniform1f(billboardProgDataNoTexture.billboardSizeUnif, 0.1f);
	glUseProgram(0);
}

void SwarmEmitter::Update()
{
	for(int i = 0; i < particles.size(); i++)
	{
		particles[i].position += particles[i].velocity;
	}
}
void SwarmEmitter::Update(glm::vec3 otherVelocity)
{
	for(int i = 0; i < particles.size(); i++)
	{
		particles[i].position += otherVelocity;
	}
}

void SwarmEmitter::Render(glutil::MatrixStack &modelMatrix,
						  glm::vec3 cameraPosition,
						  const BillboardProgDataNoTexture &billboardProgDataNoTexture)
{
	glutil::PushStack push(modelMatrix);

	glUseProgram(billboardProgDataNoTexture.theProgram);

	
	glUniformMatrix4fv(billboardProgDataNoTexture.modelToCameraMatrixUnif,
					   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
	glUniform3f(billboardProgDataNoTexture.cameraPositionUnif, 
				cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniform4f(billboardProgDataNoTexture.colorUnif,
				1.0f, 0.0f, 0.0f, 1.0f);


	for(int i = 0; i < particleCount; i++)
	{
		glUniform3f(billboardProgDataNoTexture.deltaPositionUnif, 
					particles[i].position.x, particles[i].position.y, particles[i].position.z);

		glDrawArrays(GL_POINTS, 0, 1);
	}

	glDisableVertexAttribArray(0);

	glUseProgram(0);
}



ExplosionEmitter::ExplosionEmitter(glm::vec3 newPosition, int newParticleCount,
								   int newParticleLifeTime,
								   float newVelocityMultiplier)
{
	position = newPosition;
	particleCount = newParticleCount;

	particles.resize(particleCount);

	particleLifeTime = newParticleLifeTime;

	velocityMultiplier = newVelocityMultiplier;

	isActive = false;
	isDead = false;

	vao = 0;
	vertexBO = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, glm::value_ptr(position), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void ExplosionEmitter::Init(const BillboardProgDataNoTexture &billboardProgDataNoTexture)
{
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = position + glm::vec3();
		particles[i].velocity = glm::vec3(((float)rand() / RAND_MAX - 0.5f) * 
										  ((float)rand() / RAND_MAX) * velocityMultiplier,
										  ((float)rand() / RAND_MAX - 0.5f) * 
										  ((float)rand() / RAND_MAX) * velocityMultiplier,
										  ((float)rand() / RAND_MAX - 0.5f) * 
										  ((float)rand() / RAND_MAX) * velocityMultiplier);
		particles[i].color = glm::vec4((float) rand() / RAND_MAX, 0.0f, 0.0f, 1.0f);
		particles[i].lifeTime = rand() % particleLifeTime;
	}

	glUseProgram(billboardProgDataNoTexture.theProgram);
	glUniform1f(billboardProgDataNoTexture.billboardSizeUnif, 0.1f);
	glUseProgram(0);
}

void ExplosionEmitter::SetPosition(glm::vec3 newPosition)
{
	position = newPosition;
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = newPosition;
	}
}

void ExplosionEmitter::Update()
{
	if(particleCount == 0)
	{
		isDead = true;
	}
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position += particles[i].velocity;
		particles[i].lifeTime--;
		if(particles[i].lifeTime <= 0)
		{
			std::vector<ExplosionParticle>::iterator particleToErase = 
				particles.begin();
			particleToErase += i;

			particles.erase(particleToErase);
			particleCount--;
		}
	}
}

void ExplosionEmitter::Render(glutil::MatrixStack &modelMatrix,
							  glm::vec3 cameraPosition,
							  const BillboardProgDataNoTexture &billboardProgDataNoTexture)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);

	glutil::PushStack push(modelMatrix);

	glUseProgram(billboardProgDataNoTexture.theProgram);

	glUniformMatrix4fv(billboardProgDataNoTexture.modelToCameraMatrixUnif,
					   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
	glUniform3f(billboardProgDataNoTexture.cameraPositionUnif,
				cameraPosition.x, cameraPosition.y, cameraPosition.z);

	for(int i = 0; i < particleCount; i++)
	{
		glUniform3f(billboardProgDataNoTexture.deltaPositionUnif,
					particles[i].position.x, particles[i].position.y, particles[i].position.z);
		glUniform4f(billboardProgDataNoTexture.colorUnif,
					particles[i].color.r, particles[i].color.g, 
					particles[i].color.b, particles[i].color.a);

		glDrawArrays(GL_POINTS, 0, 1);

	}

	glDisableVertexAttribArray(0);

	glUseProgram(0);
}

void ExplosionEmitter::Activate()
{
	isActive = true;
}
bool ExplosionEmitter::IsActive()
{
	return isActive;
}

bool ExplosionEmitter::IsDead()
{
	return isDead;
}