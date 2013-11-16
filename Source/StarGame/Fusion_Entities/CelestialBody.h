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


#include <memory>
#include <vector>

#include "../framework/Timer.h"
#include "../glsdk/glload/gl_3_3.h"

#include "../Fusion_Renderer/Renderer.h"
#include "../Fusion_EntitySystem/Event.h"
#include "../Fusion_EntitySystem/FusionComponents.h"


namespace FusionEngine
{
	class CelestialBody : public IEventListener
	{
	private:
		std::vector<std::shared_ptr<CelestialBody>> satellites;
		std::string id; // TODO: replace with ints

		int maxSatelliteCount;
		int currentSatelliteCount;

		float diameter;
		float offsetFromSun;
		float currentRotationAngle;
		float angularVelocity;

	public:
		CelestialBody();
		CelestialBody(int newMaxSatelliteCount, float newDiameter, float newOffsetFromSun);

		~CelestialBody();

		bool AddSatellite();
		void Update();

		std::string GetID() { return id; }

		virtual bool HandleEvent(const IEventData &eventData);
	};
}


#endif