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
};


#endif