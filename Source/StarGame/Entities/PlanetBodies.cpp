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
	if(parent != NULL)
		parent = NULL;
	revolutionDuration = Framework::Timer(Framework::Timer::TT_LOOP, 0.0f);
	position = glm::vec3();
	height = 0.0f;
	offsetFromSun = 0.0f;
	diameter = 0.0f;
	isClicked = false;
	animationID = '\0';
	fastTimer = Framework::Timer(Framework::Timer::TT_LOOP, 2.0f);
}
Satellite::Satellite(Framework::Timer newRevolutionDuration, 
					 float newHeight, float newOffsetFromSun, float newDiameter)
{
	if(mesh != NULL)
		mesh = NULL;
	if(parent != NULL)
		parent = NULL;
	revolutionDuration = newRevolutionDuration;
	position = glm::vec3();
	height = newHeight;
	offsetFromSun = newOffsetFromSun;
	diameter = newDiameter;
	isClicked = false;
	animationID = '\0';
	fastTimer = Framework::Timer(Framework::Timer::TT_LOOP, 2.0f);
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

void Satellite::Render(glutil::MatrixStack &modelMatrix, const ProgramData &data, const UnlitProgData &unlitData, const InterpProgData &interpData)
{
	{
		glutil::PushStack push(modelMatrix);

		modelMatrix.Translate(position);
		modelMatrix.Scale(diameter);

		glm::mat3 normMatrix(modelMatrix.Top());
		normMatrix = glm::transpose(glm::inverse(normMatrix));

		glUseProgram(data.theProgram);
		glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniformMatrix3fv(data.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));
		glUniform4fv(data.baseDiffuseColorUnif, 1, glm::value_ptr(glm::vec4(1.0f)));

		mesh->Render("lit");

		glUseProgram(0);
	}

	if(isClicked)
	{
		LoadClickedAnimation(modelMatrix, data, unlitData, interpData);
	}
}
void Satellite::Update()
{
	revolutionDuration.Update();

	float currTimeThroughLoop = revolutionDuration.GetAlpha();

	position.x = cosf(currTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
	position.y = sinf(currTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
}

bool Satellite::IsClicked(glm::mat4 projMat, glm::mat4 modelMat, 
						  Mouse userMouse, glm::vec4 cameraPos,
						  float windowWidth, float windowHeight)
{
	glm::vec4 parentPosition_worldSpace = modelMat * parent->GetPosition();

	Utility::Ray mouseRay = 
		userMouse.GetPickRay(projMat, modelMat, cameraPos, parentPosition_worldSpace, 
							 windowHeight, windowWidth);

	float outerRadius = glm::length(position) + diameter / 2.0f;
	float innerRadius = glm::length(position) - diameter / 2.0f;

	if(Utility::Intersections::RayIntersectsSphere(mouseRay, outerRadius) && 
	   !Utility::Intersections::RayIntersectsSphere(mouseRay, innerRadius))
	{
		isClicked = true;
		return true;
	}
	isClicked = false;
	return false;
}

#define UNLIT

void Satellite::LoadClickedAnimation(glutil::MatrixStack &modelMatrix, const ProgramData &data, const UnlitProgData &unlitData, const InterpProgData &interpData)
{
	switch(animationID)
	{
		case 'a':
		{
			float currTimeThroughLoop = revolutionDuration.GetAlpha();
			
			{
				glutil::PushStack push(modelMatrix);
		
				modelMatrix.Translate(position);	
				modelMatrix.RotateZ(80.0f);
				modelMatrix.RotateZ(glm::degrees(currTimeThroughLoop * 2 * PI));	
				modelMatrix.Translate(-0.25f, 0.0f, 0.0f);
				modelMatrix.Scale(0.5f);
				modelMatrix.RotateX(90.0f);

				glm::mat3 normMatrix = glm::mat3(modelMatrix.Top());
				normMatrix = glm::transpose(glm::inverse(normMatrix));
				
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef LIT
				glUseProgram(data.theProgram);
				glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				glUniformMatrix3fv(data.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));
#elif defined UNLIT
				glUseProgram(unlitData.theProgram);
				glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				glUniform4fv(unlitData.objectColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)));
#else
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glUseProgram(interpData.theProgram);
				glUniformMatrix4fv(interpData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				//glUniform4fv(interpData.startColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
#endif

				Framework::Mesh *effectMesh = new Framework::Mesh("UnitPlane.xml");
				effectMesh->Render();

				glUseProgram(0);
			}
			{
				glutil::PushStack push(modelMatrix);
		
				modelMatrix.Translate(position);	
				modelMatrix.RotateZ(-80.0f);
				modelMatrix.RotateZ(glm::degrees(currTimeThroughLoop * 2 * PI));	
				modelMatrix.Translate(-0.25f, 0.0f, 0.0f);
				modelMatrix.Scale(0.5f);
				modelMatrix.RotateX(90.0f);

				glm::mat3 normMatrix = glm::mat3(modelMatrix.Top());
				normMatrix = glm::transpose(glm::inverse(normMatrix));

#ifdef LIT
				glUseProgram(data.theProgram);
				glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				glUniformMatrix3fv(data.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));
#elif defined UNLIT
				glUseProgram(unlitData.theProgram);
				glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				glUniform4fv(unlitData.objectColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f)));
#else
				glUseProgram(interpData.theProgram);
				glUniformMatrix4fv(interpData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				//glUniform4fv(interpData.startColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
#endif

				Framework::Mesh *effectMesh = new Framework::Mesh("UnitPlane.xml");
				effectMesh->Render();

				glUseProgram(0);

				glDisable(GL_BLEND);
			}
		}
		break;
		case 'b':
		{
			fastTimer.Update();

			float currTimeThroughLoop = fastTimer.GetAlpha();
			glm::vec3 littlePos;

			{
				glutil::PushStack push(modelMatrix);

				littlePos.x = cosf(currTimeThroughLoop * (2.0f * PI)) * (offsetFromSun - diameter / 2.0f);
				littlePos.y = sinf(currTimeThroughLoop * (2.0f * PI)) * (offsetFromSun - diameter / 2.0f);

				modelMatrix.Translate(littlePos);
				modelMatrix.Scale(0.2f);

				glm::mat3 normMatrix(modelMatrix.Top());
				normMatrix = glm::transpose(glm::inverse(normMatrix));

#ifndef UNLIT
				glUseProgram(data.theProgram);
				glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				glUniformMatrix3fv(data.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));
				glUniform4fv(data.baseDiffuseColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
#else
				glUseProgram(unlitData.theProgram);
				glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				glUniform4fv(unlitData.objectColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
#endif

				mesh->Render("lit");

				glUseProgram(0);
			}
			{
				glutil::PushStack push(modelMatrix);

				littlePos.x = cosf(currTimeThroughLoop * (2.0f * PI)) * (offsetFromSun + diameter / 2.0f);
				littlePos.y = sinf(currTimeThroughLoop * (2.0f * PI)) * (offsetFromSun + diameter / 2.0f);

				modelMatrix.Translate(littlePos);
				modelMatrix.Scale(0.2f);

				glm::mat3 normMatrix = glm::mat3(modelMatrix.Top());
				normMatrix = glm::transpose(glm::inverse(normMatrix));

#ifndef UNLIT
				glUseProgram(data.theProgram);
				glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				glUniformMatrix3fv(data.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));
				glUniform4fv(data.baseDiffuseColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
#else
				glUseProgram(unlitData.theProgram);
				glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
				glUniform4fv(unlitData.objectColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
#endif
			
				mesh->Render("lit");

				glUseProgram(0);
			}
		}
		break;
		case 'c':
		{	
			fastTimer.Update();

			float currTimeThroughLoop = fastTimer.GetAlpha();
			glm::vec3 littlePos;
			
			glutil::PushStack push(modelMatrix);

			modelMatrix.Translate(position);

			littlePos.x = cosf(currTimeThroughLoop * (2.0f * PI)) * diameter;
			littlePos.y = sinf(currTimeThroughLoop * (2.0f * PI)) * diameter;

			modelMatrix.Translate(littlePos);
			modelMatrix.Scale(0.5f);

			glm::mat3 normMatrix = glm::mat3(modelMatrix.Top());
			normMatrix = glm::transpose(glm::inverse(normMatrix));

#ifndef UNLIT
			glUseProgram(data.theProgram);
			glUniformMatrix4fv(data.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
			glUniformMatrix3fv(data.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));
			glUniform4fv(data.baseDiffuseColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
#else
			glUseProgram(unlitData.theProgram);
			glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
			glUniform4fv(unlitData.objectColorUnif, 1, glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
#endif
			
			mesh->Render("lit");

			glUseProgram(0);
		}
		break;
	}
}

Satellite::Satellite(const Satellite &other)
{
	mesh = new Framework::Mesh(*other.mesh);
	parent = new Sun(*other.parent);
	revolutionDuration = other.revolutionDuration;
	position = other.position;
	height = other.height;
	offsetFromSun = other.offsetFromSun;
	diameter = other.diameter;
}
Satellite::~Satellite()
{
	if(mesh != NULL)
	{
		delete mesh;
	}
	if(parent != NULL)
	{
		delete parent;
	}
}
Satellite Satellite::operator=(const Satellite &other)
{
	if(this == &other)
	{
		return *this;
	}

	mesh = new Framework::Mesh(*other.mesh);
	parent = new Sun(*other.parent);
	revolutionDuration = other.revolutionDuration;
	position = other.position;
	height = other.height;
	offsetFromSun = other.offsetFromSun;
	diameter = other.diameter;

	return *this;
}



Sun::Sun()
{
	if(sunMesh != NULL)
		sunMesh = NULL;
	position = glm::vec3();
	diameter = 0.0f;
}
Sun::Sun(glm::vec3 newPosition, float newDiameter)
{
	if(sunMesh != NULL)
		sunMesh = NULL;
	position = newPosition;
	diameter = newDiameter;
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
				 const ProgramData &litData, const UnlitProgData &unlitData, const InterpProgData &interpData)
{
	{
		glutil::PushStack push(modelMatrix);
		
		modelMatrix.Translate(position);
		modelMatrix.Scale(diameter);

		glUseProgram(unlitData.theProgram);
		glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform4f(unlitData.objectColorUnif, 0.8078f, 0.8706f, 0.0f, 1.0f);

		sunMesh->Render("flat");

		int satellitesCount = satellites.size();
		for(int i = 0; i < satellitesCount; i++)
		{
			satellites[i]->Render(modelMatrix, litData, unlitData, interpData);
		}
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

bool Sun::AddSatellite(const std::string &fileName, float height, float offset, float speed, float diameter)
{
	if(satellites.size() >= 4)
	{
		return false;
	}

	Satellite *sat = new Satellite(Framework::Timer(Framework::Timer::TT_LOOP, speed), height, offset, diameter);
	sat->LoadMesh(fileName);
	sat->SetParent(this);
	satellites.push_back(sat);

	return true;
}

std::vector<Satellite*> Sun::GetSatellites()
{
	return satellites;
}

bool Sun::IsClicked(glm::mat4 projMat, glm::mat4 modelMat, 
				    Mouse userMouse, glm::vec4 cameraPos,
				    float windowWidth, float windowHeight)
{
	Utility::Ray mouseRay = 
		userMouse.GetPickRay(projMat, modelMat, cameraPos, glm::vec4(position, 1.0f), 
							 windowHeight, windowWidth);

	if(Utility::Intersections::RayIntersectsSphere(mouseRay, diameter / 2.0f))
	{
		return true;
	}
	return false;
}


Sun::Sun(const Sun &other)
{
	sunMesh = new Framework::Mesh(*other.sunMesh);
	satellites = other.satellites;
	position = other.position;
	diameter = other.diameter;
}
Sun::~Sun()
{
	if(sunMesh != NULL)
	{
		delete sunMesh;
	}
	if(!satellites.empty())
	{
		satellites.clear();
	}
}
Sun Sun::operator=(const Sun &other)
{
	if(this == &other)
	{
		return *this;
	}

	sunMesh = new Framework::Mesh(*other.sunMesh);
	satellites = other.satellites;
	diameter = other.diameter;
	position = other.position;

	return *this;
}