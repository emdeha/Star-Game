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

/// \file Mouse.h
/// \brief Contains the class which wraps the mouse for easier use.

#ifndef MOUSE_H
#define MOUSE_H


#define PI 3.14159f


#include "../glsdk/glm/glm.hpp"
#include "../framework/Utility.h"


/// \class Mouse
/// \brief Wraps the mouse for easier use.

/// This class contains the mouse's current and last positions, `bool` vars for determining
/// if a mouse button is pressed and some state setting functions.
class Mouse
{
private:
	glm::ivec2 currentPosition;
	glm::ivec2 lastPosition;

	bool isLeftButtonPressed;
	bool isRightButtonPressed;

	bool isMouseWheelUp;
	bool isMouseWheelDown;

public:
	Mouse();

	
	void SetCurrentPosition(glm::ivec2 newCurrentPosition);
	glm::ivec2 GetCurrentPosition();
	glm::ivec2 GetLastPosition();

	/// \fn OverrideLastPosition
	/// \brief Explicitly sets the mouse's last position.
	void OverrideLastPosition();

	/// \fn GetPickRay
	/// \brief Gets the mouse ray from the camera to where it points in the scene.

	/// To do this we need to convert the mouse's coordinates from 2D to 3D. We have the position
	/// of the mouse in window coordinates to convert it to `world space` coordinates we need to move all
	/// the way back through the transformation pipeline:
	///		- Convert the position to `clip space`.
	///		- From `clip space` we move to `view space`.
	///		- The nex stage is to normalize these coordinates.
	///		- The normalized coordinates are then transformed to `world space`.
	///
	///	Now, that we have the mouse coordinates in `world space`, we can construct the mouse ray:
	///		- The direction of the ray is: \f$mousePosInWorldSpace - cameraPosition\f$ 
	///		  (we need to normalize the resulting vector).
	///		- Its origin is the camera position.
	Utility::Ray GetPickRay(glm::mat4 projMat, glm::mat4 modelMat, 
						    glm::vec4 cameraPos,
						    int windowWidth, int windowHeight);

	glm::vec2 GetClipSpacePosition(int windowWidth, int windowHeight);

	glm::vec4 GetWorldSpacePosition(int windowWidth, int windowHeight,
									glm::mat4 projMat, glm::mat4 modelMat);

	glm::vec4 GetPositionAtDimension(int windowWidth, int windowHeight,
									 glm::mat4 projMat, glm::mat4 modelMat, 
									 glm::vec4 cameraPos, 
									 glm::comp dimension);


	bool IsLeftButtonDown();
	bool IsRightButtonDown();
	
	bool IsMouseWheelUp();
	bool IsMouseWheelDown();

	void PressLeftButton();
	void ReleaseLeftButton();
	void PressRightButton();
	void ReleaseRightButton();

	void MouseWheelUp();
	void MouseWheelNotUp();
	void MouseWheelDown();
	void MouseWheelNotDown();
};

#endif