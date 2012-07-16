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


#include "stdafx.h"
#include "Mouse.h"

#include <glload/gl_3_3.h>
#include <glutil\glutil.h>

Mouse::Mouse()
{
	currentPosition = glm::ivec2();
	lastPosition = glm::ivec2();

	isLeftButtonPressed = false;
	isRightButtonPressed = false;

	isMouseWheelUp = false;
	isMouseWheelDown = false;
}

inline void Mouse::SetCurrentPosition(glm::ivec2 newCurrentPosition)
{
	currentPosition = newCurrentPosition;
}

inline glm::ivec2 Mouse::GetCurrentPosition()
{
	return currentPosition;
}
inline glm::ivec2 Mouse::GetLastPosition()
{
	return lastPosition;
}

inline void Mouse::OverrideLastPosition(glm::ivec2 newLastPosition)
{
	lastPosition = newLastPosition;
}


inline bool Mouse::IsLeftButtonDown()
{
	return isLeftButtonPressed;
}
inline bool Mouse::IsRightButtonDown()
{
	return isRightButtonPressed;
}

inline bool Mouse::IsMouseWheelUp()
{
	return isMouseWheelUp;
}
inline bool Mouse::IsMouseWheelDown()
{
	return isMouseWheelDown;
}

inline void Mouse::PressLeftButton()
{
	isLeftButtonPressed = true;
}
inline void Mouse::ReleaseLeftButton()
{
	isLeftButtonPressed = false;
}
inline void Mouse::PressRightButton()
{
	isRightButtonPressed = true;
}
inline void Mouse::ReleaseRightButton()
{
	isRightButtonPressed = false;
}

inline void Mouse::MouseWheelUp()
{
	isMouseWheelUp = true;
}
inline void Mouse::MouseWheelNotUp()
{
	isMouseWheelUp = false;
}
inline void Mouse::MouseWheelDown()
{
	isMouseWheelDown = true;
}
inline void Mouse::MouseWheelNotDown()
{
	isMouseWheelDown = false;
}


Utility::Ray Mouse::GetPickRay(glm::mat4 projMat, glm::mat4 modelMat, 
							   glm::vec4 cameraPos, glm::vec4 bodyPos, 
							   float windowHeight, float windowWidth)
{
	float mouseX = this->currentPosition.x;
	float mouseY = this->currentPosition.y;

	glm::vec4 mousePos_clipSpace = glm::vec4(
											 ((mouseX * 2.0f) / windowWidth) - 1.0f,
											 (1.0f - (2.0f * mouseY) / windowHeight),
											 -1.0f, 
											 1.0f
											 );

	glm::vec4 mousePos_viewSpace = glm::inverse(projMat) * mousePos_clipSpace;

	mousePos_viewSpace = glm::vec4(mousePos_viewSpace.x / mousePos_viewSpace.w, 
								   mousePos_viewSpace.y / mousePos_viewSpace.w,
								   mousePos_viewSpace.z / mousePos_viewSpace.w,
								   1.0f);

	glm::vec4 mousePos_worldSpace = glm::inverse(modelMat) * mousePos_viewSpace;

	glm::vec4 rayDirection = glm::normalize(mousePos_worldSpace - cameraPos);
	glm::vec4 rayOrigin = cameraPos - bodyPos;

	return Utility::Ray(rayOrigin, rayDirection);
}