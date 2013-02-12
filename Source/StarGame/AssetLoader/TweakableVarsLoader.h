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


#ifndef TWEAKABLE_VARS_LOADER_H
#define TWEAKABLE_VARS_LOADER_H


#include <vector>
#include <string>


struct TweakVarData
{
	enum VarType
	{
		TYPE_TWEAK_FLOAT,
		TYPE_TWEAK_INT,
	};

	int itemIndex; // for indexed commands. -1 for non existence
	VarType currentType;

	union 
	{
		float varFloat;
		int varInt;
	};

	TweakVarData() : itemIndex(-1) {}
};

class TweakableVarsLoader
{
private:
	std::vector<std::pair<std::string, TweakVarData>> loadedTweaks;

public:
	TweakableVarsLoader() {}

	TweakableVarsLoader(const std::string &fileName);

	const std::vector<std::pair<std::string, TweakVarData>> GetAllLoadedVars();
};


#endif