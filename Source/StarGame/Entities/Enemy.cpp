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
#include "../framework/ErrorAPI.h"

#include <ctime>


inline glm::vec3 Enemy::GetPosition()
{
	return position;
}
inline int Enemy::GetResourceGivenOnKill()
{
	return resourceGivenOnKill;
}
inline bool Enemy::IsDestroyed()
{
	return isDestroyed;
}
inline bool Enemy::IsSceneUpdated()
{
	return isSceneUpdated;
}

Event Enemy::GetGeneratedEvent(const std::string &eventName)
{
	if(generatedEvents.size() > 0)
	{
		for(int i = 0; i < generatedEvents.size(); i++)
		{
			if(generatedEvents[i].GetType() == EVENT_TYPE_OTHER &&
			   strcmp(generatedEvents[i].GetArgument("what_event").varString, eventName.c_str()) == 0)
			{
				return generatedEvents[i];
			}
		}
	}

	return StockEvents::EmptyEvent();
}
void Enemy::RemoveGeneratedEvent(const std::string &eventName)
{
	if(generatedEvents.size() > 0)
	{
		for(std::vector<Event>::iterator iter = generatedEvents.begin();
			iter != generatedEvents.end();)
		{
			if(strcmp(iter->GetArgument("what_event").varString, eventName.c_str()) == 0)
			{
				generatedEvents.erase(iter);
				break;
			}
			else 
			{
				++iter;
			}
		}
	}
}
std::vector<Event> Enemy::GetGeneratedEvents()
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

/*
void Enemy::StartEmittingPain()
{
	if(!isStartedEmittingPain)
	{
		painEmitter.Init();
		painEmitter.Activate();
		isStartedEmittingPain = true;
	}
}
void Enemy::StopEmittingPain()
{
	if(isStartedEmittingPain)
	{
		painEmitter.Init();
		isStartedEmittingPain = false;
	}
}
*/

EnemyRank Enemy::GetRank()
{
	return rank;
}


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


Swarm::Swarm(int newSwarmersCount, 
			 float newTime_seconds, int newDamage,
			 const BillboardProgDataNoTexture &billboardProgDataNoTexture,
			 glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor, glm::vec4 newPainEmitColor,
			 glm::vec3 newPosition, glm::vec3 newFrontVector,
			 float newSpeed, float newLineOfSight,
			 int newHealth, int newResourceGivenOnKill)
			 : Enemy(newInitialColor, newOnFreezeColor, newPainEmitColor, newPosition, newFrontVector, newSpeed, newLineOfSight,
					 newHealth, newResourceGivenOnKill)
{
	swarmersCount = newSwarmersCount;
	damage.damage = newDamage;
	damage.time_seconds = newTime_seconds;
	swarmBody = SwarmEmitter(position, initialColor, swarmersCount);
	swarmBody.Init(billboardProgDataNoTexture);
	isCommanded = false;

	attackTimer = Framework::Timer(Framework::Timer::Type::TT_INFINITE);

	rank = RANK_ONE;
}

void Swarm::AttackSolarSystem(CelestialBody &sun, bool isSatellite, float bodyIndex)
{
	EventArg sunDamage_perTime[2];
	sunDamage_perTime[0].argType = "damage";
	sunDamage_perTime[0].argument.varType = TYPE_INTEGER;
	sunDamage_perTime[0].argument.varInteger = damage.damage;
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
	if(currentState == STATE_ATTACK)
	{
		glm::vec3 vectorBetweenPlanetAndSwarm = glm::vec3();
		if(sun.HasSatellites())
		{
			vectorBetweenPlanetAndSwarm = sun.GetOuterSatellite()->GetPosition() - position;
			frontVector = glm::normalize(vectorBetweenPlanetAndSwarm);

			float satelliteRadius = sun.GetOuterSatellite()->GetRadius();

			float distanceBetweenPlanetAndSwarm = glm::length(vectorBetweenPlanetAndSwarm);
			if(distanceBetweenPlanetAndSwarm <= satelliteRadius * satelliteRadius)
			{
				if(attackTimer.GetTimeSinceStart() >= damage.time_seconds)
				{
					AttackSolarSystem(sun, true, sun.GetOuterSatellite()->GetOffsetFromSun());
					attackTimer.Reset();
				}
			}   
		}
		else
		{
			vectorBetweenPlanetAndSwarm = sun.GetPosition() - position;
			frontVector = glm::normalize(vectorBetweenPlanetAndSwarm);

			float sunRadius = sun.GetRadius();

			float distanceBetweenPlanetAndSwarm = glm::length(vectorBetweenPlanetAndSwarm);
			if(distanceBetweenPlanetAndSwarm <= sunRadius * sunRadius)
			{
				if(attackTimer.GetTimeSinceStart() >= damage.time_seconds)
				{
					AttackSolarSystem(sun);
					attackTimer.Reset();
				}
			}
		}
	}
	else if(currentState == STATE_EVADE)
	{
		glm::vec3 vectorFromPlanetToSwarm = sun.GetPosition() - position;
		
		vectorFromPlanetToSwarm = glm::normalize(vectorFromPlanetToSwarm);
		glm::vec3 swarmDirection = glm::normalize(frontVector * speed);

		if(glm::dot(vectorFromPlanetToSwarm, swarmDirection) > 0)
		{
			speed *= -1.0f;
		}
	}
	else if(currentState == STATE_IDLE)
	{
		glm::vec3 vectorFromPlanetToSwarm = sun.GetPosition() - position;

		float distanceBetweenPlanetAndSwarm = glm::length(vectorFromPlanetToSwarm);

		if(distanceBetweenPlanetAndSwarm < lineOfSight)
		{
			currentState = STATE_ATTACK;
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
			if(currentState != STATE_STOPPED)
			{
				position += frontVector * speed;
			
				this->UpdateAI(sun);
				swarmBody.Update(frontVector * speed);
			

				if(health <= 20)
				{
					currentState = STATE_EVADE;
				}
			}

			if(health <= 0)
			{
				isDestroyed = true;
			}
		}
		else
		{
			currentState = STATE_IDLE;
			UpdateAI(CelestialBody());
		}
		/*
		if(isStartedEmittingPain)
		{
			painEmitter.SetPosition(position);
			painEmitter.Update();
		}
		*/
	}
}

void Swarm::UpdateTimersOnIdle()
{
	attackTimer.Update();
}

void Swarm::Render(glutil::MatrixStack &modelMatrix,
				   glm::vec3 cameraPosition,
				   const BillboardProgDataNoTexture &billboardProgramNoTexture,
				   const SpriteParticleProgData &spriteParticleData)
{
	if(!isDestroyed)
	{
		swarmBody.Render(modelMatrix, cameraPosition, billboardProgramNoTexture);
		/*
		if(isStartedEmittingPain)
		{
			painEmitter.Render(modelMatrix, spriteParticleData);
		}
		*/
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
		if(strcmp(_event.GetArgument("what_event").varString, "stunskilldeployed") == 0)
		{
			// WARN: It should be 'currentState'.
			lastState = STATE_ATTACK;
			currentState = STATE_STOPPED;
			swarmBody.SetColor(onFreezeColor);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stuntimeended") == 0)
		{
			currentState = lastState;
			swarmBody.SetColor(initialColor);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "shieldskilldeployed") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "destroy") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "startPain") == 0)
		{
			swarmBody.SetColor(painEmitColor);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stopPain") == 0)
		{
			swarmBody.SetColor(initialColor);
		}
		break;
	default:
		HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
		break;
	}
}

void Swarm::StopAllTimers()
{
	attackTimer.SetPause(true);
}
void Swarm::StartAllTimers()
{
	attackTimer.SetPause(false);
}


Spaceship::Spaceship(float newProjectileSpeed, int newProjectileLifeSpan,
					 int newProjectileDamage,
					 glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor, glm::vec4 newPainEmitColor,
					 glm::vec3 newPosition, glm::vec3 newFrontVector,
					 float newSpeed, float newLineOfSight,
					 int newHealth, int newResourceGivenOnKill)
					 : Enemy(newInitialColor, newOnFreezeColor, newPainEmitColor, newPosition, newFrontVector, newSpeed, newLineOfSight,
							 newHealth, newResourceGivenOnKill)
{
	projectileSpeed = newProjectileSpeed;
	projectileLifeSpan = newProjectileLifeSpan;
	projectileDamage = newProjectileDamage;
	currentState = STATE_PATROL;

	projectile = std::unique_ptr<Projectile>(new Projectile(position, 
															frontVector * projectileSpeed, 
															projectileDamage, 
															projectileLifeSpan));

	// TODO: If there is only one patrol route point, the game crashes. Maybe
	//		 sth connected with the vector range.

	patrolRoute.patrolPoints.push_back(position);
	patrolRoute.patrolPoints.push_back(glm::vec3());

	//patrolRoute.patrolPoints.push_back(glm::vec3(-4.5f, 0.0f, 4.0f));
	//patrolRoute.patrolPoints.push_back(glm::vec3(-4.5f, 0.0f, -4.0f));
	//patrolRoute.patrolPoints.push_back(glm::vec3(4.5f, 0.0f, -4.0f));

	patrolRoute.currentPatrolPointIndex = 0;
	patrolRoute.lastPatrolPointIndex = 0;
	patrolRoute.nextPatrolPointIndex = 1;

	frontVector = 
		glm::normalize(patrolRoute.patrolPoints[patrolRoute.nextPatrolPointIndex] - 
					   patrolRoute.patrolPoints[patrolRoute.currentPatrolPointIndex]); // 0.1f - speed
	patrolRoute.lastVelocity = frontVector;

	rank = RANK_ONE;
}

void Spaceship::LoadProjectileMesh(const std::string &meshFile)
{
	projectile->LoadMesh(meshFile);
}

void Spaceship::LoadMesh(const std::string &meshFile)
{
	if(!mesh.LoadMesh(meshFile)) // TODO: Proper error handling
	{
		std::string errorMessage = "cannot load mesh ";
		errorMessage += meshFile;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}
	mesh.LoadLight();
}

void Spaceship::UpdateAI(CelestialBody &sun)
{
	if(currentState == STATE_ATTACK)
	{	
		frontVector = glm::vec3();

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
				newDirection = sun.GetPosition() - position;
			}

			newDirection = glm::normalize(newDirection);
			projectile->Recreate(position, 
								 newDirection * projectileSpeed, 
								 projectileLifeSpan, projectileDamage);
		}

		projectile->Update(sun);
	}
	else if(currentState == STATE_EVADE)
	{
		glm::vec3 vectorFromPlanetToSpaceship = sun.GetPosition() - position;

		vectorFromPlanetToSpaceship = glm::normalize(vectorFromPlanetToSpaceship);
		glm::vec3 spaceshipDirection = glm::normalize(frontVector * speed);
		
		if(glm::dot(vectorFromPlanetToSpaceship, spaceshipDirection) > 0)
		{
			speed *= -1.0f;
		}
	}
	else if(currentState == STATE_PATROL)
	{		
		// Updates the patrol points.
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
				// If the end of the patrol is reached, we should restart.
				if(patrolRoute.nextPatrolPointIndex >= patrolRoute.patrolPoints.size())
				{
					patrolRoute.nextPatrolPointIndex = 0;
				}

				glm::vec3 vectorBetweenPatrolPoints = 
					patrolRoute.patrolPoints[patrolRoute.nextPatrolPointIndex] -
					patrolRoute.patrolPoints[patrolRoute.currentPatrolPointIndex];

				frontVector = glm::normalize(vectorBetweenPatrolPoints);
			}
		}
				
		glm::vec3 vectorFromSunToSpaceship = sun.GetPosition() - position;
		float distanceBetweenSunAndSpaceship = glm::length(vectorFromSunToSpaceship);

		if(distanceBetweenSunAndSpaceship < lineOfSight)
		{
			patrolRoute.lastVelocity = frontVector;
			patrolRoute.nextPatrolPoint = sun.GetPosition();
			currentState = STATE_ATTACK;
		}
		else
		{
			patrolRoute.nextPatrolPoint = patrolRoute.patrolPoints[patrolRoute.nextPatrolPointIndex];
		}
	}
}

void Spaceship::Update(bool isSunKilled, CelestialBody &sun)
{
	if(!isSunKilled)
	{
		if(currentState != STATE_STOPPED)
		{
			position += frontVector * speed;
			UpdateAI(sun);
		}
				
		if(health <= 20)
		{
			currentState = STATE_EVADE;
		}

		if(health <= 0)
		{
			isDestroyed = true;
		}
	}
	else
	{
		if(frontVector == glm::vec3())
		{
			frontVector = patrolRoute.lastVelocity;
		}
		currentState = STATE_PATROL;
		UpdateAI(CelestialBody());
	}
	/*
	if(isStartedEmittingPain)
	{
		painEmitter.SetPosition(position);
		//painEmitter.Init();
		painEmitter.Update();
	}
	*/
}

void Spaceship::Render(glutil::MatrixStack &modelMatrix,
					   int materialBlockIndex, float gamma,
					   const LitProgData &litData, const LitTextureProgData &litTextureData,
					   const SpriteParticleProgData &spriteParticleData,
					   float interpolation)
{
	{
		glutil::PushStack push(modelMatrix);


		float rotation = glm::degrees(atan2f(frontVector.x, frontVector.z));

		modelMatrix.Translate(position);
		/*
		if(isStartedEmittingPain)
		{
			painEmitter.Render(modelMatrix, spriteParticleData);
		}
		*/
		modelMatrix.RotateY(rotation);
		modelMatrix.Scale(0.05f);

		mesh.Render(modelMatrix, litTextureData, materialBlockIndex);

		glUseProgram(0);
	}
	
	if(!projectile->IsDestroyed() && currentState == STATE_ATTACK)
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
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("what_event").varString, "skilldeployed") == 0)
		{
			health -= _event.GetArgument("skillDamage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "timeended") == 0)
		{
			health -= _event.GetArgument("damage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stunskilldeployed") == 0)
		{
			// WARN: It should be 'currentState'.
			lastState = STATE_ATTACK;
			currentState = STATE_STOPPED;
			mesh.LoadLight(onFreezeColor);
			//GenerateUniformBuffers(materialBlockSize, onFreezeColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stuntimeended") == 0)
		{
			currentState = lastState;
			mesh.LoadLight();
			//GenerateUniformBuffers(materialBlockSize, initialColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "shieldskilldeployed") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "destroy") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "startPain") == 0)
		{
			mesh.LoadLight(painEmitColor);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stopPain") == 0)
		{
			mesh.LoadLight();
		}
		break;
	default:
		HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
		break;
	}
}


DeployUnit::DeployUnit(float newProjectileSpeed, int newProjectileLifeSpan,
					   int newProjectileDamage,
					   glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor, glm::vec4 newPainEmitColor,
					   glm::vec3 newPosition, glm::vec3 newFrontVector,
					   float newSpeed, float newLineOfSight,
					   int newHealth, int newResourceGivenOnKill)
					   : Enemy(newInitialColor, newOnFreezeColor, newPainEmitColor, newPosition, newFrontVector, newSpeed, newLineOfSight,
							   newHealth, newResourceGivenOnKill)
{
	projectileSpeed = newProjectileSpeed;
	projectileLifeSpan = newProjectileLifeSpan;
	projectileDamage = newProjectileDamage;

	projectile = std::unique_ptr<Projectile>(new Projectile(position,
															frontVector * projectileSpeed,
															projectileDamage, 
															projectileLifeSpan));

	isSceneUpdated = false;
	isForRejuvenation = false;
}

void DeployUnit::LoadProjectileMesh(const std::string &meshFile)
{
	projectile->LoadMesh(meshFile);
}

void DeployUnit::LoadMesh(const std::string &meshFile)
{
	// TODO: Change mesh
	// TODO: Error checking
	/*
	try
	{
		mesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(meshFile));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}

	GenerateUniformBuffers(materialBlockSize, initialColor, materialUniformBuffer);
	
	projectile->LoadMesh(meshFile); // TODO: maybe this should be removed*/

	if(!mesh.LoadMesh(meshFile)) // TODO: Proper error handling
	{
		std::string errorMessage = "cannot load mesh ";
		errorMessage += meshFile;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}
	mesh.LoadLight();
}

void DeployUnit::UpdateAI(CelestialBody &sun)
{
	if(currentState == STATE_ATTACK)
	{
		frontVector = glm::vec3();

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
				newDirection = sun.GetPosition() - position;
			}

			newDirection = glm::normalize(newDirection);
			projectile->Recreate(position, newDirection * projectileSpeed, projectileLifeSpan, projectileDamage);
		}

		projectile->Update(sun);
	}
	// WARN: Should they evade?
	else if(currentState == STATE_EVADE)
	{
		glm::vec3 vectorFromPlanetToUnit = sun.GetPosition() - position;

		vectorFromPlanetToUnit = glm::normalize(vectorFromPlanetToUnit);
		glm::vec3 unitDirection = glm::normalize(frontVector * speed);
		
		if(glm::dot(vectorFromPlanetToUnit, unitDirection) > 0)
		{
			speed *= -1.0f;
		}
	}
	else if(currentState == STATE_IDLE)
	{
		glm::vec3 vectorFromPlanetToUnit = sun.GetPosition() - position;

		float distanceBetweenPlanetAndUnit = glm::length(vectorFromPlanetToUnit);

		if(distanceBetweenPlanetAndUnit < lineOfSight)
		{
			lastFrontVector = frontVector;
			projectile->Recreate(position, frontVector * projectileSpeed, projectileLifeSpan, projectileDamage);
			currentState = STATE_ATTACK;
		}
	}
}

void DeployUnit::Update(bool isSunKilled, CelestialBody &sun)
{
	if(!isForRejuvenation && !isDestroyed)
	{
		if(!isSunKilled)
		{
			if(currentState != STATE_STOPPED)
			{
				position += frontVector * speed;
				UpdateAI(sun);
			}
			/*	
			if(health <= 20)
			{
				currentState = STATE_EVADE;
			}*/

			if(health <= 0)
			{
				isForRejuvenation = true;
			}
		}
		else
		{
			frontVector = lastFrontVector;
			position += frontVector * speed;
			currentState = STATE_IDLE;
		}
		/*
		if(isStartedEmittingPain)
		{
			painEmitter.SetPosition(position);
			painEmitter.Update();
		}
		*/
	}
}

void DeployUnit::Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
					   float gamma, const LitTextureProgData &litTextureData, const LitProgData &litData,
					   const SpriteParticleProgData &spriteParticleData,
					   float interpolation)
{
	if(!isForRejuvenation && !isDestroyed)
	{
		{
			glutil::PushStack push(modelMatrix);

			float rotation = glm::degrees(atan2f(frontVector.x, frontVector.z));

			modelMatrix.Translate(position);
			/*
			if(isStartedEmittingPain)
			{
				painEmitter.Render(modelMatrix, spriteParticleData);
			}
			*/

			modelMatrix.RotateY(rotation);
			modelMatrix.Scale(0.1f);

			mesh.Render(modelMatrix, litTextureData, materialBlockIndex);
			/*
			glBindBufferRange(GL_UNIFORM_BUFFER, materialBlockIndex, materialUniformBuffer,
							  0, sizeof(MaterialBlock));

			glm::mat3 normMatrix(modelMatrix.Top());
			normMatrix = glm::transpose(glm::inverse(normMatrix));

			glUseProgram(litData.theProgram);
			glUniformMatrix4fv(litData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
			glUniformMatrix3fv(litData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));

			mesh->Render("lit");

			glUseProgram(0);

			glBindBufferBase(GL_UNIFORM_BUFFER, materialBlockIndex, 0);*/
		}

		if(!projectile->IsDestroyed() && currentState == STATE_ATTACK)
		{
			projectile->Render(modelMatrix, materialBlockIndex, gamma, litData,
							   interpolation);
		}
	}
}

void DeployUnit::OnEvent(Event &_event)
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
			health -= _event.GetArgument("skillDamage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "timeended") == 0)
		{
			health -= _event.GetArgument("damage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stunskilldeployed") == 0)
		{
			// WARN: It should be 'currentState'.
			lastState = STATE_ATTACK;
			currentState = STATE_STOPPED;
			GenerateUniformBuffers(materialBlockSize, onFreezeColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stuntimeended") == 0)
		{
			currentState = lastState;
			GenerateUniformBuffers(materialBlockSize, initialColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "shieldskilldeployed") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "destroy") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "startPain") == 0)
		{
			GenerateUniformBuffers(materialBlockSize, painEmitColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stopPain") == 0)
		{
			GenerateUniformBuffers(materialBlockSize, initialColor, materialUniformBuffer);
		}
		break;
	default:
		HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
		break;
	}
}

void DeployUnit::Destroy()
{
	isDestroyed = true;
}
bool DeployUnit::IsForRejuvenation()
{
	return isForRejuvenation;
}
void DeployUnit::SetPosition(glm::vec3 newPosition)
{
	position = newPosition;
}
void DeployUnit::Rejuvenate(const glm::vec3 &newPosition, int newHealth,
						    glm::vec3 newFrontVector, float newSpeed)
{
	position = newPosition;
	isForRejuvenation = false;
	health = newHealth;
	frontVector = newFrontVector;
	speed = newSpeed;
	currentState = STATE_IDLE;
}


Mothership::Mothership(glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor, glm::vec4 newPainEmitColor,
					   glm::vec3 newPosition, glm::vec3 newFrontVector,
					   float newSpeed, float newLineOfSight,
					   int newHealth, int newResourceGivenOnKill)
					   : Enemy(newInitialColor, newOnFreezeColor, newPainEmitColor, newPosition, newFrontVector, newSpeed, newLineOfSight,
							   newHealth, newResourceGivenOnKill)
{
	isDeploying = false;
	rank = RANK_TWO;
}

void Mothership::LoadMesh(const std::string &meshFileName)
{
	if(!mesh.LoadMesh(meshFileName))
	{
		std::string errorMessage = "cannot load mesh ";
		errorMessage += meshFileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}
	mesh.LoadLight();
}

void Mothership::InitDeployUnits(const std::string &meshFileName, int deployUnitsCount,
								 float newProjectileSpeed, int newProjectileLifeSpan, int newProjectileDamage,
								 glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor, glm::vec4 newPainEmitColor,
								 float newSpeed, float newLineOfSight, int newHealth, int newResourceGivenOnKill)
{
	deployUnitsInfo.deployUnitsCount = deployUnitsCount;
	deployUnitsInfo.health = newHealth;
	deployUnitsInfo.initialColor = newInitialColor;
	deployUnitsInfo.lineOfSight = newLineOfSight;
	deployUnitsInfo.meshFileName = meshFileName;
	deployUnitsInfo.onFreezeColor = newOnFreezeColor;
	deployUnitsInfo.painEmitColor = newPainEmitColor;
	deployUnitsInfo.projectileDamage = newProjectileDamage;
	deployUnitsInfo.projectileLifeSpan = newProjectileLifeSpan;
	deployUnitsInfo.projectileSpeed = newProjectileSpeed;
	deployUnitsInfo.speed = newSpeed;
	deployUnitsInfo.resourceGivenOnKill = newResourceGivenOnKill;

	float rotationDegs = 30.0f;
	float decrement = (2 * rotationDegs) / (float)deployUnitsInfo.deployUnitsCount;

	for(int i = 0; i < deployUnitsInfo.deployUnitsCount; i++)
	{
		glutil::MatrixStack rotMatrix;
		rotMatrix.SetIdentity();
		rotMatrix.RotateY(rotationDegs);
			
		glm::vec4 shipFrontVector = glm::vec4(frontVector, 0.0f);
		shipFrontVector = shipFrontVector * rotMatrix.Top();
		rotationDegs -= decrement;

		std::shared_ptr<DeployUnit> newDeployUnit = 
			std::shared_ptr<DeployUnit>(new DeployUnit(deployUnitsInfo.projectileSpeed, deployUnitsInfo.projectileLifeSpan, 
													   deployUnitsInfo.projectileDamage, 
													   deployUnitsInfo.initialColor, deployUnitsInfo.onFreezeColor, 
													   deployUnitsInfo.painEmitColor, position,
													   glm::vec3(shipFrontVector), deployUnitsInfo.speed, deployUnitsInfo.lineOfSight, 
													   deployUnitsInfo.health, deployUnitsInfo.resourceGivenOnKill));
		newDeployUnit->LoadMesh(deployUnitsInfo.meshFileName);
		newDeployUnit->LoadProjectileMesh("mesh-files/UnitSphere.xml"); // WARN: Not data driven

		deployUnits.push_back(newDeployUnit);
	}
}

void Mothership::RejuvenateDeployUnits()
{
	float rotationDegs = 30.0f;
	float decrement = (2 * rotationDegs) / (float)deployUnitsInfo.deployUnitsCount;

	for(int i = 0; i < deployUnitsInfo.deployUnitsCount; i++)
	{
		glutil::MatrixStack rotMatrix;
		rotMatrix.SetIdentity();
		rotMatrix.RotateY(rotationDegs);
			
		glm::vec4 shipFrontVector = glm::vec4(frontVector, 0.0f);
		shipFrontVector = shipFrontVector * rotMatrix.Top();
		rotationDegs -= decrement;

		deployUnits[i]->Rejuvenate(position, deployUnitsInfo.health, 
								   glm::vec3(shipFrontVector), deployUnitsInfo.speed);
	}

	isDeploying = true;
}

void Mothership::UpdateAI(CelestialBody &sun)
{
	if(currentState == STATE_ATTACK && isDeploying == false)
	{
		for(int i = 0; i < deployUnits.size(); i++)
		{
			deployUnits[i]->SetPosition(position);
		}

		isDeploying = true;
		speed = 0.0f;
	}
	else if(currentState == STATE_EVADE)
	{
		glm::vec3 vectorFromPlanetToSpaceship = sun.GetPosition() - position;

		vectorFromPlanetToSpaceship = glm::normalize(vectorFromPlanetToSpaceship);
		glm::vec3 spaceshipDirection = glm::normalize(frontVector * speed);
		
		if(glm::dot(vectorFromPlanetToSpaceship, spaceshipDirection) > 0)
		{
			speed *= -1.0f;
		}
	}
	else if(currentState == STATE_IDLE)
	{
		glm::vec3 vectorFromPlanetToMothership = sun.GetPosition() - position;

		float distanceBetweenPlanetAndMothership = glm::length(vectorFromPlanetToMothership);

		if(distanceBetweenPlanetAndMothership < lineOfSight)
		{
			currentState = STATE_ATTACK;
		}
	}
}

void Mothership::Update(bool isSunKilled, CelestialBody &sun)
{
	if(!isSunKilled)
	{
		if(currentState != STATE_STOPPED)
		{
			position += frontVector * speed;
			UpdateAI(sun);

			if(isDeploying)
			{
				bool areAllForRejuvenation = true;
				for(int i = 0; i < deployUnits.size(); i++)
				{
					deployUnits[i]->Update(isSunKilled, sun);
					if(deployUnits[i]->IsForRejuvenation() == false)
					{
						areAllForRejuvenation = false;
					}
				}
				if(areAllForRejuvenation)
				{
					RejuvenateDeployUnits();
				}
			}
		}

		if(health <= 20)
		{
			currentState = STATE_EVADE;
		}

		if(health <= 0)
		{
			for(int i = 0; i < deployUnits.size(); i++)
			{
				deployUnits[i]->Destroy();
			}

			isDestroyed = true;
		}
	}
	else
	{
		currentState = STATE_IDLE;
		UpdateAI(CelestialBody());
		for(int i = 0; i < deployUnits.size(); i++)
		{
			deployUnits[i]->Update(isSunKilled);
		}
	}
	/*
	if(isStartedEmittingPain)
	{
		painEmitter.SetPosition(position);
		painEmitter.Update();
	}
	*/
}

void Mothership::Render(glutil::MatrixStack &modelMatrix,
					    int materialBlockIndex, float gamma,
					    const LitProgData &litData, const LitTextureProgData &litTextureData,
						const SpriteParticleProgData &spriteParticleData,
					    float interpolation)
{
	{
		glutil::PushStack push(modelMatrix);


		float rotation = glm::degrees(atan2f(frontVector.x, frontVector.z));

		modelMatrix.Translate(position);
		/*
		if(isStartedEmittingPain)
		{
			painEmitter.Render(modelMatrix, spriteParticleData);
		}
		*/
		modelMatrix.RotateY(rotation);
		modelMatrix.Scale(0.15f);

		mesh.Render(modelMatrix, litTextureData, materialBlockIndex);

		glUseProgram(0);
	}

	if(isDeploying)
	{
		for(int i = 0; i < deployUnits.size(); i++)
		{
			deployUnits[i]->Render(modelMatrix, materialBlockIndex, gamma, litTextureData, litData,
				spriteParticleData, interpolation);
		}
	}
}

void Mothership::OnEvent(Event &_event)
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
			health -= _event.GetArgument("skillDamage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "timeended") == 0)
		{
			health -= _event.GetArgument("damage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stunskilldeployed") == 0)
		{
			// WARN: It should be 'currentState'.
			lastState = STATE_ATTACK;
			currentState = STATE_STOPPED;
			mesh.LoadLight(onFreezeColor);
			//GenerateUniformBuffers(materialBlockSize, onFreezeColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stuntimeended") == 0)
		{
			currentState = lastState;
			mesh.LoadLight();
			//GenerateUniformBuffers(materialBlockSize, initialColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "shieldskilldeployed") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "destroy") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "startPain") == 0)
		{
			mesh.LoadLight(painEmitColor);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stopPain") == 0)
		{
			mesh.LoadLight();
		}
		break;
	default:
		HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
		break;
	}
}

std::vector<std::shared_ptr<DeployUnit>> Mothership::GetDeployUnits() const
{
	return deployUnits;
}


FastSuicideBomber::FastSuicideBomber(int newDamage, float newChargeSpeed, 
									 glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor, glm::vec4 newPainEmitColor,
									 glm::vec3 newPosition, glm::vec3 newFrontVector,
									 float newSpeed, float newLineOfSight,
									 int newHealth, int newResourceGivenOnKill)
									 : Enemy(newInitialColor, newOnFreezeColor, newPainEmitColor, newPosition, newFrontVector, newSpeed, 
											 newLineOfSight, newHealth, newResourceGivenOnKill)
{
	damage = newDamage;
	chargeSpeed = newChargeSpeed;
	rank = RANK_ONE;
}

void FastSuicideBomber::LoadMesh(const std::string &meshFile)
{
	if(!mesh.LoadMesh(meshFile))
	{
		std::string errorMessage = "cannot load mesh ";
		errorMessage += meshFile;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}
	mesh.LoadLight();
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
	
	isDestroyed = true;
}

void FastSuicideBomber::UpdateAI(CelestialBody &sun)
{
	if(currentState == STATE_ATTACK)
	{
		glm::vec3 vectorBetweenPlanetAndBomber = glm::vec3();
		if(sun.HasSatellites())
		{
			vectorBetweenPlanetAndBomber = sun.GetOuterSatellite()->GetPosition() - position;

			float satelliteRadius = sun.GetOuterSatellite()->GetRadius();

			float distanceBetweenPlanetAndBomber = glm::length(vectorBetweenPlanetAndBomber);
			if(distanceBetweenPlanetAndBomber <= satelliteRadius * satelliteRadius)
			{
				AttackSolarSystem(sun, true, sun.GetOuterSatellite()->GetOffsetFromSun());
			}
		}
		else
		{
			vectorBetweenPlanetAndBomber = sun.GetPosition() - position;

			float sunRadius = sun.GetRadius();

			float distanceBetweenPlanetAndBomber = glm::length(vectorBetweenPlanetAndBomber);
			if(distanceBetweenPlanetAndBomber <= sunRadius * sunRadius)
			{
				AttackSolarSystem(sun);
			}
		}

		frontVector = glm::normalize(vectorBetweenPlanetAndBomber);
		speed = chargeSpeed;
	}
	else if(currentState == STATE_EVADE)
	{
		glm::vec3 vectorFromPlanetToBomber = sun.GetPosition() - position;

		vectorFromPlanetToBomber = glm::normalize(vectorFromPlanetToBomber);
		glm::vec3 bomberDirection = glm::normalize(frontVector * speed);

		if(glm::dot(vectorFromPlanetToBomber, bomberDirection) > 0)
		{
			speed *= -1.0f;
		}
	}
	else if(currentState == STATE_IDLE)
	{
		glm::vec3 vectorFromPlanetToBomber = sun.GetPosition() - position;

		float distanceBetweenPlanetAndBomber = glm::length(vectorFromPlanetToBomber);

		if(distanceBetweenPlanetAndBomber < lineOfSight)
		{
			currentState = STATE_ATTACK;
		}
	}
}

void FastSuicideBomber::Update(bool isSunKilled, CelestialBody &sun)
{
	if(!isSunKilled)
	{
		if(currentState != STATE_STOPPED)
		{
			position += frontVector * speed;
			UpdateAI(sun);
		}

		if(health <= 20)
		{
			currentState = STATE_EVADE;
		}

		if(health <= 0)
		{
			isDestroyed = true;
		}
	}
	else
	{
		currentState = STATE_IDLE;
	}
	/*
	if(isStartedEmittingPain)
	{
		painEmitter.SetPosition(position);
		painEmitter.Update();
	}
	*/
}

void FastSuicideBomber::Render(glutil::MatrixStack &modelMatrix,
							   int materialBlockIndex, float gamma,
							   const LitProgData &litData, const LitTextureProgData &litTextureData,
							   const SpriteParticleProgData &spriteParticleData,
							   float interpolation)
{
	glutil::PushStack push(modelMatrix);

	
	modelMatrix.Translate(position);
	/*
	if(isStartedEmittingPain)
	{
		painEmitter.Render(modelMatrix, spriteParticleData);
	}
	*/
	modelMatrix.Scale(0.1f);
	
	mesh.Render(modelMatrix, litTextureData, materialBlockIndex);

	glUseProgram(0);
}

void FastSuicideBomber::OnEvent(Event &_event)
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
			health -= _event.GetArgument("skillDamage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "timeended") == 0)
		{
			health -= _event.GetArgument("damage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stunskilldeployed") == 0)
		{
			// WARN: It should be 'currentState'.
			lastState = STATE_ATTACK;
			currentState = STATE_STOPPED;
			mesh.LoadLight(onFreezeColor);
			//GenerateUniformBuffers(materialBlockSize, onFreezeColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stuntimeended") == 0)
		{
			currentState = lastState;
			mesh.LoadLight();
			//GenerateUniformBuffers(materialBlockSize, initialColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "shieldskilldeployed") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "destroy") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "startPain") == 0)
		{
			mesh.LoadLight(painEmitColor);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stopPain") == 0)
		{
			mesh.LoadLight(initialColor);
		}
		break;
	default:
		HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
		break;
	}
}


Asteroid::Asteroid(int newDamage, 
				   glm::vec4 newInitialColor, glm::vec4 newOnFreezeColor, glm::vec4 newPainEmitColor,
				   glm::vec3 newPosition, glm::vec3 newFrontVector,
				   float newSpeed, float newLineOfSight,
				   int newHealth, int newResourceGivenOnKill)
				   : Enemy(newInitialColor, newOnFreezeColor, newPainEmitColor, newPosition, newFrontVector, newSpeed, newLineOfSight,
						   newHealth, newResourceGivenOnKill)
{
	damage = newDamage;
	rank = RANK_ONE;
}

void Asteroid::LoadMesh(const std::string &meshFile)
{
	if(!mesh.LoadMesh(meshFile))
	{
		std::string errorMessage = "cannot load mesh ";
		errorMessage += meshFile;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}
	mesh.LoadLight();
}

void Asteroid::AttackSolarSystem(CelestialBody &sun, bool isSatellite, float bodyIndex)
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

	isDestroyed = true;
}

void Asteroid::UpdateAI(CelestialBody &sun)
{
	glm::vec3 vectorBetweenPlanetAndAsteroid = glm::vec3();
	if(sun.HasSatellites())
	{
		vectorBetweenPlanetAndAsteroid = sun.GetOuterSatellite()->GetPosition() - position;

		float satelliteRadius = sun.GetOuterSatellite()->GetRadius();

		float distanceBetweenPlanetAndAsteroid = glm::length(vectorBetweenPlanetAndAsteroid);
		if(distanceBetweenPlanetAndAsteroid <= satelliteRadius * satelliteRadius)
		{
			AttackSolarSystem(sun, true, sun.GetOuterSatellite()->GetOffsetFromSun());
		}
	}
	else
	{
		vectorBetweenPlanetAndAsteroid = sun.GetPosition() - position;

		float sunRadius = sun.GetRadius();

		float distanceBetweenPlanetAndAsteroid = glm::length(vectorBetweenPlanetAndAsteroid);
		if(distanceBetweenPlanetAndAsteroid <= sunRadius * sunRadius)
		{
			AttackSolarSystem(sun);
		}
	}
}

void Asteroid::Update(bool isSunKilled, CelestialBody &sun)
{
	if(!isSunKilled)
	{
		if(currentState != STATE_STOPPED)
		{
			position += frontVector * speed;
			UpdateAI(sun);
		}

		if(health <= 0)
		{
			isDestroyed = true;
		}
	}
	else
	{
		currentState = STATE_IDLE;
	}
	/*
	if(isStartedEmittingPain)
	{
		painEmitter.SetPosition(position);
		painEmitter.Update();
	}
	*/
}

void Asteroid::Render(glutil::MatrixStack &modelMatrix,
					  int materialBlockIndex, float gamma,
					  const LitProgData &litData, const LitTextureProgData &litTextureData,
					  const SpriteParticleProgData &spriteParticleData,
					  float interpolation)
{
	glutil::PushStack push(modelMatrix);

	modelMatrix.Translate(position);
	/*
	if(isStartedEmittingPain)
	{
		painEmitter.Render(modelMatrix, spriteParticleData);
	}
	*/
	modelMatrix.Scale(0.15f);
	
	mesh.Render(modelMatrix, litTextureData, materialBlockIndex);

	glUseProgram(0);
}

void Asteroid::OnEvent(Event &_event)
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
			health -= _event.GetArgument("skillDamage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "timeended") == 0)
		{
			health -= _event.GetArgument("damage").varInteger;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stunskilldeployed") == 0)
		{
			// WARN: It should be 'currentState'.
			lastState = STATE_ATTACK;
			currentState = STATE_STOPPED;
			mesh.LoadLight(onFreezeColor);
			//GenerateUniformBuffers(materialBlockSize, onFreezeColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stuntimeended") == 0)
		{
			currentState = lastState;
			mesh.LoadLight();
			//GenerateUniformBuffers(materialBlockSize, initialColor, materialUniformBuffer);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "shieldskilldeployed") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "destroy") == 0)
		{
			isDestroyed = true;
		}
		if(strcmp(_event.GetArgument("what_event").varString, "startPain") == 0)
		{
			mesh.LoadLight(painEmitColor);
		}
		if(strcmp(_event.GetArgument("what_event").varString, "stopPain") == 0)
		{
			mesh.LoadLight(initialColor);
		}
		break;
	default:
		HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
		break;
	}
}