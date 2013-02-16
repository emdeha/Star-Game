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
#include <algorithm>


FusionSequence::FusionSequence(char buttonA, char buttonB, char buttonC)
{
	buttons[0] = buttonA;
	buttons[1] = buttonB;
	buttons[2] = buttonC;
	buttons[3] = '\0';
	// buttons[3] = buttonD; -- if we decide to make the fusion use three buttons
}

std::string FusionSequence::GetButtons()
{
	return buttons;
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

void FusionInput::AddSequence(std::string sequenceName,
							  char buttonA, char buttonB, char buttonC)
{
	// std::lower_bound returns "the first position in which a value can be inserted 
	// without violating the ordering".
	// Therefore, I must make sure that while inserting a new sequence it is unique.

	sequences.push_back(std::make_pair(sequenceName, FusionSequence(buttonA, buttonB, buttonC)));
	/*std::sort(sequences.begin(), sequences.end(),
			  [](const std::pair<std::string, FusionSequence> &first, 
				 const std::pair<std::string, FusionSequence> &second)
	             { 
					 return first.first < second.first; 
				 });*/
}

std::string FusionInput::GetSequenceButtons(std::string sequenceName)
{
	for(std::vector<std::pair<std::string, FusionSequence>>::iterator iter = sequences.begin();
		iter != sequences.end(); ++iter)
	{
		if((*iter).first == sequenceName)
		{
			return (*iter).second.GetButtons();
		}
	}

	// There is a strange problem with this method. 
	// To reproduce it:
	//		-- Use four fusion sequences for the four satellites (qqq - fire, www - earth, eee - water, qwe - air)
	//		-- Make possible the addition of all satellites
	// On www fusion sequence you should get two satellites at the same time
	// Should investigate.

	/*std::vector<std::pair<std::string, FusionSequence>>::iterator lowerBound;
	lowerBound = std::lower_bound(sequences.begin(), sequences.end(), sequenceName, 
								  [](const std::pair<std::string, FusionSequence> &current,
									 const std::string seqName)
									 { 
							   			 return current.first == seqName; 
									 });

	if(lowerBound == sequences.begin())
	{
		lowerBound = sequences.end();
		return (--lowerBound)->second.GetButtons();
	}
	else
	{
		return (--lowerBound)->second.GetButtons();
	}*/
}

std::string FusionInput::GetSequenceName(std::string fusionButtons)
{
	for(std::vector<std::pair<std::string, FusionSequence>>::iterator iter = sequences.begin();
		iter != sequences.end(); ++iter)
	{
		if((*iter).second.GetButtons() == fusionButtons)
		{
			return (*iter).first;
		}
	}
}

Event FusionInput::Update(char newButton)
{
	if(newButton == sequenceEndButton)
	{
		for(std::vector<std::pair<std::string, FusionSequence>>::iterator iter = sequences.begin();
			iter != sequences.end(); ++iter)
		{
			Event eventToReturn = (iter)->second.Update(currentInputSequence);				
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
	EventArg currentInputButtonEventArg[4];
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
	currentInputButtonEventArg[3].argType = "isHint";
	currentInputButtonEventArg[3].argument.varType = TYPE_BOOL;
	if(currentInputSequence.length() == 3)
	{
		currentInputButtonEventArg[3].argument.varBool = true;
	}
	else 
	{
		currentInputButtonEventArg[3].argument.varBool = false;
	}
		
	Event currentInputButtonEvent = Event(4, EVENT_TYPE_OTHER, currentInputButtonEventArg);

	return currentInputButtonEvent;
}

std::string FusionInput::GetCurrentInputSequence()
{
	return currentInputSequence;
}

void FusionInput::Clear()
{
	sequences.clear();
}