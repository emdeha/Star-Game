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


#ifndef BASIC_ANIMATION_H
#define BASIC_ANIMATION_H


#include <vector>
#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>
#include "../framework/Mesh.h"
#include "../framework/Timer.h"
#include "../ProgramData/ProgramData.h"
#include "../ParticleEngine/Engine.h"


class AnimationBody
{
protected:
	std::unique_ptr<Framework::Mesh> mesh;
		
	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	AnimationBody() {}
	AnimationBody(const std::string &meshFileName);
	AnimationBody(const AnimationBody &other);
	AnimationBody operator=(const AnimationBody &other);

	virtual ~AnimationBody() {}

	virtual void Update() {}
	virtual void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
						const LitProgData &progData) {}
};


class OrbitingAnimationBody : public AnimationBody
{
private:
	Framework::Timer revolutionDuration;
	glm::vec3 position;
	glm::vec3 center;
	float offsetFromCenter;

public:
	OrbitingAnimationBody(glm::vec3 newCenter, float newOffsetFromCenter,
						  int rotationSpeed,
						  const std::string &meshFileName);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
				const LitProgData &progData);
};


class Animation
{
private:
	std::vector<std::shared_ptr<AnimationBody>> animationBodies;
		
public:
	Animation() {}

	void AddAnimationBody(const std::shared_ptr<AnimationBody> newAnimationBody);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex, const LitProgData &progData);
};


class ParticleAnimation
{
private:
	SpriteParticleEmitter burnAnim;

public:
	ParticleAnimation() {}
	ParticleAnimation(glm::vec3 newPosition, int newParticleCount,
					  int newParticleLifeTime, float newSize, bool isLooping,
					  float newVelocityMultiplier, 
					  const std::string &textureFileName);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &spriteParticleProgData);
	
	void SetPosition(glm::vec3 newPosition);

	bool IsEnded();
	bool IsActive();
	void Activate();

	void Reset();
};


class AoEAnimation
{
private:
	MeteoriteEmitter meteoriteRain;

	float animDuration_secs;

	Framework::Timer animTimer;

public:
	AoEAnimation() {}
	AoEAnimation(glm::vec3 position, 
				 float newAnimDuration_secs,
				 int explosionParticlesCount, int meteoriteParticlesCount,
				 int explosionParticlesLifeTime,
				 float explosionParticlesSize, float meteoriteParticlesSize, 
				 float meteoriteSpreadRadius,
				 float explosionParticleVelocityMultiplier,
				 float meteoriteVelocityMultiplier,
				 const std::string &explosionParticlesTextureFileName,
				 const std::string &meteoriteParticlesTextureFileName);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &spriteParticleProgData);

	void Restart();

	bool IsEnded();
};


class FrostNovaAnimation
{
private:
	RadialEmitter frostSpikes;

public:
	FrostNovaAnimation() {}
	FrostNovaAnimation(glm::vec3 position, 
					   float particleSize, float spreadRadius, float particleSpeed,
					   int particleCount, const std::string &particleTextureFileName);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &spriteParticleProgData);

	void Restart();

	bool IsEnded();
};


class NovaAnimation
{
private:
	RadialEmitter novaCircleEmitter;

public:
	NovaAnimation() {}
	NovaAnimation(glm::vec3 position, glm::vec4 color,
					 float particleSize, float spreadRadius, float particleSpeed,
					 int particleCount, const std::string &particleTextureFileName);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &spriteParticleProgData);

	void Restart();

	bool IsEnded();
};


class PassiveAoeAnimation
{
private:
	//std::vector<ParticleAnimation> lifeDrainAnims;
	std::vector<SpriteParticleEmitter> lifeDrainAnims;

	Utility::Primitives::Sprite3D skillRadiusSprite;
	
	// used to create new particles from input data when the lifeDrainAnims vector is out of space.
	struct AnimData
	{
		glm::vec3 position;
		int particleCount;
		int particleLifeTime;
		float particleSize;
		bool isParticleLooping;
		float particleVelocityMultiplier;
		std::string particleTextureFileName;

		AnimData() {}
	};
	AnimData inputAnimData;

public:
	PassiveAoeAnimation() {}
	PassiveAoeAnimation(glm::vec3 position, int particleCount,
						int particleLifeTime, float particleSize, bool isParticleLooping,
						float particleVelocityMultiplier, float skillRadiusSize,
						const std::string &particleTextureFileName, const std::string &skillRadiusTextureFileName);

	void Init();

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, 
				const SpriteParticleProgData &spriteParticleProgData, const SimpleTextureProgData &simpleTextureProgData);

	void SetFreeParticleAnimPosition(glm::vec3 newPosition);
	void SetParticleAnimPosition(glm::vec3 newPosition, int particleAnimIndex);

	void Restart();
	bool IsEnded();
};


#endif