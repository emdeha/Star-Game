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

#define TORUS_SKILL


#include <vector>

#include <glm/glm.hpp>

#include "../framework/EventSystem.h"
#include "../ParticleEngine/Engine.h"
#include "../GUISystem/GameSpecificGUI.h"
#include "BasicAnimation.h"

enum ParameterType
{
	PARAM_POSITION,
	PARAM_LIFE_DRAIN_PARTICLE_EMITTER_POSITION,
	PARAM_RANGE,
	PARAM_DAMAGE,
};

enum SkillType
{
	SKILL_TYPE_PASSIVE_AOE,
	SKILL_TYPE_AOE,
	SKILL_TYPE_SUN_NOVA,
	SKILL_TYPE_BURN,
	SKILL_TYPE_SAT_PASSIVE_AOE,
	SKILL_TYPE_SAT_CHAIN,
	SKILL_TYPE_SAT_FROSTNOVA,
	SKILL_TYPE_SAT_SHIELD,
		
	SKILL_TYPE_COUNT,
};


// TODO: Considering the small amount of different skills (~20-30), for the 
// sake of performance and less bugs, the skillType should be an enum.
class Skill
{
protected:
	friend class Scene;

	char fusionCombination[4];
	std::string skillType;
	std::vector<Event> generatedEvents;

	int skillApplyCost;
	int skillResearchCost;

	bool isStarted;
	bool isDeployed;

	bool isResearched;
	int boxIndexForUpgrade;

public:
	Skill() { }
	Skill(const std::string &newSkillType,
		  char fusionCombA, char fusionCombB, char fusionCombC,
		  int newSkillApplyCost, int newSkillResearchCost, int newBoxIndexForUpgrade)
	{
		fusionCombination[0] = fusionCombA;
		fusionCombination[1] = fusionCombB;
		fusionCombination[2] = fusionCombC;
		fusionCombination[3] = '\0';

		skillType = newSkillType;

		skillApplyCost = newSkillApplyCost;
		skillResearchCost = newSkillResearchCost;

		isResearched = false;
		isDeployed = false;
		boxIndexForUpgrade = newBoxIndexForUpgrade;
	}

	virtual void Update() {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						glm::vec3 cameraPosition,
						const BillboardProgDataNoTexture &progData) {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						const SimpleProgData &simpleData) {}
	virtual void Render(glutil::MatrixStack &modelMatrix, const LitProgData &litData,
						GLuint materialBlockIndex) {}
	virtual void Render(glutil::MatrixStack &modelMatrix, 
						const SpriteParticleProgData &spriteParticleProgData, const SimpleProgData &simpleData) {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						const SpriteParticleProgData &spriteParticleProgData) {}
	virtual void Render(glutil::MatrixStack &modelMatrix,
						const SpriteParticleProgData &spriteParticleProgData, const SimpleTextureProgData &SimpleTextureProgData) {}

	virtual void OnEvent(Event &_event) {}

	virtual void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3) {}
	virtual void SetParameter(ParameterType paramType, int newParam_int) {}
	virtual void SetParameter(ParameterType paramType, float newParam_float) {}

	virtual bool IsIntersectingObject(glm::vec3 objectPosition) { return false; }
	virtual bool IsIntersectingObject(glm::vec3 objectPosition, int objectIndex) { return false; }
	
	virtual float GetRange() { return 0.0f; }
	virtual int GetDamage() { return 0; }

	virtual void SetSkillAnim(glm::vec3 position, int enemyIndex) {}

	std::string GetSkillType() { return skillType; }
	int GetApplyCost() { return skillApplyCost; }
	int GetResearchCost() { return skillResearchCost; }
	glm::vec3 GetPosition() { return glm::vec3(); }
	bool IsDeployed() { return isDeployed; }
	bool IsStarted() { return isStarted; }
	bool IsResearched() { return isResearched; }

	bool Research() { isResearched = true; }

	
	// Only for EVENT_TYPE_OTHER
	Event GetGeneratedEvent(const std::string &	eventName);
	void RemoveGeneratedEvent(const std::string &eventName);
	// Gets the generated events and empties the event list.
	// (!)It is an one little dangerous method. You can lose a lot of events that way.
	std::vector<Event> GetGeneratedEvents();
};


class AOESkill : public Skill
{
private:
	int damage;
	float range;
	glm::vec3 position;
	
	AOESelector skillSelector;

	AoEAnimation aoeAnim;

public:
	AOESkill() : Skill() {}
	AOESkill(glm::vec3 newPosition,
			 int newDamage, float newRange,
			 const std::string &newSkillType,
			 char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0', 
			 int skillApplyCost = 0, int skillResearchCost = 0, int boxIndexForUpgrade = -1);

	void Update();
	void Render(glutil::MatrixStack	&modelMatrix, 
				const SpriteParticleProgData &spriteParticleProgData, const SimpleProgData &progData);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();
	int GetDamage() { return damage; }

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);

	bool IsIntersectingObject(glm::vec3 objectPosition);
};


// Represents a passive defense AOE around a planet. When an enemy enters it, it gets
// damage over second.
class PassiveAOESkill : public Skill
{
private:
	int damage;
	float damageApplyTime_seconds;
	Framework::Timer attackTimer;
	Framework::Timer skillLife;
	float range;
	glm::vec3 position;

	//Utility::Primitives::Circle skillVisibleRadius;
	PassiveAoeAnimation skillAnim;

public:
	PassiveAOESkill() : Skill() {}
	PassiveAOESkill(glm::vec3 newPosition,
					int newDamage, float newDamageApplyTime_seconds, int newSkillLife,
					float newRange,
					const std::string &newSkillType,
					char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0', 
					int skillApplyCost = 0, int skillResearchCost = 0, int boxIndexForUpgrade = -1);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix,
				const SpriteParticleProgData &spriteParticleProgData, const SimpleTextureProgData &simpleTextureProgData);
				//const SimpleProgData &progData);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();
	int GetDamage() { return damage; }

	bool IsIntersectingObject(glm::vec3 objectPosition/*, int objectIndex = -1*/);

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);
	void SetSkillAnim(glm::vec3 position, int enemyIndex);
};


class SunNovaSkill : public Skill
{
private:
	int damage;
	float range; // The maximum skill expansion
	float scaleRate;
	float currentScale;
	glm::vec3 position;

	NovaAnimation sunNovaAnim;

public:
	SunNovaSkill() : Skill() {}
	SunNovaSkill(glm::vec3 newPosition,
				 int newDamage, 
				 float newRange, float newScaleRate,
				 const std::string &skillType,
				 char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0', 
				 int skillApplyCost = 0, int skillResearchCost = 0, int boxIndexForUpgrade = -1);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &progData);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();
	int GetDamage() { return damage; }

	bool IsIntersectingObject(glm::vec3 objectPosition);
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

public:
	SatelliteChainingSkill() : Skill() {}
	SatelliteChainingSkill(glm::vec3 newPosition,
						   int newDamage, float newRange, 
						   glm::vec3 newProjectileVelocity, glm::vec4 newProjectileColor,
						   float newProjectileRadius,
						   const std::string &meshFileName, 
						   const std::string &skillType,
						   char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0', 
						   int skillApplyCost = 0, int skillResearchCost = 0, int boxIndexForUpgrade = -1);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const LitProgData &litData, 
				GLuint materialBlockIndex);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();
	int GetDamage() { return damage; }

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);

	// WARN: Will add artificial radius for easier intersection.
	bool IsIntersectingObject(glm::vec3 objectPosition);
};


class SatelliteChainingNova : public Skill
{
private:
	int damage;
	float range; // The maximum skill expansion
	float scaleRate;
	float currentScale;
	glm::vec3 position;
	
	NovaAnimation satNovaAnim;

public:
	SatelliteChainingNova() : Skill() {}
	SatelliteChainingNova(glm::vec3 newPosition, 
						  int newDamage, 
						  float newRange, float newScaleRate,
						  const std::string &skillType,
						  char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0', 
						  int skillApplyCost = 0, int skillResearchCost = 0, int boxIndexForUpgrade = -1);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &progData);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();
	int GetDamage() { return damage; }

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);

	bool IsIntersectingObject(glm::vec3 objectPosition);
};


class FrostNovaSkill : public Skill
{
private:
	int damage; // Must be sth small. This is a stunning skill.
	float stunTime_seconds;
	Framework::Timer stunTimer;
	float range;	
	float scaleRate;
	float currentScale;
	glm::vec3 position;

	FrostNovaAnimation skillAnimation;

public:
	FrostNovaSkill() : Skill() {}
	FrostNovaSkill(int newDamage, float newStunTime_seconds,
				   float newRange, float newScaleRate,
				   glm::vec3 newPosition,
				   const std::string &skillType,
				   char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0', 
				   int skillApplyCost = 0, int skillResearchCost = 0, int boxIndexForUpgrade = -1);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const SpriteParticleProgData &spriteProgData);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();
	int GetDamage() { return damage; }

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);

	bool IsIntersectingObject(glm::vec3 objectPosition);
};


class ShieldSkill : public Skill
{
private:
	glm::vec3 position;
	int defensePoints; // if below zero, skill deactivates
	int startingDefensePoints;
	float range;
	
	bool isStarted;

	Animation skillAnimation;

public:
	ShieldSkill() : Skill() {}
	ShieldSkill(glm::vec3 newPosition, 
				int newDefensePoints, float newRange, 
				const std::string &skillType, 
				char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0', 
				int skillApplyCost = 0, int skillResearchCost = 0, int boxIndexForUpgrade = -1);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, const LitProgData &litData,
				GLuint materialBlockIndex);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);

	bool IsIntersectingObject(glm::vec3 objectPosition);
};


class BurnSkill : public Skill
{
private:
	glm::vec3 position;
	float range;
	int damage;
	float duration_seconds;
	//
	float damageApplyTime_seconds;
	float damageApplyTimeDuration_seconds;
	//
	Framework::Timer attackTimer;

	//bool isDeployed;

	Utility::Primitives::Circle skillRadius;

	ParticleAnimation burnAnim;

public:
	BurnSkill() : Skill() {}
	BurnSkill(glm::vec3 newPosition, 
			  int newDamage, float newDamageApplyTime_seconds, float newDuration_seconds,
			  float newRange,
			  const std::string &skillType,
			  char fusionCombA = '\0', char fusionCombB = '\0', char fusionCombC = '\0', 
			  int skillApplyCost = 0, int skillResearchCost = 0, int boxIndexForUpgrade = -1);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, 
				const SpriteParticleProgData &spriteParticleProgData, const SimpleProgData &simpleData);

	void OnEvent(Event &_event);

	float GetRange();
	glm::vec3 GetPosition();
	int GetDamage() { return damage; }
	bool IsDeployed();

	void SetParameter(ParameterType paramType, glm::vec3 newParam_vec3);

	bool IsIntersectingObject(glm::vec3 objectPosition);
};


#endif