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
#include "../framework/ErrorAPI.h"

#include <glload/gl_3_3.h>
#include <glutil/glutil.h>

Mouse::Mouse()
{
	currentPosition = glm::ivec2();
	lastPosition = glm::ivec2();

	isLeftButtonPressed = false;
	isRightButtonPressed = false;

	isMouseWheelUp = false;
	isMouseWheelDown = false;
}

void Mouse::SetCurrentPosition(glm::ivec2 newCurrentPosition)
{
	currentPosition = newCurrentPosition;
}

glm::ivec2 Mouse::GetCurrentPosition()
{
	return currentPosition;
}
glm::ivec2 Mouse::GetLastPosition()
{
	return lastPosition;
}

void Mouse::OverrideLastPosition(glm::ivec2 newLastPosition)
{
	lastPosition = newLastPosition;
}


bool Mouse::IsLeftButtonDown()
{
	return isLeftButtonPressed;
}
bool Mouse::IsRightButtonDown()
{
	return isRightButtonPressed;
}

bool Mouse::IsMouseWheelUp()
{
	return isMouseWheelUp;
}
bool Mouse::IsMouseWheelDown()
{
	return isMouseWheelDown;
}

void Mouse::PressLeftButton()
{
	isLeftButtonPressed = true;
}
void Mouse::ReleaseLeftButton()
{
	isLeftButtonPressed = false;
}
void Mouse::PressRightButton()
{
	isRightButtonPressed = true;
}
void Mouse::ReleaseRightButton()
{
	isRightButtonPressed = false;
}

void Mouse::MouseWheelUp()
{
	isMouseWheelUp = true;
}
void Mouse::MouseWheelNotUp()
{
	isMouseWheelUp = false;
}
void Mouse::MouseWheelDown()
{
	isMouseWheelDown = true;
}
void Mouse::MouseWheelNotDown()
{
	isMouseWheelDown = false;
}


Utility::Ray Mouse::GetPickRay(glm::mat4 projMat, glm::mat4 modelMat, 
							   glm::vec4 cameraPos, 
							   int windowWidth, int windowHeight)
{
	glm::vec4 mousePos_worldSpace = this->GetWorldSpacePosition(windowWidth, windowHeight,
																projMat, modelMat);

	glm::vec4 rayDirection = glm::normalize(mousePos_worldSpace - cameraPos);
	glm::vec4 rayOrigin = cameraPos;

	return Utility::Ray(rayOrigin, rayDirection);
}


glm::vec2 Mouse::GetClipSpacePosition(int windowWidth, int windowHeight)
{
	float mouseX = float(this->currentPosition.x);
	float mouseY = float(this->currentPosition.y);

	glm::vec2 mousePos_clipSpace = glm::vec2(
											 ((mouseX * 2.0f) / windowWidth) - 1.0f,
											 (1.0f - (2.0f * mouseY) / windowHeight)
											);

	return mousePos_clipSpace;
}

glm::vec4 Mouse::GetWorldSpacePosition(int windowWidth, int windowHeight,
									   glm::mat4 projMat, glm::mat4 modelMat)
{
	float mouseX = float(this->currentPosition.x);
	float mouseY = float(this->currentPosition.y);

	glm::vec4 mousePos_clipSpace = glm::vec4(
											 ((mouseX * 2.0f) / windowWidth) - 1.0f,
											 (1.0f - (2.0f * mouseY) / windowHeight),
											 0.0f,
											 1.0f
											 );

	glm::vec4 mousePos_viewSpace = glm::inverse(projMat) * mousePos_clipSpace;

	mousePos_viewSpace = mousePos_viewSpace / mousePos_viewSpace.w;

	glm::vec4 mousePos_worldSpace = glm::inverse(modelMat) * mousePos_viewSpace;

	return mousePos_worldSpace;
}

glm::vec4 Mouse::GetPositionAtDimension(int windowWidth, int windowHeight,
										glm::mat4 projMat, glm::mat4 modelMat, 
										glm::vec4 cameraPos, 
										glm::comp dimension)
{
	glm::vec4 mousePos_worldSpace = GetWorldSpacePosition(windowWidth, windowHeight, 
														  projMat, modelMat);

	glm::vec4 direction = glm::normalize(mousePos_worldSpace - cameraPos);

	float distance = 0.0f;

	switch(dimension)
	{
	case glm::comp::X:
		distance = -cameraPos.x / direction.x;
		break;
	case glm::comp::Y:
		distance = -cameraPos.y / direction.y;
		break;
	case glm::comp::Z:
		distance = -cameraPos.z / direction.z;
		break;
	default:
		HandleUnexpectedError("invalid mouse dimension", __LINE__, __FILE__);
		break;
	}

	return glm::vec4(cameraPos + direction * distance);
}