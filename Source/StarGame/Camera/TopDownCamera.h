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

/// \file TopDownCamera.h
/// \brief Implements the top-down camera

#ifndef TOP_DOWN_CAMERA_H
#define TOP_DOWN_CAMERA_H

#include <glm/glm.hpp>


/// \class TopDownCamera
/// \brief Implements a top-down camera.

/// The camera is represented by a target and position in spherical coordinates
///
/// The functions CalcMatrix and ResolveCamPosition are used to apply the camera to the world.
class TopDownCamera
{
private:
	glm::vec3 camTarget; ///< Represents the target to which the camera looks

	float height; ///< Represents the radial distance to the target.
	float anglePhi_degs; ///< Represents the azumutal angle relative to the target.
	float angleTheta_degs; ///< Represents the polar angle relative to the target.

public:
	TopDownCamera(); 
	TopDownCamera(glm::vec3 newCamTarget, 
				  float newHeight, float newAnglePhi_degs, float newAngleTheta_degs);

	/// \fn CalcMatrix
	/// \brief The function used to calculate the camera matrix.

	/// The matrix is calculated based on the camera's position 
	/// (which is calculated through ResolveCamPosition()) and
	/// the camera's target.
	///
	/// To calculate the matrix we need to acquire some directions:
	///		- the look direction
	///		- the up direction
	///		- the right direction
	///		- the perpendicular direction to the right and look
	///
	/// We then use these to construct the camera's rotational matrix.
	/// 
	/// For the translational matrix we use the camera position.
	glm::mat4 CalcMatrix();

	/// \fn ResolveCamPosition
	/// \brief The function used to convert the camera's position from spherical to cartesian coordinates.

	/// The camera's position is calculated based on the direction of the target to the camera, 
	/// the camera's height and the camera target.
	///
	/// We have the height and the target's coordinate's. The only thing we need to calculate is 
	/// the direction: (\f$\theta\f$ is the polar angle and \f$\phi\f$ is the azumutal angle)
	///		- _x_ coordinate is \f$sin(\theta) \times cos(\phi)\f$
	///		- _y_ coordinate is \f$cos(\theta)\f$
	///		- _z_ coordinate is \f$sin(\theta) \times sin(\phi)\f$
	glm::vec3 ResolveCamPosition();
};

#endif