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

#include <algorithm>


#define PI 3.14159f


Satellite::Satellite()
{
	mesh.reset();
	parent.reset();
	revolutionDuration = Framework::Timer(Framework::Timer::TT_LOOP, 0.0f);
	position = glm::vec3();
	offsetFromSun = 0.0f;
	diameter = 0.0f;
	isClicked = false;
}
Satellite::Satellite(Framework::Timer newRevolutionDuration,
					 float newOffsetFromSun, float newDiameter)
{
	mesh.reset();
	parent.reset();
	revolutionDuration = newRevolutionDuration;
	position = glm::vec3();
	offsetFromSun = newOffsetFromSun;
	diameter = newDiameter;
	isClicked = false;
}

void Satellite::LoadMesh(const std::string &fileName)
{
	try
	{
		std::auto_ptr<Framework::Mesh> newMesh(new Framework::Mesh(fileName));
		mesh = newMesh;
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}
}

void Satellite::Render(glutil::MatrixStack &modelMatrix, const LitProgData &litData, 
														 const UnlitProgData &unlitData, 
														 const SimpleProgData &interpData)
{
	{
		glutil::PushStack push(modelMatrix);
		
		modelMatrix.Translate(position);
		modelMatrix.Scale(diameter);

		glm::mat3 normMatrix(modelMatrix.Top());
		normMatrix = glm::transpose(glm::inverse(normMatrix));

		glUseProgram(litData.theProgram);
		glUniformMatrix4fv(litData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniformMatrix3fv(litData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));
		glUniform4fv(litData.baseDiffuseColorUnif, 1, glm::value_ptr(glm::vec4(1.0f)));

		mesh->Render("lit");

		glUseProgram(0);
	}

	if(isClicked)
	{
		LoadClickedAnimation(modelMatrix, interpData);
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
	Utility::Ray mouseRay = 
		userMouse.GetPickRay(projMat, modelMat, cameraPos,
							 windowWidth, windowHeight);

	float outerRadius = offsetFromSun + diameter;
	float innerRadius = offsetFromSun - diameter;



	//*** HACKS HACKS HACKS ***\\

	// TODO: hack. Should look for a better solution. 
	//	     If not found, this suits the game's needs.
	// float precisionGain = 1.0f + i / 100.0f;
	// Solved. Will be left for further investigation
		
	glutil::MatrixStack distMat;
	distMat.Scale(1.0f, 1.0f, 5.0f);
	//*** HACKS HACKS HACKS ***\\

	
	glm::vec3 parentPosition = glm::vec3(parent->GetPosition());

	if(Utility::Intersections::RayIntersectsEllipsoid(mouseRay, parentPosition, outerRadius, distMat.Top()) && 
	   !Utility::Intersections::RayIntersectsEllipsoid(mouseRay, parentPosition, innerRadius, distMat.Top()))
	{
		isClicked = true;
		return true;
	}
	isClicked = false;
	return false;
}

void Satellite::LoadClickedAnimation(glutil::MatrixStack &modelMatrix, const SimpleProgData &interpData)
{
	// TODO: Play with blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// TODO: Initialization should be done elsewhere
	Utility::BasicMeshGeneration::Torus2D torus(
		glm::vec4(1.0f, 0.0f, 0.0f, 0.5f),
		parent->GetPosition(),
		offsetFromSun - diameter, 
		offsetFromSun + diameter, 90);

	torus.Init();
	torus.Draw(modelMatrix, interpData);

	glDisable(GL_BLEND);

	Utility::BasicMeshGeneration::Torus2D torusOut1(
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		parent->GetPosition(),
		offsetFromSun - diameter - 0.05f, 
		offsetFromSun - diameter, 90);

	torusOut1.Init();
	torusOut1.Draw(modelMatrix, interpData);

	Utility::BasicMeshGeneration::Torus2D torusOut2(
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		parent->GetPosition(),
		offsetFromSun + diameter, 
		offsetFromSun + diameter + 0.05f, 90);

	torusOut2.Init();
	torusOut2.Draw(modelMatrix, interpData);
}


inline void Satellite::SetParent(const Sun &newParent)
{
	std::auto_ptr<Sun> pointedParent(new Sun(newParent));
	parent = pointedParent;
}


Satellite::Satellite(const Satellite &other)
{
	std::auto_ptr<Framework::Mesh> newMesh(new Framework::Mesh(*other.mesh));
	mesh = newMesh;
	std::auto_ptr<Sun> newParent(new Sun(*other.parent));
	parent = newParent;

	revolutionDuration = other.revolutionDuration;
	position = other.position;
	offsetFromSun = other.offsetFromSun;
	diameter = other.diameter;
}
Satellite::~Satellite()
{
}
Satellite Satellite::operator=(const Satellite &other)
{
	if(this == &other)
	{
		return *this;
	}
	
	std::auto_ptr<Framework::Mesh> newMesh(new Framework::Mesh(*other.mesh));
	mesh = newMesh;
	std::auto_ptr<Sun> newParent(new Sun(*other.parent));
	parent = newParent;

	revolutionDuration = other.revolutionDuration;
	position = other.position;
	offsetFromSun = other.offsetFromSun;
	diameter = other.diameter;

	return *this;
}



Sun::Sun()
{
	sunMesh.reset();
	satellites.resize(0);
	position = glm::vec3();
	diameter = 0.0f;
	satelliteCap = 0;
	isClicked = false;
}
Sun::Sun(glm::vec3 newPosition, float newDiameter, int newSatelliteCap)
{
	sunMesh.reset();
	satellites.resize(0);
	position = newPosition;
	diameter = newDiameter;
	satelliteCap = newSatelliteCap;
	isClicked = false;
}

void Sun::LoadMesh(const std::string &fileName)
{
	try
	{
		std::auto_ptr<Framework::Mesh> newSunMesh(new Framework::Mesh(fileName));
		sunMesh = newSunMesh;
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}
}

void Sun::Render(glutil::MatrixStack &modelMatrix,
				 const LitProgData &litData, 
				 const UnlitProgData &unlitData, 
				 const SimpleProgData &interpData)
{
	{
		glutil::PushStack push(modelMatrix);
		
		modelMatrix.Translate(position);		

		int satellitesCount = satellites.size();
		for(int i = 0; i < satellitesCount; i++)
		{
			satellites[i]->Render(modelMatrix, litData, unlitData, interpData);
		}
				
		modelMatrix.Scale(diameter);

		glUseProgram(unlitData.theProgram);
		glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform4f(unlitData.objectColorUnif, 0.8078f, 0.8706f, 0.0f, 1.0f);

		sunMesh->Render("flat");
	}	

	if(isClicked)
	{

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

bool Sun::AddSatellite(const std::string &fileName, float offset, float speed, float diameter)
{
	if(satellites.size() >= satelliteCap)
	{
		return false;
	}

	Satellite 
		*sat(new Satellite(Framework::Timer(Framework::Timer::TT_LOOP, speed), offset, diameter));
	sat->LoadMesh(fileName);
	sat->SetParent(*this);
	satellites.push_back(sat);

	return true;
}

bool Sun::RemoveSatellite()
{
	if(satellites.empty())
	{
		return false;
	}

	satellites.pop_back();

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
		userMouse.GetPickRay(projMat, modelMat, cameraPos,
							 windowWidth, windowHeight);

	if(Utility::Intersections::RayIntersectsSphere(mouseRay, position, diameter / 2.0f))
	{
		isClicked = true;
		return true;
	}
	isClicked = false;
	return false;
}

void Sun::IsSatelliteClicked(glm::mat4 projMat, glm::mat4 modelMat, 
							 Mouse userMouse, glm::vec4 cameraPos,
							 float windowWidth, float windowHeight)
{
	for(int i = 0; i < satellites.size(); i++)
	{
		Utility::Ray mouseRay = 
			userMouse.GetPickRay(projMat, modelMat, cameraPos,
								 windowWidth, windowHeight);

		float outerRadius = satellites[i]->GetOffsetFromSun() + satellites[i]->GetDiameter();
		float innerRadius = satellites[i]->GetOffsetFromSun() - satellites[i]->GetDiameter();



		//*** HACKS HACKS HACKS ***\\

		// TODO: hack. Should look for a better solution. 
		//	     If not found, this suits the game's needs.
		// float precisionGain = 1.0f + i / 100.0f;
		// Solved. Will be left for further investigation
		
		glutil::MatrixStack distMat;
		distMat.Scale(1.0f, 1.0f, 5.0f);
		//*** HACKS HACKS HACKS ***\\



		if(Utility::Intersections::RayIntersectsEllipsoid(mouseRay, position, outerRadius, distMat.Top()) && 
		   !Utility::Intersections::RayIntersectsEllipsoid(mouseRay, position, innerRadius, distMat.Top()))
		{
			satellites[i]->SetIsClicked(true);
		}
		else satellites[i]->SetIsClicked(false);
	}
}


Sun::Sun(const Sun &other)
{
	std::auto_ptr<Framework::Mesh> newSunMesh(new Framework::Mesh(*other.sunMesh));
	sunMesh = newSunMesh;

	satellites = other.satellites;
	position = other.position;
	diameter = other.diameter;
	isClicked = other.isClicked;
}

template <typename T>
void DeletePointedTo(T *pointedObject)
{
	delete pointedObject;
}

Sun::~Sun()
{
	std::for_each(satellites.begin(), satellites.end(), DeletePointedTo<Satellite>);
}

Sun Sun::operator=(const Sun &other)
{
	if(this == &other)
	{
		return *this;
	}

	std::auto_ptr<Framework::Mesh> newSunMesh(new Framework::Mesh(*other.sunMesh));
	sunMesh = newSunMesh;

	satellites = other.satellites;
	diameter = other.diameter;
	position = other.position;
	isClicked = other.isClicked;

	return *this;
}