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
#include "../glsdk/glm/glm.hpp"


enum EventType
{
	EVENT_TYPE_EMPTY,
	EVENT_TYPE_DEFAULT,
	EVENT_TYPE_OTHER,

	EVENT_TYPE_ON_CLICK,
	EVENT_TYPE_ON_HOVER,
	EVENT_TYPE_SPACE_BTN_CLICK,
	EVENT_TYPE_UNCLICK,
	EVENT_TYPE_BUTTON_PRESSED,

	EVENT_TYPE_SHOT_FIRED, 
	EVENT_TYPE_ATTACKED,
	EVENT_TYPE_KILLED,
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
	static const short MAX_ARGS = 20;

	EventType type;
	EventArg args[MAX_ARGS];

	short numberOfArgs;

public:
	Event() {}
	Event(EventType newType);
	Event(short newNumberOfArgs, EventType newType, EventArg newArgs[]);

	Variant GetArgument(const std::string &argType);
	// void SetArgument(const std::string &newArgType, Variant arg);

	EventType GetType();
};


class StockEvents
{
public:
	static Event EventOnRightClick(const char *objectName,
								   glm::ivec2 mouseCoordinates_windowSpace = glm::ivec2())
	{
		EventArg rightClickEventArg[4];
		rightClickEventArg[0].argType = "rightClick";
		rightClickEventArg[0].argument.varType = TYPE_BOOL;
		rightClickEventArg[0].argument.varBool = true; 
		rightClickEventArg[1].argType = "coordX";
		rightClickEventArg[1].argument.varType = TYPE_FLOAT;
		rightClickEventArg[1].argument.varFloat = mouseCoordinates_windowSpace.x;
		rightClickEventArg[2].argType = "coordY";
		rightClickEventArg[2].argument.varType = TYPE_FLOAT;
		rightClickEventArg[2].argument.varFloat = mouseCoordinates_windowSpace.y;
		rightClickEventArg[3].argType = "object";
		rightClickEventArg[3].argument.varType = TYPE_STRING;
		strcpy_s(rightClickEventArg[3].argument.varString, objectName);

		return Event(4, EVENT_TYPE_ON_CLICK, rightClickEventArg);
	}
	static Event EventOnLeftClick(const char *objectName,	
								  glm::ivec2 mouseCoordinates_windowSpace = glm::ivec2())
	{
		EventArg rightClickEventArg[4];
		rightClickEventArg[0].argType = "rightClick";
		rightClickEventArg[0].argument.varType = TYPE_BOOL;
		rightClickEventArg[0].argument.varBool = false; 
		rightClickEventArg[1].argType = "coordX";
		rightClickEventArg[1].argument.varType = TYPE_FLOAT;
		rightClickEventArg[1].argument.varFloat = mouseCoordinates_windowSpace.x;
		rightClickEventArg[2].argType = "coordY";
		rightClickEventArg[2].argument.varType = TYPE_FLOAT;
		rightClickEventArg[2].argument.varFloat = mouseCoordinates_windowSpace.y;
		rightClickEventArg[3].argType = "object";
		rightClickEventArg[3].argument.varType = TYPE_STRING;
		strcpy_s(rightClickEventArg[3].argument.varString, objectName);

		return Event(4, EVENT_TYPE_ON_CLICK, rightClickEventArg);
	}

	static Event EventOnButtonPressed(char button, const char *objectName)
	{
		EventArg buttonPressedEventArg[2];
		buttonPressedEventArg[0].argType = "button";
		buttonPressedEventArg[0].argument.varType = TYPE_INTEGER;
		buttonPressedEventArg[0].argument.varInteger = (int)button;
		buttonPressedEventArg[1].argType = "object";
		buttonPressedEventArg[1].argument.varType = TYPE_STRING;
		strcpy_s(buttonPressedEventArg[1].argument.varString, objectName);

		return Event(2, EVENT_TYPE_BUTTON_PRESSED, buttonPressedEventArg);
	}

	static Event EventOnHover()
	{
		return Event(EVENT_TYPE_ON_HOVER);
	}

	static Event EventOnHover(const char *objectName,	
							  glm::ivec2 mouseCoordinates_windowSpace = glm::ivec2())
	{
		EventArg hoveredEventArg[3];
		hoveredEventArg[0].argType = "coordX";
		hoveredEventArg[0].argument.varType = TYPE_FLOAT;
		hoveredEventArg[0].argument.varFloat = mouseCoordinates_windowSpace.x;
		hoveredEventArg[1].argType = "coordY";
		hoveredEventArg[1].argument.varType = TYPE_FLOAT;
		hoveredEventArg[1].argument.varFloat = mouseCoordinates_windowSpace.y;
		hoveredEventArg[2].argType = "object";
		hoveredEventArg[2].argument.varType = TYPE_STRING;
		strcpy_s(hoveredEventArg[2].argument.varString, objectName);

		return Event(3, EVENT_TYPE_ON_HOVER, hoveredEventArg);
	}

	static Event EmptyEvent()
	{
		return Event(EVENT_TYPE_EMPTY);
	}
};


#endif