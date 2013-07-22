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


#ifndef CELESTIAL_BODY_H
#define CELESTIAL_BODY_H


#include <memory>
#include <vector>

#include "../framework/Timer.h"
#include "../glsdk/glload/gl_3_3.h"


namespace FusionEngine
{
	class Scene;
}


class CelestialBody
{
private:
	std::unique_ptr<FusionEngine::Scene> scene;

	std::unique_ptr<CelestialBody> parent;
	std::vector<std::shared_ptr<CelestialBody>> satellites;

	float diameter;
	float offsetFromSun;

	Framework::Timer revolutionTimer;
	
public:
	CelestialBody() :
		scene(std::unique_ptr<FusionEngine::Scene>()), 
		parent(std::unique_ptr<CelestialBody>(new CelestialBody())), satellites(0), diameter(0.0f),
		offsetFromSun(0.0f), revolutionTimer() {}
	CelestialBody(FusionEngine::Scene &newScene, float newDiameter, float newOffsetFromSun, float cycleDuration) :
		scene(std::unique_ptr<FusionEngine::Scene>(&newScene)),
		parent(std::unique_ptr<CelestialBody>(new CelestialBody())), satellites(0), diameter(newDiameter),
	    offsetFromSun(newOffsetFromSun), revolutionTimer(Framework::Timer::TT_LOOP, cycleDuration) {}

	void Update();

	bool AddSatellite(GLuint shaderProg);
};


#endif