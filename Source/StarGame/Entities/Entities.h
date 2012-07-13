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


#ifndef ENTITIES_H
#define ENTITIES_H

#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/Timer.h"
#include "../StarGame/ProgramData.h"


class Satellite
{
private:
	Framework::Mesh *mesh;
	Framework::Timer revolutionDuration;

	glm::vec3 position;

	float height;
	float offsetFromSun;

	float radius;

public:
	Satellite();
	Satellite(Framework::Timer newRevolutionDuration, 
			  float newHeight, float newOffsetFromSun, float newRadius);

	void LoadMesh(const std::string &fileName);

	void Render(glutil::MatrixStack &modelMatrix, ProgramData &data);
	void Update();

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

public:
	Sun();
	Sun(glm::vec3 newPosition);

	void LoadMesh(const std::string &fileName);

	void Render(glutil::MatrixStack &modelMatrix, 
				ProgramData &data, UnlitProgData &unlitData);
	void Update();

	void AddSatellite(const std::string &fileName, 
					  float height, float offset, float speed, float radius);

public:
	Sun(const Sun &other);
	~Sun();
	Sun operator=(const Sun &other);
};

#endif