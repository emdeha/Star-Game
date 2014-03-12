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


#ifndef FE_CELESTIAL_BODY_H
#define FE_CELESTIAL_BODY_H


#pragma warning(push, 1)
#include <memory>
#include <vector>

#include "../framework/Timer.h"
#include "../glsdk/glload/gl_3_3.h"
#pragma warning(pop)

#include "../Fusion_Renderer/Renderer.h"
#include "../Fusion_EventManager/Event.h"
#include "Skill.h"
#include "Composable.h"


namespace FusionEngine
{
	enum CelestialBodyType
	{
		FE_FIRE_SAT,
		FE_WATER_SAT,
		FE_AIR_SAT,
		FE_EARTH_SAT,
		FE_SUN,

		FE_CELESTIAL_BODY_BAD = -1
	};

	class CelestialBody : public Composable, public IEventListener
	{
	private:
		std::vector<std::shared_ptr<CelestialBody>> satellites;
		std::vector<std::shared_ptr<Skill>> skills;

		CelestialBodyType type;

		int maxSatelliteCount;
		int currentSatelliteCount;
		int health;

		float diameter;
		float offsetFromSun;
		float currentRotationAngle;
		float angularVelocity;

	public:
		CelestialBody();
		CelestialBody(CelestialBodyType newType,
					  int newMaxSatelliteCount, int newHealth, float newDiameter, float newOffsetFromSun);

		~CelestialBody();

		bool AddSatellite(CelestialBodyType satType);
		bool AddSkill(const std::string &skillName, const SkillData &newSkillData);

		void Update();

		std::vector<std::shared_ptr<Skill>> GetSkills() { return skills; }
		std::vector<std::shared_ptr<Skill>> GetAllSkills();
		std::vector<std::shared_ptr<CelestialBody>> GetSatellites() { return satellites; }
		int GetHealth() { return health; }

		virtual bool HandleEvent(const IEventData &eventData);

	private:
		void UpdateCollision();
	};
}


#endif