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


#ifndef MOUSE_H
#define MOUSE_H

#define PI 3.14159f

#include <glm\glm.hpp>
#include "../framework/Utility.h"

// A simple mouse wrapping class.
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

	void OverrideLastPosition(glm::ivec2 newLastPosition);


	Utility::Ray GetPickRay(glm::mat4 projMat, glm::mat4 modelMat, 
							glm::vec4 cameraPos, glm::vec4 bodyPos,
							float windowWidth, float windowHeight);


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