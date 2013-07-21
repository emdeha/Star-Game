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


#ifndef INITIALIZATION_POLICIES_H
#define INITIALIZATION_POLICIES_H


#include "../ProgramData/ProgramData.h"

#include "../glsdk/glm/glm.hpp"
#include <vector>


template
<
	class ParticleType
>
class StandardInitPolicy
{
public:
	void Init(std::vector<ParticleType> &particles,
			  const BillboardProgData &programData)
	{
		for(int i = 0; i < particles.size(); i++)
		{
			particles[i].position = glm::vec3();
			particles[i].velocity = glm::vec3((float)rand() / RAND_MAX * 0.01f,
											  (float)rand() / RAND_MAX * 0.01f,
											  (float)rand() / RAND_MAX * 0.01f);
		}

		glUseProgram(programData.theProgram);
		glUniform1f(programData.billboardSizeUnif, 0.1f);
		glUseProgram(0);
	}
};




#endif