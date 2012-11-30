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


#ifndef ENGINE_H
#define ENGINE_H


#include "../framework/Utility.h"
#include "../StarGame/ShaderManager.h"
#include <vector>


// Tried the policy based design but it was too slow. 
// The other solution I can think of is function pointers.

struct StandardParticle
{
	glm::vec3 position;
	glm::vec3 velocity;
};


class ParticleEmitter
{
private:
	std::vector<StandardParticle> particles;

	glm::vec3 position;
	int particleCount;

	GLuint vao;
	GLuint vertexBO;
	
	Texture2D texture;

public:
	ParticleEmitter() {}
	ParticleEmitter(glm::vec3 newPosition, int newParticleCount);

	void Init(const BillboardProgData &billboardedProgData);

	void Update();
	void Render(glutil::MatrixStack &modelMatrix,
				glm::vec3 cameraPosition,
				const BillboardProgData &programData);
};







#endif