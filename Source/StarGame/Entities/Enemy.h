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


#ifndef ENEMY_H
#define ENEMY_H


#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>

#include "../framework/Mesh.h"

#include "../ProgramData/ProgramData.h"
#include "MaterialBlock.h"


class Spaceship
{
private:
	glm::vec3 position;

	std::auto_ptr<Framework::Mesh> mesh;

	int materialBlockSize;
	GLuint materialUniformBuffer;

public:
	Spaceship(glm::vec3 newPosition);

	void LoadMesh(const std::string &meshFile);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
				float gamma, 
				const LitProgData &litData);
};


#endif