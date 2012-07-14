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
#include "PlanetBodies.h"

#define PI 3.14159f


Satellite::Satellite()
{
	if(mesh != NULL)
		mesh = NULL;
	revolutionDuration = Framework::Timer(Framework::Timer::TT_LOOP, 0.0f);
	position = glm::vec3();
	height = 0.0f;
	offsetFromSun = 0.0f;
	radius = 0.0f;
}
Satellite::Satellite(Framework::Timer newRevolutionDuration, 
					 float newHeight, float newOffsetFromSun, float newRadius)
{
	if(mesh != NULL)
		mesh = NULL;
	revolutionDuration = newRevolutionDuration;
	position = glm::vec3(0.0f, newHeight, 0.0f);
	height = newHeight;
	offsetFromSun = newOffsetFromSun;
	radius = newRadius;
}

void Satellite::LoadMesh(const std::string &fileName)
{
	try
	{
		mesh = new Framework::Mesh(fileName);
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}
}

void Satellite::Render(glutil::MatrixStack &modelMatrix, const ProgramData &data)
{
	glutil::PushStack push(modelMatrix);

	modelMatrix.Translate(position);
	modelMatrix.Scale(radius);

	glm::mat3 normMatrix(modelMatrix.Top());
	normMatrix = glm::transpose(glm::inverse(normMatrix));

	glUseProgram(data.theProgram);
	glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
	glUniformMatrix3fv(data.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));

	mesh->Render("lit");

	glUseProgram(0);
}
void Satellite::Update()
{
	revolutionDuration.Update();

	float currTimeThroughLoop = revolutionDuration.GetAlpha();

	position.x = cosf(currTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
	position.y = sinf(currTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
}


Sun::Sun()
{
	if(sunMesh != NULL)
		sunMesh = NULL;
	position = glm::vec3();
}
Sun::Sun(glm::vec3 newPosition)
{
	if(sunMesh != NULL)
		sunMesh = NULL;
	position = newPosition;
}

void Sun::LoadMesh(const std::string &fileName)
{
	try
	{
		sunMesh = new Framework::Mesh(fileName);
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}
}

void Sun::Render(glutil::MatrixStack &modelMatrix,
				 const ProgramData &litData, const UnlitProgData &unlitData)
{
	glutil::PushStack push(modelMatrix);

	modelMatrix.Translate(position);

	glUseProgram(unlitData.theProgram);
	glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
	glUniform4f(unlitData.objectColorUnif, 0.8078f, 0.8706f, 0.0f, 1.0f);

	sunMesh->Render("flat");

	int satellitesCount = satellites.size();
	for(int i = 0; i < satellitesCount; i++)
	{
		satellites[i]->Render(modelMatrix, litData);
	}
}
void Sun::Update()
{
	int satellitesCount = satellites.size();
	for(int i = 0; i < satellitesCount; i++)
	{
		satellites[i]->Update();
	}
}

void Sun::AddSatellite(const std::string &fileName, float height, float offset, float speed, float radius)
{
	Satellite *sat = new Satellite(Framework::Timer(Framework::Timer::TT_LOOP, speed), height, offset, radius);
	sat->LoadMesh(fileName);
	satellites.push_back(sat);
}

Sun::Sun(const Sun &other)
{
	sunMesh = new Framework::Mesh(*other.sunMesh);
	position = other.position;
}
Sun::~Sun()
{
	if(sunMesh != NULL)
	{
		delete sunMesh;
	}
}
Sun Sun::operator=(const Sun &other)
{
	if(this == &other)
	{
		return *this;
	}

	sunMesh = new Framework::Mesh(*other.sunMesh);
	position = other.position;

	return *this;
}