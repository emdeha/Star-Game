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
	FusionSequence(char buttonA = '\0', char buttonB = '\0', char buttonC = '\0');

	std::string GetButtons();

	Event Update(const std::string &currentInputSequence);
};



class FusionInput
{
private:
	std::vector<std::pair<std::string, FusionSequence>> sequences;

	std::string currentInputSequence;
	char sequenceEndButton;
	char validInputSequences[3];

public:
	FusionInput();
	FusionInput(char newSequenceEndButton);

	void AddSequence(std::string sequenceName,
					 char buttonA = ' ', char buttonB = ' ', char buttonC = ' ');

	std::string GetSequenceButtons(std::string sequenceName);
	std::string GetSequenceName(std::string sequenceButtons);
	std::string GetCurrentInputSequence();

	Event Update(char newButton);

	void Clear();
};


#endif