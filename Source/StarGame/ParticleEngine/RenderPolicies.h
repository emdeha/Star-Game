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


#ifndef RENDER_POLICIES_H
#define RENDER_POLICIES_H


#include "../glsdk/glm/glm.hpp"
#include <vector>


#include "../framework/Utility.h"


template
<
	class ParticleType
>
class StandardRenderPolicy
{
public:
	void Render(glutil::MatrixStack &modelMatrix,
				glm::vec3 cameraPosition,
				const BillboardProgData &programData,
				Texture2D &texture,
				std::vector<ParticleType> particles)
	{
		glutil::PushStack push(modelMatrix);

		glUseProgram(programData.theProgram);

		glUniformMatrix4fv(programData.modelToCameraMatrixUnif,
						   1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform3f(programData.cameraPositionUnif,
					cameraPosition.x, cameraPosition.y, cameraPosition.z);

		texture.Bind(GL_TEXTURE0);

		for(int i = 0; i < particles.size(); i++)
		{
			glUniform3f(programData.deltaPositionUnif,
						particles[i].position.x, particles[i].position.y, particles[i].position.z);

			glDrawArrays(GL_POINTS, 0, 1);
		}

		glDisableVertexAttribArray(0);

		glUseProgram(0);
	}
};


#endif