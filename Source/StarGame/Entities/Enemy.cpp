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
#include "Enemy.h"


static void GenerateUniformBuffers(int &materialBlockSize, 
								   glm::vec4 diffuseColor,
								   GLuint &materialUniformBuffer)
{
	MaterialBlock material;
	material.diffuseColor = diffuseColor;
	material.specularColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	material.shininessFactor = 0.3f;


	int uniformBufferAlignSize = 0;
 	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);

	materialBlockSize = sizeof(MaterialBlock);
	materialBlockSize += uniformBufferAlignSize -
		(materialBlockSize % uniformBufferAlignSize);

	
	glGenBuffers(1, &materialUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, materialUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, &material, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Spaceship::Spaceship(glm::vec3 newPosition, 
					 glm::vec3 newVelocity,
					 float newRotationX,
					 float newRotationY,
					 float newScaleFactor,
					 float newProjectileSpeed,
					 int newProjectileLifeSpan, int newProjectileDamage)
{
	position = newPosition;
	velocity = newVelocity;

	rotationX = newRotationX;
	rotationY = newRotationY;
	scaleFactor = newScaleFactor;

	health = 100;

	currentState = PATROL_STATE;

	projectileSpeed = newProjectileSpeed;
	projectileLifeSpan = newProjectileLifeSpan;
	projectileDamage = newProjectileDamage;

	lineOfSight = 3.0f;

	projectile = 
		std::unique_ptr<Projectile>(new Projectile(position, 
												   velocity * projectileSpeed, 
												   projectileDamage, 
												   projectileLifeSpan));


	patrolRoute.patrolPoints.push_back(position);
	patrolRoute.patrolPoints.push_back(glm::vec3(4.5f, 4.0f, 0.0f));
	patrolRoute.patrolPoints.push_back(glm::vec3(-4.5f, 4.0f, 0.0f));
	patrolRoute.patrolPoints.push_back(glm::vec3(-4.5f, -4.0f, 0.0f));
	patrolRoute.patrolPoints.push_back(glm::vec3(4.5f, -4.0f, 0.0f));
	//patrolRoute.patrolPoints.push_back(position);
	patrolRoute.currentPatrolPointIndex = 0;
	patrolRoute.lastPatrolPointIndex = 0;
	patrolRoute.nextPatrolPointIndex = 1;

	velocity = 
		glm::normalize(patrolRoute.patrolPoints[patrolRoute.nextPatrolPointIndex] - 
					   patrolRoute.patrolPoints[patrolRoute.currentPatrolPointIndex]) * 0.1f; // 0.1f - speed
}

void Spaceship::LoadProjectileMesh(const std::string &meshFile)
{
	projectile->LoadMesh(meshFile);
}

void Spaceship::LoadMesh(const std::string &meshFile)
{
	try
	{
		mesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(meshFile));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}

	GenerateUniformBuffers(materialBlockSize, 
						   glm::vec4(0.21f, 0.42f, 0.34f, 1.0f), 
						   materialUniformBuffer);

	projectile->LoadMesh(meshFile);
}

void Spaceship::UpdateAI(Sun &sun)
{
	if(currentState == ATTACK_STATE)
	{	
		velocity = glm::vec3();

		if(projectile->IsDestroyed())
		{
			glm::vec3 newDirection = glm::vec3();

			if(sun.HasSatellites())
			{
				glm::vec3 satellitePosition = sun.GetOuterSatellite()->GetPosition();
				newDirection = satellitePosition - position;
			}
			else
			{
				newDirection = glm::vec3(sun.GetPosition()) - position;
			}

			newDirection = glm::normalize(newDirection);
			projectile->Recreate(position, 
								 newDirection * projectileSpeed, 
								 projectileLifeSpan);
		}

		projectile->Update(sun);
	}
	else if(currentState == EVADE_STATE)
	{
		position += velocity;
	}
	else if(currentState == PATROL_STATE)
	{		
		if(patrolRoute.patrolPoints.size() > patrolRoute.nextPatrolPointIndex)
		{
			glm::vec3 vectorBetweenShipAndPatrolPoint = 
				patrolRoute.patrolPoints[patrolRoute.nextPatrolPointIndex] -
				position;
			float distanceBetweenShipAndPatrolPoint = glm::length(vectorBetweenShipAndPatrolPoint);

			if(distanceBetweenShipAndPatrolPoint <= 0.1f)
			{		
				patrolRoute.currentPatrolPointIndex = patrolRoute.nextPatrolPointIndex;
				patrolRoute.nextPatrolPointIndex++;
				if(patrolRoute.nextPatrolPointIndex >= patrolRoute.patrolPoints.size())
				{
					patrolRoute.nextPatrolPointIndex = 0;
				}

				glm::vec3 vectorBetweenPatrolPoints = 
					patrolRoute.patrolPoints[patrolRoute.nextPatrolPointIndex] -
					patrolRoute.patrolPoints[patrolRoute.currentPatrolPointIndex];

				velocity = glm::normalize(vectorBetweenPatrolPoints) * 0.1f; // 0.1f - speed. 
			}
		}
		/*else
		{
			patrolRoute.currentPatrolPointIndex = patrolRoute.patrolPoints.size() - 1;
			patrolRoute.nextPatrolPointIndex = 0;
		}*/
		position += velocity;
		
		glm::vec3 vectorFromSunToSpaceship = glm::vec3(sun.GetPosition()) - position;
		float distanceBetweenSunAndSpaceship = glm::length(vectorFromSunToSpaceship);

		if(distanceBetweenSunAndSpaceship < lineOfSight)
		{
			currentState = ATTACK_STATE;
		}
	}
}

void Spaceship::Update(bool isSunKilled, Sun &sun)
{
	if(health <= 20)
	{
		glm::vec3 newVelocity = (glm::vec3(sun.GetPosition()) - position) * 0.1f;
		newVelocity = glm::normalize(-newVelocity);
		currentState = EVADE_STATE;
	}

	if(!isSunKilled)
	{
		UpdateAI(sun);
	}
	else
	{
		currentState = PATROL_STATE;
		velocity = glm::vec3(-0.1f, 0.0f, 0.0f);
	}
}

void Spaceship::Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
					   float gamma, 
					   const LitProgData &litData,
					   float interpolation)
{
	{
		glutil::PushStack push(modelMatrix);


		glm::vec3 viewPosition = position + velocity * interpolation;
		modelMatrix.Translate(viewPosition);
		modelMatrix.Scale(scaleFactor);
		modelMatrix.RotateX(rotationX);
		modelMatrix.RotateY(rotationY);


		glBindBufferRange(GL_UNIFORM_BUFFER, materialBlockIndex, materialUniformBuffer,
						  0, sizeof(MaterialBlock));

		glm::mat3 normMatrix(modelMatrix.Top());
		normMatrix = glm::transpose(glm::inverse(normMatrix));

		glUseProgram(litData.theProgram);
		glUniformMatrix4fv(litData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniformMatrix3fv(litData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));

		mesh->Render("lit");

		glUseProgram(0);

		glBindBufferBase(GL_UNIFORM_BUFFER, materialBlockIndex, 0);
	}

	if(!projectile->IsDestroyed() && currentState == ATTACK_STATE)
	{
		projectile->Render(modelMatrix, materialBlockIndex, gamma, litData,
						   interpolation);
	}
}

void Spaceship::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_SHOT_FIRED:
		break;
	case EVENT_TYPE_ATTACKED:
		break;
	}
}



Swarm::Swarm(glm::vec3 newPosition, glm::vec3 newDirection, float newSpeed,
			 int newSwarmEntitiesCount, int newHealth, int newDamage, int newTime_milliseconds,
			 const BillboardProgDataNoTexture &billboardProgramNoTexture)
{
	position = newPosition;
	direction = newDirection;
	speed = newSpeed;
	swarmEntitiesCount = newSwarmEntitiesCount;
	health = newHealth;
	damageOverTime.damage = newDamage;
	damageOverTime.time_milliseconds = newTime_milliseconds;
	currentState = PATROL_STATE;
	swarmBody = SwarmEmitter(position, swarmEntitiesCount);
	swarmBody.Init(billboardProgramNoTexture);
}

void Swarm::Update()
{
	swarmBody.Update();
	//position += direction * speed;
}

void Swarm::Render(glutil::MatrixStack &modelMatrix,
				   glm::vec3 cameraPosition,
				   const BillboardProgDataNoTexture &billboardProgramNoTexture)
{
	swarmBody.Render(modelMatrix, cameraPosition, billboardProgramNoTexture);
}

void Swarm::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ATTACKED:
		break;
	}
}