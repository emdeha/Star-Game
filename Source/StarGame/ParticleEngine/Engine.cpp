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
#include "../framework/ErrorAPI.h"


ParticleEmitter::ParticleEmitter(glm::vec3 newPosition, int newParticleCount)
{
	position = newPosition;
	particleCount = newParticleCount;

	particles.resize(particleCount);

	vao = 0;
	vertexBO = 0;

	texture = Texture2D();
	if(!texture.Load("../data/images/particle.png"))
	{
		std::string errorMessage = "cannot load particle texture ";
		errorMessage += "../data/images/particle.png";
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}


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



SwarmEmitter::SwarmEmitter(glm::vec3 newPosition, glm::vec4 newColor, int newParticleCount)
{
	position = newPosition;
	color = newColor;
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
	glBindVertexArray(vao);
	
	glUniformMatrix4fv(billboardProgDataNoTexture.modelToCameraMatrixUnif,
					   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
	glUniform3f(billboardProgDataNoTexture.cameraPositionUnif, 
				cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniform4f(billboardProgDataNoTexture.colorUnif,
				color.r, color.g, color.b, color.a);


	for(int i = 0; i < particleCount; i++)
	{
		glUniform3f(billboardProgDataNoTexture.deltaPositionUnif, 
					particles[i].position.x, particles[i].position.y, particles[i].position.z);

		glDrawArrays(GL_POINTS, 0, 1);
	}

	glDisableVertexAttribArray(0);

	glUseProgram(0);
}

void SwarmEmitter::SetColor(glm::vec4 newColor)
{
	color = newColor;
}


ExplosionEmitter::ExplosionEmitter(glm::vec3 newPosition, int newParticleCount,
								   int newParticleLifeTime,
								   float newVelocityMultiplier)
{
	position = newPosition;
	startingParticleCount = newParticleCount;
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

void ExplosionEmitter::Init()
{
	isActive = false;
	isDead = false;
	particles.resize(startingParticleCount);
	particleCount = startingParticleCount;
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
	glUniform1f(billboardProgDataNoTexture.billboardSizeUnif, 0.1f);

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


TexturedExplosionEmitter::TexturedExplosionEmitter(glm::vec3 newPosition, int newParticleCount,
												   int newParticleLifeTime, 
												   float newSize,
												   float newVelocityMultiplier,
												   const std::string &textureFileName)
{
	position = newPosition;
	startingParticleCount = newParticleCount;
	particleCount = newParticleCount;
	particleLifeTime = newParticleLifeTime;
	velocityMultiplier = newVelocityMultiplier;
	size = newSize;

	particles.resize(particleCount);

	isActive = false;
	isDead = false;

	vao = 0;
	vertexBO = 0;
	textureCoordsBO = 0;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, glm::value_ptr(position), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	texture = std::shared_ptr<Texture2D>(new Texture2D());
	if(!texture->Load(textureFileName, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE))
	{
		std::string errorMessage = "cannot load texture";
		errorMessage += textureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}
}

void TexturedExplosionEmitter::Init()
{
	isActive = false;
	isDead = false;
	particles.resize(startingParticleCount);
	particleCount = startingParticleCount;
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = position + glm::vec3();
		particles[i].velocity = glm::vec3(((float)rand() / RAND_MAX - 0.5f) * 
										  ((float)rand() / RAND_MAX) * velocityMultiplier,
										  0.0f,
										  //((float)rand() / RAND_MAX - 0.5f) * 
										  //((float)rand() / RAND_MAX) * velocityMultiplier,
										  ((float)rand() / RAND_MAX - 0.5f) * 
										  ((float)rand() / RAND_MAX) * velocityMultiplier);
		particles[i].color = glm::vec4(0.9f, 0.5f, 0.0f, 1.0f);
		particles[i].lifeTime = rand() % particleLifeTime;
	}
}

void TexturedExplosionEmitter::SetPosition(glm::vec3 newPosition)
{
	position = newPosition;
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = newPosition;
	}
}

void TexturedExplosionEmitter::Update()
{
	if(particleCount == 0)
	{
		isDead = true;
	}
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position += particles[i].velocity;
		particles[i].lifeTime--;
		particles[i].color.g -= 0.001f;
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

void TexturedExplosionEmitter::Render(glutil::MatrixStack &modelMatrix,
									  glm::vec3 cameraPosition,
									  const BillboardProgData &billboardProgData)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);

	glutil::PushStack push(modelMatrix);

	glUseProgram(billboardProgData.theProgram);

	glUniformMatrix4fv(billboardProgData.modelToCameraMatrixUnif, 
					   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
	glUniform3f(billboardProgData.cameraPositionUnif,
				cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniform1f(billboardProgData.billboardSizeUnif, size);//0.1f);

	texture->Bind(GL_TEXTURE0);
	
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_DST_COLOR, GL_ONE);

	for(int i = 0; i < particleCount; i++)
	{
		glUniform3f(billboardProgData.deltaPositionUnif,
					particles[i].position.x, particles[i].position.y, particles[i].position.z);
		glUniform4f(billboardProgData.colorUnif,
					particles[i].color.r, particles[i].color.g,
					particles[i].color.b, particles[i].color.a);
		
		glDrawArrays(GL_POINTS, 0, 1);
	}
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glDisableVertexAttribArray(0);

	glUseProgram(0);
}

void TexturedExplosionEmitter::Activate()
{
	isActive = true;
}
bool TexturedExplosionEmitter::IsActive()
{
	return isActive;
}

bool TexturedExplosionEmitter::IsDead()
{
	return isDead;
}


SpriteParticleEmitter::SpriteParticleEmitter(glm::vec3 newPosition, int newParticleCount,
											 int newParticleLifeTime, float newSize,
											 float newVelocityMultiplier, 
											 const std::string &textureFileName)
{
	position = newPosition;
	startingParticleCount = newParticleCount;
	particleCount = newParticleCount;
	particleLifeTime = newParticleLifeTime;
	velocityMultiplier = newVelocityMultiplier;
	size = newSize;

	particles.resize(particleCount);

	isActive = false;
	isDead = false;

	vao = 0;
	vertexBO = 0;
	textureCoordsBO = 0;
	indexBO = 0;

	texture = std::shared_ptr<Texture2D>(new Texture2D());
	if(!texture->Load(textureFileName, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE))
	{
		std::string errorMessage = "cannot load texture";
		errorMessage += textureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}


	std::vector<float> vertexData;
	std::vector<float> textureCoordsData;
	std::vector<unsigned short> indexData;

	vertexData.push_back(position.x - size);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z - size); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z - size); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z); vertexData.push_back(1.0f);

	vertexData.push_back(position.x - size);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z); vertexData.push_back(1.0f);
		
	textureCoordsData.push_back(0.0f); textureCoordsData.push_back(1.0f);
	textureCoordsData.push_back(1.0f); textureCoordsData.push_back(1.0f);
	textureCoordsData.push_back(1.0f); textureCoordsData.push_back(0.0f);
	textureCoordsData.push_back(0.0f); textureCoordsData.push_back(0.0f);

	indexData.push_back(0); indexData.push_back(1); indexData.push_back(2);
	indexData.push_back(2); indexData.push_back(3); indexData.push_back(0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);	
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	glGenBuffers(1, &textureCoordsBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * textureCoordsData.size(), &textureCoordsData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SpriteParticleEmitter::Init()
{
	isActive = false;
	isDead = false;
	particles.resize(startingParticleCount);
	particleCount = startingParticleCount;
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = position + glm::vec3();
		particles[i].velocity = glm::vec3(((float)rand() / RAND_MAX - 0.5f) * 
										  ((float)rand() / RAND_MAX) * velocityMultiplier,
										  0.0f,
										  //((float)rand() / RAND_MAX - 0.5f) * 
										  //((float)rand() / RAND_MAX) * velocityMultiplier,
										  ((float)rand() / RAND_MAX - 0.5f) * 
										  ((float)rand() / RAND_MAX) * velocityMultiplier);
		particles[i].color = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
		particles[i].lifeTime = rand() % particleLifeTime;
	}
}

void SpriteParticleEmitter::SetPosition(glm::vec3 newPosition)
{
	position = newPosition;
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = newPosition;
	}
}

void SpriteParticleEmitter::Update()
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

void SpriteParticleEmitter::Render(glutil::MatrixStack &modelMatrix, 
								   const SpriteParticleData &progData)
{
	glUseProgram(progData.theProgram);
	glBindVertexArray(vao);
	{
		// TODO: Add particle shader with deltaPos optimization
		glutil::PushStack push(modelMatrix);
		glUniformMatrix4fv(progData.modelToCameraMatrixUnif,
						   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));

		glEnableVertexAttribArray(progData.positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glVertexAttribPointer(progData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(progData.textureCoordAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
		glVertexAttribPointer(progData.textureCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		texture->Bind(GL_TEXTURE0);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
		for(int i = 0; i < particleCount; i++)
		{			
			//glutil::PushStack push(modelMatrix);
			//modelMatrix.Translate(particles[i].position.x, particles[i].position.y, particles[i].position.z);

			//glUniformMatrix4fv(progData.modelToCameraMatrixUnif, 
			//				   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
			glUniform3f(progData.deltaPositionUnif,
						particles[i].position.x, particles[i].position.y, particles[i].position.z);
			glUniform4f(progData.colorUnif,
						particles[i].color.r, particles[i].color.g,
						particles[i].color.b, particles[i].color.a);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		}
		
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glDisableVertexAttribArray(progData.positionAttrib);
		glDisableVertexAttribArray(progData.textureCoordAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		texture->Unbind();
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

void SpriteParticleEmitter::Activate()
{
	isActive = true;
}
bool SpriteParticleEmitter::IsActive()
{
	return isActive;
}

bool SpriteParticleEmitter::IsDead()
{
	return isDead;
}


RayEmitter::RayEmitter(glm::vec3 newPosition, int newParticleCount,
					   float newRayLength)
{
	position = newPosition;
	particleCount = newParticleCount;
	rayLength = newRayLength;
	vertexBO = 0;
	vao = 0;
	particles.resize(particleCount);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, glm::value_ptr(position), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void RayEmitter::Init()
{
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = position;
		particles[i].velocity = glm::vec3(0.0f, (float)rand() / RAND_MAX, 0.0f);
		particles[i].color = glm::vec4(0.4f, 0.6f, 0.0f, 1.0f);
	}
}

void RayEmitter::Update()
{
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position += particles[i].velocity;
		if(glm::length(particles[i].position - position) >= rayLength)
		{
			particles[i].position = position;
		}
	}
}

void RayEmitter::Render(glutil::MatrixStack &modelMatrix, 
						glm::vec3 cameraPosition,
						const BillboardProgDataNoTexture &billboardData)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);

	{
		glutil::PushStack push(modelMatrix);

		glUseProgram(billboardData.theProgram);

		glUniformMatrix4fv(billboardData.modelToCameraMatrixUnif,
						   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform3f(billboardData.cameraPositionUnif,
					cameraPosition.x, cameraPosition.y, cameraPosition.z);
		glUniform1f(billboardData.billboardSizeUnif, 0.2f);

		for(int i = 0; i < particleCount; i++)
		{
			glUniform3f(billboardData.deltaPositionUnif,
						particles[i].position.x, particles[i].position.y, particles[i].position.z);
			glUniform4f(billboardData.colorUnif,
						particles[i].color.r, particles[i].color.g, 
						particles[i].color.b, particles[i].color.a);

			glDrawArrays(GL_POINTS, 0, 1);

		}
	}

	glDisableVertexAttribArray(0);

	glUseProgram(0);
}