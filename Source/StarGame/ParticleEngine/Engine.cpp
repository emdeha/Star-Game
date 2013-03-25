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
											 int newParticleLifeTime, float newSize, bool newIsLooping,
											 float newVelocityMultiplier,  
											 const std::string &textureFileName)
{
	position = newPosition;
	startingParticleCount = newParticleCount;
	particleCount = newParticleCount;
	particleLifeTime = newParticleLifeTime;
	velocityMultiplier = newVelocityMultiplier;
	size = newSize;
	
	isLooping = newIsLooping;
	//particles.resize(particleCount); // double resize resulting in bugs...
	
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
										  //0.0f,
										  ((float)rand() / RAND_MAX - 0.5f) * 
										  ((float)rand() / RAND_MAX) * velocityMultiplier,
										  ((float)rand() / RAND_MAX - 0.5f) * 
										  ((float)rand() / RAND_MAX) * velocityMultiplier);
		particles[i].interpColorBegin = glm::vec4(0.5f, 0.0f, 0.0f, 1.0f);
		particles[i].interpColorEnd = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		particles[i].color = particles[i].interpColorBegin;
		particles[i].lifeTime = (rand() % particleLifeTime) / 2.0f + particleLifeTime / 2.0f;
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
		float colorLerpFactor = particles[i].lifeTime / particleLifeTime;
		particles[i].color = 
			glm::mix(particles[i].interpColorBegin, particles[i].interpColorEnd, colorLerpFactor);
		if(particles[i].lifeTime <= 0)
		{
			if(isLooping)
			{
				particles[i].position = position + glm::vec3();
				particles[i].velocity = glm::vec3(((float)rand() / RAND_MAX - 0.5f) * 
												  ((float)rand() / RAND_MAX) * velocityMultiplier,
												  //0.0f,
												  ((float)rand() / RAND_MAX - 0.5f) * 
												  ((float)rand() / RAND_MAX) * velocityMultiplier,
												  ((float)rand() / RAND_MAX - 0.5f) * 
												  ((float)rand() / RAND_MAX) * velocityMultiplier);
				particles[i].color = particles[i].interpColorBegin;
				particles[i].lifeTime = (rand() % particleLifeTime) / 2.0f + particleLifeTime / 2.0f;
			}
			else
			{
				std::vector<ExplosionParticle>::iterator particleToErase =
					particles.begin();
				particleToErase += i;

				particles.erase(particleToErase);
				particleCount--;
			}
		}
	}
}

void SpriteParticleEmitter::Render(glutil::MatrixStack &modelMatrix, 
								   const SpriteParticleProgData &progData)
{
	glUseProgram(progData.theProgram);
	glBindVertexArray(vao);
	{
		glutil::PushStack push(modelMatrix);
		glUniformMatrix4fv(progData.modelToCameraMatrixUnif,
						   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));

		glEnableVertexAttribArray(progData.positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glVertexAttribPointer(progData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(progData.texCoordAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
		glVertexAttribPointer(progData.texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		texture->Bind(GL_TEXTURE0);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
		for(int i = 0; i < particleCount; i++)
		{			
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
		glDisableVertexAttribArray(progData.texCoordAttrib);

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


MeteoriteEmitter::MeteoriteEmitter(glm::vec3 newPosition, 
								   int newExplosionParticlesCount, int newMeteoriteParticlesCount,
								   int newExplosionParticlesLifeTime, 
								   float newExplosionParticlesSize, float newMeteoriteParticlesSize,
								   float newMeteoriteSpreadRadius,
								   float newExplosionParticlesVelocityMultiplier,
								   float newMeteoriteVelocityMultiplier,
								   const std::string &explosionParticlesTextureFileName,
								   const std::string &meteoriteParticlesTextureFileName)
{/*
	position = newPosition;	
	explosionParticlesCount = newExplosionParticlesCount;
	meteoriteParticlesCount = newMeteoriteParticlesCount;
	explosionParticlesLifeTime = newExplosionParticlesLifeTime;
	explosionParticlesSize = newExplosionParticlesSize;
	meteoriteParticlesSize = newMeteoriteParticlesSize;
	explosionParticlesVelocityMultiplier = newExplosionParticlesVelocityMultiplier;

	isActive = false;
	isDead = false;

	vao = 0;
	explosion*/
	position = newPosition;
	meteoriteParticlesCount = newMeteoriteParticlesCount;
	meteoriteStartingParticlesCount = meteoriteParticlesCount;
	meteoriteParticlesSize = newMeteoriteParticlesSize;
	meteoriteSpreadRadius = newMeteoriteSpreadRadius;
	meteoriteVelocityMultiplier = newMeteoriteVelocityMultiplier;

	isOnHitExplosionUpdate = false;
	
	isActive = false;
	isDead = false;

	vao = 0;
	meteoriteVertexBO = 0;
	textureCoordsBO = 0;
	indexBO = 0;

	meteoriteTexture = std::shared_ptr<Texture2D>(new Texture2D());
	if(!meteoriteTexture->Load(meteoriteParticlesTextureFileName, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE))
	{
		std::string errorMessage = "cannot load texture";
		errorMessage += meteoriteParticlesTextureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}

	std::vector<float> vertexData;
	std::vector<float> textureCoordsData;
	std::vector<unsigned short> indexData;

	vertexData.push_back(position.x - meteoriteParticlesSize);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z - meteoriteParticlesSize); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z - meteoriteParticlesSize); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z); vertexData.push_back(1.0f);

	vertexData.push_back(position.x - meteoriteParticlesSize);
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

	glGenBuffers(1, &meteoriteVertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, meteoriteVertexBO);	
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

	onHitExplosionEmitter = SpriteParticleEmitter(position, newExplosionParticlesCount, newExplosionParticlesLifeTime, 
												  newExplosionParticlesSize, false, newExplosionParticlesVelocityMultiplier, 
												  explosionParticlesTextureFileName);
}
// resize on init!!!
void MeteoriteEmitter::Init()
{
	isActive = false;
	isDead = false;
	meteorites.resize(meteoriteStartingParticlesCount);
	meteoriteParticlesCount = meteoriteStartingParticlesCount;
	for(int i = 0; i < meteoriteParticlesCount; i++)
	{
		meteorites[i].position = position + 
								 glm::vec3(((float)rand() / RAND_MAX * meteoriteSpreadRadius) - meteoriteSpreadRadius, 
										   3.0f, 
										   ((float)rand() / RAND_MAX * meteoriteSpreadRadius) - meteoriteSpreadRadius); // simulating height
		meteorites[i].velocity = 
			glm::vec3(0.0f, 
					  fabs(((float)rand() / RAND_MAX - 0.5f)) * (-meteoriteVelocityMultiplier), 
					  0.0f); // only rand speed on y
	}
	onHitExplosionEmitter.Init();
}

void MeteoriteEmitter::Update()
{
	if(meteoriteParticlesCount == 0)
	{
		isDead = true;
	}
	for(int i = 0; i < meteoriteParticlesCount; i++)
	{
		meteorites[i].position += meteorites[i].velocity;
		if(meteorites[i].position.y < 0.0f) // later: delete meteorite and set a bool flag indicating whether the onHit should be updated
		{		
			onHitExplosionEmitter.SetPosition(meteorites[i].position);
			onHitExplosionEmitter.Init();
			isOnHitExplosionUpdate = true;
			//onHitExplosionEmitter.Update(); 
			meteorites[i].position = position + 
									 glm::vec3(((float)rand() / RAND_MAX * meteoriteSpreadRadius) - meteoriteSpreadRadius, 
											   3.0f, 
											   ((float)rand() / RAND_MAX * meteoriteSpreadRadius) - meteoriteSpreadRadius);
		}
	}
	if(isOnHitExplosionUpdate)
	{
		onHitExplosionEmitter.Update(); 
		if(onHitExplosionEmitter.IsDead())
		{
			isOnHitExplosionUpdate = false;
		}
	}
}

void MeteoriteEmitter::Render(glutil::MatrixStack &modelMatrix,
							  const SpriteParticleProgData &progData)
{
	glUseProgram(progData.theProgram);
	glBindVertexArray(vao);
	{
		glutil::PushStack push(modelMatrix);
		glUniformMatrix4fv(progData.modelToCameraMatrixUnif, 
						   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));

		glEnableVertexAttribArray(progData.positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, meteoriteVertexBO);
		glVertexAttribPointer(progData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(progData.texCoordAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
		glVertexAttribPointer(progData.texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		meteoriteTexture->Bind(GL_TEXTURE0);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
		for(int i = 0; i < meteoriteParticlesCount; i++)
		{			
			glUniform3f(progData.deltaPositionUnif,
						meteorites[i].position.x, meteorites[i].position.y, meteorites[i].position.z);
			glUniform4f(progData.colorUnif, 1.0f, 1.0f, 1.0f, 1.0f);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		}
		
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glDisableVertexAttribArray(progData.positionAttrib);
		glDisableVertexAttribArray(progData.texCoordAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		meteoriteTexture->Unbind();
	}
	glBindVertexArray(0);
	glUseProgram(0);

	if(isOnHitExplosionUpdate)
	{
		onHitExplosionEmitter.Render(modelMatrix, progData);
	}
}

void MeteoriteEmitter::Activate()
{
	isActive = true;
}
bool MeteoriteEmitter::IsActive()
{
	return isActive;
}

bool MeteoriteEmitter::IsDead()
{
	return isDead;
}


RadialEmitter::RadialEmitter(glm::vec3 newPosition, 
							 float newParticleSize, float newSpreadRadius, float newParticleSpeed,
							 int newParticleCount, const std::string &particleTextureFileName)
{
	position = newPosition;
	particleSize = newParticleSize;
	spreadRadius = newSpreadRadius;
	particleCount = newParticleCount;
	startingParticleCount = particleCount;
	particleSpeed = newParticleSpeed;

	isActive = false;
	isDead = false;

	vao = 0;
	vertexBO = 0;
	textureCoordsBO = 0;
	indicesBO = 0;

	particleTexture = std::shared_ptr<Texture2D>(new Texture2D());
	if(!particleTexture->Load(particleTextureFileName, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE))
	{
		std::string errorMessage = "cannot load texture";
		errorMessage += particleTextureFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}

	std::vector<float> vertexData;
	std::vector<float> textureCoordsData;
	std::vector<unsigned short> indexData;

	vertexData.push_back(position.x - particleSize);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z - particleSize); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z - particleSize); vertexData.push_back(1.0f);

	vertexData.push_back(position.x);
	vertexData.push_back(position.y);
	vertexData.push_back(position.z); vertexData.push_back(1.0f);

	vertexData.push_back(position.x - particleSize);
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


	glGenBuffers(1, &indicesBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RadialEmitter::Init()
{
	isActive = false;
	isDead = false;
	particleCount = startingParticleCount;
	particles.resize(particleCount);
	
	float rotationDegs = 360.0f; // the length of the arc the particles should make
	float decrement = rotationDegs / (float)particleCount;
	glutil::MatrixStack rotMatrix;
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = position + glm::vec3();
		
		rotMatrix.SetIdentity();
		rotMatrix.RotateY(rotationDegs);
		glm::vec4 frontVector = glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f);
		frontVector = frontVector * rotMatrix.Top();

		particles[i].velocity = glm::vec3(frontVector) * particleSpeed;

		rotationDegs -= decrement;
	}
}

void RadialEmitter::Update()
{
	if(particleCount == 0)
	{
		isDead = true;
	}
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position += particles[i].velocity;

		glm::vec3 particleToOriginVector = particles[i].position - position;
		if(glm::length(particleToOriginVector) > spreadRadius)
		{
			std::vector<StandardParticle>::iterator particleToErase =
				particles.begin();
			particleToErase += i;

			particles.erase(particleToErase);
			particleCount--;
		}
	}
}

void RadialEmitter::Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &progData)
{
	glUseProgram(progData.theProgram);
	glBindVertexArray(vao);
	{
		glutil::PushStack push(modelMatrix);
		glUniformMatrix4fv(progData.modelToCameraMatrixUnif, 
						   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));

		glEnableVertexAttribArray(progData.positionAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glVertexAttribPointer(progData.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(progData.texCoordAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBO);
		glVertexAttribPointer(progData.texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		particleTexture->Bind(GL_TEXTURE0);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBO);
		for(int i = 0; i < particleCount; i++)
		{			
			glUniform3f(progData.deltaPositionUnif,
						particles[i].position.x, particles[i].position.y, particles[i].position.z);
			glUniform4f(progData.colorUnif, 1.0f, 1.0f, 1.0f, 1.0f);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		}
		
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glDisableVertexAttribArray(progData.positionAttrib);
		glDisableVertexAttribArray(progData.texCoordAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		particleTexture->Unbind();
	}
	glBindVertexArray(0);
	glUseProgram(0);
}

void RadialEmitter::Activate()
{
	isActive = true;
}
bool RadialEmitter::IsActive()
{
	return isActive;
}

bool RadialEmitter::IsDead()
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