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


/*Event Skill::GetGeneratedEvent(const std::string &eventName)
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
	else
	{
		return StockEvents::EmptyEvent();
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
}*/

/*
RaySkill::RaySkill(std::shared_ptr<CelestialBody> newSkillOwner,
				   int newDamage, int newDefense,
				   float newRange,
				   const std::string &newSkillType,
				   char fusionCombA, char fusionCombB, char fusionCombC)
				   : Skill(newSkillType,
						   fusionCombA, fusionCombB, fusionCombC)
{
	skillOwner = newSkillOwner;
	damage = newDamage;
	defense = newDefense;
	range = newRange;

	isStarted = false;

	ray = RayEmitter(glm::vec3(skillOwner->GetPosition()), 100, range);
	ray.Init();
}

void RaySkill::Update()
{
	if(isStarted)
	{
		EventArg skillDeployedEventArgs[3];
		skillDeployedEventArgs[0].argType = "skillrange";
		skillDeployedEventArgs[0].argument.varType = TYPE_FLOAT;
		skillDeployedEventArgs[0].argument.varFloat = range;
		skillDeployedEventArgs[1].argType = "skilldamage";
		skillDeployedEventArgs[1].argument.varType = TYPE_INTEGER;
		skillDeployedEventArgs[1].argument.varInteger = damage;
		skillDeployedEventArgs[2].argType = "what_event";
		skillDeployedEventArgs[2].argument.varType = TYPE_STRING;
		strcpy(skillDeployedEventArgs[2].argument.varString, "skilldeployed");
		Event skillDeployedEvent = Event(3, EVENT_TYPE_OTHER, skillDeployedEventArgs);
		generatedEvents.push_back(skillDeployedEvent);

		ray.Update();
	}
}

void RaySkill::Render(glutil::MatrixStack &modelMatrix,
					  glm::vec3 cameraPosition,
					  const BillboardProgDataNoTexture &progData)
{
	if(isStarted)
	{
		ray.Render(modelMatrix, cameraPosition, progData);
	}
}

void RaySkill::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("buttons").varString, fusionCombination) == 0)
		{
			isStarted = true;
		}
	}
}

std::shared_ptr<CelestialBody> RaySkill::GetOwner()
{
	return skillOwner;
}

Event RaySkill::GetGeneratedEvent(const std::string &eventName)
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
	else
	{
		return StockEvents::EmptyEvent();
	}
}
void RaySkill::RemoveGeneratedEvent(const std::string &eventName)
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
std::vector<Event> RaySkill::GetGeneratedEvents()
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
*/


AOESkill::AOESkill(//std::shared_ptr<CelestialBody> newSkillOwner,
				   glm::vec3 newPosition,
				   int newDamage, float newRange,
				   const std::string &newSkillType,
				   char fusionCombA, char fusionCombB, char fusionCombC)
				   : Skill(newSkillType,
						   fusionCombA, fusionCombB, fusionCombC)
{
	//skillOwner = newSkillOwner;
	damage = newDamage;
	range = newRange;
	position = newPosition;

	isStarted = false;

	skillSelector = 
		AOESelector(position/*glm::vec3(skillOwner->GetPosition())*/, range, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
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
/*
std::shared_ptr<CelestialBody> AOESkill::GetOwner()
{
	return skillOwner;
}*/
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

Event AOESkill::GetGeneratedEvent(const std::string &eventName)
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
	else
	{
		return StockEvents::EmptyEvent();
	}
}
void AOESkill::RemoveGeneratedEvent(const std::string &eventName)
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
std::vector<Event> AOESkill::GetGeneratedEvents()
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


PassiveAOESkill::PassiveAOESkill(//std::shared_ptr<CelestialBody> newSkillOwner,
								 glm::vec3 newPosition,
								 int newDamage, int newDamageApplyTime_seconds,
								 float newRange,
								 const std::string &newSkillType,
								 char fusionCombA, char fusionCombB, char fusionCombC)
								 : Skill(newSkillType,
										 fusionCombA, fusionCombB, fusionCombC)
{
	//skillOwner = newSkillOwner;
	damage = newDamage;
	damageApplyTime_seconds = newDamageApplyTime_seconds;
	attackTimer = Framework::Timer(Framework::Timer::TT_INFINITE, damageApplyTime_seconds);
	range = newRange;
	position = newPosition;
	//position = glm::vec3(skillOwner->GetPosition());

	isStarted = false;

	skillVisibleRadius = 
		Utility::Primitives::Circle(glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), glm::vec4(position, 1.0f), //glm::vec4(skillOwner->GetPosition(), 1.0f),
									range, 90);
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

		//position = glm::vec3(skillOwner->GetPosition());		
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
/*
std::shared_ptr<CelestialBody> PassiveAOESkill::GetOwner()
{
	return skillOwner;
}*/
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

Event PassiveAOESkill::GetGeneratedEvent(const std::string &eventName)
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
void PassiveAOESkill::RemoveGeneratedEvent(const std::string &eventName)
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
std::vector<Event> PassiveAOESkill::GetGeneratedEvents()
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