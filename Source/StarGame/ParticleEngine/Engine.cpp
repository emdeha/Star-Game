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


SimpleParticleEmitter::SimpleParticleEmitter(glm::vec3 newPosition, int newNumberOfParticles)
{
	position = newPosition;
	numberOfParticles = newNumberOfParticles;


	particleSprites = 
		Utility::Primitives::SpriteArray(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, 0.1f, false);
	particles.resize(numberOfParticles);
}

void SimpleParticleEmitter::Init()
{
	particleSprites.Init(numberOfParticles,
						 "../data/images/diamond.png");
	for(int i = 0; i < particles.size(); i++)
	{
		particles[i] = SimpleParticle(position, glm::vec3());
		particles[i].Init();
		//particleSprites.AddPosition(glm::vec4(particles[i].GetPosition(), 1.0f));
	}
}

void SimpleParticleEmitter::Update()
{
	for(int i = 0; i < particles.size(); i++)
	{
		particles[i].Update();
	}
}

void SimpleParticleEmitter::Draw(glutil::MatrixStack &modelMat, 
								 const TextureProgData &textureProgData)
{
	particleSprites.Draw(modelMat, textureProgData, GetParticlePositions());
	/*for(int i = 0; i < particles.size(); i++)
	{
		particles[i].Draw(modelMat, textureProgData);
	}*/
}

std::vector<glm::vec3> SimpleParticleEmitter::GetParticlePositions()
{
	std::vector<glm::vec3> positions;
	for(int i = 0; i < numberOfParticles; i++)
	{
		positions.push_back(particles[i].GetPosition());
	}

	return positions;
}


SimpleParticle::SimpleParticle(glm::vec3 newPosition, glm::vec3 newVelocity)
{
	position = newPosition;
	velocity = glm::vec3(((float)rand() / (float)RAND_MAX) / 100.0f,
						 ((float)rand() / (float)RAND_MAX) / 100.0f, 
						 0.0f);

	/*particleSprite = 
		Utility::Primitives::Sprite(glm::vec4(position, 1.0f),
									glm::vec4(1.0f, 0.5f, 0.7f, 1.0f),
									0.1f, 0.1f, false);
	particleSprite.Init("../data/images/diamond.png");*/
}

void SimpleParticle::Init()
{
}

void SimpleParticle::Update()
{
	position += velocity;
}

void SimpleParticle::Draw(/*glutil::MatrixStack &modelMat, const TextureProgData &textureProgData*/)
{
	/*{
		glutil::PushStack push(modelMat);

		modelMat.Translate(position.x, position.y, position.z);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		particleSprite.Draw(modelMat, textureProgData);

		glDisable(GL_BLEND);
	}*/
}