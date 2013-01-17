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

enum ParameterType
{
	PARAM_POSITION,
	PARAM_RANGE,
	PARAM_DAMAGE,
};


// TODO: Considering the small amount of different skills (~20-30), for the 
// sake of performance and less bugs, the skillType should be an enum.
class Skill
{
protected:
	char fusionCombination[4];
	std::string skillType;
	//std::vector<Event> generatedEvents;

public:
	Skill() { }
	Skill(const std::string &newSkillType,
		  char fusionCombA, char fusionCombB, char fusionCombC)
	{
		fusionCombination[0] = fusionCombA;
		fusionCombination[1] = fusionCombB;
		fusionCombination[2] = fusionCombC;
		fusionCombination[3] = '\0';

		skillType = newSkillType;
	}

	virtual void Update() {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						glm::vec3 cameraPosition,
						const BillboardProgDataNoTexture &progData) {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						const SimpleProgData &simpleData) {}
	virtual void Render(glutil::MatrixStack &modelMatrix, const LitProgData &litData,
						GLuint materialBlockIndex) {}

	virtual void OnEvent(Event &_event) {}
	// Only for EVENT_TYPE_OTHER
	virtual Event GetGeneratedEvent(const std::string &eventName) 
	{ 
		Event eventToReturn = StockEvents::EmptyEvent();
		return eventToReturn; 
	}
	virtual void RemoveGeneratedEvent(const std::string &eventName) {}
	// Gets the generated events and empties the event list.
	// (!)It is an one little dangerous method. You can lose a lot of events that way.
	virtual std::vector<Event> GetGeneratedEvents() 
	{ 
		std::vector<Event> eventsToReturn;
		return eventsToReturn;
	}

	virtual void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3) {}
	virtual void SetParameter(ParameterType paramType, int newParam_int) {}
	virtual void SetParameter(ParameterType paramType, float newParam_float) {}

	virtual bool IsIntersectingObject(glm::vec3 objectPosition) { return false; }

	virtual std::string GetSkillType() { return skillType; }
	virtual float GetRange() { return 0.0f; }
	virtual glm::vec3 GetPosition() { return glm::vec3(); }
};

/*
class RaySkill : public Skill
{
private:
	std::shared_ptr<CelestialBody> skillOwner;

	int damage;
	int defense;
	
	float range;

	RayEmitter ray;

	bool isStarted;

	std::vector<Event> generatedEvents;

public:
	RaySkill() : Skill() {}
	RaySkill(std::shared_ptr<CelestialBody> newSkillOwner,
			 int newDamage, int newDefense,
			 float newRange,
			 const std::string &newSkillType,
			 char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0');

	void Update();
	void Render(glutil::MatrixStack &modelMatrix,
				glm::vec3 cameraPosition,
				const BillboardProgDataNoTexture &progData);

	void OnEvent(Event &_event);
	std::shared_ptr<CelestialBody> GetOwner();

	// Only for EVENT_TYPE_OTHER
	Event GetGeneratedEvent(const std::string &eventName);
	void RemoveGeneratedEvent(const std::string &eventName);
	// Gets the generated events and empties the event list.
	// (!)It is an one little dangerous method. You can lose a lot of events that way.
	std::vector<Event> GetGeneratedEvents();
};
*/

class AOESkill : public Skill
{
private:
	int damage;
	float range;
	glm::vec3 position;
	
	AOESelector skillSelector;
	std::vector<Event> generatedEvents;
	bool isStarted;

public:
	AOESkill() : Skill() {}
	AOESkill(glm::vec3 newPosition,
			 int newDamage, float newRange,
			 const std::string &newSkillType,
			 char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0');

	void Update();
	void Render(glutil::MatrixStack	&modelMatrix, 
				const SimpleProgData &progData);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);

	bool IsIntersectingObject(glm::vec3 objectPosition);

	// Only for EVENT_TYPE_OTHER
	Event GetGeneratedEvent(const std::string &	eventName);
	void RemoveGeneratedEvent(const std::string &eventName);
	// Gets the generated events and empties the event list.
	// (!)It is an one little dangerous method. You can lose a lot of events that way.
	std::vector<Event> GetGeneratedEvents();
};


// Represents a passive defense AOE around a planet. When an enemy enters it, it gets
// damage over second.
class PassiveAOESkill : public Skill
{
private:
	int damage;
	int damageApplyTime_seconds;
	Framework::Timer attackTimer;
	float range;
	glm::vec3 position;

	Utility::Primitives::Circle skillVisibleRadius;
	std::vector<Event> generatedEvents;
	bool isStarted;

public:
	PassiveAOESkill() : Skill() {}
	PassiveAOESkill(glm::vec3 newPosition,
					int newDamage, int newDamageApplyTime_seconds,
					float newRange,
					const std::string &newSkillType,
					char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0');

	void Update();
	void Render(glutil::MatrixStack &modelMatrix,
				const SimpleProgData &progData);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();

	bool IsIntersectingObject(glm::vec3 objectPosition);

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);

	// Only for EVENT_TYPE_OTHER
	Event GetGeneratedEvent(const std::string &eventName);
	void RemoveGeneratedEvent(const std::string &eventName);
	// Gets the generated events and empties the event list.
	// (!)It is an one little dangerous method. You can lose a lot of events that way.
	std::vector<Event> GetGeneratedEvents();
};


class SunNovaSkill : public Skill
{
private:
	int damage;
	float range; // The maximum skill expansion
	float scaleRate;
	float currentScale;
	glm::vec3 position;

	Utility::Primitives::Circle skillExpansionRadius; // Maybe a Torus2D outline would be better.
	std::vector<Event> generatedEvents;
	bool isStarted;

public:
	SunNovaSkill() : Skill() {}
	SunNovaSkill(glm::vec3 newPosition,
				 int newDamage, 
				 float newRange, float newScaleRate,
				 const std::string &skillType,
				 char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0');

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const SimpleProgData &progData);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();

	bool IsIntersectingObject(glm::vec3 objectPosition);

	// Only for EVENT_TYPE_OTHER
	Event GetGeneratedEvent(const std::string &eventName);
	void RemoveGeneratedEvent(const std::string &eventName);
	// Gets the generated events and empties the event list.
	// (!)It is an one little dangerous method. You can lose a lot of events that way.
	std::vector<Event> GetGeneratedEvents();
};


class SatelliteChainingSkill : public Skill
{
private:
	int damage;
	float range;
	glm::vec3 startingPosition;
	glm::vec3 currentPosition;

	glm::vec3 projectileVelocity;
	glm::vec4 projectileColor;
	float projectileRadius;
	std::unique_ptr<Framework::Mesh> projectileMesh;
	// for lighting
	int materialBlockSize;
	GLuint materialUniformBuffer;

	std::vector<Event> generatedEvents;
	bool isStarted;

public:
	SatelliteChainingSkill() : Skill() {}
	SatelliteChainingSkill(glm::vec3 newPosition,
						   int newDamage, float newRange, 
						   glm::vec3 newProjectileVelocity, glm::vec4 newProjectileColor,
						   float newProjectileRadius,
						   const std::string &meshFileName, 
						   const std::string &skillType,
						   char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0');

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const LitProgData &litData, 
				GLuint materialBlockIndex);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);

	// WARN: Will add artificial radius for easier intersection.
	bool IsIntersectingObject(glm::vec3 objectPosition);

	// Only for EVENT_TYPE_OTHER
	Event GetGeneratedEvent(const std::string &eventName);
	void RemoveGeneratedEvent(const std::string &eventName);
	// Gets the generated events and empties the event list.
	// (!)It is an one little dangerous method. You can lose a lot of events that way.
	std::vector<Event> GetGeneratedEvents();
};

#endif