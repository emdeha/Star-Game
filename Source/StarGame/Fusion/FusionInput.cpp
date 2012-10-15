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

/*
FusionSequence::FusionSequence(char newEndButton, char buttonA, char buttonB, char buttonC)
{
	buttons[0] = buttonA;
	buttons[1] = buttonB;
	buttons[2] = buttonC;
	// buttons[3] = buttonD; -- if we decide to make the fusion use three buttons

	endButton = newEndButton;

	nextButton = 0;

	isFinished = false;
}

Event FusionSequence::Update(char newButton)
{
	if(nextButton == 0 && newButton == buttons[nextButton])
	{
		isFinished = false;
	}	

	
	if(isFinished)
	{
		return StockEvents::EmptyEvent();
	}
	if(newButton == endButton)
	{
		// Send event.
		nextButton = 0;
		isFinished = true;

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
	if(nextButton > 3 && newButton != endButton) 
	{
		std::printf("You can't input more buttons. Type \'F\' if you want to send fusion.\n");
		return StockEvents::EmptyEvent();
	}	
	

	if(newButton == buttons[nextButton])
	{
		nextButton++;
	}
}*/



/*FusionInput::FusionInput()
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
	sequences.push_back(FusionSequence(sequenceEndButton, buttonA, buttonB, buttonC));
}

Event FusionInput::Update(char newButton)
{
	for(std::vector<FusionSequence>::iterator iter = sequences.begin();
		iter != sequences.end(); ++iter)
	{
		Event eventToReturn = iter->Update(newButton);
		if(eventToReturn.GetType() != EVENT_TYPE_EMPTY)
		{
			return eventToReturn;
		}
	}
}*/