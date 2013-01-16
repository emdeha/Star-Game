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

#include <ctime>


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

	health = 100; // TODO: newHealth!

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
	patrolRoute.currentPatrolPointIndex = 0;
	patrolRoute.lastPatrolPointIndex = 0;
	patrolRoute.nextPatrolPointIndex = 1;

	velocity = 
		glm::normalize(patrolRoute.patrolPoints[patrolRoute.nextPatrolPointIndex] - 
					   patrolRoute.patrolPoints[patrolRoute.currentPatrolPointIndex]) * 0.1f; // 0.1f - speed
	patrolRoute.lastVelocity = velocity;
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

	projectile->LoadMesh(meshFile); // TODO: maybe this should be removed
}

void Spaceship::UpdateAI(CelestialBody &sun)
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
		position += velocity;
		
		glm::vec3 vectorFromSunToSpaceship = glm::vec3(sun.GetPosition()) - position;
		float distanceBetweenSunAndSpaceship = glm::length(vectorFromSunToSpaceship);

		if(distanceBetweenSunAndSpaceship < lineOfSight)
		{
			patrolRoute.lastVelocity = velocity;
			currentState = ATTACK_STATE;
		}
	}
}

void Spaceship::Update(bool isSunKilled, CelestialBody &sun)
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
		if(velocity == glm::vec3())
		{
			velocity = patrolRoute.lastVelocity;
		}
		currentState = PATROL_STATE;
		UpdateAI(CelestialBody());
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



Swarm::Swarm(glm::vec3 newPosition, glm::vec3 newVelocity,
			 int newSwarmEntitiesCount, int newHealth, int newDamage, int newTime_seconds,
			 float newLineOfSight,
			 const BillboardProgDataNoTexture &billboardProgramNoTexture)
{
	position = newPosition;
	velocity = newVelocity;
	swarmEntitiesCount = newSwarmEntitiesCount;
	health = newHealth;
	damageOverTime.damage = newDamage;
	damageOverTime.time_seconds = newTime_seconds;
	currentState = IDLE_STATE;
	lineOfSight = newLineOfSight;
	swarmBody = SwarmEmitter(position, swarmEntitiesCount);
	swarmBody.Init(billboardProgramNoTexture);
	isDestroyed = false;
	isCommanded = false;

	attackTimer = Framework::Timer(Framework::Timer::Type::TT_INFINITE);
}

void Swarm::AttackSolarSystem(CelestialBody &sun, bool isSatellite, float bodyIndex)
{
	EventArg sunDamage_perTime[2];
	sunDamage_perTime[0].argType = "damage";
	sunDamage_perTime[0].argument.varType = TYPE_INTEGER;
	sunDamage_perTime[0].argument.varInteger = damageOverTime.damage;
	sunDamage_perTime[1].argType = "bodyIndex";
	if(isSatellite)
	{
		sunDamage_perTime[1].argument.varType = TYPE_FLOAT;
		sunDamage_perTime[1].argument.varFloat = bodyIndex;
	}
	else
	{
		sunDamage_perTime[1].argument.varType = TYPE_INTEGER;
		sunDamage_perTime[1].argument.varInteger = -1;
	}
	Event sunDamageEvent(2, EVENT_TYPE_ATTACKED, sunDamage_perTime);

	sun.OnEvent(sunDamageEvent);
}

void Swarm::UpdateAI(CelestialBody &sun)
{
	if(currentState == ATTACK_STATE)
	{
		glm::vec3 vectorBetweenPlanetAndSwarm = glm::vec3();
		if(sun.HasSatellites())
		{
			vectorBetweenPlanetAndSwarm = sun.GetOuterSatellite()->GetPosition() - position;
		}
		else
		{
			vectorBetweenPlanetAndSwarm = glm::vec3(sun.GetPosition()) - position;
		}
		float distanceBetweenPlanetAndSwarm = glm::length(vectorBetweenPlanetAndSwarm);


		if(sun.HasSatellites())
		{
			if(distanceBetweenPlanetAndSwarm <= 
				sun.GetOuterSatellite()->GetDiameter() * sun.GetOuterSatellite()->GetDiameter())
			{
				//velocity = sun.GetOuterSatellite()->GetVelocity();

				if(attackTimer.GetTimeSinceStart() >= damageOverTime.time_seconds)
				{
					AttackSolarSystem(sun, true, sun.GetOuterSatellite()->GetOffsetFromSun());
					attackTimer.Reset();
				}
			}
			else
			{				
				glm::vec3 vectorToSatellite = 
					sun.GetOuterSatellite()->GetPosition() - position;

				velocity = glm::normalize(vectorToSatellite) * 0.07f; // TODO: Magick
			}
		}
		else
		{
			if(distanceBetweenPlanetAndSwarm <= sun.GetRadius() * sun.GetRadius())
			{
				isCommanded = false;

				velocity = glm::vec3();

				if(attackTimer.GetTimeSinceStart() >= damageOverTime.time_seconds)
				{
					AttackSolarSystem(sun);
					attackTimer.Reset();
				}
			}
			else
			{
				glm::vec3 direction = glm::vec3();
				if(sun.HasSatellites())
				{
					direction = sun.GetOuterSatellite()->GetPosition() - position;
				}
				else
				{
					direction = glm::vec3(sun.GetPosition()) - position;
				}

				if(!isCommanded)
				{
					direction = glm::normalize(direction);
					velocity = direction * 0.1f;
				}

				if(health <= 20.0f)
				{
					currentState = EVADE_STATE;
				}
		
				isCommanded = true;
			}
		}
	}
	else if(currentState == EVADE_STATE)
	{
		glm::vec3 vectorFromPlanetToSwarm = glm::vec3(sun.GetPosition()) - position;

		vectorFromPlanetToSwarm = glm::normalize(vectorFromPlanetToSwarm);
		glm::vec3 checkVelocity = glm::normalize(velocity);

		if(fabs(glm::length(vectorFromPlanetToSwarm - checkVelocity)) <= 0.0001f)
		{
			velocity *= -1.0f;
		}
	}
	else if(currentState == IDLE_STATE)
	{
		glm::vec3 vectorFromPlanetToSwarm = glm::vec3();
		if(sun.HasSatellites())
		{
			vectorFromPlanetToSwarm = sun.GetOuterSatellite()->GetPosition() - position;
		}
		else vectorFromPlanetToSwarm = glm::vec3(sun.GetPosition()) - position;

		float distanceBetweenPlanetAndSwarm = glm::length(vectorFromPlanetToSwarm);

		if(distanceBetweenPlanetAndSwarm < lineOfSight)
		{
			currentState = ATTACK_STATE;
		}
	}
}

void Swarm::Update(bool isSunKilled, CelestialBody &sun)
{
	if(!isDestroyed)
	{
		attackTimer.Update();

		if(!isSunKilled)
		{
			position += velocity;

			this->UpdateAI(sun);
			swarmBody.Update(velocity);

			if(health <= 20)
			{
				currentState = EVADE_STATE;
			}

			if(health <= 0)
			{
				isDestroyed = true;
			}
		}
		else
		{
			currentState = IDLE_STATE;
		}
	}
	//position += direction * speed;
}

void Swarm::Render(glutil::MatrixStack &modelMatrix,
				   glm::vec3 cameraPosition,
				   const BillboardProgDataNoTexture &billboardProgramNoTexture)
{
	if(!isDestroyed)
	{
		swarmBody.Render(modelMatrix, cameraPosition, billboardProgramNoTexture);
	}
}

void Swarm::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ATTACKED:
		break;
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("what_event").varString, "skilldeployed") == 0)
		{
			health -= _event.GetArgument("skillDamage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "timeended") == 0)
		{
			health -= _event.GetArgument("damage").varInteger;
		}
		break;
	}
}



FastSuicideBomber::FastSuicideBomber(glm::vec3 newPosition, glm::vec3 newVelocity,
									 int newHealth, int newDamage, float newLineOfSight,
									 float newScaleFactor)
{
	position = newPosition;
	velocity = newVelocity;

	health = newHealth;
	damage = newDamage;

	lineOfSight = newLineOfSight;

	scaleFactor = newScaleFactor;

	currentState = IDLE_STATE;
	generatedEvents.resize(0);
	isDestroyed = false;
}

void FastSuicideBomber::LoadMesh(const std::string &meshFile)
{
	try
	{
		mesh = std::auto_ptr<Framework::Mesh>(new Framework::Mesh(meshFile));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}

	GenerateUniformBuffers(materialBlockSize,
						   glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
						   materialUniformBuffer);
}

void FastSuicideBomber::AttackSolarSystem(CelestialBody &sun, bool isSatellite, float bodyIndex)
{
	EventArg damageEventArgs[2];
	damageEventArgs[0].argType = "damage";
	damageEventArgs[0].argument.varType = TYPE_INTEGER;
	damageEventArgs[0].argument.varInteger = damage;
	damageEventArgs[1].argType = "bodyIndex";
	if(isSatellite)
	{
		damageEventArgs[1].argument.varType = TYPE_FLOAT;
		damageEventArgs[1].argument.varFloat = bodyIndex;
	}
	else
	{
		damageEventArgs[1].argument.varType = TYPE_INTEGER;
		damageEventArgs[1].argument.varInteger = -1;
	}
	Event damageEvent(2, EVENT_TYPE_ATTACKED, damageEventArgs);

	sun.OnEvent(damageEvent);



	EventArg explodeEventArgs[2];
	explodeEventArgs[0].argType = "what_event";
	explodeEventArgs[0].argument.varType = TYPE_STRING;
	strcpy(explodeEventArgs[0].argument.varString, "explStarted");
	explodeEventArgs[1].argType = "expl_index";
	explodeEventArgs[1].argument.varType = TYPE_INTEGER;
	explodeEventArgs[1].argument.varInteger = 0;

	Event explodeEvent(2, EVENT_TYPE_OTHER, explodeEventArgs);

	generatedEvents.push_back(explodeEvent);
	//scene.OnEvent(explodeEvent);
}

void FastSuicideBomber::UpdateAI(CelestialBody &sun)
{
	if(currentState == ATTACK_STATE)
	{
		if(sun.HasSatellites())
		{
			glm::vec3 vectorBetweenSatelliteAndBomber = 
				sun.GetOuterSatellite()->GetPosition() - position;
			float distanceBetweenSatelliteAndBomber = glm::length(vectorBetweenSatelliteAndBomber);

			if(distanceBetweenSatelliteAndBomber <=
			   sun.GetOuterSatellite()->GetDiameter() * 
			   sun.GetOuterSatellite()->GetDiameter())
			{
				AttackSolarSystem(sun, true, sun.GetOuterSatellite()->GetOffsetFromSun());
			}
			else
			{
				glm::vec3 vectorToSatellite =
					sun.GetOuterSatellite()->GetPosition() - position;

				velocity = glm::normalize(vectorToSatellite) * 0.003f; // TODO: Magick
			}
		}
		else
		{
			glm::vec3 vectorBetweenSunAndBomber = glm::vec3(sun.GetPosition()) - position;
			float distanceBetweenSunAndBomber = glm::length(vectorBetweenSunAndBomber);

			if(distanceBetweenSunAndBomber <= sun.GetRadius() * sun.GetRadius())
			{
				AttackSolarSystem(sun);
				isDestroyed = true;
				// Destroy yourself
			}
			else
			{
				glm::vec3 direction = glm::vec3();
				direction = glm::vec3(sun.GetPosition()) - position;

				direction = glm::normalize(direction);
				velocity = direction * 0.03f;
			}
		}
	}
	else if(currentState == IDLE_STATE)
	{
		glm::vec3 vectorFromPlanetToBomber = glm::vec3();
		if(sun.HasSatellites())
		{
			vectorFromPlanetToBomber = sun.GetOuterSatellite()->GetPosition() - position;
		}
		else
		{
			vectorFromPlanetToBomber = glm::vec3(sun.GetPosition()) - position;
		}

		float distanceBetweenPlanetAndBomber = glm::length(vectorFromPlanetToBomber);

		if(distanceBetweenPlanetAndBomber < lineOfSight)
		{
			currentState = ATTACK_STATE;
		}
	}
}

void FastSuicideBomber::Update(bool isSunKilled, CelestialBody &sun)
{
	if(!isDestroyed)
	{
		if(!isSunKilled)
		{
			position += velocity;
			this->UpdateAI(sun);

			if(health <= 0)
			{
				isDestroyed = true;
				// Destroy yourself
			}
		}
		else
		{
			currentState = IDLE_STATE;
		}
	}
}

void FastSuicideBomber::Render(glutil::MatrixStack &modelMatrix,
							   const LitProgData &litData,
							   int materialBlockIndex,
							   float interpolation, float gamma)
{
	if(!isDestroyed)
	{
		glutil::PushStack push(modelMatrix);

		glm::vec3 viewPosition = position + velocity * interpolation;
		modelMatrix.Translate(viewPosition);
		modelMatrix.Scale(scaleFactor);

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
}

std::vector<Event> FastSuicideBomber::GetGeneratedEvents()
{
	std::vector<Event> eventsToReturn;

	if(generatedEvents.size() > 0)
	{
		eventsToReturn = generatedEvents;
		generatedEvents.resize(0);
	}
	else 
	{
		eventsToReturn.push_back(StockEvents::EmptyEvent());
	}

	return eventsToReturn;
}

void FastSuicideBomber::OnEvent(std::shared_ptr<Skill> sender, Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_SHOT_FIRED:
		break;
	case EVENT_TYPE_ATTACKED:
		break;
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("what_event").varString, "skilldeployed") == 0)
		{
			// TODO: the position must be relative to the object emitting the skill!!!
			if(glm::length(position - glm::vec3(sender->GetOwner()->GetPosition())) 
			   < 
			   _event.GetArgument("skillrange").varFloat)
			{
				health -= _event.GetArgument("skilldamage").varInteger;
			}
		}
		break;
	}
}