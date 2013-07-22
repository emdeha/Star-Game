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


#ifndef SPACESHIP_H
#define SPACESHIP_H


#include "../glsdk/glm/glm.hpp"

#include "States.h"
#include "Structs.h"


namespace FusionEngine
{
	class Scene;
}


class Projectile
{
private:
	bool isDestroyed;

	int lifeSpan;
	int initialLifeSpan;
	int damage;
	float speed;

	glm::vec3 velocity;

private:
	void CheckTargetHit(FusionEngine::Scene &scene);

public:
	Projectile() : 
		isDestroyed(true), lifeSpan(0), initialLifeSpan(0), damage(0), speed(0.0f), velocity(glm::vec3()) {}
	Projectile(int newLifeSpan, int newInitialLifeSpan, int newDamage, float newSpeed) :
		isDestroyed(true), lifeSpan(newLifeSpan), initialLifeSpan(newInitialLifeSpan), damage(newDamage), 
		speed(newSpeed), velocity(glm::vec3()) {}

	void Update(FusionEngine::Scene &scene);
};


class Spaceship
{
private:
	BehaviorState currentState;
	BehaviorState lastState;

	PatrolRoute patrolRoute;

	glm::vec3 velocity;
	float speed;
	float rangeOfSight;

	int health;

	bool isDestroyed;

private:
	void UpdateAI();

public:
	Spaceship() :
		currentState(BehaviorState::STATE_IDLE), lastState(BehaviorState::STATE_IDLE),
		patrolRoute(PatrolRoute()), velocity(glm::vec3()), speed(0.0f), rangeOfSight(0.0f), 
		health(0), isDestroyed(true) {}
	Spaceship(int newHealth, float newSpeed, float newRangeOfSight, BehaviorState startingState, 
			  std::vector<glm::vec3> patrolRoutePoints) :
		health(newHealth), currentState(startingState), lastState(startingState), speed(newSpeed), 
		rangeOfSight(newRangeOfSight), patrolRoute(PatrolRoute()), velocity(glm::vec3()), isDestroyed(false)
	{
		for(auto patrolPoint = patrolRoutePoints.begin(); patrolPoint != patrolRoutePoints.end(); ++patrolPoint)
		{
			patrolRoute.patrolPoints.push_back((*patrolPoint));
		}

		patrolRoute.currentPatrolPointIndex = 0;
		patrolRoute.lastPatrolPointIndex = 0;
		patrolRoute.nextPatrolPointIndex = 1;

		velocity = glm::normalize(patrolRoute.patrolPoints[patrolRoute.nextPatrolPointIndex] -
								  patrolRoute.patrolPoints[patrolRoute.currentPatrolPointIndex]);
		patrolRoute.lastVelocity = velocity;
	}

	void Update(FusionEngine::Scene &scene);
};


#endif