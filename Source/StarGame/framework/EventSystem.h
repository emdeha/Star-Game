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


#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H


#include <string>


enum EventType
{
	EVENT_TYPE_DEFAULT,
	EVENT_TYPE_ON_CLICK,
	EVENT_TYPE_ON_HOVER,
	EVENT_TYPE_SPACE_BTN_CLICK,
};


enum VariantType
{
	TYPE_INTEGER,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_BOOL,

	TYPE_COUNT,

	TYPE_NONE,
};

struct Variant
{	
	VariantType varType;

	union
	{
		int varInteger;
		float varFloat;
		char varString[20];
		bool varBool;
	};
};

struct EventArg
{
	std::string argType;
	Variant argument;
};


class Event
{
private:
	static const short MAX_ARGS = 8;

	EventType type;
	EventArg args[MAX_ARGS];

	short numberOfArgs;

public:
	Event(short newNumberOfArgs, EventType newType, EventArg newArgs[]);

	Variant GetArgument(const std::string &argType);
	// void SetArgument(const std::string &newArgType, Variant arg);

	EventType GetType();
};

#endif