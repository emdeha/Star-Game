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
#include "../StarGame/ShaderManager.h"
#include <vector>


// Tried the policy based design but it was too slow. 
// The other solution I can think of is function pointers.

struct StandardParticle
{
	glm::vec3 position;
	glm::vec3 velocity;
};

struct ExplosionParticle
{
	glm::vec4 color;
	glm::vec4 interpColorBegin;
	glm::vec4 interpColorEnd;

	glm::vec3 position;
	glm::vec3 velocity;

	float lifeTime;
};

struct MeteoriteParticle
{
	glm::vec3 position;
	glm::vec3 velocity;
};

struct RayParticle
{
	glm::vec4 color;

	glm::vec3 position;
	glm::vec3 velocity;
};


class ParticleEmitter
{
private:
	std::vector<StandardParticle> particles;

	glm::vec3 position;
	int particleCount;

	GLuint vao;
	GLuint vertexBO;
	
	Texture2D texture;

public:
	ParticleEmitter() {}
	ParticleEmitter(glm::vec3 newPosition, int newParticleCount);

	void Init(const BillboardProgData &billboardedProgData);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix,
				glm::vec3 cameraPosition,
				const BillboardProgData &programData);
};



class SwarmEmitter
{
private:
	std::vector<StandardParticle> particles;

	glm::vec3 position;
	glm::vec4 color;
	int particleCount;

	GLuint vao;
	GLuint vertexBO;

public:
	SwarmEmitter() {}
	SwarmEmitter(glm::vec3 newPosition, glm::vec4 newColor, int newParticleCount);

	void Init(const BillboardProgDataNoTexture &billboardProgDataNoTexture);

	void Update();
	void Update(glm::vec3 otherVelocity);
	void Render(glutil::MatrixStack &modelMatrix,
				glm::vec3 cameraPosition,
				const BillboardProgDataNoTexture &billboardProgDataNoTexture);

	void SetColor(glm::vec4 newColor);
};



class ExplosionEmitter
{
private:
	std::vector<ExplosionParticle> particles;
	
	bool isActive;
	bool isDead;

	glm::vec3 position;
	float velocityMultiplier;
	int particleCount;
	int startingParticleCount;

	int particleLifeTime;

	GLuint vao;
	GLuint vertexBO;

public:
	ExplosionEmitter() {}
	ExplosionEmitter(glm::vec3 newPosition, int newParticleCount,
					 int newParticleLifeTime,
					 float newVelocityMultiplier);

	void Init();

	void Update();
	void Render(glutil::MatrixStack &modelMatrix,
				glm::vec3 cameraPosition,
				const BillboardProgDataNoTexture &billboardProgDataNoTexture);

	void SetPosition(glm::vec3 newPosition);

	void Activate();
	bool IsActive();

	bool IsDead();
};


class TexturedExplosionEmitter
{
private:
	std::vector<ExplosionParticle> particles;

	bool isActive;
	bool isDead;

	glm::vec3 position;
	float velocityMultiplier;
	float size;
	int particleCount;
	int startingParticleCount;

	int particleLifeTime;

	GLuint vao;
	GLuint vertexBO;
	GLuint textureCoordsBO;

	std::shared_ptr<Texture2D> texture;

public:
	TexturedExplosionEmitter() {}
	TexturedExplosionEmitter(glm::vec3 newPosition, int newParticleCount,
							 int newParticleLifeTime, float newSize,
							 float newVelocityMultiplier,
							 const std::string &textureFileName);

	void Init();

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, 
				glm::vec3 cameraPosition,
				const BillboardProgData &billboardProgData);

	void SetPosition(glm::vec3 newPosition);

	void Activate();
	bool IsActive();

	bool IsDead();
};


class SpriteParticleEmitter
{
private:
	std::vector<ExplosionParticle> particles;

	bool isActive;
	bool isDead;

	bool isLooping;

	glm::vec3 position;
	float velocityMultiplier;
	float size;
	int particleCount;
	int startingParticleCount;
	int particleLifeTime;

	GLuint vao;
	GLuint vertexBO;
	GLuint textureCoordsBO;
	GLuint indexBO;

	std::shared_ptr<Texture2D> texture;

public:
	SpriteParticleEmitter() {}
	SpriteParticleEmitter(glm::vec3 newPosition, int newParticleCount,
						  int newParticleLifeTime, float newSize, bool newIsLooping, 
						  float newVelocityMultiplier, 
						  const std::string &textureFileName);

	void Init();

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, 
				const SpriteParticleProgData &progData);

	void SetPosition(glm::vec3 newPosition);

	void Activate();
	bool IsActive();

	bool IsDead();
};


class MeteoriteEmitter
{
private:
	std::vector<MeteoriteParticle> meteorites;
	SpriteParticleEmitter onHitExplosionEmitter; // add a vector later
	//std::vector<ExplosionParticle> explosionParticles;

	bool isActive;
	bool isDead;

	glm::vec3 position;
	//float explosionParticlesVelocityMultiplier;
	//float explosionParticlesSize;
	//float explosionParticlesLifeTime;
	float meteoriteParticlesSize;
	//int explosionParticlesCount;
	int meteoriteParticlesCount;
	int meteoriteStartingParticlesCount;

	GLuint vao;
	GLuint meteoriteVertexBO;
	//GLuint explosionVertexBO;
	GLuint textureCoordsBO;
	GLuint indexBO;

	std::shared_ptr<Texture2D> meteoriteTexture;
	//std::shared_ptr<Texture2D> explosionTexture;

public:
	MeteoriteEmitter() {}
	MeteoriteEmitter(glm::vec3 newPosition, 
					 int newExplosionParticlesCount, int newMeteoriteParticlesCount,
					 int newExplosionParticlesLifeTime, 
					 float newExplosionParticlesSize, float newMeteoriteParticlesSize,
					 float newExplosionParticlesVelocityMultiplier,
					 const std::string &explosionParticlesTextureFileName,
					 const std::string &meteoriteParticlesTextureFileName);

	void Init();

	void Update();
	void Render(glutil::MatrixStack &modelMatrix,
				const SpriteParticleProgData &progData);

	void Activate();
	bool IsActive();

	bool IsDead();
};


class RayEmitter
{
private:
	std::vector<RayParticle> particles;
	float rayLength;

	glm::vec3 position;
	int particleCount;

	GLuint vertexBO;
	GLuint vao;

public:
	RayEmitter() {}
	RayEmitter(glm::vec3 newPosition, int newParticleCount,
			   float newRayLength);

	void Init();

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, 
				glm::vec3 cameraPosition,
				const BillboardProgDataNoTexture &billboardData);
};


#endif