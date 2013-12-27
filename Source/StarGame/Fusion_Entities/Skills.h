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
#pragma warning(pop)

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
	protected:
		friend class World;
		friend class CelestialBody;

		char fusionCombination[4];

		int applyCost;
		int researchCost;

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
		}
		virtual ~Skill() {}

		virtual void Update() {}
		virtual void Render() {} // TODO: Export to renderer

		virtual void Activate(CelestialBody *skillHolder) {}

		virtual bool HandleEvent(const IEventData &eventData);

	public:
		bool IsForSequence(const std::string &fusionSequence);
	};

	class SatelliteCreationSkill : public Skill
	{
	private:
		friend class World;
		friend class CelestialBody;

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