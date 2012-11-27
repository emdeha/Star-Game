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
#include "Engine.h"


ParticleEmitter::ParticleEmitter(glm::vec3 newPosition, int newParticleCount)
{
	position = newPosition;
	particleCount = newParticleCount;

	particles.resize(particleCount);

	vao = 0;
	vertexBO = 0;

	texture = Texture2D();
	texture.Load("../data/images/particle.png");
}

void ParticleEmitter::Init(const BillboardProgData &billboardedProgData)
{
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position = glm::vec3();
		particles[i].velocity = glm::vec3((float)rand() / RAND_MAX * 0.01f,
										  (float)rand() / RAND_MAX * 0.01f,
										  (float)rand() / RAND_MAX * 0.01f);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, glm::value_ptr(position), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glUseProgram(billboardedProgData.theProgram);
	glUniform1f(billboardedProgData.billboardSizeUnif, 0.1f);
	glUseProgram(0);
}

void ParticleEmitter::Update()
{
	for(int i = 0; i < particleCount; i++)
	{
		particles[i].position += particles[i].velocity;
	}
}

void ParticleEmitter::Render(glutil::MatrixStack &modelMatrix,
							 glm::vec3 cameraPosition,
							 const BillboardProgData &billboardedProgData)
{
	glutil::PushStack push(modelMatrix);

	glUseProgram(billboardedProgData.theProgram);

	
	glUniformMatrix4fv(billboardedProgData.modelToCameraMatrixUnif,
					   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
	glUniform3f(billboardedProgData.cameraPositionUnif, 
				cameraPosition.x, cameraPosition.y, cameraPosition.z);

	texture.Bind(GL_TEXTURE0);

	for(int i = 0; i < particleCount; i++)
	{
		glUniform3f(billboardedProgData.deltaPositionUnif, 
					particles[i].position.x, particles[i].position.y, particles[i].position.z);

		glDrawArrays(GL_POINTS, 0, 1);
	}

	glDisableVertexAttribArray(0);

	glUseProgram(0);
}