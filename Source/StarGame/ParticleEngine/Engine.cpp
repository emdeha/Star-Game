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


SimpleParticleEmitter::SimpleParticleEmitter(glm::vec3 newPosition, int newNumberOfParticles)
{
	position = newPosition;
	numberOfParticles = newNumberOfParticles;


	particleSprites = 
		Utility::Primitives::SpriteArray(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0.1f, 0.1f, false);
	particles.resize(numberOfParticles);
}

void SimpleParticleEmitter::Init()
{
	particleSprites.Init(numberOfParticles,
						 "../data/images/diamond.png");
	for(int i = 0; i < particles.size(); i++)
	{
		particles[i] = SimpleParticle(position, glm::vec3());
		particles[i].Init();
	}
}

void SimpleParticleEmitter::Update()
{
	for(int i = 0; i < particles.size(); i++)
	{
		particles[i].Update();
	}
}

void SimpleParticleEmitter::Draw(glutil::MatrixStack &modelMat, 
								 const TextureProgData &textureProgData)
{
	particleSprites.Draw(modelMat, textureProgData, GetParticlePositions());

}

std::vector<glm::vec3> SimpleParticleEmitter::GetParticlePositions()
{
	std::vector<glm::vec3> positions;
	for(int i = 0; i < numberOfParticles; i++)
	{
		positions.push_back(particles[i].GetPosition());
	}

	return positions;
}


SimpleParticle::SimpleParticle(glm::vec3 newPosition, glm::vec3 newVelocity)
{
	position = newPosition;
	velocity = glm::vec3(((float)rand() / (float)RAND_MAX) / 100.0f,
						 ((float)rand() / (float)RAND_MAX) / 100.0f, 
						 0.0f);
}

void SimpleParticle::Init()
{
}

void SimpleParticle::Update()
{
	position += velocity;
}

void SimpleParticle::Draw(/*glutil::MatrixStack &modelMat, const TextureProgData &textureProgData*/)
{
	/*{
		glutil::PushStack push(modelMat);

		modelMat.Translate(position.x, position.y, position.z);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		particleSprite.Draw(modelMat, textureProgData);

		glDisable(GL_BLEND);
	}*/
}









#define MAX_PARTICLES 1000
#define PARTICLE_LIFETIME 10.0f

#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f


struct Particle
{
	float type;
	glm::vec3 position;
	glm::vec3 velocity;
	float lifeTime_milliseconds;
};


TransformFeedbackParticleEmitter::TransformFeedbackParticleEmitter()
{
	currentVertexBuffer = 0;
	currentTransformFeedbackBuffer = 1;
	isFirst = true;
	time = 0;
	vao = 0;
	texture = std::shared_ptr<Texture2D>(new Texture2D());
}

TransformFeedbackParticleEmitter::~TransformFeedbackParticleEmitter()
{
	if(particleTFBO[0] != 0)
	{
		glDeleteTransformFeedbacks(2, particleTFBO);
	}
	if(particleVBO[0] != 0)
	{
		glDeleteBuffers(2, particleVBO);
	}
}


bool TransformFeedbackParticleEmitter::Init(const BillboardProgData &billboardData, const ParticleProgData &particleData,
										    const glm::vec3 &position)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	std::vector<Particle> particles(MAX_PARTICLES);

	particles[0].type = PARTICLE_TYPE_LAUNCHER;
	particles[0].position = position;
	particles[0].velocity = glm::vec3(0.0f, 0.0001f, 0.0f);
	particles[0].lifeTime_milliseconds = 0.0f;

	glGenTransformFeedbacks(2, particleTFBO);
	glGenBuffers(2, particleVBO);

	for(unsigned int i = 0; i < 2; i++)
	{
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particleTFBO[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleVBO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particles.size(), &particles[0], GL_DYNAMIC_DRAW);
	}


	// TODO: remove pointer
	const GLchar *varyings[4];
	varyings[0] = "outType";
	varyings[1] = "outPosition";
	varyings[2] = "outVelocity";
	varyings[3] = "outAge";
	
	glTransformFeedbackVaryings(particleData.theProgram, 4, varyings, GL_INTERLEAVED_ATTRIBS);

	
	glUseProgram(particleData.theProgram);

	glUniform1i(particleData.randomTextureUnif, 3);
	glUniform1f(particleData.launcherLifetimeUnif, 100.0f);
	glUniform1f(particleData.shellLifetimeUnif, 10000.0f);
	glUniform1f(particleData.secondaryShellLifetimeUnif, 2500.0f);

	
	if(!randomTexture.InitRandomTexture(1000))
	{
		return false;
	}
	randomTexture.Bind(GL_TEXTURE3);

	glUseProgram(0);


	glUseProgram(billboardData.theProgram);

	glUniform1i(billboardData.samplerUnif, 0);
	glUniform1f(billboardData.billboardSizeUnif, 0.01f);

	texture = std::shared_ptr<Texture2D>(new Texture2D());
	if(!texture->Load("../data/images/fireworks_red.jpg"))
	{
		throw ("Texture not found\n");
	}

	glUseProgram(0);


	return true;
}

void TransformFeedbackParticleEmitter::Render(glutil::MatrixStack &modelMatrix,
											  const BillboardProgData &billboardData, const ParticleProgData &particleData,
											  glm::vec3 cameraPosition, int deltaTime_milliseconds)
{
	time += deltaTime_milliseconds;

	UpdateParticles(particleData, deltaTime_milliseconds);
	RenderParticles(modelMatrix, billboardData, cameraPosition);

	currentVertexBuffer = currentTransformFeedbackBuffer;
	currentTransformFeedbackBuffer = (currentTransformFeedbackBuffer + 1) & 0x1;
}

void TransformFeedbackParticleEmitter::UpdateParticles(const ParticleProgData &particleData, int deltaTime_milliseconds)
{
	glBindVertexArray(vao);

	glUseProgram(particleData.theProgram);

	glUniform1f(particleData.timeUnif, time);
	glUniform1f(particleData.deltaTime_millisecondsUnif, deltaTime_milliseconds);

	randomTexture.Bind(GL_TEXTURE3);

	glUseProgram(0);
	
	glEnable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, particleVBO[currentVertexBuffer]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, particleTFBO[currentTransformFeedbackBuffer]);

	glEnableVertexAttribArray(particleData.particleTypeAttrib);
	glEnableVertexAttribArray(particleData.particlePositionAttrib);
	glEnableVertexAttribArray(particleData.particleVelocityAttrib);
	glEnableVertexAttribArray(particleData.particleAgeAttrib);


	// Invalid VAO/VBO pointer usage X 4
	glVertexAttribPointer(particleData.particleTypeAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
	glVertexAttribPointer(particleData.particlePositionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);
	glVertexAttribPointer(particleData.particleVelocityAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)16);
	glVertexAttribPointer(particleData.particleAgeAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)28);
	//

	// No transform feedback buffers
	glBeginTransformFeedback(GL_POINTS);

	if(isFirst)
	{
		// Array object is not active
		glDrawArrays(GL_POINTS, 0, 1);
		isFirst = false;
	}
	else
	{
		// Array object is not active
		glDrawTransformFeedback(GL_POINTS, particleTFBO[currentVertexBuffer]);
	}
	
	// Transform feedback must be enabled and not paused
	glEndTransformFeedback();

	glDisableVertexAttribArray(particleData.particleTypeAttrib);
	glDisableVertexAttribArray(particleData.particlePositionAttrib);
	glDisableVertexAttribArray(particleData.particleVelocityAttrib);
	glDisableVertexAttribArray(particleData.particleAgeAttrib);
}

void TransformFeedbackParticleEmitter::RenderParticles(glutil::MatrixStack &modelMatrix, const BillboardProgData &billboardData,
													   glm::vec3 cameraPosition)
{
	glBindVertexArray(vao);

	glUseProgram(billboardData.theProgram);

	glUniform3f(billboardData.cameraPositionUnif, 
				cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniformMatrix4fv(billboardData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));

	glUseProgram(0);



	glDisable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_ARRAY_BUFFER, particleVBO[currentTransformFeedbackBuffer]);

	glEnableVertexAttribArray(billboardData.positionAttrib);

	// Invalid VAO/VBO pointer usage
	glVertexAttribPointer(billboardData.positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);

	// Array object is not active
	glDrawTransformFeedback(GL_POINTS, particleTFBO[currentTransformFeedbackBuffer]);

	glDisableVertexAttribArray(billboardData.positionAttrib);
}