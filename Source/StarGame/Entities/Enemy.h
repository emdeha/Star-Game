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


// TODO: If two projectiles try to kill the sun in the same time, 
//		 the one that didn't succeed freezes and starts to flicker infinitely.

// MEASURING UNITS - 1.0f = 1/10 of screen on camera spherical coords = (12.5f, 90.0f, 135.0f)
// Should make them some other type


#ifndef ENEMY_H
#define ENEMY_H


#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>

#include "../framework/Mesh.h"
#include "../framework/EventSystem.h"

#include "../ProgramData/ProgramData.h"
#include "../Entities/PlanetBodies.h"
#include "MaterialBlock.h"

#include "../ParticleEngine/Engine.h"


enum BehaviorState
{
	ATTACK_STATE,
	EVADE_STATE,
	PATROL_STATE, 
};


class Projectile
{
private:
	glm::vec3 position;
	glm::vec3 velocity;

	int damage;
	int lifeSpan;

	bool isDestroyed;

	
	std::unique_ptr<Framework::Mesh> mesh;

	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	Projectile(glm::vec3 newPosition, glm::vec3 newVelocity,
			   int newDamage, int newLifeSpan);
	
	void LoadMesh(const std::string &meshFile);
	void LoadMesh(const std::auto_ptr<Framework::Mesh> newMesh);

	void Update(Sun &sun);
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
				float gamma, 
				const LitProgData &litData,
				float interpolation);

	void CheckTargetHit(Sun &sun);

	void OnTargetHit(Sun &sun, Event &_event); // Should be an OnEvent(Event &_event);

	
	void Recreate(glm::vec3 newPosition, 
				  glm::vec3 newVelocity,
				  int newLifeSpan);
	bool IsDestroyed();
};

inline bool Projectile::IsDestroyed()
{
	return isDestroyed;
}


struct PatrolRoute
{
	std::vector<glm::vec3> patrolPoints;
	int currentPatrolPointIndex;
	int nextPatrolPointIndex;
	int lastPatrolPointIndex;
};


class Spaceship
{
private:
	glm::vec3 position;
	glm::vec3 velocity;

	float rotationX;
	float rotationY;
	float scaleFactor;

	float lineOfSight;

	int health;

	float projectileSpeed;
	int projectileLifeSpan;
	int projectileDamage;

	BehaviorState currentState;

	PatrolRoute patrolRoute;

	std::unique_ptr<Projectile> projectile;
	std::unique_ptr<Framework::Mesh> mesh;

	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	Spaceship(glm::vec3 newPosition, 
			  glm::vec3 newVelocity,
			  float newRotationX,
			  float newRotationY,
			  float newScaleFactor,
			  float newProjectileSpeed,
			  int newProjectileLifeSpan, int newProjectileDamage);

	void LoadMesh(const std::string &meshFile);
	void LoadProjectileMesh(const std::string &meshFile);

	void UpdateAI(Sun &sun);

	void Update(bool isSunKilled, Sun &sun = Sun());
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
				float gamma, 
				const LitProgData &litData,
				float interpolation);

	void OnEvent(Event &_event);
};


struct DamageOverTime
{
	int damage;
	int time_milliseconds;
};


class Swarm
{
private:
	glm::vec3 position;
	glm::vec3 direction;
	float speed;

	int swarmEntitiesCount;
	int health;
	DamageOverTime damageOverTime;

	BehaviorState currentState;

	SwarmEmitter swarmBody;

public:
	Swarm() {}
	Swarm(glm::vec3 newPosition, glm::vec3 newDirection, float newSpeed,
		  int newSwarmEntitiesCount, int newHealth, int newDamage, int newTime_milliseconds,
		  const BillboardProgDataNoTexture &billboardProgramNoTexture);

	void UpdateAI(Sun &sun);
	void Update();

	void Render(glutil::MatrixStack &modelMatrix, 
				glm::vec3 cameraPosition,
				const BillboardProgDataNoTexture &billboardProgramNoTexture);

	void OnEvent(Event &_event);
};



#endif