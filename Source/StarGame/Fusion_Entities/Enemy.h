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


#ifndef FE_ENEMY_H
#define FE_ENEMY_H


#pragma warning(push, 0)
#include <string>
#pragma warning(pop)

#include "../glsdk/glm/glm.hpp"

#include "States.h"
#include "IUpdatable.h"


namespace FusionEngine
{
	class Enemy : public IUpdatable
	{
	private:
		std::string name;

		BehaviorState currentState;

		glm::vec3 frontVector;
		
		int health;
		float speed;
		float attackRange;

	private:
		void UpdateAI();

	public:
		Enemy() {}
		Enemy(const std::string &newName, float newSpeed, glm::vec3 newFrontVector)
			: name(newName), speed(newSpeed), health(100), frontVector(newFrontVector), currentState(FE_STATE_IDLE) {}

		void Update();
	};
}


#endif