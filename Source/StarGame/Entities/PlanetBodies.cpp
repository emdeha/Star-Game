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
							 int newSatelliteCap, int newHealth, 
							 int newCurrentResource, int newSatelliteConstructionCost,
							 bool _isSun)
{
	position = newPosition;
	color = newColor;
	diameter = newDiameter;
	satelliteCap = newSatelliteCap;
	health = newHealth;
	satelliteConstructionCost = newSatelliteConstructionCost;
	isSun = true; // no matter what the value of _isSun is, the body would be created as a sun
	isClicked = false;
	currentResource = newCurrentResource;
	generatedEvents.resize(0);
	parent.reset();
	satellites.resize(0);
	bodyMesh.reset();
}
CelestialBody::CelestialBody(Framework::Timer newRevolutionDuration, glm::vec4 newColor,
							 float newOffsetFromParent, float newDiameter,
							 SatelliteType newSkillType, int newHealth, ResourceData newResource,
							 bool _isSun)
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

	resource.resourceGainTime = newResource.resourceGainTime;
	resource.resourceGain_perTime = newResource.resourceGain_perTime;
	resource.resourceTimer = Framework::Timer(Framework::Timer::TT_SINGLE, resource.resourceGainTime);

	GenerateUniformBuffers(materialBlockSize, color, materialUniformBuffer);
}

void CelestialBody::InitSatelliteOrbit()
{	
	hoverOrbit = SatelliteOrbit(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
								parent->GetPosition(),
								skillType.satelliteOffsetFromSun + diameter,
								skillType.satelliteOffsetFromSun - diameter,
								2.2f); // Change: gamma
	hoverOrbit.Init();
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

		for(int i = 0; i < skills.size(); i++)
		{
			skills[i]->Update();
		}

		std::shared_ptr<Skill> sunNovaSkill;
		for(int i = 0; i < skills.size(); i++)
		{
			if(skills[i]->GetSkillType() == "sunNovaSkill")
			{
				sunNovaSkill = skills[i];
			}
		}
		for(int i = 0; i < satellites.size(); i++)
		{
			if(sunNovaSkill->IsIntersectingObject(satellites[i]->GetPosition()))
			{
				std::vector<std::shared_ptr<Skill>> satelliteSkills = satellites[i]->GetAllSkills();
				for(int skillIndex = 0; skillIndex < satelliteSkills.size(); skillIndex++)
				{
					if(satelliteSkills[skillIndex]->GetSkillType() == "satChainSkill")
					{
						EventArg satIntersectedEventArgs[1];
						satIntersectedEventArgs[0].argType = "isIntersected";
						satIntersectedEventArgs[0].argument.varType = TYPE_BOOL;
						satIntersectedEventArgs[0].argument.varBool = true;
						Event satIntersectedEvent(1, EVENT_TYPE_OTHER, satIntersectedEventArgs);

						satelliteSkills[skillIndex]->OnEvent(satIntersectedEvent);
					}
				}
			}
		}
	}

	if(!isSun)
	{
		revolutionDuration.Update();

		float currentTimeThroughLoop = revolutionDuration.GetAlpha();
		float offsetFromSun = skillType.satelliteOffsetFromSun;

		position.x = sinf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
		position.z = cosf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromSun;

		for(int i = 0; i < skills.size(); i++)
		{
			skills[i]->Update();
			skills[i]->SetParameter(PARAM_POSITION, position);
		}

		if(resource.resourceTimer.Update())
		{
			EventArg matterGeneratedEventArg[2];
			matterGeneratedEventArg[0].argType = "what_event";
			matterGeneratedEventArg[0].argument.varType = TYPE_STRING;
			strcpy(matterGeneratedEventArg[0].argument.varString, "satGainedResource");
			matterGeneratedEventArg[1].argType = "how_much";
			matterGeneratedEventArg[1].argument.varType = TYPE_INTEGER;
			matterGeneratedEventArg[1].argument.varInteger = resource.resourceGain_perTime;
			Event matterGeneratedEvent = Event(2, EVENT_TYPE_OTHER, matterGeneratedEventArg); 
			
			parent->OnEvent(matterGeneratedEvent);
			resource.resourceTimer.Reset();
		}
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
	
	for(int i = 0; i < skills.size(); i++)
	{
		skills[i]->Render(modelMatrix, simpleData);
		skills[i]->Render(modelMatrix, litData, materialBlockIndex);
	}

	if(isClicked && !isSun)
	{
		glutil::PushStack push(modelMatrix);
		modelMatrix.RotateX(0.0f);

		hoverOrbit.Draw(modelMatrix, simpleData);
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
		case EVENT_TYPE_OTHER:
			if(strcmp(_event.GetArgument("what_event").varString, "satGainedResource") == 0)
			{
				currentResource += _event.GetArgument("how_much").varInteger;
			}
			if(strcmp(_event.GetArgument("what_event").varString, "enemyGainedResource") == 0)
			{
				currentResource += _event.GetArgument("how_much").varInteger;
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
	if(currentResource < satelliteConstructionCost)
	{
		std::printf("Not enough resource.\n");
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
	
	currentResource -= satelliteConstructionCost; // Maybe an event should be sent.

	ResourceData satResourceData;
	satResourceData.resourceGainTime = 3.0f;
	satResourceData.resourceGain_perTime = 5;

	std::shared_ptr<CelestialBody>
		newSat(new CelestialBody(Framework::Timer(Framework::Timer::TT_LOOP, speed),
								 satelliteColor, satelliteOffset, diameter,
								 type, satelliteHealth, satResourceData));
	newSat->LoadMesh(fileName);
	newSat->SetParent(this);
	newSat->InitSatelliteOrbit();

	std::shared_ptr<PassiveAOESkill> satSkill = 
		std::shared_ptr<PassiveAOESkill>(new PassiveAOESkill(glm::vec3(),//newSat, 
															 20,
															 1,
															 2.0f,
															 "passiveAOESkill",
															 'q', 'q', 'e'));
	std::shared_ptr<SatelliteChainingNova> satChainSkill =
		std::shared_ptr<SatelliteChainingNova>(new SatelliteChainingNova(newSat->GetPosition(),
																		 20, 2.0f, 0.08f, 
																		 "satChainSkill"));
	if(type == SATELLITE_WATER)
	{
		std::shared_ptr<FrostNovaSkill> satFrostNovaSkill = 
			std::shared_ptr<FrostNovaSkill>(new FrostNovaSkill(5, 3, 2.0f, 0.1f, 
															   newSat->GetPosition(), 
															   "satFrostNova",
															   'q', 'w', 'q'));
		newSat->AddSkill(satFrostNovaSkill);
	}
	if(type == SATELLITE_EARTH)
	{
		std::shared_ptr<ShieldSkill> satShieldSkill =
			std::shared_ptr<ShieldSkill>(new ShieldSkill(newSat->GetPosition(), 
														 3, 0.5f, "satShieldSkill", 'w', 'e', 'w'));
		newSat->AddSkill(satShieldSkill);
	}
																		 
	newSat->AddSkill(satSkill);
	newSat->AddSkill(satChainSkill);

	satellites.push_back(newSat);

	return true;
}
void CelestialBody::AddSkill(const std::shared_ptr<Skill> newSkill)
{
	skills.push_back(newSkill);
}

bool CelestialBody::RemoveSatellite()
{
	if(isSun)
	{
		if(satellites.empty())
		{
			return false;
		}

		int satelliteType = satellites.back()->GetSatelliteType();
		satellites.pop_back();

		EventArg satelliteRemovedEventArgs[2];
		satelliteRemovedEventArgs[0].argType = "what_event";
		satelliteRemovedEventArgs[0].argument.varType = TYPE_STRING;
		std::strcpy(satelliteRemovedEventArgs[0].argument.varString, "satelliteRemoved");
		satelliteRemovedEventArgs[1].argType = "satType";
		satelliteRemovedEventArgs[1].argument.varType = TYPE_INTEGER;
		satelliteRemovedEventArgs[1].argument.varInteger = satelliteType;
		Event satelliteRemovedEvent = Event(2, EVENT_TYPE_OTHER, satelliteRemovedEventArgs);
		generatedEvents.push_back(satelliteRemovedEvent);

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

		int satelliteType = (*index_iterator)->GetSatelliteType();
		satellites.erase(index_iterator);

		EventArg satelliteRemovedEventArgs[2];
		satelliteRemovedEventArgs[0].argType = "what_event";
		satelliteRemovedEventArgs[0].argument.varType = TYPE_STRING;
		std::strcpy(satelliteRemovedEventArgs[0].argument.varString, "satelliteRemoved");
		satelliteRemovedEventArgs[1].argType = "satType";
		satelliteRemovedEventArgs[1].argument.varType = TYPE_INTEGER;
		satelliteRemovedEventArgs[1].argument.varInteger = satelliteType;
		Event satelliteRemovedEvent = Event(2, EVENT_TYPE_OTHER, satelliteRemovedEventArgs);
		generatedEvents.push_back(satelliteRemovedEvent);

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
				int satelliteType = (*iter)->GetSatelliteType();
				satellites.erase(iter);

				EventArg satelliteRemovedEventArgs[2];
				satelliteRemovedEventArgs[0].argType = "what_event";
				satelliteRemovedEventArgs[0].argument.varType = TYPE_STRING;
				std::strcpy(satelliteRemovedEventArgs[0].argument.varString, "satelliteRemoved");
				satelliteRemovedEventArgs[1].argType = "satType";
				satelliteRemovedEventArgs[1].argument.varType = TYPE_INTEGER;
				satelliteRemovedEventArgs[1].argument.varInteger = satelliteType;
				Event satelliteRemovedEvent = Event(2, EVENT_TYPE_OTHER, satelliteRemovedEventArgs);
				generatedEvents.push_back(satelliteRemovedEvent);

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

bool CelestialBody::IsClicked(Utility::Ray mouseRay)
{
	if(isSun)
	{
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
		float outerRadius = skillType.satelliteOffsetFromSun + diameter;
		float innerRadius = skillType.satelliteOffsetFromSun - diameter;



		//*** HACKS HACKS HACKS ***\\

		// TODO: hack. Should look for a better solution. 
		//	     If not found, this suits the game's needs.
		// float precisionGain = 1.0f + i / 100.0f;
		// Solved. Will be left for further investigation
		
		glutil::MatrixStack distMat;
		distMat.Scale(1.0f, 5.0f, 1.0f);
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
bool CelestialBody::IsSatelliteClicked(Utility::Ray mouseRay)
{
	for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = satellites.begin();
		iter != satellites.end();
		++iter)
	{
		return (*iter)->IsClicked(mouseRay);
	}
}

bool CelestialBody::IsSun()
{
	return isSun;
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
Event CelestialBody::GetGeneratedEvent(const std::string &eventName)
{
	if(generatedEvents.size() > 0)
	{
		for(int i = 0; i < generatedEvents.size(); i++)
		{
			if(generatedEvents[i].GetType() == EVENT_TYPE_OTHER &&
			   strcmp(generatedEvents[i].GetArgument("what_event").varString, eventName.c_str()) == 0)
			{
				return generatedEvents[i];
			}
		}
	}
	else
	{
		return StockEvents::EmptyEvent();
	}
}
void CelestialBody::RemoveGeneratedEvent(const std::string &eventName)
{
	if(generatedEvents.size() > 0)
	{
		for(std::vector<Event>::iterator iter = generatedEvents.begin();
			iter != generatedEvents.end();)
		{
			if(strcmp(iter->GetArgument("what_event").varString, eventName.c_str()) == 0)
			{
				generatedEvents.erase(iter);
				break;
			}
			else 
			{
				++iter;
			}
		}
	}
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

	return nullptr;
}

std::vector<std::shared_ptr<Skill>> CelestialBody::GetAllSkills()
{
	std::vector<std::shared_ptr<Skill>> allSkills;

	for(int i = 0; i < skills.size(); i++)
	{
		allSkills.push_back(skills[i]);
	}

	for(int i = 0; i < satellites.size(); i++)
	{
		std::vector<std::shared_ptr<Skill>> allSatSkills = 
			satellites[i]->GetAllSkills();

		allSkills.insert(allSkills.end(), allSatSkills.begin(), allSatSkills.end());
	}

	return allSkills;
}

const glm::vec3 CelestialBody::GetPosition() const
{
	return position;
}
const int CelestialBody::GetCurrentResource() const
{
	return currentResource;
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

inline void CelestialBody::SetParent(CelestialBody *newParent)
{
	if(!isSun)
	{
		parent = std::unique_ptr<CelestialBody>(newParent);
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