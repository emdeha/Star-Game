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


/// \ingroup module_StarGame

/// \file PlanetBodies.h
/// \brief Implements the planets.

#ifndef PLANET_BODIES_H
#define PLANET_BODIES_H


#include <glload/gl_3_3.h>
#include <glutil/glutil.h>

#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/Timer.h"
#include "../framework/Utility.h"
#include "../framework/EventSystem.h"

#include "../ProgramData/ProgramData.h"
#include "../GUISystem/GameSpecificGUI.h"
#include "MaterialBlock.h"
#include "../Mouse/Mouse.h"
#include "Skills.h"


// NOTE: Maybe the meshes of both the sun and satellites must be loaded in the constructors.
//		 This means removing the LoadMesh function. For now, I will keep it that way in case I
//		 decide to use different meshes in the future.


enum SatelliteType
{
	SATELLITE_FIRE,
	SATELLITE_WATER,
	SATELLITE_AIR,
	SATELLITE_EARTH,
};

struct SatelliteSkill
{
	SatelliteType satelliteTypeForSkill;
	float satelliteOffsetFromSun;
	// Add skill characteristics
};

struct ResourceData
{
	int resourceGain_perTime;
	float resourceGainTime;
	Framework::Timer resourceTimer;
};


static bool typesTable[] = { false, false, false, false };


class CelestialBody
{
private:
	std::unique_ptr<Framework::Mesh> bodyMesh;
	
	std::unique_ptr<CelestialBody> parent;
	std::vector<std::shared_ptr<CelestialBody>> satellites;

	std::vector<std::shared_ptr<Skill>> skills; // TODO: Is a pointer needed?

	SatelliteOrbit hoverOrbit; // TODO: Not sure if this should be here on somewhere more
							   //	    GUI specific.

	std::vector<Event> generatedEvents;

	SatelliteSkill skillType;
	ResourceData resource;

	Framework::Timer revolutionDuration;

	glm::vec4 color;
	glm::vec3 position;

	float diameter;

	bool isClicked;
	bool isSun;

	int health;
	int satelliteCap;

	int currentResource;


	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	CelestialBody() {}
	CelestialBody(const CelestialBody &other);
	~CelestialBody();
	CelestialBody(glm::vec3 newPosition, glm::vec4 newColor, float newDiameter,
				  int newSatelliteCap, int newHealth, int newCurrentResource,
				  bool _isSun = true);												// isSun = true means that a
																				    // sun will be created
	CelestialBody(Framework::Timer newRevolutionDuration, glm::vec4 newColor,
				  float newOffsetFromParent, float newDiameter,
				  SatelliteType newSkillType, int newHealth, ResourceData newResource,
				  bool _isSun = false);											   // isSun = false means that a 
																				   // satellite will be created

	void InitSatelliteOrbit();

	void LoadMesh(const std::string &fileName);
	
	void Update();
	void Render(glutil::MatrixStack &modelMatrix, GLuint materialBlockIndex,
				float gamma, 
				const LitProgData &litData,
				const UnlitProgData &unlitData,
				const SimpleProgData &simpleData,
				float interpolation);

	void OnEvent(Event &_event);

	bool AddSatellite(const std::string &fileName,
					  glm::vec4 satelliteColor,
					  float speed, float diameter,
					  SatelliteType type, int satelliteHealth);
	void AddSkill(const std::shared_ptr<Skill> newSkill);

	bool RemoveSatellite();
	bool RemoveSatellite(std::vector<std::shared_ptr<CelestialBody>>::iterator index_iterator);
	bool RemoveSatellite(SatelliteType type);
	void RemoveSatellites();	

	bool IsClicked(Utility::Ray mouseRay);
	bool IsSatelliteClicked(Utility::Ray mouseRay);

	bool IsSun();

	bool HasSatellites();

	// Gets the generated events and DESTROYS them.
	std::vector<Event> GetGeneratedEvents();

	Event GetGeneratedEvent(const std::string &eventName);
	void RemoveGeneratedEvent(const std::string &eventName);

	const bool GetIsClicked() const;
	const bool GetIsSatelliteClicked(SatelliteType type) const;

	const float GetRadius() const;
	const float GetDiameter() const;

	const int GetHealth() const;
	
	std::shared_ptr<CelestialBody> GetSatellite(SatelliteType type);
	std::shared_ptr<CelestialBody> GetOuterSatellite();
	std::vector<std::shared_ptr<CelestialBody>> GetSatellites();
	
	// Gets the sun's and its satellites' skills.
	std::vector<std::shared_ptr<Skill>> GetAllSkills();

	const glm::vec3 GetPosition() const;
	const int GetCurrentResource() const;


	// Satellite-specific methods
	float GetOffsetFromSun();
	SatelliteType GetSatelliteType();

	void SetParent(CelestialBody *newParent);
	void SetIsClicked(bool newIsClicked);

	void Stop();
	void Start();
};


#endif