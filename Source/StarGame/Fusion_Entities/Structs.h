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


#ifndef FE_STRUCTS_H
#define FE_STRUCTS_H


#pragma warning(push, 0)
#include <vector>
#include "../glsdk/glm/glm.hpp"
#pragma warning(pop)


struct PatrolRoute
{
	std::vector<glm::vec3> patrolPoints;
	glm::vec3 lastVelocity;
	glm::vec3 nextPatrolPoint;
	unsigned int currentPatrolPointIndex;
	unsigned int nextPatrolPointIndex;
	unsigned int lastPatrolPointIndex;

	PatrolRoute() :
		patrolPoints(0), lastVelocity(glm::vec3()), nextPatrolPoint(glm::vec3()), 
		currentPatrolPointIndex(0), nextPatrolPointIndex(0), lastPatrolPointIndex(0) {}
};

struct DamageOverTime
{
	unsigned int damage;
	unsigned int time_seconds;

	DamageOverTime() :
		damage(0), time_seconds(0) {}
};


#endif