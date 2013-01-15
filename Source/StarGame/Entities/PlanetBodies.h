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
#include "MaterialBlock.h"
#include "../Mouse/Mouse.h"


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


static bool typesTable[] = { false, false, false, false };

class Sun;


/// \class SatelliteOrbit
/// \brief Highlights the satellite's orbit.
class SatelliteOrbit
{
private:
	glm::vec4 mainColor; 
	glm::vec4 outlineColor;

	glm::vec4 position;

	float outerRadius;
	float innerRadius;


	Utility::Primitives::Torus2D mainOrbit;
	Utility::Primitives::Torus2D orbitOutlineOne;
	Utility::Primitives::Torus2D orbitOutlineTwo;

public:
	SatelliteOrbit();
	SatelliteOrbit(glm::vec4 newMainColor, glm::vec4 newOutlineColor,
				   glm::vec4 newPosition, 
				   float newOuterRadius, float newInnerRadius,
				   float gamma);

	/// \fn Draw
	/// \brief Draws the orbit.
	void Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &simpleData);
};


/// \class Satellite
/// \brief Represents a sun's satellite.

/// A satellite has a parent around which it rotates. It also has a diameter.
/// A satellite checks for clicks on it, loads on-click animations, etc.
class Satellite
{
private:
	std::unique_ptr<Framework::Mesh> mesh; ///< Represents the mesh file for the satellite
	std::unique_ptr<Sun> parent; ///< The satellite's parent

	Framework::Timer revolutionDuration; ///< The time needed for the satellite to make one revolution around its parent

	glm::vec4 color;
	glm::vec3 position; ///< The calculated satellite position

	SatelliteSkill skillType;
	// float offsetFromSun; ///< Based on this parameter the satellite's position is calculated	

	float diameter; ///< _diameter_ of the satellite

	bool isClicked;
	bool isDisabled;


	int health;


	int materialBlockSize; ///< The size of the material block. Used for uniform buffer init.
	GLuint materialUniformBuffer; ///< The 'pointer' to the uniform buffer.

public:
	Satellite(Framework::Timer newRevolutionDuration, 
			  glm::vec4 newColor,
			  float newOffsetFromSun, float newDiameter,
			  SatelliteType satelliteType, 
			  int health);

	/// \fn LoadMesh
	/// \brief Loads the satellite's mesh
	void LoadMesh(const std::string &fileName);

	/// \fn Render
	/// \brief Renders the satellite and its on-click animation.
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
				float gamma,
				const LitProgData &litData, 
				const UnlitProgData &unlitData, 
				const SimpleProgData &interpData,
				float interpolation);

	/// \fn Update
	/// \brief Updates the satellite's position.

	/// The position is based on the current time through the satellite's 
	/// revolution and its offset from sun.
	///
	/// \f$position.x = cos(currTimeThroughLoop \times (2 \times \pi)) \times offsetFromSun\f$
	///
	/// \f$position.y = sin(currTimeThroughLoop \times (2 \times \pi)) \times offsetFromSun\f$
	void Update();


	void Disable();


	void OnEvent(Event &_event);

	/// \fn IsClicked
	/// \brief Checks for click on the satellite.

	/// In order to check for click we need to do two things:
	///		- To get the mouse ray from the camera to the object
	///		- To check for intersections between the ray and the object
	/// Getting the ray is achieved with the userMouse.GetPickRay() function.
	///
	/// We check for intersections with Utility::Intersections::RayIntersectsEllipsoid().
	/// The test against collision of the ray with an ellipsoid because the camera is at an 
	/// 135.0 degree angle relative to the scene's plane. This leads to orbit squashing
	/// and the collision checks become inaccurate.
	bool IsClicked(glm::mat4 projMat, glm::mat4 modelMat, 
				   Mouse userMouse, glm::vec4 cameraPos,
				   int windowWidth, int windowHeight);

	/// \fn LoadClickedAnimation
	/// \brief Loads the animation which is played on-click.
	void LoadClickedAnimation(glutil::MatrixStack &modelMatrix,
							  const SimpleProgData &simpleData,
							  float gamma);


	/// \fn SetParent
	/// \brief Sets the satellite's parent. 
	void SetParent(const Sun &newParent);

	/// \fn SetIsClicked
	/// \brief Sets the isClicked variable to newIsClicked.
	void SetIsClicked(bool newIsClicked);

	/// \fn GetOffsetFromSun
	/// \brief Gets the satellite's offset from sun.

	// TODO: Not so useful (maybe).
	float GetOffsetFromSun();

	/// \fn GetDiameter
	/// \brief Gets the satellite's diameter.

	// TODO: Not so useful (maybe).
	float GetDiameter();

	
	SatelliteType GetSatelliteType();

	glm::vec3 GetPosition();
	glm::vec3 GetVelocity();

	int GetHealth();

	void Stop();
	void Start();
};


inline void Satellite::SetIsClicked(bool newIsClicked)
{
	isClicked = newIsClicked;
}

inline float Satellite::GetOffsetFromSun()
{
	return skillType.satelliteOffsetFromSun;
}

inline float Satellite::GetDiameter()
{
	return diameter;
}

inline glm::vec3 Satellite::GetPosition()
{
	return position;
}

inline int Satellite::GetHealth()
{
	return health;
}

inline SatelliteType Satellite::GetSatelliteType()
{
	return skillType.satelliteTypeForSkill;
}


/// \class Sun
/// \brief Represent the main planet of the solar system.

/// A sun handles the satellites in the scene. All of the sun's satellites
/// are in a vector of pointers to them.
///
/// The sun creates its satellites on-click based on the matter you have. It 
/// is also the main base of your solar system.
class Sun
{
private:
	std::unique_ptr<Framework::Mesh> sunMesh; ///< Represents the mesh file for the sun.

	// TODO: Find a solution to the smart_ptr problem. Satellite removal freezes the game.
	std::vector<std::shared_ptr<Satellite>> satellites; ///< The sun's satellites.
	std::vector<Event> generatedEvents;

	glm::vec4 color;
	glm::vec3 position; ///< The sun's position.

	int health;


	float diameter; ///< Its _diameter_.

	int satelliteCap;

	bool isClicked;
	bool isSatelliteClicked;

	// These two will be needed only for testing purposes.
	int currentSatelliteType;

public:
	Sun();
	Sun(glm::vec3 newPosition, glm::vec4 newColor, 
		float newDiameter, 
		int newSatelliteCap, int newHealth);

	/// \fn LoadMesh
	/// \brief Loads the sun's mesh.
	void LoadMesh(const std::string &fileName);

	/// \fn Render
	/// \brief Renders the sun and calls the satellite's render functions.

	/// *Important* The scaling of the sun's mesh to the desired diameter must be done _after_
	/// rendering the satellites. If it is not done that way, the scale will be passed
	/// to the satellites.
	///
	/// The sun is rendered with the unlit program.
	void Render(glutil::MatrixStack &modelMatrix, GLuint materialBlockIndex,
				float gamma,
				const LitProgData &litData, 
				const UnlitProgData &unlitData, 
				const SimpleProgData &interpData,
				float interpolation);
	
	/// \fn Update
	/// \brief Calls the satellite's update functions (for now).
	void Update();

	void OnEvent(Event &_event);

	/// \fn AddSatellite
	/// \brief Adds a satellite to the sun.

	/// This function also checks if the satellite cap is reached (for now - 4). 
	///
	/// After this check the function creates a new satellite, loads its mesh,
	/// sets its parent and pushes back the vector with satellites.
	///
	///If everything went OK the function returns `true`.
	bool AddSatellite(const std::string &fileName, 
					  glm::vec4 satelliteColor,
					  float speed, float diameter,
					  SatelliteType type, int satelliteHealth);

	/// \fn RemoveSatellite()
	/// \brief Removes the last added satellite.
	bool RemoveSatellite();

	/// \fn RemoveSatellite(int index)
	/// \brief Removes the satellite at position `index`
	bool RemoveSatellite(std::vector<std::shared_ptr<Satellite>>::iterator index_iterator);

	void RemoveSatellites();

	/// \fn GetSatellites
	/// \brief Gets the sun satellites. Used to access this info from elsewhere.
	std::vector<std::shared_ptr<Satellite>> GetSatellites();

	/// \fn GetPosition
	/// \brief Gets the sun position.
	const glm::vec4 GetPosition() const;

	/// \fn GetSatelliteCap
	/// \brief Gets the satellite cap limit.

	// NOTE: Maybe not so useful.
	const int GetSatelliteCap() const;


	std::shared_ptr<Satellite> GetOuterSatellite();

	/// \fn IsClicked
	/// \brief Checks if the sun is clicked. 

	/// This check is simpler than the satellite's one (see Satellite::IsClicked). 
	///
	/// The function gets the ray from the camera to the body and checks for intersection
	/// of the ray against a sphere.

	// TODO: Make a Sun on-click animation.
	bool IsClicked(glm::mat4 projMat, glm::mat4 modelMat, 
				   Mouse userMouse, glm::vec4 cameraPos,
				   int windowWidth, int windowHeight);

	// NOTE: Maybe remove
	void IsSatelliteClicked(glm::mat4 projMat, glm::mat4 modelMat, 
						    Mouse userMouse, glm::vec4 cameraPos,
						    int windowWidth, int windowHeight);

	bool HasSatellites();

	// Gets the generated events and destroys them
	std::vector<Event> GetGeneratedEvents();
	// Cleans a generated event. ERROR PRONE!!!
	// void CleanupGeneratedEvent(const Event &_event);

	const bool GetIsClicked() const;
	const bool GetIsSatelliteClicked() const;


	const float GetRadius() const;

	const int GetHealth() const;

public:
	Sun(const Sun &other);
};

inline const glm::vec4 Sun::GetPosition() const 
{
	return glm::vec4(position, 1.0f);
}

inline const int Sun::GetSatelliteCap() const 
{
	return satelliteCap;
}

inline const bool Sun::GetIsClicked() const 
{
	return isClicked;
}

inline const bool Sun::GetIsSatelliteClicked() const 
{
	return isSatelliteClicked;
}

inline const float Sun::GetRadius() const
{
	return diameter / 2.0f;
}

inline const int Sun::GetHealth() const
{
	return health;
}

inline bool Sun::HasSatellites()
{
	return !satellites.empty(); // '!' because empty retrns true if the vector is empty. 
								// We need to check if it is not.
}



class CelestialBody
{
private:
	std::unique_ptr<Framework::Mesh> bodyMesh;
	
	std::unique_ptr<CelestialBody> parent;
	std::vector<std::shared_ptr<CelestialBody>> satellites;

	std::vector<Event> generatedEvents;

	SatelliteSkill skillType;

	Framework::Timer revolutionDuration;

	glm::vec4 color;
	glm::vec3 position;

	float diameter;

	bool isClicked;
	bool isSun;

	int health;
	int satelliteCap;


	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	CelestialBody() {}
	CelestialBody(const CelestialBody &other);
	CelestialBody(glm::vec3 newPosition, glm::vec4 newColor, float newDiameter,
				  int newSatelliteCap, int newHealth, bool _isSun = true);			// isSun = true means that a
																				    // sun will be created
	CelestialBody(Framework::Timer newRevolutionDuration, glm::vec4 newColor,
				  float newOffsetFromParent, float newDiameter,
				  SatelliteType newSkillType, int newHealth, bool _isSun = false); // isSun = false means that a 
																				   // satellite will be created

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

	bool RemoveSatellite();
	bool RemoveSatellite(std::vector<std::shared_ptr<CelestialBody>>::iterator index_iterator);
	bool RemoveSatellite(SatelliteType type);
	void RemoveSatellites();	

	bool IsClicked(glm::mat4 projMat, glm::mat4 modelMat,	
				   Mouse userMouse, glm::vec4 cameraPos,
				   int windowWidth, int windowHeight);

	bool IsSatelliteClicked(glm::mat4 projMat, glm::mat4 modelMat,	
						    Mouse userMouse, glm::vec4 cameraPos,
							int windowWidth, int windowHeight);

	bool HasSatellites();

	// Gets the generated events and DESTROYS them.
	std::vector<Event> GetGeneratedEvents();

	const bool GetIsClicked() const;
	const bool GetIsSatelliteClicked(SatelliteType type) const;

	const float GetRadius() const;

	const int GetHealth() const;
	
	std::shared_ptr<CelestialBody> GetSatellite(SatelliteType type);
	std::shared_ptr<CelestialBody> GetOuterSatellite();
	std::vector<std::shared_ptr<CelestialBody>> GetSatellites();

	const glm::vec3 GetPosition() const;


	// Satellite-specific methods
	float GetOffsetFromSun();
	SatelliteType GetSatelliteType();

	void SetParent(const CelestialBody &newParent);
	void SetIsClicked(bool newIsClicked);

	void Stop();
	void Start();

	// Should be a GUI control which only appears on satellite hover. Like the AOE.
	void LoadClickedAnimation(glutil::MatrixStack &modelMatrix,
							  const SimpleProgData &simpleData,
							  float gamma);
};


#endif