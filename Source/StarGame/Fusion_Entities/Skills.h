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


#ifndef FE_SKILLS_H
#define FE_SKILLS_H


#pragma warning(push, 1)
#include <map>
#include <string>
#include "../framework/Timer.h"
#pragma warning(pop)

#include "../framework/Utility.h"
#include "../Fusion_EntitySystem/EventManager.h"


namespace FusionEngine
{
	class FusionInput : public IEventListener 
	{
	private:
		struct FusionSequence 
		{
			char buttons[4];

			FusionSequence(char buttonA, char buttonB, char buttonC)
			{
				buttons[0] = buttonA;
				buttons[1] = buttonB;
				buttons[2] = buttonC;
				buttons[3] = '\0';
			}
		};

	private:
		std::map<std::string, FusionSequence> sequences;

		std::string currentInputSequence;
		char sequenceEndButton;
		char validInputButtons[3];

	private:
		bool IsValidInputButton(char button) const; 

	public:
		FusionInput() {}
		FusionInput(char newSequenceEndButton,
					char validInputSequenceA, char validInputSequenceB, char validInputSequenceC,
					EventManager &eventManager);

		void AddSequence(const std::string &sequenceName,
						 char buttonA, char buttonB, char buttonC);

		std::string GetSequenceButtons(const std::string &sequenceName) const;
		std::string GetSequenceName(const std::string &sequenceName) const;
		std::string GetCurrentInputSequence() const;
		std::string GetValidInputButtons() const;
		char GetSequenceEndButton() const;

		unsigned int GetCurrentInputIndex() const;

		void Clear();

		virtual bool HandleEvent(const IEventData &eventData);
	};

	inline std::string FusionInput::GetCurrentInputSequence() const
	{
		return currentInputSequence;
	}
	inline std::string FusionInput::GetValidInputButtons() const
	{
		return validInputButtons;
	}
	inline char FusionInput::GetSequenceEndButton() const
	{
		return sequenceEndButton;
	}

	inline unsigned int FusionInput::GetCurrentInputIndex() const
	{
		return currentInputSequence.length();
	}

	inline void FusionInput::Clear()
	{
		sequences.clear();
	}


	enum CelestialBodyType
	{
		FE_CELESTIALBODY_SUN,

		FE_CELESTIALBODY_FIRE,
		FE_CELESTIALBODY_AIR,
		FE_CELESTIALBODY_EARTH,
		FE_CELESTIALBODY_WATER,

		FE_SATELLITE_BAD = -1
	};

	class World;
	class CelestialBody;

	class Skill : public IEventListener 
	{
	private:
		char fusionCombination[4];

		int applyCost;
		int researchCost;

	protected:
		bool isDeployed;
		bool isActive;

	public:
		Skill() {}
		Skill(char fusionCombA, char fusionCombB, char fusionCombC,
			  int newApplyCost, int newResearchCost) 
			  : applyCost(newApplyCost), researchCost(newResearchCost)
		{
			fusionCombination[0] = fusionCombA;
			fusionCombination[1] = fusionCombB;
			fusionCombination[2] = fusionCombC;
			fusionCombination[3] = '\0';

			isDeployed = false;
			isActive = false;
		}
		virtual ~Skill() {}

		virtual void Update() {}
		virtual void Render() {} // TODO: Export to renderer

		virtual void Activate(CelestialBody *skillHolder) {}

		virtual bool HandleEvent(const IEventData &eventData);

		virtual bool IsCollidable() { return false; }

	public:
		bool IsForSequence(const std::string &fusionSequence);

		virtual glm::vec3 GetPosition() { return glm::vec3(0.0f); }
		virtual float GetRange() { return 0.0f; }
	};

	class SatelliteCreationSkill : public Skill
	{
	private:
		CelestialBodyType satelliteToAdd;

	public:
		SatelliteCreationSkill(CelestialBodyType newSatelliteToAdd, 
							   char fusionCombA, char fusionCombB, char fusionCombC,
							   int newApplyCost, int newResearchCost)
							   : satelliteToAdd(newSatelliteToAdd),
							     Skill(fusionCombA, fusionCombB, fusionCombC, 
									   newApplyCost, newResearchCost) {}

		void Activate(CelestialBody *skillHolder);
	};

	class BurnSkill : public Skill
	{
	private:
		Utility::Primitives::Circle	applicationDisc;

		glm::vec3 position;
		int damage;

		float range;
		float duration_seconds;
		float damageApplyTime_seconds;
		float damageApplyTimeDuration_seconds;

		Framework::Timer attackTimer;

	public:
		BurnSkill(int newDamage, float newRange, float newDuration_seconds, float newDamageApplyTime_seconds,
				  char fusionCombA, char fusionCombB, char fusionCombC,
				  int newApplyCost, int newResearchCost);

		void Update();
		void Render();

		void Activate(CelestialBody *skillHolder);

		bool HandleEvent(const IEventData &eventData);
	};

	class AOESkill : public Skill
	{
	private:
		Utility::Primitives::Circle applicationDisc;
		
		glm::vec3 position;

		int damage;
		float range;

	public:
		AOESkill(int newDamage, float newRange,
				 char fusionCombA, char fusionCombB, char fusionCombC,
				 int newApplyCost, int newResearchCost);

		void Update();
		void Render();

		void Activate(CelestialBody *skillHolder);

		bool HandleEvent(const IEventData &eventData);
	};

	class PassiveAOESkill : public Skill
	{
	private:
		Utility::Primitives::Circle skillVisibilityDisc;

		glm::vec3 position;

		int damage;
		
		float range;
		float damageApplyTime_seconds;

		Framework::Timer attackTimer;
		Framework::Timer skillLifeTimer;

	public:
		PassiveAOESkill(int newDamage, float newRange, float newDamageApplyTime_seconds, float skillLife,
						char fusionCombA, char fusionCombB, char fusionCombC,
						int newApplyCost, int newResearchCost);

		void Update();
		void Render();

		void Activate(CelestialBody *skillHolder);
	};

	class SunNovaSkill : public Skill
	{
	private:
		Utility::Primitives::Torus2D novaExpansionDisc;

		glm::vec3 position;
		
		int damage;

		float range;
		float scaleRate;
		float currentScale;

	public:
		SunNovaSkill(int newDamage, float newRange, float newScaleRate,
					 char fusionCombA, char fusionCombB, char fusionCombC,
					 int newApplyCost, int newResearchCost);

		void Update();
		void Render();

		void Activate(CelestialBody *skillHolder);
	};

	class ShieldSkill : public Skill
	{
	private:
		Utility::Primitives::Circle skillEffectDisc;

		glm::vec3 position;

		std::string holderID; // should be added to every skill

		int defensePoints;
		int currentDefensePoints;

		float range;

	public:
		ShieldSkill(int newDefensePoints, float newRange,
					char fusionCombA, char fusionCombB, char fusionCombC,
					int newApplyCost, int newResearchCost);

		void Update();
		void Render();

		void Activate(CelestialBody *skillHolder);

		bool HandleEvent(const IEventData &eventData);

		bool IsCollidable() { return true; }

		glm::vec3 GetPosition() { return position; }
		float GetRange() { return range; }
	};

	// Many similarities between the frost and SunNova
	class FrostSkill : public Skill
	{
	private:
		Utility::Primitives::Torus2D frostExpansionDisc;

		std::string holderID;

		glm::vec3 position;
		
		int damage;

		float range;
		float scaleRate;
		float currentScale;

	public:
		FrostSkill(int newDamage, float newRange, float newScaleRate,
				   char fusionCombA, char fusionCombB, char fusionCombC,
				   int newApplyCost, int newResearchCost);

		void Update();
		void Render();

		void Activate(CelestialBody *skillHolder);
	};

	class ChainSkill : public Skill
	{
	private:
		Utility::Primitives::Torus2D chainExpansionDisc;

		std::string holderID;

		glm::vec3 position;

		int damage;

		float range;
		float scaleRate;
		float currentScale;

	public:
		ChainSkill(int newDamage, float newRange, float newScaleRate,
				   char fusionCombA, char fusionCombB, char fusionCombC,
				   int newApplyCost, int newResearchCost);

		void Update();
		void Render();

		void Activate(CelestialBody *skillHolder);

		bool HandleEvent(const IEventData &eventData);
	};

	class UltimateSkill : public Skill
	{
	public:
		UltimateSkill(char fusionCombA, char fusionCombB, char fusionCombC,
					  int newApplyCost, int newResearchCost)
					  : Skill(fusionCombA, fusionCombB, fusionCombC,
							  newApplyCost, newResearchCost) {}

		void Activate(CelestialBody *skillHolder);
	};
}


#endif