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
#include "Skills.h"
#include "MaterialBlock.h"


Event Skill::GetGeneratedEvent(const std::string &eventName)
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
void Skill::RemoveGeneratedEvent(const std::string &eventName)
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
std::vector<Event> Skill::GetGeneratedEvents()
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


// for lighting
static void GenerateUniformBuffers(int &materialBlockSize, glm::vec4 diffuseColor, GLuint &materialUniformBuffer)
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


AOESkill::AOESkill(glm::vec3 newPosition,
				   int newDamage, float newRange,
				   const std::string &newSkillType,
				   char fusionCombA, char fusionCombB, char fusionCombC)
				   : Skill(newSkillType,
						   fusionCombA, fusionCombB, fusionCombC)
{
	damage = newDamage;
	range = newRange;
	position = newPosition;

	isStarted = false;

	skillSelector = 
		AOESelector(position, range, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
	skillSelector.Init();
}

void AOESkill::Update()
{
	if(isStarted)
	{
		skillSelector.Update(position);
	}
}

void AOESkill::Render(glutil::MatrixStack &modelMatrix, const SimpleProgData &progData)
{
	if(isStarted)
	{
		glutil::PushStack push(modelMatrix);
		modelMatrix.Translate(position);

		skillSelector.Draw(modelMatrix, progData);
	}
}

void AOESkill::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("buttons").varString, fusionCombination) == 0)
		{
			EventArg skillDeployedEventArgs[3];
			skillDeployedEventArgs[0].argType = "skillRange";
			skillDeployedEventArgs[0].argument.varType = TYPE_FLOAT;
			skillDeployedEventArgs[0].argument.varFloat = range;
			skillDeployedEventArgs[1].argType = "skillDamage";
			skillDeployedEventArgs[1].argument.varType = TYPE_INTEGER;
			skillDeployedEventArgs[1].argument.varInteger = damage;
			skillDeployedEventArgs[2].argType = "what_event";
			skillDeployedEventArgs[2].argument.varType = TYPE_STRING;
			strcpy(skillDeployedEventArgs[2].argument.varString, "skilldeployed");
			Event skillDeployedEvent = Event(3, EVENT_TYPE_OTHER, skillDeployedEventArgs);
			generatedEvents.push_back(skillDeployedEvent);

			isStarted = true;
		}
		break;
	}
}

float AOESkill::GetRange()
{
	return range;
}
glm::vec3 AOESkill::GetPosition()
{
	return position;
}

void AOESkill::SetParameter(ParameterType paramType, glm::vec3 newParam_vec3)
{
	switch(paramType)
	{
	case PARAM_POSITION:
		position = newParam_vec3;
		break;
	default:
		break;
	}
}

bool AOESkill::IsIntersectingObject(glm::vec3 objectPosition)
{
	float distanceBetweenObjectAndSkill = glm::length(position - objectPosition);

	if(distanceBetweenObjectAndSkill < range)
	{
		return true;
	}
	
	return false;
}


PassiveAOESkill::PassiveAOESkill(glm::vec3 newPosition,
								 int newDamage, int newDamageApplyTime_seconds,
								 float newRange,
								 const std::string &newSkillType,
								 char fusionCombA, char fusionCombB, char fusionCombC)
								 : Skill(newSkillType,
										 fusionCombA, fusionCombB, fusionCombC)
{
	damage = newDamage;
	damageApplyTime_seconds = newDamageApplyTime_seconds;
	attackTimer = Framework::Timer(Framework::Timer::TT_INFINITE, damageApplyTime_seconds);
	range = newRange;
	position = newPosition;

	isStarted = false;

	skillVisibleRadius = 
		Utility::Primitives::Circle(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), position, range, 90);
	skillVisibleRadius.Init();
}

void PassiveAOESkill::Update()
{
	if(isStarted)
	{
		attackTimer.Update();
		if(attackTimer.GetTimeSinceStart() >= damageApplyTime_seconds)
		{
			EventArg enemyDamage_perTime[2];
			enemyDamage_perTime[0].argType = "what_event";
			enemyDamage_perTime[0].argument.varType = TYPE_STRING;
			strcpy(enemyDamage_perTime[0].argument.varString, "timeended");
			enemyDamage_perTime[1].argType = "damage";
			enemyDamage_perTime[1].argument.varType = TYPE_INTEGER;
			enemyDamage_perTime[1].argument.varInteger = damage;

			Event enemyDamageEvent(2, EVENT_TYPE_OTHER, enemyDamage_perTime);
			// Make sure there aren't any duplicated events.
			RemoveGeneratedEvent("timeended");
			generatedEvents.push_back(enemyDamageEvent);

			attackTimer.Reset();
		}
	}
}

void PassiveAOESkill::Render(glutil::MatrixStack &modelMatrix,
							 const SimpleProgData &progData)
{
	if(isStarted)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glutil::PushStack push(modelMatrix);
		modelMatrix.Translate(position);

		skillVisibleRadius.Draw(modelMatrix, progData);

		glDisable(GL_BLEND);
	}
}

void PassiveAOESkill::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("buttons").varString, fusionCombination) == 0)
		{
			EventArg skillDeployedEventArgs[3];
			skillDeployedEventArgs[0].argType = "skillRange";
			skillDeployedEventArgs[0].argument.varType = TYPE_FLOAT;
			skillDeployedEventArgs[0].argument.varFloat = range;
			skillDeployedEventArgs[1].argType = "skillDamage";
			skillDeployedEventArgs[1].argument.varType = TYPE_INTEGER;
			skillDeployedEventArgs[1].argument.varInteger = damage;
			skillDeployedEventArgs[2].argType = "what_event";
			skillDeployedEventArgs[2].argument.varType = TYPE_STRING;
			strcpy(skillDeployedEventArgs[2].argument.varString, "skilldeployed");
			Event skillDeployedEvent = Event(3, EVENT_TYPE_OTHER, skillDeployedEventArgs);
			generatedEvents.push_back(skillDeployedEvent);

			isStarted = true;
		}
		break;
	}
}

float PassiveAOESkill::GetRange()
{
	return range;
}
glm::vec3 PassiveAOESkill::GetPosition()
{
	return position;
}

void PassiveAOESkill::SetParameter(ParameterType paramType, glm::vec3 newParam_vec3)
{
	switch(paramType)
	{
	case PARAM_POSITION:
		position = newParam_vec3;
		break;
	default:
		break;
	}
}

bool PassiveAOESkill::IsIntersectingObject(glm::vec3 objectPosition)
{
	float distanceBetweenObjectAndSkill = glm::length(position - objectPosition);

	if(distanceBetweenObjectAndSkill < range)
	{
		return true;
	}

	return false;
}


SunNovaSkill::SunNovaSkill(glm::vec3 newPosition,
						   int newDamage, 
						   float newRange, float newScaleRate,
						   const std::string &skillType,
						   char fusionCombA, char fusionCombB, char fusionCombC)
						   : Skill(skillType, fusionCombA, fusionCombB, fusionCombC)
{
	position = newPosition;
	damage = newDamage;
	range = newRange;
	scaleRate = newScaleRate;
	currentScale = 1.0f;
	isStarted = false;

#ifdef CIRCLE_SKILL
	skillExpansionRadius = Utility::Primitives::Circle(glm::vec4(0.7f, 0.5f, 0.0f, 0.5f), position, currentScale, 90);
#elif defined TORUS_SKILL
	skillExpansionRadius = Utility::Primitives::Torus2D(glm::vec4(0.7f, 0.5f, 0.0f, 0.5f), position, currentScale, currentScale + 0.03f, 90);
#endif
	skillExpansionRadius.Init();
}

void SunNovaSkill::Update()
{
	if(isStarted)
	{
		if(currentScale <= range)
		{
			currentScale += scaleRate;
		}
		else
		{
			currentScale = 1.0f;
			isStarted = false;
		}
	}
}

void SunNovaSkill::Render(glutil::MatrixStack &modelMatrix, const SimpleProgData &progData)
{
	if(isStarted)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glutil::PushStack push(modelMatrix);
		modelMatrix.Translate(position);
		modelMatrix.Scale(currentScale, 0.0f, currentScale);

		skillExpansionRadius.Draw(modelMatrix, progData);

		glDisable(GL_BLEND);
	}
}

void SunNovaSkill::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("buttons").varString, fusionCombination) == 0)
		{
			EventArg skillDeployedEventArgs[3];
			skillDeployedEventArgs[0].argType = "skillRange";
			skillDeployedEventArgs[0].argument.varType = TYPE_FLOAT;
			skillDeployedEventArgs[0].argument.varFloat = range;
			skillDeployedEventArgs[1].argType = "skillDamage";
			skillDeployedEventArgs[1].argument.varType = TYPE_INTEGER;
			skillDeployedEventArgs[1].argument.varInteger = damage;
			skillDeployedEventArgs[2].argType = "what_event";
			skillDeployedEventArgs[2].argument.varType = TYPE_STRING;
			strcpy(skillDeployedEventArgs[2].argument.varString, "skilldeployed");
			Event skillDeployedEvent = Event(3, EVENT_TYPE_OTHER, skillDeployedEventArgs);
			generatedEvents.push_back(skillDeployedEvent);

			isStarted = true;
		}
		break;
	}
}

float SunNovaSkill::GetRange()
{
	return range;
}
glm::vec3 SunNovaSkill::GetPosition()
{
	return position;
}

bool SunNovaSkill::IsIntersectingObject(glm::vec3 objectPosition)
{
	if(isStarted)
	{
		float distanceBetweenObjectAndSkill = glm::length(position - objectPosition);

		if(distanceBetweenObjectAndSkill < currentScale &&
		   distanceBetweenObjectAndSkill >= currentScale - 0.1f)
		{
			return true;
		}

		return false;
	}
	else return false;
}


SatelliteChainingSkill::SatelliteChainingSkill(glm::vec3 newPosition,
											   int newDamage, float newRange, 
											   glm::vec3 newProjectileVelocity, glm::vec4 newProjectileColor,
											   float newProjectileRadius,
											   const std::string &meshFileName, 
											   const std::string &skillType,
											   char fusionCombA, char fusionCombB, char fusionCombC)
											   : Skill(skillType, fusionCombA, fusionCombB, fusionCombC)
{
	currentPosition = newPosition;
	startingPosition = newPosition;
	damage = newDamage;
	range = newRange;
	projectileVelocity = newProjectileVelocity;
	projectileColor = newProjectileColor;
	projectileRadius = newProjectileRadius;
	materialBlockSize = 0;
	materialUniformBuffer = 0;
	isStarted = false;

	try
	{
		projectileMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(meshFileName));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}

	GenerateUniformBuffers(materialBlockSize, projectileColor, materialUniformBuffer);
}

void SatelliteChainingSkill::Update()
{
	if(isStarted)
	{
		if(glm::length(currentPosition - startingPosition) <= range)
		{
			currentPosition += projectileVelocity;
		}
		else
		{
			currentPosition = startingPosition;
			isStarted = false;
		}
	}
}

void SatelliteChainingSkill::Render(glutil::MatrixStack &modelMatrix, const LitProgData &litData, 
									GLuint materialBlockIndex)
{
	if(isStarted)
	{
		glutil::PushStack push(modelMatrix);
		modelMatrix.Translate(currentPosition);
		modelMatrix.Scale(projectileRadius); 

		glBindBufferRange(GL_UNIFORM_BUFFER, materialBlockIndex, materialUniformBuffer,
						  0, sizeof(MaterialBlock));

		glm::mat3 normMatrix(modelMatrix.Top());
		normMatrix = glm::transpose(glm::inverse(normMatrix));

		glUseProgram(litData.theProgram);
		glUniformMatrix4fv(litData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniformMatrix3fv(litData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));


		projectileMesh->Render("lit");

		glUseProgram(0);
		glBindBufferBase(GL_UNIFORM_BUFFER, materialBlockIndex, 0);
	}
}

void SatelliteChainingSkill::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		// ???: Should the skill be activated on fusion?
		if(strcmp(_event.GetArgument("buttons").varString, fusionCombination) == 0)
		{
			EventArg skillDeployedEventArgs[3];
			skillDeployedEventArgs[0].argType = "skillRange";
			skillDeployedEventArgs[0].argument.varType = TYPE_FLOAT;
			skillDeployedEventArgs[0].argument.varFloat = range;
			skillDeployedEventArgs[1].argType = "skillDamage";
			skillDeployedEventArgs[1].argument.varType = TYPE_INTEGER;
			skillDeployedEventArgs[1].argument.varInteger = damage;
			skillDeployedEventArgs[2].argType = "what_event";
			skillDeployedEventArgs[2].argument.varType = TYPE_STRING;
			strcpy(skillDeployedEventArgs[2].argument.varString, "skilldeployed");
			Event skillDeployedEvent = Event(3, EVENT_TYPE_OTHER, skillDeployedEventArgs);
			generatedEvents.push_back(skillDeployedEvent);

			isStarted = true;
		}
		if(_event.GetArgument("isIntersected").varBool == true)
		{
			EventArg skillDeployedEventArgs[3];
			skillDeployedEventArgs[0].argType = "skillRange";
			skillDeployedEventArgs[0].argument.varType = TYPE_FLOAT;
			skillDeployedEventArgs[0].argument.varFloat = range;
			skillDeployedEventArgs[1].argType = "skillDamage";
			skillDeployedEventArgs[1].argument.varType = TYPE_INTEGER;
			skillDeployedEventArgs[1].argument.varInteger = damage;
			skillDeployedEventArgs[2].argType = "what_event";
			skillDeployedEventArgs[2].argument.varType = TYPE_STRING;
			strcpy(skillDeployedEventArgs[2].argument.varString, "skilldeployed");
			Event skillDeployedEvent = Event(3, EVENT_TYPE_OTHER, skillDeployedEventArgs);
			generatedEvents.push_back(skillDeployedEvent);

			isStarted = true;
		}
		break;
	}
}

float SatelliteChainingSkill::GetRange()
{
	return range;
}
glm::vec3 SatelliteChainingSkill::GetPosition()
{
	return currentPosition;
}

void SatelliteChainingSkill::SetParameter(ParameterType paramType, glm::vec3 newParam_vec3)
{
	switch(paramType)
	{
	case PARAM_POSITION:
		if(!isStarted)
		{
			startingPosition = newParam_vec3;
			currentPosition = newParam_vec3;
		}
		break;
	default:
		break;
	}
}

bool SatelliteChainingSkill::IsIntersectingObject(glm::vec3 objectPosition)
{
	if(isStarted)
	{
		float distanceBetweenObjectAndSkill = glm::length(currentPosition - objectPosition);

		if(distanceBetweenObjectAndSkill < projectileRadius + 1.0f) // 1.0f = the object's radius. Makes the intersection easier.
		{
			return true;
		}

		return false;
	}
	else return false;
}



SatelliteChainingNova::SatelliteChainingNova(glm::vec3 newPosition, 
											 int newDamage, 
											 float newRange, float newScaleRate,
											 const std::string &skillType,
											 char fusionCombA, char fusionCombB, char fusionCombC)
											 : Skill(skillType, fusionCombA, fusionCombB, fusionCombC)
{
	position = newPosition;
	damage = newDamage;
	range = newRange;
	scaleRate = newScaleRate;
	currentScale = 1.0f;
	isStarted = false;
	generatedEvents.resize(0);

#ifdef CIRCLE_SKILL
	skillExpansionRadius = Utility::Primitives::Circle(glm::vec4(0.2f, 0.0f, 0.8f, 0.5f), position, currentScale, 90);
#elif defined TORUS_SKILL 
	skillExpansionRadius = Utility::Primitives::Torus2D(glm::vec4(0.2f, 0.0f, 0.8f, 0.5f), position, currentScale, currentScale + 0.03f, 90);
#endif
	skillExpansionRadius.Init();
}

void SatelliteChainingNova::Update()
{
	if(isStarted)
	{
		if(currentScale <= range)
		{
			currentScale += scaleRate;
		}
		else
		{
			currentScale = 1.0f;
			isStarted = false;
		}
	}
}

void SatelliteChainingNova::Render(glutil::MatrixStack &modelMatrix, const SimpleProgData &progData)
{
	if(isStarted)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glutil::PushStack push(modelMatrix);
		//position.y = 0.1f; // -- layering on y
		modelMatrix.Translate(position);
		modelMatrix.Scale(currentScale, 0.0f, currentScale);

		skillExpansionRadius.Draw(modelMatrix, progData);

		glDisable(GL_BLEND);
	}
}

void SatelliteChainingNova::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		// ???: Should the skill be activated on fusion?
		if(strcmp(_event.GetArgument("buttons").varString, fusionCombination) == 0)
		{
			EventArg skillDeployedEventArgs[3];
			skillDeployedEventArgs[0].argType = "skillRange";
			skillDeployedEventArgs[0].argument.varType = TYPE_FLOAT;
			skillDeployedEventArgs[0].argument.varFloat = range;
			skillDeployedEventArgs[1].argType = "skillDamage";
			skillDeployedEventArgs[1].argument.varType = TYPE_INTEGER;
			skillDeployedEventArgs[1].argument.varInteger = damage;
			skillDeployedEventArgs[2].argType = "what_event";
			skillDeployedEventArgs[2].argument.varType = TYPE_STRING;
			strcpy(skillDeployedEventArgs[2].argument.varString, "skilldeployed");
			Event skillDeployedEvent = Event(3, EVENT_TYPE_OTHER, skillDeployedEventArgs);
			generatedEvents.push_back(skillDeployedEvent);

			isStarted = true;
		}
		if(_event.GetArgument("isIntersected").varBool == true)
		{
			EventArg skillDeployedEventArgs[3];
			skillDeployedEventArgs[0].argType = "skillRange";
			skillDeployedEventArgs[0].argument.varType = TYPE_FLOAT;
			skillDeployedEventArgs[0].argument.varFloat = range;
			skillDeployedEventArgs[1].argType = "skillDamage";
			skillDeployedEventArgs[1].argument.varType = TYPE_INTEGER;
			skillDeployedEventArgs[1].argument.varInteger = damage;
			skillDeployedEventArgs[2].argType = "what_event";
			skillDeployedEventArgs[2].argument.varType = TYPE_STRING;
			strcpy(skillDeployedEventArgs[2].argument.varString, "skilldeployed");
			Event skillDeployedEvent = Event(3, EVENT_TYPE_OTHER, skillDeployedEventArgs);
			generatedEvents.push_back(skillDeployedEvent);

			isStarted = true;
		}
		break;
	}
}

float SatelliteChainingNova::GetRange()
{
	return range;
}
glm::vec3 SatelliteChainingNova::GetPosition()
{
	return position;
}

void SatelliteChainingNova::SetParameter(ParameterType paramType, glm::vec3 newParam_vec3)
{
	switch(paramType)
	{
	case PARAM_POSITION:
		position = newParam_vec3;
		break;
	default:
		break;
	};
}

bool SatelliteChainingNova::IsIntersectingObject(glm::vec3 objectPosition)
{
	if(isStarted)
	{
		float distanceBetweenObjectAndSkill = glm::length(position - objectPosition);

		if(distanceBetweenObjectAndSkill < currentScale &&
		   distanceBetweenObjectAndSkill >= currentScale - 0.1f)
		{
			return true;
		}

		return false;
	}
	else return false;
}


FrostNovaSkill::FrostNovaSkill(int newDamage, int newStunTime_seconds,
							   float newRange, float newScaleRate,
							   glm::vec3 newPosition,
							   const std::string &skillType,
							   char fusionCombA, char fusionCombB, char fusionCombC)
							   : Skill(skillType, fusionCombA, fusionCombB, fusionCombC)
{
	damage = newDamage;
	stunTime_seconds = newStunTime_seconds;
	stunTimer = Framework::Timer(Framework::Timer::TT_INFINITE, stunTime_seconds);
	stunTimer.SetPause(true);
	range = newRange;
	scaleRate = newScaleRate;
	position = newPosition;
	currentScale = 1.0f;
	isStarted = false;

#ifdef CIRCLE_SKILL
	skillExpansionRadius = Utility::Primitives::Circle(glm::vec4(0.2f, 0.0f, 0.8f, 0.5f), position, currentScale, 90);
#elif defined TORUS_SKILL 
	skillExpansionRadius = Utility::Primitives::Torus2D(glm::vec4(0.2f, 0.0f, 0.8f, 0.5f), position, currentScale, currentScale + 0.03f, 90);
#endif
	skillExpansionRadius.Init();
}

void FrostNovaSkill::Update()
{
	stunTimer.Update();
	if(stunTimer.GetTimeSinceStart() >= stunTime_seconds && stunTimer.IsPaused() == false)
	{
		EventArg enemyStunFinishedEventArgs[1];
		enemyStunFinishedEventArgs[0].argType = "what_event";
		enemyStunFinishedEventArgs[0].argument.varType = TYPE_STRING;
		strcpy(enemyStunFinishedEventArgs[0].argument.varString, "stuntimeended");

		Event enemyStunFinishedEvent(1, EVENT_TYPE_OTHER, enemyStunFinishedEventArgs);
		RemoveGeneratedEvent("stuntimeended");
		generatedEvents.push_back(enemyStunFinishedEvent);

		stunTimer.SetPause(true);
	}

	if(isStarted)
	{
		if(currentScale <= range)
		{
			currentScale += scaleRate;
		}
		else
		{
			currentScale = 1.0f;
			isStarted = false;
		}
	}
}

void FrostNovaSkill::Render(glutil::MatrixStack &modelMatrix, const SimpleProgData &progData)
{
	if(isStarted)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glutil::PushStack push(modelMatrix);
		modelMatrix.Translate(position);
		modelMatrix.Scale(currentScale, 0.0f, currentScale);
		
		skillExpansionRadius.Draw(modelMatrix, progData);

		glDisable(GL_BLEND);
	}
}

void FrostNovaSkill::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("buttons").varString, fusionCombination) == 0 && !isStarted)
		{
			EventArg skillDeployedEventArgs[3];
			skillDeployedEventArgs[0].argType = "skillRange";
			skillDeployedEventArgs[0].argument.varType = TYPE_FLOAT;
			skillDeployedEventArgs[0].argument.varFloat = range;
			skillDeployedEventArgs[1].argType = "skillDamage";
			skillDeployedEventArgs[1].argument.varType = TYPE_INTEGER;
			skillDeployedEventArgs[1].argument.varInteger = damage;
			skillDeployedEventArgs[2].argType = "what_event";
			skillDeployedEventArgs[2].argument.varType = TYPE_STRING;
			strcpy(skillDeployedEventArgs[2].argument.varString, "stunskilldeployed");
			Event skillDeployedEvent = Event(3, EVENT_TYPE_OTHER, skillDeployedEventArgs);
			generatedEvents.push_back(skillDeployedEvent);

			isStarted = true;
			stunTimer.SetPause(false);
		}
	}
}

float FrostNovaSkill::GetRange()
{
	return range;
}
glm::vec3 FrostNovaSkill::GetPosition()
{
	return position;
}

void FrostNovaSkill::SetParameter(ParameterType paramType, glm::vec3 newParam_vec3)
{
	switch(paramType)
	{
	case PARAM_POSITION:
		position = newParam_vec3;
		break;
	default:
		break;
	}
}

bool FrostNovaSkill::IsIntersectingObject(glm::vec3 objectPosition)
{
	if(isStarted)
	{
		float distanceBetweenObjectAndSkill = glm::length(position - objectPosition);

		if(distanceBetweenObjectAndSkill < currentScale)
		{
			stunTimer.SetPause(false);
			stunTimer.Reset();			

			return true;
		}

		return false;
	}
	else return false;
}


ShieldSkill::ShieldSkill(glm::vec3 newPosition, 
						 int newDefensePoints, float newRange, 
						 const std::string &skillType, 
						 char fusionCombA, char fusionCombB, char fusionCombC)
						 : Skill(skillType, fusionCombA, fusionCombB, fusionCombC)
{
	position = newPosition;
	defensePoints = newDefensePoints;
	startingDefensePoints = defensePoints;
	range = newRange;
	isStarted = false;

	std::shared_ptr<OrbitingAnimationBody> bodyOne =
		std::shared_ptr<OrbitingAnimationBody>(new OrbitingAnimationBody(position, range, 1.0f, "../data/mesh-files/UnitSphere.xml"));
	std::shared_ptr<OrbitingAnimationBody> bodyTwo =
		std::shared_ptr<OrbitingAnimationBody>(new OrbitingAnimationBody(position, range - 0.2f, 2.5f, "../data/mesh-files/UnitSphere.xml"));
	
	skillAnimation.AddAnimationBody(bodyTwo);
	skillAnimation.AddAnimationBody(bodyOne);

	//skillRadius = Utility::Primitives::Circle(glm::vec4(0.7f, 0.5f, 0.0f, 1.0f), position, range, 90);
	//skillRadius.Init();
}

void ShieldSkill::Update()
{
	if(isStarted)
	{
		if(defensePoints == 0)
		{
			isStarted = false;
			defensePoints = startingDefensePoints;
		}

		skillAnimation.Update();
	}
}

void ShieldSkill::Render(glutil::MatrixStack &modelMatrix, const LitProgData &litData,
						 GLuint materialBlockIndex)
{
	if(isStarted)
	{
		glutil::PushStack push(modelMatrix);
		modelMatrix.Translate(position);

		skillAnimation.Render(modelMatrix, materialBlockIndex, litData);
		//skillRadius.Draw(modelMatrix, simpleData);
	}
}

void ShieldSkill::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ATTACKED:
		defensePoints--;
		break;
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("buttons").varString, fusionCombination) == 0)
		{
			EventArg skillDeployedEventArgs[1];
			skillDeployedEventArgs[0].argType = "what_event";
			skillDeployedEventArgs[0].argument.varType = TYPE_STRING;
			strcpy(skillDeployedEventArgs[0].argument.varString, "shieldskilldeployed");
			Event skillDeployedEvent = Event(1, EVENT_TYPE_OTHER, skillDeployedEventArgs);
			generatedEvents.push_back(skillDeployedEvent);

			isStarted = true;
		}
		break;
	default:
		break;
	}
}

float ShieldSkill::GetRange()
{
	return range;
}
glm::vec3 ShieldSkill::GetPosition()
{
	return position;
}

void ShieldSkill::SetParameter(ParameterType paramType, glm::vec3 newParam_vec3)
{
	switch(paramType)
	{
	case PARAM_POSITION:
		position = newParam_vec3;
		break;
	default:
		break;
	}
}

bool ShieldSkill::IsIntersectingObject(glm::vec3 objectPosition)
{
	float distanceBetweenObjectAndSkill = glm::length(position - objectPosition);

	if(distanceBetweenObjectAndSkill < range)
	{
		return true;
	}

	return false;
}