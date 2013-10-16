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

#include "../Fusion_Scene/Scene.h"
#include "../Fusion_Renderer/Renderer.h"
#include "../Fusion_EntitySystem/Event.h"


namespace FusionEngine
{
	class EventManager;

	class NewCelestialBody : public IEventListener
	{
	public:
		enum
		{
			EVENT_ON_CLICK,
		};

		//std::unique_ptr<NewCelestialBody> parent;
		std::vector<std::shared_ptr<NewCelestialBody>> satellites;
		std::string id; // TODO: replace with ints

		float diameter;
		float offsetFromSun;

		Framework::Timer revolutionTimer;

	public:
		NewCelestialBody();
		NewCelestialBody(float newDiameter, float newOffsetFromSun, float cycleDuration);

		~NewCelestialBody();

		std::string GetID() { return id; }

		virtual bool HandleEvent(const IEventData &eventData);
	};
}

bool AddSatellite(FusionEngine::NewCelestialBody *celestialBody, 
				  FusionEngine::Render::RendererType, GLuint shaderProg, 
				  float newDiameter, float newOffsetFromSun, float cycleDuration);
void Update(FusionEngine::NewCelestialBody *celestialBody);


#endif