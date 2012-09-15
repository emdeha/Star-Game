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


#include "EventSystem.h"
#include <assert.h>


Event::Event(EventType newType)
{
	type = newType;

	numberOfArgs = 0;
}

Event::Event(short newNumberOfArgs, EventType newType, EventArg newArgs[])
{
	assert(newNumberOfArgs < MAX_ARGS && 
		   newNumberOfArgs >= 0);

	type = newType;

	for(int i = 0; i < newNumberOfArgs; i++)
	{
		args[i].argType = newArgs[i].argType;
		args[i].argument.varType = newArgs[i].argument.varType;
		switch(args[i].argument.varType)
		{
		case TYPE_BOOL:
			args[i].argument.varBool = newArgs[i].argument.varBool;
			break;
		case TYPE_INTEGER:
			args[i].argument.varInteger = newArgs[i].argument.varInteger;
			break;
		case TYPE_STRING:
			strcpy(args[i].argument.varString, newArgs[i].argument.varString);
			break;
		case TYPE_FLOAT:
			args[i].argument.varFloat = newArgs[i].argument.varFloat;
			break;
		case TYPE_NONE:
			break;
		}
	}

	numberOfArgs = newNumberOfArgs;
}

Variant Event::GetArgument(const std::string &argType)
{	
	if(numberOfArgs > 0)
	{
		for(int i = 0; i < numberOfArgs; i++)
		{
			if(args[i].argType == argType)
				return args[i].argument;
		}

		// TODO: Return a `false` argument if no argument is found
		Variant falseVariant;
		falseVariant.varType = TYPE_STRING;
		strcpy(falseVariant.varString, "falseVariant");

		return falseVariant;
	}
	// else return falseVariant;
}

EventType Event::GetType()
{
	return type;
}