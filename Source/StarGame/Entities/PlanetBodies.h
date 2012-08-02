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
#include "../ProgramData/ProgramData.h"
#include "../Mouse/Mouse.h"


// NOTE: Maybe the meshes of both the sun and satellites must be loaded in the constructors.
//		 This means removing the LoadMesh function. For now, I will keep it that way in case I
//		 decide to use different meshes in the future.


/// \fn CorrectGamma()
/// \brief Applies gamma correction to colors.
static glm::vec4 CorrectGamma(const glm::vec4 &inputColor, float gamma)
{
	glm::vec4 outputColor;
	outputColor[0] = powf(inputColor[0], 1.0f / gamma);
	outputColor[1] = powf(inputColor[1], 1.0f / gamma);
	outputColor[2] = powf(inputColor[2], 1.0f / gamma);
	outputColor[3] = inputColor[3];

	return outputColor;
}


/// \struct MaterialBlock
/// \brief Contains the material characteristics of an object.
struct MaterialBlock
{
	glm::vec4 diffuseColor; 
	glm::vec4 specularColor; 
	float shininessFactor;

	float padding[3]; ///< Padding for compatibility with GLSL
};


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


	Utility::BasicMeshGeneration::Torus2D mainOrbit;
	Utility::BasicMeshGeneration::Torus2D orbitOutlineOne;
	Utility::BasicMeshGeneration::Torus2D orbitOutlineTwo;

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
	std::auto_ptr<Framework::Mesh> mesh; ///< Represents the mesh file for the satellite
	std::auto_ptr<Sun> parent; ///< The satellite's parent

	Framework::Timer revolutionDuration; ///< The time needed for the satellite to make one revolution around its parent

	glm::vec3 position; ///< The calculated satellite position

	float offsetFromSun; ///< Based on this parameter the satellite's position is calculated	

	float diameter; ///< _diameter_ of the satellite

	bool isClicked;


	int materialBlockSize; ///< The size of the material block. Used for uniform buffer init.
	GLuint materialUniformBuffer; ///< The 'pointer' to the uniform buffer.

public:
	Satellite(Framework::Timer newRevolutionDuration, 
			  float newOffsetFromSun, float newDiameter);

	/// \fn LoadMesh
	/// \brief Loads the satellite's mesh
	void LoadMesh(const std::string &fileName);

	/// \fn Render
	/// \brief Renders the satellite and its on-click animation.
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
				float gamma,
				const LitProgData &litData, 
				const UnlitProgData &unlitData, 
				const SimpleProgData &interpData);

	/// \fn Update
	/// \brief Updates the satellite's position.

	/// The position is based on the current time through the satellite's 
	/// revolution and its offset from sun.
	///
	/// \f$position.x = cos(currTimeThroughLoop \times (2 \times \pi)) \times offsetFromSun\f$
	///
	/// \f$position.y = sin(currTimeThroughLoop \times (2 \times \pi)) \times offsetFromSun\f$
	void Update();

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
				   float windowWidth, float windowHeight);

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

public:
	Satellite(const Satellite &other);
	~Satellite();
	Satellite operator=(const Satellite &other);
};


inline void Satellite::SetIsClicked(bool newIsClicked)
{
	isClicked = newIsClicked;
}

inline float Satellite::GetOffsetFromSun()
{
	return offsetFromSun;
}

inline float Satellite::GetDiameter()
{
	return diameter;
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
	std::auto_ptr<Framework::Mesh> sunMesh; ///< Represents the mesh file for the sun.

	// TODO: Find a solution to the smart_ptr problem. Satellite removal freezes the game.
	std::vector<Satellite*> satellites; ///< The sun's satellites.

	glm::vec3 position; ///< The sun's position.

	float diameter; ///< Its _diameter_.

	int satelliteCap;

	bool isClicked;

public:
	Sun();
	Sun(glm::vec3 newPosition, float newDiameter, int newSatelliteCap);

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
				const LitProgData &litData, const UnlitProgData &unlitData, const SimpleProgData &interpData);
	
	/// \fn Update
	/// \brief Calls the satellite's update functions (for now).
	void Update();

	/// \fn AddSatellite
	/// \brief Adds a satellite to the sun.

	/// This function also checks if the satellite cap is reached (for now - 4). 
	///
	/// After this check the function creates a new satellite, loads its mesh,
	/// sets its parent and pushes back the vector with satellites.
	///
	///If everything went OK the function returns `true`.
	bool AddSatellite(const std::string &fileName, 
					  float offset, float speed, float diameter);

	/// \fn RemoveSatellite()
	/// \brief Removes the last added satellite.
	bool RemoveSatellite();

	/// \fn RemoveSatellite(int index)
	/// \brief Removes the satellite at position `index`
	bool RemoveSatellite(int index);

	/// \fn GetSatellites
	/// \brief Gets the sun satellites. Used to access this info from elsewhere.
	std::vector<Satellite*> GetSatellites();

	/// \fn GetPosition
	/// \brief Gets the sun position.
	glm::vec4 GetPosition();

	/// \fn GetSatelliteCap
	/// \brief Gets the satellite cap limit.

	// NOTE: Maybe not so useful.
	int GetSatelliteCap();

	/// \fn IsClicked
	/// \brief Checks if the sun is clicked. 

	/// This check is simpler than the satellite's one (see Satellite::IsClicked). 
	///
	/// The function gets the ray from the camera to the body and checks for intersection
	/// of the ray against a sphere.

	// TODO: Make a Sun on-click animation.
	bool IsClicked(glm::mat4 projMat, glm::mat4 modelMat, 
				   Mouse userMouse, glm::vec4 cameraPos,
				   float windowWidth, float windowHeight);

	// NOTE: Maybe remove
	void IsSatelliteClicked(glm::mat4 projMat, glm::mat4 modelMat, 
						    Mouse userMouse, glm::vec4 cameraPos,
						    float windowWidth, float windowHeight);

public:
	Sun(const Sun &other);
	~Sun();
	Sun operator=(const Sun &other);
};

inline glm::vec4 Sun::GetPosition()
{
	return glm::vec4(position, 1.0f);
}

inline int Sun::GetSatelliteCap()
{
	return satelliteCap;
}



#endif