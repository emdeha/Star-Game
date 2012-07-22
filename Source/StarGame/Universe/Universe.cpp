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
#include "Universe.h"

Universe::Universe()
{
	lights.resize(0);
	suns.resize(0);
}

void Universe::AddSunLight(SunLight *newSunLight)
{
	lights.push_back(newSunLight);
}
void Universe::AddSun(Sun *newSun)
{
	suns.push_back(newSun);
}

void Universe::RenderUniverse(glutil::MatrixStack &modelMatrix,
							  const ProgramData &litData, const UnlitProgData &unLitData, const InterpProgData &interpData)
{
	int sizeLights = lights.size();
	for(int i = 0; i < sizeLights; i++)
	{
		lights[i]->Render(modelMatrix, litData);
	}

	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		suns[i]->Render(modelMatrix, litData, unLitData, interpData);
	}
}
void Universe::UpdateUniverse()
{
	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		suns[i]->Update();
	}
}