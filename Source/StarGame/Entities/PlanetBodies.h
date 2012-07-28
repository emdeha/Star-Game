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


#ifndef PLANET_BODIES_H
#define PLANET_BODIES_H


#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/Timer.h"
#include "../ProgramData/ProgramData.h"
#include "../Mouse/Mouse.h"


// NOTE: Body positions need to be passed in world space in order to do proper clicking.

class Sun;

class Satellite
{
private:
	Framework::Mesh *mesh;
	Sun *parent;

	Framework::Timer revolutionDuration;
	Framework::Timer fastTimer;

	glm::vec3 position;

	float height;
	float offsetFromSun;

	float diameter;

	bool isClicked;


	char animationID;

public:
	Satellite();
	Satellite(Framework::Timer newRevolutionDuration, 
			  float newHeight, float newOffsetFromSun, float newDiameter);

	void LoadMesh(const std::string &fileName);

	void Render(glutil::MatrixStack &modelMatrix, const LitProgData &data, const UnlitProgData &unlitData, const InterpProgData &interpData);
	void Update();

	bool IsClicked(glm::mat4 projMat, glm::mat4 modelMat, 
				   Mouse userMouse, glm::vec4 cameraPos,
				   float windowWidth, float windowHeight);

	void LoadClickedAnimation(glutil::MatrixStack &modelMatrix, const LitProgData &data, const UnlitProgData &unlitData, const InterpProgData &interpData);

	void SetParent(const Sun *newParent);
	void SetAnimationID(char newAnimationID)
	{
		animationID = newAnimationID;
	}

	void SetIsClicked(bool newIsClicked)
	{
		isClicked = newIsClicked;
	}

	float GetOffsetFromSun()
	{
		return offsetFromSun;
	}
	float GetDiameter()
	{
		return diameter;
	}

public:
	Satellite(const Satellite &other);
	~Satellite();
	Satellite operator=(const Satellite &other);

private:
	glm::vec4 CalculatePosition();
};



class Sun
{
private:
	Framework::Mesh *sunMesh;

	std::vector<Satellite*> satellites;

	glm::vec3 position;

	float diameter;

	bool isClicked;

public:
	Sun();
	Sun(glm::vec3 newPosition, float newDiameter);

	void LoadMesh(const std::string &fileName);

	void Render(glutil::MatrixStack &modelMatrix,
				const LitProgData &litData, const UnlitProgData &unlitData, const InterpProgData &interpData);
	void Update();

	bool AddSatellite(const std::string &fileName, 
					  float height, float offset, float speed, float diameter);

	std::vector<Satellite*> GetSatellites();

	glm::vec4 GetPosition();

	bool IsClicked(glm::mat4 projMat, glm::mat4 modelMat, 
				   Mouse userMouse, glm::vec4 cameraPos,
				   float windowWidth, float windowHeight);

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

inline void Satellite::SetParent(const Sun *newParent)
{
	parent = new Sun(*newParent);
}


#endif