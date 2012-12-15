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


#ifndef SKILLS_H
#define SKILLS_H


#include <vector>

#include <glm/glm.hpp>

#include "../framework/EventSystem.h"
#include "../ParticleEngine/Engine.h"


class ISkill
{
public:
	virtual void Update() = 0;
};

class RaySkill : public ISkill
{
private:
	int damage;
	int defense;
	
	float range;

	char fusionCombination[4];

	RayEmitter ray;
	std::vector<Event> generatedEvents;

public:
	RaySkill() {}
	RaySkill(int newDamage, int newDefense,
			 float newRange,
			 char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0');

	void Update();
	void Render(glutil::MatrixStack &modelMatrix,
				glm::vec3 cameraPosition,
				const BillboardProgDataNoTexture &progData);
};


#endif