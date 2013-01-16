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


static int FindEmptySatelliteType()
{
	for(int type = 0; type < 4; type++)
	{
		if(typesTable[type] == false)
		{
			typesTable[type] = true;
			return type;
		}
	}
}



SatelliteOrbit::SatelliteOrbit()
{
	mainColor = glm::vec4();
	outlineColor = glm::vec4();

	position = glm::vec4();

	outerRadius = 0.0f;
	innerRadius = 0.0f;
}

SatelliteOrbit::SatelliteOrbit(glm::vec4 newMainColor, glm::vec4 newOutlineColor,
							   glm::vec4 newPosition, 
							   float newOuterRadius, float newInnerRadius,
							   float gamma)
{
	mainColor = Utility::CorrectGamma(newMainColor, gamma);
	outlineColor = Utility::CorrectGamma(newOutlineColor, gamma);
	position = newPosition;
	outerRadius = newOuterRadius;
	innerRadius = newInnerRadius;

	mainOrbit = Utility::Primitives::Torus2D(mainColor,
													  position,
													  innerRadius,
													  outerRadius,
													  90);
	orbitOutlineOne = Utility::Primitives::Torus2D(outlineColor,
															position,
															innerRadius - 0.05f,
															innerRadius,
															90);
	orbitOutlineTwo = Utility::Primitives::Torus2D(outlineColor,
															position,
															outerRadius,
															outerRadius + 0.05f,
															90);
	mainOrbit.Init();
	orbitOutlineOne.Init();
	orbitOutlineTwo.Init();
}

void SatelliteOrbit::Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &simpleData)
{
	// TODO: Play with blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mainOrbit.Draw(modelMatrix, simpleData);

	glDisable(GL_BLEND);

	orbitOutlineOne.Draw(modelMatrix, simpleData);
	orbitOutlineTwo.Draw(modelMatrix, simpleData);
}


static void GenerateUniformBuffers(int &materialBlockSize, glm::vec4 diffuseColor, GLuint &materialUniformBuffer)
{
	MaterialBlock material;
	material.diffuseColor = diffuseColor;
	material.specularColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	material.shininessFactor = 0.3f;


	int uniformBufferAlignSize = 0;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);

	materialBlockSize = sizeof(MaterialBlock);
	materialBlockSize += uniformBufferAlignSize -
		(materialBlockSize % uniformBufferAlignSize);

	
	glGenBuffers(1, &materialUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, materialUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, &material, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


Satellite::Satellite(Framework::Timer newRevolutionDuration,
					 glm::vec4 newColor,
					 float newOffsetFromSun, float newDiameter,
					 SatelliteType satelliteType, int newHealth)
{
	mesh.reset();
	parent.reset();
	revolutionDuration = newRevolutionDuration;
	color = newColor;
	position = glm::vec3();
	skillType.satelliteOffsetFromSun = newOffsetFromSun;
	skillType.satelliteTypeForSkill = satelliteType;
	diameter = newDiameter;
	isClicked = false;
	isDisabled = false;
	health = newHealth;

	GenerateUniformBuffers(materialBlockSize, color, materialUniformBuffer);
}

void Satellite::LoadMesh(const std::string &fileName)
{
	try
	{
		mesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(fileName));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}
}

void Satellite::Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex, 
					   float gamma,
					   const LitProgData &litData, 
					   const UnlitProgData &unlitData, 
					   const SimpleProgData &interpData,
					   float interpolation)
{
	{
		glutil::PushStack push(modelMatrix);
		

		modelMatrix.Translate(position);
		modelMatrix.Scale(diameter);


		glBindBufferRange(GL_UNIFORM_BUFFER, materialBlockIndex, materialUniformBuffer,
							0, sizeof(MaterialBlock));

		glm::mat3 normMatrix(modelMatrix.Top());
		normMatrix = glm::transpose(glm::inverse(normMatrix));
		
		glUseProgram(litData.theProgram);
		glUniformMatrix4fv(litData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniformMatrix3fv(litData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));

		mesh->Render("lit");

		glUseProgram(0);

		glBindBufferBase(GL_UNIFORM_BUFFER, materialBlockIndex, 0);
	}

	// TODO: Put in the OnEvent function.
	if(isClicked)
	{
		LoadClickedAnimation(modelMatrix, interpData, gamma);
	}
}
void Satellite::Update()
{
	revolutionDuration.Update();

	float currTimeThroughLoop = revolutionDuration.GetAlpha();
	float offsetFromSun = skillType.satelliteOffsetFromSun;

	position.x = cosf(currTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
	position.y = sinf(currTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
}

glm::vec3 Satellite::GetVelocity()
{
	float currTimeThroughLoop = revolutionDuration.GetAlpha();
	float offsetFromSun = skillType.satelliteOffsetFromSun;

	glm::vec3 satelliteVelocity;
	satelliteVelocity.x = cosf(currTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
	satelliteVelocity.y = sinf(currTimeThroughLoop * (2.0f * PI)) * offsetFromSun;

	return satelliteVelocity;
}

void Satellite::Disable()
{

}

void Satellite::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ON_CLICK:
		std::printf("Satellite clicked!\n");
		isClicked = true;
		break;
	case EVENT_TYPE_ON_HOVER:
		std::printf("Satellite hovered!\n");
		isClicked = true;
		break;
	case EVENT_TYPE_ATTACKED:
		health -= _event.GetArgument("damage").varInteger;
		break;
	default:
		break;
	};
}

bool Satellite::IsClicked(glm::mat4 projMat, glm::mat4 modelMat, 
						  Mouse userMouse, glm::vec4 cameraPos,
						  int windowWidth, int windowHeight)
{
	Utility::Ray mouseRay = 
		userMouse.GetPickRay(projMat, modelMat, cameraPos,
							 windowWidth, windowHeight);

	float offsetFromSun = skillType.satelliteOffsetFromSun;

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
		//isClicked = true;
		return true;
	}
    isClicked = false;
	return false;
}

void Satellite::LoadClickedAnimation(glutil::MatrixStack &modelMatrix, const SimpleProgData &simpleData, float gamma)
{
	float offsetFromSun = skillType.satelliteOffsetFromSun;

	SatelliteOrbit orbit(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
						 parent->GetPosition(), 
						 offsetFromSun + diameter, offsetFromSun - diameter,
						 gamma);

	
	glutil::PushStack push(modelMatrix);
	modelMatrix.RotateX(90.0f);

	orbit.Draw(modelMatrix, simpleData);
}


inline void Satellite::SetParent(const Sun &newParent)
{
	parent = std::unique_ptr<Sun>(new Sun(newParent));
}


void Satellite::Stop()
{
	revolutionDuration.SetPause();
}
void Satellite::Start()
{
	revolutionDuration.SetPause(false);
}


Sun::Sun()
{
	sunMesh.reset();
	satellites.resize(0);
	generatedEvents.resize(0);
	color = glm::vec4();
	position = glm::vec3();
	diameter = 0.0f;
	satelliteCap = 0;
	isClicked = false;
	isSatelliteClicked = false;
	health = 0;
	currentSatelliteType = 0;
}
Sun::Sun(glm::vec3 newPosition, glm::vec4 newColor,
		 float newDiameter, int newSatelliteCap, int newHealth)
{
	sunMesh.reset();
	satellites.resize(0);
	generatedEvents.resize(0);
	color = newColor;
	position = newPosition;
	diameter = newDiameter;
	satelliteCap = newSatelliteCap;
	isClicked = false;
	isSatelliteClicked = false;
	currentSatelliteType = 0;
	health = newHealth;
}

void Sun::LoadMesh(const std::string &fileName)
{
	try
	{
		sunMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(fileName));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}
}

void Sun::Render(glutil::MatrixStack &modelMatrix, GLuint materialBlockIndex,
				 float gamma,
				 const LitProgData &litData, 
				 const UnlitProgData &unlitData, 
				 const SimpleProgData &interpData,
				 float interpolation)
{
	{
		glutil::PushStack push(modelMatrix);
		
		modelMatrix.Translate(position);		

		int satellitesCount = satellites.size();
		for(std::vector<std::shared_ptr<Satellite>>::iterator iter = satellites.begin();
			iter != satellites.end();
			++iter)
		{
			(*iter)->Render(modelMatrix, 
							materialBlockIndex, 
							gamma, 
							litData, unlitData, interpData,
							interpolation);
		}
				
		modelMatrix.Scale(diameter);

		glm::vec4 sunColor = Utility::CorrectGamma(color, gamma);

		glUseProgram(unlitData.theProgram);
		glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform4f(unlitData.objectColorUnif, sunColor.x, sunColor.y, sunColor.z, sunColor.w);

		sunMesh->Render("flat");
	}	

	if(isClicked)
	{
		isClicked = false;
	}
}
void Sun::Update()
{
	for(std::vector<std::shared_ptr<Satellite>>::iterator iter = satellites.begin();
		iter != satellites.end();
		++iter)
	{
		if((*iter)->GetHealth() <= 0)
		{
			typesTable[(*iter)->GetSatelliteType()] = false;
			RemoveSatellite(iter);
			break;
		}
	}

	for(std::vector<std::shared_ptr<Satellite>>::iterator iter = satellites.begin();
		iter != satellites.end();
		++iter)
	{
		(*iter)->Update();
	}
}

void Sun::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ON_CLICK:
		if(_event.GetArgument("rightClick").varBool == true)
		{
			if(!this->RemoveSatellite())
			{
				std::printf("No satellites left.\n");
			}
			else
			{
				typesTable[satellites.size()] = false;
			}
		}
		else if(_event.GetArgument("rightClick").varBool == false)
		{
			if(this->AddSatellite("mesh-files/UnitSphere.xml", 
								  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
								  10.0f, 0.5f,
								  SatelliteType(FindEmptySatelliteType()), 
								  5))
			{
				//currentSatelliteType = FindEmptySatelliteType();
			}
			else
			{
				std::printf("Satellite cap reached!\n");
			}
		}
		break;
	case EVENT_TYPE_ATTACKED:
		if(_event.GetArgument("bodyIndex").varInteger == -1)
		{
			if(satellites.empty())
			{
				health -= _event.GetArgument("damage").varInteger;
			}
			// The commented event can't be sent to the universe. 
			// That is due to the lack of a global event queue.
			if(health <= 0)
			{
				EventArg sunKilledEventArg[2];
				sunKilledEventArg[0].argType = "what";
				sunKilledEventArg[0].argument.varType = TYPE_STRING;
				strcpy(sunKilledEventArg[0].argument.varString, "sun"); 
				sunKilledEventArg[1].argType = "index";
				sunKilledEventArg[1].argument.varType = TYPE_INTEGER;
				sunKilledEventArg[1].argument.varInteger = 0; // Should be some unique index.
				Event sunKilledEvent(2, EVENT_TYPE_KILLED, sunKilledEventArg);

				generatedEvents.push_back(sunKilledEvent);
			}
		}
		else 
		{
			for(std::vector<std::shared_ptr<Satellite>>::iterator iter = satellites.begin();
				iter != satellites.end(); 
				++iter)
			{
				if((*iter)->GetOffsetFromSun() == _event.GetArgument("bodyIndex").varFloat)
				{
					(*iter)->OnEvent(_event);
					break;
				}
			}
		}
	default:
		break;
	}
}

bool Sun::AddSatellite(const std::string &fileName, 
					   glm::vec4 satelliteColor,
					   float speed, float diameter,
					   SatelliteType type, int satelliteHealth)
{
	if(satellites.size() >= satelliteCap)
	{
		return false;
	}

	float satelliteOffset = 0.0f;
	switch(type)
	{
	case SATELLITE_FIRE:
		satelliteOffset = 1.75f;
		break;
	case SATELLITE_WATER:
		satelliteOffset = 2.75;
		break;
	case SATELLITE_AIR:
		satelliteOffset = 3.75;
		break;
	case SATELLITE_EARTH:
		satelliteOffset = 4.75f;
		break;
	}

	std::shared_ptr<Satellite> 
		sat(new Satellite(Framework::Timer(Framework::Timer::TT_LOOP, speed),
						  satelliteColor,
						  satelliteOffset, diameter,
						  type, satelliteHealth));
	satellites.push_back(sat);
	sat->LoadMesh(fileName);
	sat->SetParent(*this);

	return true;
}

bool Sun::RemoveSatellite(std::vector<std::shared_ptr<Satellite>>::iterator index_iterator)
{
	if(satellites.empty())
	{
		return false;
	}
	if(std::find(satellites.begin(), satellites.end(), (*index_iterator)) == satellites.end())
	{
		return false;
	}

	satellites.erase(index_iterator);

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

void Sun::RemoveSatellites()
{
	if(!satellites.empty())
	{
		satellites.erase(satellites.begin(), satellites.end());
	}
}

std::vector<std::shared_ptr<Satellite>> Sun::GetSatellites()
{
	return satellites;
}

std::shared_ptr<Satellite> Sun::GetOuterSatellite()
{
	if(!satellites.empty())
	{
		float maxOffset = 0.0f;
		std::shared_ptr<Satellite> outerSatellite;

		for(std::vector<std::shared_ptr<Satellite>>::iterator iter = satellites.begin();
			iter != satellites.end();
			++iter)
		{
			if((*iter)->GetOffsetFromSun() > maxOffset)
			{
				maxOffset = (*iter)->GetOffsetFromSun();
				outerSatellite = (*iter);
			}
		}

		return outerSatellite;
	}

	return NULL;
}

bool Sun::IsClicked(glm::mat4 projMat, glm::mat4 modelMat, 
				    Mouse userMouse, glm::vec4 cameraPos,
				    int windowWidth, int windowHeight)
{
	Utility::Ray mouseRay = 
		userMouse.GetPickRay(projMat, modelMat, cameraPos,
							 windowWidth, windowHeight);

	if(Utility::Intersections::RayIntersectsSphere(mouseRay, position, diameter / 2.0f))
	{
		// event
		//isClicked = true;
		return true;
	}
	//isClicked = false;
	return false;
}

void Sun::IsSatelliteClicked(glm::mat4 projMat, glm::mat4 modelMat, 
							 Mouse userMouse, glm::vec4 cameraPos,
							 int windowWidth, int windowHeight)
{
	for(std::vector<std::shared_ptr<Satellite>>::iterator iter = satellites.begin();
		iter != satellites.end();
		++iter)
	{
		Utility::Ray mouseRay = 
			userMouse.GetPickRay(projMat, modelMat, cameraPos,
								 windowWidth, windowHeight);

		float outerRadius = (*iter)->GetOffsetFromSun() + (*iter)->GetDiameter();
		float innerRadius = (*iter)->GetOffsetFromSun() - (*iter)->GetDiameter();



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
			(*iter)->SetIsClicked(true);
		}
		else (*iter)->SetIsClicked(false);
	}
}

std::vector<Event> Sun::GetGeneratedEvents()
{
	std::vector<Event> eventsToReturn;

	if(generatedEvents.size() > 0)
	{
		eventsToReturn = generatedEvents;
		generatedEvents.resize(0);
	}
	else 
	{
		eventsToReturn.push_back(StockEvents::EmptyEvent());
	}

	return eventsToReturn;
}
/*void Sun::CleanupGeneratedEvent(const Event &_event)
{
	if(generatedEvents.end() != std::find(generatedEvents.begin(), generatedEvents.end(), _event))
	{
		generatedEvents.erase(std::find(generatedEvents.begin(), generatedEvents.end(), _event));
	}
}*/

Sun::Sun(const Sun &other)
{
	sunMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(*other.sunMesh));

	satellites = other.satellites;
	position = other.position;
	diameter = other.diameter;
	isClicked = other.isClicked;
}




CelestialBody::CelestialBody(const CelestialBody &other)
{
	bodyMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(*other.bodyMesh));
}
CelestialBody::~CelestialBody()
{
	parent.release();
	bodyMesh.release();
}
CelestialBody::CelestialBody(glm::vec3 newPosition, glm::vec4 newColor, float newDiameter,
							 int newSatelliteCap, int newHealth, bool _isSun)
{
	position = newPosition;
	color = newColor;
	diameter = newDiameter;
	satelliteCap = newSatelliteCap;
	health = newHealth;
	isSun = true; // no matter what the value of _isSun is, the body would be created as a sun
	isClicked = false;
	generatedEvents.resize(0);
	parent.reset();
	satellites.resize(0);
	bodyMesh.reset();
}
CelestialBody::CelestialBody(Framework::Timer newRevolutionDuration, glm::vec4 newColor,
							 float newOffsetFromParent, float newDiameter,
							 SatelliteType newSkillType, int newHealth, bool _isSun)
{
	revolutionDuration = newRevolutionDuration;
	color = newColor;
	diameter = newDiameter;
	skillType.satelliteOffsetFromSun = newOffsetFromParent;
	skillType.satelliteTypeForSkill = newSkillType;
	health = newHealth;
	isSun = false; // no matter what the value of _isSun is, the body would be created as a sun
	bodyMesh.reset();
	parent.reset();
	satellites.resize(0);
	generatedEvents.resize(0);
	isClicked = false;

	GenerateUniformBuffers(materialBlockSize, color, materialUniformBuffer);
}

void CelestialBody::LoadMesh(const std::string &fileName)
{
	try
	{
		bodyMesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(fileName));
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}
}

void CelestialBody::Update()
{
	if(isSun)
	{
		for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = satellites.begin();
			iter != satellites.end(); ++iter)
		{
			if((*iter)->GetHealth() <= 0)
			{
				typesTable[(*iter)->GetSatelliteType()] = false;
				RemoveSatellite(iter);
				break;
			}
			else
			{
				(*iter)->Update();
			}
		}
	}

	if(!isSun)
	{
		revolutionDuration.Update();

		float currentTimeThroughLoop = revolutionDuration.GetAlpha();
		float offsetFromSun = skillType.satelliteOffsetFromSun;

		position.x = cosf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
		position.y = sinf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
	}
}

void CelestialBody::Render(glutil::MatrixStack &modelMatrix, GLuint materialBlockIndex,
						   float gamma, 
						   const LitProgData &litData,
						   const UnlitProgData &unlitData,
						   const SimpleProgData &simpleData,
						   float interpolation)
{
	{
		if(isSun)
		{
			glutil::PushStack push(modelMatrix);
		
			modelMatrix.Translate(position);
			
			for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = satellites.begin();
				iter != satellites.end(); ++iter)
			{
				(*iter)->Render(modelMatrix, materialBlockIndex,
								gamma, litData, unlitData, simpleData, interpolation);
			}

			modelMatrix.Scale(diameter);

			glm::vec4 sunColor = Utility::CorrectGamma(color, gamma);

			glUseProgram(unlitData.theProgram);
			glUniformMatrix4fv(unlitData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
			glUniform4f(unlitData.objectColorUnif, sunColor.r, sunColor.g, sunColor.b, sunColor.a);

			bodyMesh->Render("flat");
		}
		else
		{
			glutil::PushStack push(modelMatrix);
			
			modelMatrix.Translate(position);
			modelMatrix.Scale(diameter);

			glBindBufferRange(GL_UNIFORM_BUFFER, materialBlockIndex, materialUniformBuffer,
							  0, sizeof(MaterialBlock));

			glm::mat3 normMatrix(modelMatrix.Top());
			normMatrix = glm::transpose(glm::inverse(normMatrix));

			glUseProgram(litData.theProgram);
			glUniformMatrix4fv(litData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
			glUniformMatrix3fv(litData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));

			bodyMesh->Render("lit");

			glUseProgram(0);

			glBindBufferBase(GL_UNIFORM_BUFFER, materialBlockIndex, 0);
		}
	}

	if(isClicked && !isSun)
	{
		LoadClickedAnimation(modelMatrix, simpleData, gamma);
	}
	else if(isClicked)
	{
		isClicked = false;
	}
}

void CelestialBody::OnEvent(Event &_event)
{
	if(!isSun)
	{
		switch(_event.GetType())
		{
		case EVENT_TYPE_ON_CLICK:
			std::printf("Satellite clicked!\n");
			isClicked = true;
			break;
		case EVENT_TYPE_ON_HOVER:
			std::printf("Satellite hovered!\n");
			isClicked = true;
			break;
		case EVENT_TYPE_ATTACKED:
			health -= _event.GetArgument("damage").varInteger;
			break;
		default:
			break;
		}
	}
	else
	{
		switch(_event.GetType())
		{
		case EVENT_TYPE_ON_CLICK:
			if(_event.GetArgument("rightClick").varBool == true)
			{
				if(!this->RemoveSatellite())
				{
					std::printf("No satellites left.\n");
				}
				else
				{
					typesTable[satellites.size()] = false;
				}
			}
			else if(_event.GetArgument("rightClick").varBool == false)
			{
				if(this->AddSatellite("mesh-files/UnitSphere.xml",
									  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
									  10.0f, 0.5f, 
									  SatelliteType(FindEmptySatelliteType()),
									  5))
				{
					// currentSatelliteType = FindEmptySatelliteType();
				}
				else
				{
					std::printf("Satellite cap reached!\n");
				}
			}
			break;
		case EVENT_TYPE_ATTACKED:
			if(_event.GetArgument("bodyIndex").varInteger == -1)
			{
				if(satellites.empty())
				{
					health -= _event.GetArgument("damage").varInteger;
				}

				// Is this a good way to send events to the Scene?
				if(health <= 0)
				{
					EventArg sunKilledEventArg[2];
					sunKilledEventArg[0].argType = "what";
					sunKilledEventArg[0].argument.varType = TYPE_STRING;
					strcpy(sunKilledEventArg[0].argument.varString, "sun"); 
					sunKilledEventArg[1].argType = "index";
					sunKilledEventArg[1].argument.varType = TYPE_INTEGER;
					sunKilledEventArg[1].argument.varInteger = 0; // Should be some unique index.
					Event sunKilledEvent(2, EVENT_TYPE_KILLED, sunKilledEventArg);

					generatedEvents.push_back(sunKilledEvent);
				}
			}
			else
			{
				for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = satellites.begin();
					iter != satellites.end(); 
					++iter)
				{
					if((*iter)->GetOffsetFromSun() == _event.GetArgument("bodyIndex").varFloat)
					{
						(*iter)->OnEvent(_event);
						break;
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

bool CelestialBody::AddSatellite(const std::string &fileName,
								 glm::vec4 satelliteColor,
								 float speed, float diameter,
								 SatelliteType type, int satelliteHealth)
{
	if(satellites.size() >= satelliteCap)
	{
		return false;
	}

	float satelliteOffset = 0.0f;
	switch(type)
	{
	case SATELLITE_FIRE:
		satelliteOffset = 1.75f;
		break;
	case SATELLITE_WATER:
		satelliteOffset = 2.75f;
		break;
	case SATELLITE_AIR:
		satelliteOffset = 3.75f;
		break;
	case SATELLITE_EARTH:
		satelliteOffset = 4.75f;
		break;
	default:
		break;
	}

	std::shared_ptr<CelestialBody>
		newSat(new CelestialBody(Framework::Timer(Framework::Timer::TT_LOOP, speed),
								 satelliteColor, satelliteOffset, diameter,
								 type, satelliteHealth));
	satellites.push_back(newSat);
	newSat->LoadMesh(fileName);
	newSat->SetParent(*this);

	return true;
}

bool CelestialBody::RemoveSatellite()
{
	if(isSun)
	{
		if(satellites.empty())
		{
			return false;
		}

		satellites.pop_back();

		return true;
	}
}
bool CelestialBody::RemoveSatellite(std::vector<std::shared_ptr<CelestialBody>>::iterator index_iterator)
{
	if(isSun)
	{
		if(satellites.empty())
		{
			return false;
		}
		if(std::find(satellites.begin(), satellites.end(), (*index_iterator)) == satellites.end())
		{
			return false;
		}

		satellites.erase(index_iterator);

		return true;
	}
}
bool CelestialBody::RemoveSatellite(SatelliteType type)
{
	if(isSun)
	{
		if(satellites.empty())
		{
			return false;
		}

		for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = satellites.begin();
			iter != satellites.end(); 
			)
		{
			if((*iter)->GetSatelliteType() == type)
			{
				satellites.erase(iter);
				break;
			}
			else
			{
				++iter;
			}
		}
	}
}
void CelestialBody::RemoveSatellites()
{
	if(isSun)
	{
		if(!satellites.empty())
		{
			satellites.erase(satellites.begin(), satellites.end());
		}
	}
}

bool CelestialBody::IsClicked(glm::mat4 projMat, glm::mat4 modelMat,	
							  Mouse userMouse, glm::vec4 cameraPos,
							  int windowWidth, int windowHeight)
{
	if(isSun)
	{
		Utility::Ray mouseRay = 
			userMouse.GetPickRay(projMat, modelMat, cameraPos,
								 windowWidth, windowHeight);

		if(Utility::Intersections::RayIntersectsSphere(mouseRay, position, diameter / 2.0f))
		{
			// event!
			//isClicked = true;
			return true;
		}
		isClicked = false;
		return false;
	}
	else
	{
		Utility::Ray mouseRay = 
			userMouse.GetPickRay(projMat, modelMat, cameraPos, 
								 windowWidth, windowHeight);

		float outerRadius = skillType.satelliteOffsetFromSun + diameter;
		float innerRadius = skillType.satelliteOffsetFromSun - diameter;



		//*** HACKS HACKS HACKS ***\\

		// TODO: hack. Should look for a better solution. 
		//	     If not found, this suits the game's needs.
		// float precisionGain = 1.0f + i / 100.0f;
		// Solved. Will be left for further investigation
		
		glutil::MatrixStack distMat;
		distMat.Scale(1.0f, 1.0f, 5.0f);
		//*** HACKS HACKS HACKS ***\\



		if(Utility::Intersections::RayIntersectsEllipsoid(mouseRay, parent->GetPosition(), outerRadius, distMat.Top()) &&
		   !Utility::Intersections::RayIntersectsEllipsoid(mouseRay, parent->GetPosition(), innerRadius, distMat.Top()))
		{
			isClicked = true;
			return true;
		}
		isClicked = false;
		return false;
	}
}
bool CelestialBody::IsSatelliteClicked(glm::mat4 projMat, glm::mat4 modelMat,	
									   Mouse userMouse, glm::vec4 cameraPos,
									   int windowWidth, int windowHeight)
{
	for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = satellites.begin();
		iter != satellites.end();
		++iter)
	{
		return (*iter)->IsClicked(projMat, modelMat, userMouse, cameraPos, windowWidth, windowHeight);
	}
}

std::vector<Event> CelestialBody::GetGeneratedEvents()
{
	std::vector<Event> eventsToReturn;

	if(generatedEvents.size() > 0)
	{
		eventsToReturn = generatedEvents;
		generatedEvents.resize(0);
	}
	else
	{
		eventsToReturn.push_back(StockEvents::EmptyEvent());
	}

	return eventsToReturn;
}

bool CelestialBody::HasSatellites()
{
	return !satellites.empty();
}

const bool CelestialBody::GetIsClicked() const
{
	return isClicked;
}
const bool CelestialBody::GetIsSatelliteClicked(SatelliteType type) const
{
	for(std::vector<std::shared_ptr<CelestialBody>>::const_iterator iter = satellites.begin();
		iter != satellites.end();
		++iter)
	{
		if((*iter)->GetSatelliteType() == type)
		{
			return (*iter)->GetIsClicked();
		}
	}

	std::printf("No such satellite. (PlanetBodies.cpp/CelestialBody::GetIsSatelliteClicked)\n");
	return false;
}
const float CelestialBody::GetRadius() const
{
	return diameter / 2.0f;
}
const float CelestialBody::GetDiameter() const
{
	return diameter;
}
const int CelestialBody::GetHealth() const
{
	return health;
}
std::vector<std::shared_ptr<CelestialBody>> CelestialBody::GetSatellites()
{
	return satellites;
}
std::shared_ptr<CelestialBody> CelestialBody::GetSatellite(SatelliteType type)
{
	for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = satellites.begin();
		iter != satellites.end();
		++iter)
	{
		if((*iter)->GetSatelliteType() == type)
		{
			return (*iter);
		}
	}

	std::printf("No such satellite. (PlanetBodies.cpp/CelestialBody::GetSatellite)\n");
	return nullptr;
}
std::shared_ptr<CelestialBody> CelestialBody::GetOuterSatellite()
{
	if(!satellites.empty())
	{
		float maxOffset = 0.0f;
		std::shared_ptr<CelestialBody> outerSatellite;

		for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = satellites.begin();
			iter != satellites.end();
			++iter)
		{
			if((*iter)->GetOffsetFromSun() > maxOffset)
			{
				maxOffset = (*iter)->GetOffsetFromSun();
				outerSatellite = (*iter);
			}
		}

		return outerSatellite;
	}

	return NULL;//nullptr;
}

const glm::vec3 CelestialBody::GetPosition() const
{
	return position;
}

// Satellite-specific methods
float CelestialBody::GetOffsetFromSun()
{
	if(!isSun)
	{
		return skillType.satelliteOffsetFromSun;
	}
}
SatelliteType CelestialBody::GetSatelliteType()
{
	if(!isSun)
	{
		return skillType.satelliteTypeForSkill;
	}
}

inline void CelestialBody::SetParent(const CelestialBody &newParent)
{
	if(!isSun)
	{
		parent = std::unique_ptr<CelestialBody>(new CelestialBody(newParent));
	}
}
void CelestialBody::SetIsClicked(bool newIsClicked)
{
	if(!isSun)
	{
		isClicked = newIsClicked;
	}
}

void CelestialBody::Stop()
{
	if(!isSun)
	{
		revolutionDuration.SetPause(true);
	}
}
void CelestialBody::Start()
{
	if(!isSun)
	{
		revolutionDuration.SetPause(false);
	}
}

void CelestialBody::LoadClickedAnimation(glutil::MatrixStack &modelMatrix,
										 const SimpleProgData &simpleData,
										 float gamma)
{
	if(!isSun)
	{
		float offsetFromSun = skillType.satelliteOffsetFromSun;
	
		SatelliteOrbit orbit(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
							 glm::vec4(parent->GetPosition(), 0.0f), 
							 offsetFromSun + diameter, offsetFromSun - diameter,
							 gamma);

	
		glutil::PushStack push(modelMatrix);
		modelMatrix.RotateX(90.0f);

		orbit.Draw(modelMatrix, simpleData);
	}
}