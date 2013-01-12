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
#include "../GUISystem/GameSpecificGUI.h"
#include "PlanetBodies.h"


enum ParameterType
{
	PARAM_POSITION,
	PARAM_RANGE,
	PARAM_DAMAGE,
};


class Skill
{
protected:
	char fusionCombination[4];
	std::vector<Event> generatedEvents;

public:
	Skill() { generatedEvents.resize(0); }
	Skill(char fusionCombA, char fusionCombB, char fusionCombC)
	{
		fusionCombination[0] = fusionCombA;
		fusionCombination[1] = fusionCombB;
		fusionCombination[2] = fusionCombC;
		fusionCombination[3] = '\0';
		
		generatedEvents.resize(0);
	}

	virtual void Update() {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						glm::vec3 cameraPosition,
						const BillboardProgDataNoTexture &progData) {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						const SimpleProgData &simpleData) {}

	virtual void OnEvent(Event &_event) {}
	// Only for EVENT_TYPE_OTHER
	Event GetGeneratedEvent(const std::string &	eventName);
	// Gets the generated events and empties the event list.
	// (!)It is an one little dangerous method. You can lose a lot of events that way.
	std::vector<Event> GetGeneratedEvents();

	virtual std::shared_ptr<Sun> GetOwner()
	{
		return std::shared_ptr<Sun>();
	}

	virtual void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3) {}
	virtual void SetParameter(ParameterType paramType, int newParam_int) {}
	virtual void SetParameter(ParameterType paramType, float newParam_float) {}

	virtual bool IsIntersectingObject(glm::vec3 objectPosition) { return false; }
};


class RaySkill : public Skill
{
private:
	std::shared_ptr<Sun> skillOwner;

	int damage;
	int defense;
	
	float range;

	RayEmitter ray;

	bool isStarted;

public:
	RaySkill() : Skill() {}
	RaySkill(std::shared_ptr<Sun> newSkillOwner,
			 int newDamage, int newDefense,
			 float newRange,
			 char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0');

	void Update();
	void Render(glutil::MatrixStack &modelMatrix,
				glm::vec3 cameraPosition,
				const BillboardProgDataNoTexture &progData);

	void OnEvent(Event &_event);
	std::shared_ptr<Sun> GetOwner();
};


class AOESkill : public Skill
{
private:
	std::shared_ptr<Sun> skillOwner;

	int damage;
	float range;
	glm::vec3 position;
	
	AOESelector skillSelector;
	std::vector<Event> generatedEvents;
	bool isStarted;

public:
	AOESkill() : Skill() {}
	AOESkill(std::shared_ptr<Sun> newSkillOwner,
			 int newDamage, float newRange,
			 char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0');

	void Update();
	void Render(glutil::MatrixStack	&modelMatrix, 
				const SimpleProgData &progData);

	void OnEvent(Event &_event);
	std::shared_ptr<Sun> GetOwner();

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);
	//void SetParameter(ParameterType paramType, int newParam_int);
	//void SetParameter(ParameterType paramType, float newParam_float);

	bool IsIntersectingObject(glm::vec3 objectPosition);
};


#endif