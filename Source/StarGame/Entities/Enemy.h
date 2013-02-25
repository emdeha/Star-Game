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
#include "../AssetLoader/MeshLoader.h"
#include "MaterialBlock.h"
#include "Skills.h"

#include "../ParticleEngine/Engine.h"


// WARN: Violates DRY!
enum EnemyType
{
	ENEMY_TYPE_SWARM,
	ENEMY_TYPE_SPACESHIP,
	ENEMY_TYPE_MOTHERSHIP,
	ENEMY_TYPE_FAST_SUICIDE_BOMBER,
	ENEMY_TYPE_ASTEROID, // Should the asteroids be randomly spawned on waves, 
						 // or should they act like background?

	ENEMY_TYPE_COUNT,
};

enum BehaviorState
{
	STATE_ATTACK,
	STATE_IDLE,
	STATE_EVADE,
	STATE_PATROL,
	// should it be a stopped state or a boolean?
	STATE_STOPPED,
};

enum EnemyRank
{
	RANK_ONE,
	RANK_TWO,

	RANK_COUNT,
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
	
	void Recreate(glm::vec3 newPosition, glm::vec3 newVelocity, int newLifeSpan, int newDamage);
	bool IsDestroyed() { return isDestroyed; }
};


class Enemy
{
protected:
	friend class Scene;


	EnemyRank rank;

	glm::vec4 initialColor;
	glm::vec4 onFreezeColor;

	glm::vec3 position;
	glm::vec3 frontVector;
	glm::vec3 lastFrontVector;
	float speed;

	float lineOfSight;

	int health;
	int resourceGivenOnKill;

	BehaviorState currentState;
	BehaviorState lastState;

	bool isDestroyed;
	// Only for child objects which are being updated by their parents
	bool isSceneUpdated; 

	std::vector<Event> generatedEvents;

public:
	Enemy() {}
	Enemy(glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor,
		  glm::vec3 newPosition, glm::vec3 newFrontVector, float newSpeed,
		  float newLineOfSight, int newHealth, int newResourceGivenOnKill)
	{
		initialColor = newInitialColor;
		onFreezeColor = newOnFreezeColor;
		position = newPosition;
		frontVector = newFrontVector;
		lastFrontVector = frontVector;
		speed = newSpeed;
		lineOfSight = newLineOfSight;
		health = newHealth;
		resourceGivenOnKill = newResourceGivenOnKill;
		currentState = STATE_IDLE;
		lastState = currentState;
		isDestroyed = false;
		isSceneUpdated = true;
		generatedEvents.resize(0);
	}

	virtual ~Enemy() {}

	virtual void UpdateAI(CelestialBody &sun) {}
	virtual void Update(bool isSunKilled, CelestialBody &sun = CelestialBody()) {}
	virtual void Render(glutil::MatrixStack &modelMatrix, 
						int materialBlockIndex,	float gamma, 
						const LitProgData &litData,
						float interpolation) {}
	virtual void Render(glutil::MatrixStack &modelMatrix, 
						glm::vec3 cameraPosition,
						const BillboardProgDataNoTexture &billboardProgDataNoTexture) {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						int materialBlockIndex, float gamma, 
						const LitProgData &litData, const SimpleTextureProgData &simpleTexData,
						float interpolation) {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						int materialBlockIndex, float gamma,
						const LitProgData &litDaat, const LitTextureProgData &litTextureData,
						float interpolation) {}

	virtual void OnEvent(Event &_event) 
	{
		switch(_event.GetType()) { default: break; }
	}

	virtual glm::vec3 GetPosition();
	virtual int GetResourceGivenOnKill();
	virtual bool IsDestroyed();
	virtual bool IsSceneUpdated();

	virtual void SetDamage(int newDamage, EnemyType enemyType) {}
	virtual void SetChargeSpeed(int newChargeSpeed) {}

	EnemyRank GetRank();

	// WARN: Not sure if I should keep this.

	// Only for EVENT_TYPE_OTHER
	Event GetGeneratedEvent(const std::string &eventName);
	void RemoveGeneratedEvent(const std::string &eventName);
	// Gets the generated events and empties the event list.
	// (!)It is an one little dangerous method. You can lose a lot of events that way.
	std::vector<Event> GetGeneratedEvents();
};


class Swarm : public Enemy
{
private:
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
		  float newTime_seconds, int newDamage,
		  const BillboardProgDataNoTexture &billboardProgDataNoTexture,
		  glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor,
		  glm::vec3 newPosition, glm::vec3 newFrontVector,
		  float newSpeed, float newLineOfSight,
		  int newHealth, int newResourceGivenOnKill);

	void UpdateAI(CelestialBody &sun);
	void Update(bool isSunKilled, CelestialBody &sun = CelestialBody());
	void Render(glutil::MatrixStack &modelMatrix, 
				glm::vec3 cameraPosition,
				const BillboardProgDataNoTexture &billboardProgDataNoTexture);

	void OnEvent(Event &_event);

	void SetDamage(int newDamage, EnemyType type) 
	{ 
		if(type == ENEMY_TYPE_SWARM)
		{
			damage.damage = newDamage; 
		}
	}
};


class Spaceship : public Enemy
{
private:
	float projectileSpeed;
	int projectileLifeSpan;
	int projectileDamage;

	PatrolRoute patrolRoute;

	std::unique_ptr<Projectile> projectile;
	Mesh mesh;

	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	Spaceship() {}
	Spaceship(float newProjectileSpeed, int newProjectileLifeSpan,
			  int newProjectileDamage,
			  glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor,
			  glm::vec3 newPosition, glm::vec3 newFrontVector,
			  float newSpeed, float newLineOfSight,
			  int newHealth, int newResourceGivenOnKill);

	void UpdateAI(CelestialBody &sun);
	void Update(bool isSunKilled, CelestialBody &sun = CelestialBody());
	void Render(glutil::MatrixStack &modelMatrix,
				int materialBlockIndex, float gamma,
				const LitProgData &litData, const LitTextureProgData &litTextureData,
				float interpolation);

	void OnEvent(Event &_event);

	void LoadMesh(const std::string &meshFileName);
	void LoadProjectileMesh(const std::string &meshFileName);

	void SetDamage(int newDamage, EnemyType type) 
	{ 
		if(type == ENEMY_TYPE_SPACESHIP)
		{
			projectileDamage = newDamage; 
		}
	}
};


class DeployUnit : public Enemy
{
private:
	float projectileSpeed;
	int projectileLifeSpan;
	int projectileDamage;

	bool isForRejuvenation;

	std::unique_ptr<Projectile> projectile;
	std::unique_ptr<Framework::Mesh> mesh;

	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	DeployUnit() {}
	DeployUnit(float newProjectileSpeed, int newProjectileLifeSpan,
			   int newProjectileDamage,
			   glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor,
			   glm::vec3 newPosition, glm::vec3 newFrontVector,
			   float newSpeed, float newLineOfSight,
			   int newHealth, int newResourceGivenOnKill);

	void UpdateAI(CelestialBody &sun);
	void Update(bool isSunKilled, CelestialBody &sun = CelestialBody());
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
				float gamma, const LitProgData &litData,
				float interpolation);

	void OnEvent(Event &_event);

	void LoadMesh(const std::string &meshFileName);
	void LoadProjectileMesh(const std::string &meshFileName);

	
	void Rejuvenate(const glm::vec3 &newPosition, int newHealth, glm::vec3 newFrontVector, float newSpeed);
	void Destroy();
	bool IsForRejuvenation();
	void SetPosition(glm::vec3 newPosition);
};


class Mothership : public Enemy
{
private:
	struct DeployUnitsInfo
	{
		std::string meshFileName;
		int deployUnitsCount;
		float projectileSpeed;
		int projectileLifeSpan;
		int projectileDamage;
		glm::vec4 initialColor;
		glm::vec4 onFreezeColor;
		float speed;
		float lineOfSight;
		int health;
		int resourceGivenOnKill;
	};
	
private:	
	DeployUnitsInfo deployUnitsInfo;
	std::vector<std::shared_ptr<DeployUnit>> deployUnits;

	bool isDeploying;
	Mesh mesh;

	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	Mothership() {}
	Mothership(glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor,
			   glm::vec3 newPosition, glm::vec3 newFrontVector,
			   float newSpeed, float newLineOfSight,
			   int newHealth, int newResourceGivenOnKill);

	void UpdateAI(CelestialBody &sun);
	void Update(bool isSunKilled, CelestialBody &sun = CelestialBody());
	void Render(glutil::MatrixStack &modelMatrix,
				int materialBlockIndex, float gamma,
				const LitProgData &litData, const LitTextureProgData &litTextureData,
				float interpolation);

	void OnEvent(Event &_event);

	void LoadMesh(const std::string &meshFileName);

	bool IsMothership() { return true; }
	void InitDeployUnits(const std::string &meshFileName, int deployUnitsCount,
						 float projectileSpeed, int projectileLifeSpan, int projectileDamage,
						 glm::vec4 initialColor, glm::vec4 onFreezeColor,
						 float speed, float lineOfSight, int health, int resourceGivenOnKill);
	void RejuvenateDeployUnits();

	void SetDamage(int newDamage, EnemyType type)
	{
		if(type == ENEMY_TYPE_MOTHERSHIP)
		{
			deployUnitsInfo.projectileDamage = newDamage;
		}
	}

	// First way of getting the deployUnits for skill intersection tests.
	// We get the deploy units, push them to all units and make intersection tests against them.
	// Or we make that in the beginning.
	// WARN: Leads to adding an isUpdated boolean
	std::vector<std::shared_ptr<DeployUnit>> GetDeployUnits() const; 
};


class FastSuicideBomber : public Enemy
{
private:
	int damage;
	float chargeSpeed;
	
	Mesh mesh;

	int materialBlockSize;
	GLuint materialUniformBuffer;

private:
	void AttackSolarSystem(CelestialBody &sun, bool isSatellite = false, float bodyIndex = -1.0f);

public:
	FastSuicideBomber() {}
	FastSuicideBomber(int newDamage, float newChargeSpeed,
					  glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor,
					  glm::vec3 newPosition, glm::vec3 newFrontVector,
					  float newSpeed, float newLineOfSight,
					  int newHealth, int newResourceGivenOnKill);

	void UpdateAI(CelestialBody &sun);
	void Update(bool isSunKilled, CelestialBody &sun = CelestialBody());
	void Render(glutil::MatrixStack &modelMatrix,
				int materialBlockIndex, float gamma,
				const LitProgData &litData, const LitTextureProgData &litTextureData,
				float interpolation);

	void OnEvent(Event &_event);

	void LoadMesh(const std::string &meshFile);

	void SetDamage(int newDamage, EnemyType type) 
	{
		if(type == ENEMY_TYPE_FAST_SUICIDE_BOMBER)
		{
			damage = newDamage; 
		}
	}
	void SetChargeSpeed(int newChargeSpeed) { chargeSpeed = newChargeSpeed; }
};


class Asteroid : public Enemy
{
private:
	int damage;
	
	Mesh mesh;

	int materialBlockSize;
	GLuint materialUniformBuffer;

private:
	void AttackSolarSystem(CelestialBody &sun, bool isSatellite = false, float bodyIndex = -1.0f);

public:
	Asteroid() {}
	Asteroid(int newDamage, 
			 glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor,
			 glm::vec3 newPosition, glm::vec3 newFrontVector,
			 float newSpeed, float newLineOfSight,
			 int newHealth, int newResourceGivenOnKill);

	void UpdateAI(CelestialBody &sun);
	void Update(bool isSunKilled, CelestialBody &sun = CelestialBody());
	void Render(glutil::MatrixStack &modelMatrix,
				int materialBlockIndex, float gamma,
				const LitProgData &litData, const LitTextureProgData &litTextureData,
				float interpolation);

	void OnEvent(Event &_event);

	void LoadMesh(const std::string &meshFile);

	void SetDamage(int newDamage, EnemyType type) 
	{
		if(type == ENEMY_TYPE_ASTEROID)
		{
			damage = newDamage; 
		}
	}
};


#endif