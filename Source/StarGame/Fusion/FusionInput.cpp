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
#include "FusionInput.h"


FusionSequence::FusionSequence(char buttonA, char buttonB, char buttonC)
{
	buttons[0] = buttonA;
	buttons[1] = buttonB;
	buttons[2] = buttonC;
	buttons[3] = '\0';
	// buttons[3] = buttonD; -- if we decide to make the fusion use three buttons
}

Event FusionSequence::Update(const std::string &currentInputSequence)
{
	if(currentInputSequence == buttons)
	{ 
		EventArg fusionCompletedEventArg[2];
		fusionCompletedEventArg[0].argType = "what_event";
		fusionCompletedEventArg[0].argument.varType = TYPE_STRING;
		strcpy(fusionCompletedEventArg[0].argument.varString, "fusion_seq");
		fusionCompletedEventArg[1].argType = "buttons";
		fusionCompletedEventArg[1].argument.varType = TYPE_STRING;
		strcpy(fusionCompletedEventArg[1].argument.varString, buttons);
		Event fusionCompletedEvent = Event(2, EVENT_TYPE_OTHER, fusionCompletedEventArg);

		return fusionCompletedEvent;
	}

	return StockEvents::EmptyEvent();
}



FusionInput::FusionInput()
{
	sequences.resize(0);
}
FusionInput::FusionInput(char newSequenceEndButton)
{
	sequences.resize(0);
	sequenceEndButton = newSequenceEndButton;
}

void FusionInput::AddSequence(char buttonA, char buttonB, char buttonC)
{
	sequences.push_back(FusionSequence(buttonA, buttonB, buttonC));
}

Event FusionInput::Update(char newButton)
{
	if(newButton == sequenceEndButton)
	{
		for(std::vector<FusionSequence>::iterator iter = sequences.begin();
			iter != sequences.end(); ++iter)
		{
			Event eventToReturn = (iter)->Update(currentInputSequence);				
			if(eventToReturn.GetType() != EVENT_TYPE_EMPTY)
			{
				currentInputSequence.resize(0);
				return eventToReturn;
			}
		}

		currentInputSequence.resize(0);
		return StockEvents::EmptyEvent();
	}

	if(currentInputSequence.length() >= 3)
	{
		currentInputSequence.clear();
		//std::printf("You can't input more buttons. Type \'F\' if you want to send fusion.\n");
		//return StockEvents::EmptyEvent();
	}
	currentInputSequence += newButton;

	// TODO: Should I check whether the input button is part of the sequence?
	EventArg currentInputButtonEventArg[3];
	currentInputButtonEventArg[0].argType = "what_event";
	currentInputButtonEventArg[0].argument.varType = TYPE_STRING;
	strcpy(currentInputButtonEventArg[0].argument.varString, "fusion_button");
	currentInputButtonEventArg[1].argType = "what_button";
	currentInputButtonEventArg[1].argument.varType = TYPE_STRING;
	std::string newButtonString = "";
	newButtonString += newButton;
	strcpy(currentInputButtonEventArg[1].argument.varString, newButtonString.c_str());
	currentInputButtonEventArg[2].argType = "button_pos";
	currentInputButtonEventArg[2].argument.varType = TYPE_INTEGER;
	currentInputButtonEventArg[2].argument.varInteger = currentInputSequence.length();
		
	Event currentInputButtonEvent = Event(3, EVENT_TYPE_OTHER, currentInputButtonEventArg);

	return currentInputButtonEvent;
}