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


#ifndef UNIVERSE_H
#define UNIVERSE_H

#include <vector>

#include "../Entities/Lights.h"
#include "../Entities/PlanetBodies.h"

class Universe
{
private:
	std::vector<SunLight*> lights;
	std::vector<Sun*> suns;
	// std::vector<Enemies*> enemies;

public:
	Universe();

	void AddSunLight(SunLight *newSunLight);
	void AddSun(Sun *newSun);

	void UpdateUniverse();
	void RenderUniverse(glutil::MatrixStack &modelMatrix,
						const ProgramData &litData, const UnlitProgData &unLitData);

	// TODO: Add Load function
};

#endif