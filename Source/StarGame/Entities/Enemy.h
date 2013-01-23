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
#include "Skills.h"

#include "../ParticleEngine/Engine.h"


enum BehaviorState
{
	STATE_ATTACK,
	STATE_IDLE,
	STATE_EVADE,
	STATE_PATROL,
	// should it be a stopped state or a boolean?
	STATE_STOPPED,
};

struct PatrolRoute
{
	std::vector<glm::vec3> patrolPoints;
	glm::vec3 lastVelocity;
	glm::vec3 nextPatrolPoint;
	int currentPatrolPointIndex;
	int nextPatrolPointIndex;
	int lastPatrolPointIndex;
};

struct DamageOverTime
{
	int damage;
	unsigned int time_seconds;
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

	void Update(CelestialBody &sun);
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
				float gamma, 
				const LitProgData &litData,
				float interpolation);

	void CheckTargetHit(CelestialBody &sun);

	void OnTargetHit(CelestialBody &sun, Event &_event); // Should be an OnEvent(Event &_event);

	
	void Recreate(glm::vec3 newPosition, glm::vec3 newVelocity, int newLifeSpan);
	bool IsDestroyed() { return isDestroyed; }
};


class Enemy
{
protected:
	glm::vec3 position;
	glm::vec3 frontVector;
	float speed;

	float lineOfSight;

	int health;

	BehaviorState currentState;
	BehaviorState lastState;

	bool isDestroyed;

public:
	Enemy() {}
	Enemy(glm::vec3 newPosition, glm::vec3 newFrontVector, float newSpeed,
		  float newLineOfSight, int newHealth)
	{
		position = newPosition;
		frontVector = newFrontVector;
		speed = newSpeed;
		lineOfSight = newLineOfSight;
		health = newHealth;
		currentState = STATE_IDLE;
		lastState = currentState;
		isDestroyed = false;
	}

	virtual void UpdateAI(CelestialBody &sun) {}
	virtual void Update(bool isSunKilled, CelestialBody &sun = CelestialBody()) {}
	virtual void Render(glutil::MatrixStack &modelMatrix, 
						int materialBlockIndex,	float gamma, 
						const LitProgData &litData,
						float interpolation) {}
	virtual void Render(glutil::MatrixStack &modelMatrix, 
						glm::vec3 cameraPosition,
						const BillboardProgDataNoTexture &billboardProgDataNoTexture) {}

	virtual void OnEvent(Event &_event) {}

	virtual glm::vec3 GetPosition();
	virtual bool IsDestroyed();
};


class Swarm : public Enemy
{
private:
	glm::vec4 initialColor;
	glm::vec4 onFreezeColor;
	int swarmersCount;
	
	DamageOverTime damage;
	Framework::Timer attackTimer;

	bool isCommanded;

	SwarmEmitter swarmBody;

private:
	void AttackSolarSystem(CelestialBody &sun, bool isSatellite = false, float bodyIndex = -1.0f);

public:
	Swarm() {}
	Swarm(int newSwarmersCount, 
		  int newTime_seconds, int newDamage,
		  const BillboardProgDataNoTexture &billboardProgDataNoTexture,
		  glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor,
		  glm::vec3 newPosition, glm::vec3 newFrontVector,
		  float newSpeed, float newLineOfSight,
		  int newHealth);

	void UpdateAI(CelestialBody &sun);
	void Update(bool isSunKilled, CelestialBody &sun = CelestialBody());
	void Render(glutil::MatrixStack &modelMatrix, 
				glm::vec3 cameraPosition,
				const BillboardProgDataNoTexture &billboardProgDataNoTexture);

	void OnEvent(Event &_event);
};


class Spaceship : public Enemy
{
private:
	glm::vec4 initialColor;
	glm::vec4 onFreezeColor;
	float projectileSpeed;
	int projectileLifeSpan;
	int projectileDamage;

	PatrolRoute patrolRoute;

	std::unique_ptr<Projectile> projectile;
	std::unique_ptr<Framework::Mesh> mesh;

	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	Spaceship() {}
	Spaceship(float newProjectileSpeed, int newProjectileLifeSpan,
			  int newProjectileDamage,
			  glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor,
			  glm::vec3 newPosition, glm::vec3 newFrontVector,
			  float newSpeed, float newLineOfSight,
			  int newHealth);

	void UpdateAI(CelestialBody &sun);
	void Update(bool isSunKilled, CelestialBody &sun = CelestialBody());
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
				float gamma, 
				const LitProgData &litData,
				float interpolation);

	void OnEvent(Event &_event);

	void LoadMesh(const std::string &meshFileName);
	void LoadProjectileMesh(const std::string &meshFileName);
};


/*
class FastSuicideBomber
{
private:
	glm::vec3 position;
	glm::vec3 velocity;

	int health;
	int damage;

	float lineOfSight;

	float scaleFactor;

	BehaviorState currentState;

	std::auto_ptr<Framework::Mesh> mesh;
	std::vector<Event> generatedEvents;

	int materialBlockSize;
	GLuint materialUniformBuffer;

	bool isDestroyed;

	void AttackSolarSystem(CelestialBody &sun, bool isSatellite = false, float bodyIndex = -1.0f);

public:
	FastSuicideBomber() {}
	FastSuicideBomber(glm::vec3 newPosition, glm::vec3 newVelocity,
					  int newHealth, int newDamage, float newLineOfSight, 
					  float newScaleFactor);

	void LoadMesh(const std::string &meshFile);

	void UpdateAI(CelestialBody &sun);
	void Update(bool isSunKilled, CelestialBody &sun = CelestialBody());

	void Render(glutil::MatrixStack &modelMatrix, const LitProgData &litData, 
				int materialBlockIndex,
				float interpolation, float gamma);

	// Gets the generated events and destroys them
	std::vector<Event> GetGeneratedEvents();
	//void CleanupGeneratedEvent(const Event &_event);

	glm::vec3 GetPosition()
	{
		return position;
	}

	// Skill sender is the sender of the events. Later I should do sth more general to
	// all the event handling functions.
	void OnEvent(std::shared_ptr<Skill> sender, Event &_event);
};
*/

#endif