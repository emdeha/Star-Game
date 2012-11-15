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


#ifndef ENGINE_H
#define ENGINE_H


#include "../framework/Utility.h"

#include <vector>


class SimpleParticle;

class SimpleParticleEmitter
{
private:
	glm::vec3 position;

	int numberOfParticles;

	Utility::Primitives::SpriteArray particleSprites;
	std::vector<SimpleParticle> particles;

public:
	SimpleParticleEmitter() {}
	SimpleParticleEmitter(glm::vec3 newPosition, int newNumberOfParticles);

	void Init();

	void Update();

	void Draw(glutil::MatrixStack &modelMat, const TextureProgData &textureProgData);

	std::vector<glm::vec3> GetParticlePositions();
};


class SimpleParticle
{
private:
	glm::vec3 position;
	glm::vec3 velocity;


	//Utility::Primitives::Sprite particleSprite;

public:
	SimpleParticle() {};
	SimpleParticle(glm::vec3 newPosition, glm::vec3 newVelocity);

	void Init();

	void Update();

	void Draw(/*glutil::MatrixStack &modelMat, const TextureProgData &textureProgData*/);

	glm::vec3 GetPosition()
	{
		return position;
	}
};


#endif