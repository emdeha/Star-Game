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


RaySkill::RaySkill(int newDamage, int newDefense,
				   float newRange,
				   char fusionCombA, char fusionCombB, char fusionCombC)
{
	damage = newDamage;
	defense = newDefense;
	range = newRange;
	fusionCombination[0] = fusionCombA;
	fusionCombination[1] = fusionCombB;
	fusionCombination[2] = fusionCombC;
	fusionCombination[3] = '\0';

	isStarted = false;

	ray = RayEmitter(glm::vec3(), 100, range);
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