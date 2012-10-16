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


#ifndef FUSION_INPUT_H
#define FUSION_INPUT_H


#include <vector>
#include <string>
#include "../framework/EventSystem.h"


// Maybe it is a good idea to put this class as a private member of FusionInput.
class FusionSequence
{
private:
	char buttons[4];

public:
	FusionSequence(char buttonA = '\0', char buttonB = '\0', char buttonC = '\0')
	{
		buttons[0] = buttonA;
		buttons[1] = buttonB;
		buttons[2] = buttonC;
		buttons[3] = '\0';
		// buttons[3] = buttonD; -- if we decide to make the fusion use three buttons
	}

	Event Update(const std::string &currentInputSequence)
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
};



class FusionInput
{
private:
	std::vector<FusionSequence> sequences;

	std::string currentInputSequence;
	char sequenceEndButton;

public:
	FusionInput()
	{
		sequences.resize(0);		
	}
	FusionInput(char newSequenceEndButton)
	{
		sequences.resize(0);
		sequenceEndButton = newSequenceEndButton;
	}

	void AddSequence(char buttonA = ' ', char buttonB = ' ', char buttonC = ' ')
	{
		sequences.push_back(FusionSequence(buttonA, buttonB, buttonC));
	}

	Event Update(char newButton)
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
			std::printf("You can't input more buttons. Type \'F\' if you want to send fusion.\n");
			return StockEvents::EmptyEvent();
		}
		currentInputSequence += newButton;


		return StockEvents::EmptyEvent();
	}
};


#endif