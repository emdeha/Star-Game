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


#ifndef UTILITY_H
#define UTILITY_H

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))


#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/Timer.h"
#include <glm/glm.hpp>
#include <vector>

#include "../ProgramData/ProgramData.h"

namespace Utility
{
	class Ray
	{
	public:
		glm::vec4 origin;
		glm::vec4 direction;

		Ray();
		Ray(glm::vec4 newOrigin, glm::vec4 newDirection);
	};

	namespace Intersections
	{
		bool RayIntersectsSphere(Ray mouseRay, float sphereRadius);
	}

	namespace BasicMeshGeneration
	{
		class Torus2D
		{
		private:
			float innerRadius;
			float outerRadius;
			short resolution;

			static void Generate(Torus2D &other);

		public:
			Torus2D(float newInnerRadius, float newOuterRadius, 
					int newResolution);

			void Init();
			void Draw(glutil::MatrixStack &modelMatrix, const InterpProgData &data, 
					  unsigned int positionAttrib);

			unsigned int GetVertexBO();
			unsigned int GetIndexBO();
			unsigned int GetVao();

		public:		
			unsigned int vao;
			unsigned int indexBO;
			unsigned int vertexBO;

			std::vector<float> vertexData;
			// TODO: Use pointer, vector or sth like that
			unsigned short indexData[724];
		};
	}
}

#endif