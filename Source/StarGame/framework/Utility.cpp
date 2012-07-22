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


#include "Utility.h"
#include <memory>

#define PI 3.14159f

Utility::Ray::Ray()
{
	origin = glm::vec4();
	direction = glm::vec4();
}
Utility::Ray::Ray(glm::vec4 newOrigin, glm::vec4 newDirection)
{
	origin = newOrigin;
	direction = newDirection;
}


bool Utility::Intersections::RayIntersectsSphere(Ray mouseRay, float sphereRadius)
{
	float a = glm::dot(mouseRay.direction, mouseRay.direction);
	float b = glm::dot(mouseRay.direction, mouseRay.origin) * 2.0f;
	float c = glm::dot(mouseRay.origin, mouseRay.origin) - sphereRadius * sphereRadius;

	float discriminant = b * b - 4 * a * c;

	if(discriminant >= 0.0f)
	{
		return true;
	}
	else 
		return false;
}


Utility::BasicMeshGeneration::Torus2D::Torus2D(float newInnerRadius, float newOuterRadius, 
											   int newResolution)
{
	innerRadius = newInnerRadius;
	outerRadius = newOuterRadius;
	resolution = newResolution;

	//vertexData.resize(0);

	vao = 0;
	indexBO = 0;
	vertexBO = 0;
}

void Utility::BasicMeshGeneration::Torus2D::Init()
{
	std::vector<float> vertexData;
	std::vector<unsigned short> indexData;

	short currentCoord = 0;
	unsigned short currentIndex = 0;
	int currentIndexPos = 0;

	for(int i = 0; i <= 360; i += 360 / resolution)
	{		
		vertexData.push_back(cosf(i * (PI / 180.0f)) * outerRadius);
		vertexData.push_back(0.0f);
		vertexData.push_back(sinf(i * (PI / 180.0f)) * outerRadius);
		vertexData.push_back(1.0f);
		
		vertexData.push_back(cosf(i * (PI / 180.0f)) * innerRadius);
		vertexData.push_back(0.0f);
		vertexData.push_back(sinf(i * (PI / 180.0f)) * innerRadius);
		vertexData.push_back(1.0f);

		indexData.push_back(currentIndex + 1);
		indexData.push_back(currentIndex + 2);		

		currentIndexPos += 2;
		currentCoord += 8;
		currentIndex += 2;
	}

	indexData[currentIndexPos - 1] = 1;
	indexData.push_back(2);
	indexData.push_back(currentIndex - 2);



	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 sizeof(float) * vertexData.size(), &vertexData[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 sizeof(unsigned short) * indexData.size(), &indexData[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);

	glBindVertexArray(0);
}


void Utility::BasicMeshGeneration::Torus2D::Draw(glutil::MatrixStack &modelMatrix, const InterpProgData &data)
{
	glUseProgram(data.theProgram);
	glBindVertexArray(vao);
	{
		glutil::PushStack push(modelMatrix);

		modelMatrix.RotateX(90.0f);

		glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform4f(data.colorUnif, 1.0f, 0.0f, 0.0f, 1.0f);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glEnableVertexAttribArray(data.positionAttrib);
		glVertexAttribPointer(data.positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawElements(GL_TRIANGLE_STRIP, resolution * 4, GL_UNSIGNED_SHORT, 0);
	}
	glDisableVertexAttribArray(0);
	glUseProgram(0);
}