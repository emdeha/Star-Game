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
#include "BasicAnimation.h"
#include "MaterialBlock.h"
#include "../framework/ErrorAPI.h"


#define PI 3.141592


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


AnimationBody::AnimationBody(const AnimationBody &other)
{
	this->materialBlockSize = other.materialBlockSize;
	this->materialUniformBuffer = other.materialUniformBuffer;
	this->mesh(other.mesh.get());
}

AnimationBody AnimationBody::operator=(const AnimationBody &other)
{
	this->materialBlockSize = other.materialBlockSize;
	this->materialUniformBuffer = other.materialUniformBuffer;
	this->mesh(other.mesh.get());

	return *this;
}

AnimationBody::AnimationBody(const std::string &meshFileName)
{
	// TODO: Change mesh
	// TODO: Error checking
	try
	{
		mesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(meshFileName));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}

	GenerateUniformBuffers(materialBlockSize, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), materialUniformBuffer);
}


OrbitingAnimationBody::OrbitingAnimationBody(glm::vec3 newCenter, float newOffsetFromCenter, 
											 int rotationSpeed, 
											 const std::string &meshFileName)
											 : AnimationBody(meshFileName)
{
	center = newCenter;
	offsetFromCenter = newOffsetFromCenter;
	revolutionDuration = Framework::Timer(Framework::Timer::TT_LOOP, rotationSpeed);
}

void OrbitingAnimationBody::Update()
{
	revolutionDuration.Update();

	float currentTimeThroughLoop = revolutionDuration.GetAlpha();
	
	position.x = sinf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromCenter;
	position.z = cosf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromCenter;
}

void OrbitingAnimationBody::Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
								   const LitProgData &progData)
{
	{
		glutil::PushStack push(modelMatrix);
		modelMatrix.Translate(position);
		modelMatrix.Scale(0.2f);

		glBindBufferRange(GL_UNIFORM_BUFFER, materialBlockIndex, materialUniformBuffer, 0, sizeof(MaterialBlock));

		glm::mat3 normMatrix(modelMatrix.Top());
		normMatrix = glm::transpose(glm::inverse(normMatrix));

		glUseProgram(progData.theProgram);
		glUniformMatrix4fv(progData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniformMatrix3fv(progData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));

		mesh->Render("lit");

		glUseProgram(0);

		glBindBufferBase(GL_UNIFORM_BUFFER, materialBlockIndex, 0);
	}
}

void Animation::AddAnimationBody(const std::shared_ptr<AnimationBody> newAnimationBody)
{
	animationBodies.push_back(newAnimationBody);
}

void Animation::Update()
{
	for(int animBody = 0; animBody < animationBodies.size(); animBody++)
	{
		animationBodies[animBody]->Update();
	}
}

void Animation::Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex, const LitProgData &progData)
{
	for(int animBody = 0; animBody < animationBodies.size(); animBody++)
	{
		animationBodies[animBody]->Render(modelMatrix, materialBlockIndex, progData);
	}
}


ParticleAnimation::ParticleAnimation(glm::vec3 newPosition, int newParticleCount,
									 int newParticleLifeTime, float newSize, bool isLooping,
									 float newVelocityMultiplier, 
									 const std::string &textureFileName)
{
	burnAnim = 
		SpriteParticleEmitter(newPosition, newParticleCount, newParticleLifeTime, newSize, isLooping, newVelocityMultiplier, textureFileName);
	burnAnim.Init();
}

void ParticleAnimation::Update()
{
	burnAnim.Update();
}

void ParticleAnimation::Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &spriteParticleProgData)
{
	burnAnim.Render(modelMatrix, spriteParticleProgData);
}


AoEAnimation::AoEAnimation(glm::vec3 position, 
						   float newAnimDuration_secs,
						   int explosionParticlesCount, int meteoriteParticlesCount,
						   int explosionParticlesLifeTime,
						   float explosionParticlesSize, float meteoriteParticlesSize, 
						   float meteoriteSpreadRadius,
						   float explosionParticleVelocityMultiplier,
						   float meteoriteVelocityMultiplier,
						   const std::string &explosionParticlesTextureFileName,
						   const std::string &meteoriteParticlesTextureFileName)
{
	meteoriteRain = MeteoriteEmitter(position, explosionParticlesCount, meteoriteParticlesCount,
									 explosionParticlesLifeTime, explosionParticlesSize, meteoriteParticlesSize,
									 meteoriteSpreadRadius, explosionParticleVelocityMultiplier,
									 meteoriteVelocityMultiplier,
									 explosionParticlesTextureFileName, meteoriteParticlesTextureFileName);
	meteoriteRain.Init();

	animDuration_secs = newAnimDuration_secs;
	animTimer = Framework::Timer(Framework::Timer::TT_SINGLE, animDuration_secs);
}

void AoEAnimation::Update()
{
	if(!animTimer.Update())
	{
		meteoriteRain.Update();
	}
}

void AoEAnimation::Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &spriteParticleProgData)
{
	if(animTimer.GetProgression() < animTimer.GetDuration())
	{
		meteoriteRain.Render(modelMatrix, spriteParticleProgData);
	}
}

void AoEAnimation::Restart()
{
	animTimer.Reset();
}

bool AoEAnimation::IsEnded()
{
	return animTimer.GetProgression() >= animTimer.GetDuration();
}


FrostNovaAnimation::FrostNovaAnimation(glm::vec3 position, 
									   float particleSize, float spreadRadius, float particleSpeed,
									   int particleCount, const std::string &particleTextureFileName)
{
	frostSpikes = RadialEmitter(position, particleSize, spreadRadius, particleSpeed, particleCount, 
								particleTextureFileName);
	frostSpikes.Init();
}

void FrostNovaAnimation::Update()
{
	frostSpikes.Update();
}

void FrostNovaAnimation::Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &spriteParticleProgData)
{
	frostSpikes.Render(modelMatrix, spriteParticleProgData);
}

void FrostNovaAnimation::Restart()
{
	frostSpikes.Init();
}

bool FrostNovaAnimation::IsEnded()
{
	return frostSpikes.IsDead();
}


SunNovaAnimation::SunNovaAnimation(glm::vec3 position, glm::vec4 color,
								   float particleSize, float spreadRadius, float particleSpeed,
								   int particleCount, const std::string &particleTextureFileName)
{
	novaCircleEmitter = RadialEmitter(position, particleSize, spreadRadius, particleSpeed, particleCount, 
									  particleTextureFileName, color);
	novaCircleEmitter.Init();
}

void SunNovaAnimation::Update()
{
	novaCircleEmitter.Update();
}

void SunNovaAnimation::Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &spriteParticleProgData)
{
	novaCircleEmitter.Render(modelMatrix, spriteParticleProgData);
}

void SunNovaAnimation::Restart()
{
	novaCircleEmitter.Init();
}

bool SunNovaAnimation::IsEnded()
{
	return novaCircleEmitter.IsDead();
}