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
#include "../framework/ErrorAPI.h"

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
	
}
CelestialBody::~CelestialBody()
{
	parent.release();
}
CelestialBody::CelestialBody(glm::vec3 newPosition, glm::vec4 newColor, float newDiameter,
							 int newSatelliteCap, int newHealth, 
							 int newCurrentResource, int newSatelliteConstructionCost,
							 bool _isSun)
{
	position = newPosition;
	color = newColor;
	diameter = newDiameter;
	clickableRadius = diameter / 2.0f;
	satelliteCap = newSatelliteCap;
	health = newHealth;
	satelliteConstructionCost = newSatelliteConstructionCost;
	isSun = true; // no matter what the value of _isSun is, the body would be created as a sun
	isClicked = false;
	currentResource = newCurrentResource;
	initialResource = currentResource;
	generatedEvents.resize(0);
	parent.reset();
	satellites.resize(0);
	//bodyMesh.reset();

	isDrawingUpgradeButtons = false;
	
	InitSatelliteStats();
}
CelestialBody::CelestialBody(float speed, float newOffsetFromParent, float newDiameter, int newHealth,
							 float resourceGainTime, int resourceGain_perTime, SatelliteType newSatType, 
							 glm::vec4 newColor, bool _isSun)
{
	revolutionDuration = Framework::Timer(Framework::Timer::TT_LOOP, speed);
	color = newColor;
	diameter = newDiameter;
	clickableRadius = diameter;
	health = newHealth;
	satType = newSatType;
	isSun = false; // no matter what the value of _isSun is, the body would be created as a sun
	//bodyMesh.reset();
	parent.reset();
	satellites.resize(0);
	generatedEvents.resize(0);
	isClicked = false;

	isDrawingUpgradeButtons = false;
	
	satelliteStats[satType].resourceGainTime = resourceGainTime;
	satelliteStats[satType].resourceGain_perTime = resourceGain_perTime;
	satelliteStats[satType].satelliteOffsetFromSun = newOffsetFromParent;
	satelliteStats[satType].speed = speed;
	satelliteStats[satType].health = health;
	satelliteStats[satType].diameter = diameter;

	// WARN: data repeats with satellite stats
	resource.resourceGainTime = resourceGainTime;
	resource.resourceGain_perTime = resourceGain_perTime;
	resource.resourceTimer = Framework::Timer(Framework::Timer::TT_SINGLE, resource.resourceGainTime);

	//GenerateUniformBuffers(materialBlockSize, color, materialUniformBuffer);
}

void CelestialBody::InitSatelliteOrbit(const std::vector<std::string> &textureFileNames)
{	
	hoverOrbit = SatelliteOrbit(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
								parent->GetPosition(),
								textureFileNames,//texturesFileNames,
								satelliteStats[satType].satelliteOffsetFromSun + diameter / 2.0f,
								satelliteStats[satType].satelliteOffsetFromSun - diameter / 2.0f,
								2.2f); // Change: gamma
	hoverOrbit.Init();
}
void CelestialBody::InitSunSkillUpgradeButtons(const std::vector<std::string> &textureFileNames)
{
	sunSkillUpgradeBtns = SunSkillUpgradeButtons(1.0f, 1.0f, diameter / 2.0f, position, 
												 textureFileNames);//skillTexturesFileNames);
	sunSkillUpgradeBtns.Init();
}

void CelestialBody::InitSatelliteStats()
{
	satelliteStats[SATELLITE_FIRE].diameter = 0.8;
	satelliteStats[SATELLITE_FIRE].health = 5;
	satelliteStats[SATELLITE_FIRE].speed = 8.0f;	
	satelliteStats[SATELLITE_FIRE].satelliteOffsetFromSun = 2.75f;
	satelliteStats[SATELLITE_FIRE].resourceGainTime = 3.0f;
	satelliteStats[SATELLITE_FIRE].resourceGain_perTime = 5;

	satelliteStats[SATELLITE_EARTH].diameter = 0.5f;
	satelliteStats[SATELLITE_EARTH].health = 5;
	satelliteStats[SATELLITE_EARTH].speed = 5.0f;	
	satelliteStats[SATELLITE_EARTH].satelliteOffsetFromSun = 1.75f;
	satelliteStats[SATELLITE_EARTH].resourceGainTime = 3.0f;
	satelliteStats[SATELLITE_EARTH].resourceGain_perTime = 5;

	satelliteStats[SATELLITE_AIR].diameter = 0.5f;
	satelliteStats[SATELLITE_AIR].health = 5;
	satelliteStats[SATELLITE_AIR].speed = 7.0f;
	satelliteStats[SATELLITE_AIR].satelliteOffsetFromSun = 3.75f;
	satelliteStats[SATELLITE_AIR].resourceGainTime = 3.0f;
	satelliteStats[SATELLITE_AIR].resourceGain_perTime = 5;

	satelliteStats[SATELLITE_WATER].diameter = 0.5f;
	satelliteStats[SATELLITE_WATER].health = 5;
	satelliteStats[SATELLITE_WATER].speed = 10.0f;
	satelliteStats[SATELLITE_WATER].satelliteOffsetFromSun = 4.75f;
	satelliteStats[SATELLITE_WATER].resourceGainTime = 3.0f;
	satelliteStats[SATELLITE_WATER].resourceGain_perTime = 5;
}

void CelestialBody::LoadMesh(const std::string &fileName)
{

	if(!mesh.LoadMesh(fileName))
	{
		std::string errorMessage = "cannot load mesh ";
		errorMessage += fileName;
		HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	}
	if(!isSun)
	{
		mesh.LoadLight();
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
			//if(skills[i]->IsStarted())
			//{
				skills[i]->Update();
			//}
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

	if(!isSun && !revolutionDuration.IsPaused())
	{
		revolutionDuration.Update();

		float currentTimeThroughLoop = revolutionDuration.GetAlpha();
		float offsetFromSun = satelliteStats[satType].satelliteOffsetFromSun;

		position.x = sinf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromSun;
		position.z = cosf(currentTimeThroughLoop * (2.0f * PI)) * offsetFromSun;

		for(int i = 0; i < skills.size(); i++)
		{
			//if(skills[i]->IsStarted())
			//{
				skills[i]->Update();
				skills[i]->SetParameter(PARAM_POSITION, position);
			//}
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
						   const SimpleTextureProgData &textureData,
						   const LitTextureProgData &litTextureData,
						   const SpriteParticleProgData &spriteParticleProgData,
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
								gamma, litData, unlitData, simpleData, textureData, litTextureData, spriteParticleProgData, 
								interpolation);
			}

			modelMatrix.Scale(diameter / 2.0f);

			glm::vec4 sunColor = Utility::CorrectGamma(color, gamma);

			glUseProgram(textureData.theProgram);
			glUniform4f(textureData.colorUnif, sunColor.r, sunColor.g, sunColor.b, sunColor.a);
			glUseProgram(0);

			mesh.Render(modelMatrix, textureData);
		}
		else
		{
			glutil::PushStack push(modelMatrix);
			
			modelMatrix.Translate(position);
			modelMatrix.Scale(diameter / 2.0f);

			mesh.Render(modelMatrix, litTextureData, materialBlockIndex);
		}
	}
	
	for(int i = 0; i < skills.size(); i++)
	{
		//if(skills[i]->IsStarted())
		//{
			skills[i]->Render(modelMatrix, simpleData);
			skills[i]->Render(modelMatrix, litData, materialBlockIndex);
			skills[i]->Render(modelMatrix, spriteParticleProgData, simpleData);
			skills[i]->Render(modelMatrix, spriteParticleProgData);
			skills[i]->Render(modelMatrix, spriteParticleProgData, textureData);
		//}
	}
	
	if(isClicked && isSun && isDrawingUpgradeButtons)
	{
		sunSkillUpgradeBtns.Draw(modelMatrix, textureData);
	}
	if(isClicked && !isSun)
	{
		glutil::PushStack push(modelMatrix);
		modelMatrix.RotateX(0.0f);

		hoverOrbit.Draw(modelMatrix, simpleData, textureData, isDrawingUpgradeButtons);
	}
	else if(!isClicked)
	{
		clickableRadius = diameter / 2.0f;
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
			isDrawingUpgradeButtons = true;
			break;
		case EVENT_TYPE_ON_HOVER:
			isClicked = true;
			//isDrawingUpgradeButtons = false;
			break;
		case EVENT_TYPE_ATTACKED:
			health -= _event.GetArgument("damage").varInteger;
			break;
		case EVENT_TYPE_OTHER:
			if(strcmp(_event.GetArgument("what_event").varString, "skillUpgr") == 0)
			{
				std::printf("sad");
			}
			if(strcmp(_event.GetArgument("what_event").varString, "satUnhov") == 0)
			{
				isDrawingUpgradeButtons = false;
			}
			break;
		default:
			HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
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
			}
			else if(_event.GetArgument("rightClick").varBool == false)
			{
			}
			isDrawingUpgradeButtons = true;
			break;
		case EVENT_TYPE_ON_HOVER:
			isClicked = true;
			if(clickableRadius <= diameter / 2.0f)
			{
				clickableRadius *= 2.5f;
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
			if(strcmp(_event.GetArgument("what_event").varString, "skilldeployed") == 0)
			{
				if(currentResource > _event.GetArgument("skillCost").varInteger)
				{
					currentResource -= _event.GetArgument("skillCost").varInteger;
				}
				else
				{
					EventArg insufficientResourcesEventArg[1];
					insufficientResourcesEventArg[0].argType = "what_event";
					insufficientResourcesEventArg[0].argument.varType = TYPE_STRING;
					strcpy(insufficientResourcesEventArg[0].argument.varString, "insufRes");
					Event insufficientResourcesEvent = Event(1, EVENT_TYPE_OTHER, insufficientResourcesEventArg);

					generatedEvents.push_back(insufficientResourcesEvent);
				}
			}
			if(strcmp(_event.GetArgument("what_event").varString, "skillUpgr") == 0)
			{
				if(_event.GetArgument("satType").varInteger == -1)
				{
					isClicked = true;
					sunSkillUpgradeBtns.ChangeTexture((TextureTypeSun)_event.GetArgument("skillIndex").varInteger,
													  _event.GetArgument("index").varInteger);
				}
				else
				{
					for(int i = 0; i < satellites.size(); i++)
					{
						if(satellites[i]->satType == SatelliteType(_event.GetArgument("satType").varInteger))
						{
							satellites[i]->hoverOrbit.ChangeUpgradeButtonTexture(
								TEXTURE_TYPE_SAT_UPGRADE, _event.GetArgument("index").varInteger);
						}
					}
				}
			}
			if(strcmp(_event.GetArgument("what_event").varString, "sunUnhov") == 0)
			{
				isDrawingUpgradeButtons = false;
			}
			break;
		default:
			HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
			break;
		}
	}
}

bool CelestialBody::AddSatellite(const std::string &fileName,
								 glm::vec4 satelliteColor,
								 SatelliteType type, bool drainResource, 
								 bool addAtProgress, float progress,
								 bool addHealth, int newHealth)
{
	if(satellites.size() >= satelliteCap)
	{
		// EXPECTED
		// TODO: Print ingame text
		return false;
	}
	if(drainResource)
	{
		if(currentResource < satelliteConstructionCost)
		{
			// EXPECTED
			// TODO: Print ingame text
			std::printf("Not enough resource.\n");
			return false;
		}
	
		currentResource -= satelliteConstructionCost; // Maybe an event should be sent.
	}

	ResourceData satResourceData;
	satResourceData.resourceGainTime = satelliteStats[type].resourceGainTime;
	satResourceData.resourceGain_perTime = satelliteStats[type].resourceGain_perTime;

	std::shared_ptr<CelestialBody>
		newSat(new CelestialBody(satelliteStats[type].speed, satelliteStats[type].satelliteOffsetFromSun, satelliteStats[type].diameter,
								 satelliteStats[type].health, satelliteStats[type].resourceGainTime, satelliteStats[type].resourceGain_perTime,
								 type, satelliteColor));
	if(addAtProgress)
	{
		newSat->SetProgress(progress);
	}
	if(addHealth)
	{
		newSat->health = newHealth;
	}
	newSat->LoadMesh(fileName);
	newSat->SetParent(this);

	std::vector<std::string> skillTextures;
	switch(type)
	{
	case SATELLITE_WATER:		
		skillTextures.push_back("../data/images/" + satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].skillUpgradedTexture);
		break;
	case SATELLITE_FIRE:
		skillTextures.push_back("../data/images/" + satSkillStats[SKILL_TYPE_SAT_CHAIN].skillUpgradedTexture);
		break;
	case SATELLITE_EARTH:
		skillTextures.push_back("../data/images/" + satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].skillUpgradedTexture);
		break;
	case SATELLITE_AIR:
		skillTextures.push_back("../data/images/" + satSkillStats[SKILL_TYPE_SAT_SHIELD].skillUpgradedTexture);
		break;
	}
	skillTextures.push_back("../data/images/skill-noupgrade.jpg");

	newSat->InitSatelliteOrbit(skillTextures);

	if(type == satSkillStats[SKILL_TYPE_SAT_CHAIN].forWhichSatellite)
	{
		std::shared_ptr<SatelliteChainingNova> satChainSkill =
			std::shared_ptr<SatelliteChainingNova>(new SatelliteChainingNova(newSat->GetPosition(),
																			 satSkillStats[SKILL_TYPE_SAT_CHAIN].damage, 
																			 satSkillStats[SKILL_TYPE_SAT_CHAIN].range, 
																			 satSkillStats[SKILL_TYPE_SAT_CHAIN].scaleRate, 
																			 "satChainSkill",
																			 '\0', '\0', '\0', 
																			 0, 
																			 satSkillStats[SKILL_TYPE_SAT_CHAIN].skillResearchCost, 
																			 satSkillStats[SKILL_TYPE_SAT_CHAIN].upgradeBoxIndex));
		newSat->AddSkill(satChainSkill);
	}
	if(type == satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].forWhichSatellite)
	{
		std::shared_ptr<PassiveAOESkill> satSkill = 
			std::shared_ptr<PassiveAOESkill>(new PassiveAOESkill(glm::vec3(), 
																 satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].damage,
																 satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].damageApplyTime_secs, 
																 satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].duration_secs,
																 satSkillStats[SKILL_TYPE_SAT_PASSIVE_AOE].range,
																 "satPassiveAOESkill", // WARN: that line is soo dangerous (lalala)
																 'q', 'q', 'e',
																 satSkillStats[SKILL_TYPE_PASSIVE_AOE].skillApplyCost,
																 satSkillStats[SKILL_TYPE_PASSIVE_AOE].skillResearchCost,
																 satSkillStats[SKILL_TYPE_PASSIVE_AOE].upgradeBoxIndex));	
																				 
		newSat->AddSkill(satSkill);
	}
	if(type == satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].forWhichSatellite)
	{
		std::shared_ptr<FrostNovaSkill> satFrostNovaSkill = 
			std::shared_ptr<FrostNovaSkill>(new FrostNovaSkill(satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].damage, 
															   satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].stunTime_secs,
															   satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].range,
															   satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].scaleRate, 
															   newSat->GetPosition(), 
															   "satFrostNova",
															   'q', 'w', 'q',
															   satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].skillApplyCost,
															   satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].skillResearchCost, 
															   satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].upgradeBoxIndex));
		newSat->AddSkill(satFrostNovaSkill);
	}
	if(type == satSkillStats[SKILL_TYPE_SAT_SHIELD].forWhichSatellite)
	{
		std::shared_ptr<ShieldSkill> satShieldSkill =
			std::shared_ptr<ShieldSkill>(new ShieldSkill(newSat->GetPosition(), 
														 satSkillStats[SKILL_TYPE_SAT_SHIELD].defensePoints, 
														 satSkillStats[SKILL_TYPE_SAT_SHIELD].range, 
														 "satShieldSkill", 'w', 'e', 'w',
														 satSkillStats[SKILL_TYPE_SAT_SHIELD].skillApplyCost,
														 satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].skillResearchCost,
														 satSkillStats[SKILL_TYPE_SAT_FROSTNOVA].upgradeBoxIndex));
		newSat->AddSkill(satShieldSkill);
	}


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
			// EXPECTED
			// TODO: Print ingame text
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
			// EXPECTED
			// TODO: Print ingame text
			return false;
		}
		if(std::find(satellites.begin(), satellites.end(), (*index_iterator)) == satellites.end())
		{
			// EXPECTED
			// TODO: Print ingame text
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
			// EXPECTED
			// TODO: Print ingame text
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
		if(Utility::Intersections::RayIntersectsSphere(mouseRay, position, clickableRadius))
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
		float outerRadius = satelliteStats[satType].satelliteOffsetFromSun + diameter;
		float innerRadius = satelliteStats[satType].satelliteOffsetFromSun - diameter;



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
CelestialBody::SkillButtonClickedData CelestialBody::IsSkillUpgradeButtonClicked(Utility::Ray mouseRay, int &buttonIndex)
{
	if(isSun)
	{
		bool isSunUpgrButtonClicked = sunSkillUpgradeBtns.IsClicked(mouseRay, buttonIndex);
		if(isSunUpgrButtonClicked)
		{
			return SkillButtonClickedData(true, buttonIndex, 
									      (int)sunSkillUpgradeBtns.GetSkillTextureTypeAtButton(buttonIndex));
		}
	}
	else
	{
		bool isUpgrButtonClicked = hoverOrbit.IsUpgradeButtonClicked(mouseRay, buttonIndex);
		if(isUpgrButtonClicked)
		{
			return SkillButtonClickedData(true, buttonIndex, 0);
		}
	}
	return SkillButtonClickedData(false, -1, -1);
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
		return StockEvents::EmptyEvent();
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

	std::string errorMessage = "no such satellite ";
	switch(type)
	{
	case SATELLITE_FIRE:
		errorMessage += "SATELLITE_FIRE";
		break;
	case SATELLITE_EARTH:
		errorMessage += "SATELLITE_EARTH";
		break;
	case SATELLITE_WATER:
		errorMessage += "SATELLITE_WATER";
		break;
	case SATELLITE_AIR:
		errorMessage += "SATELLITE_AIR";
		break;
	default:
		HandleUnexpectedError("invalid satellite type", __LINE__, __FILE__);
		break;
	}
	HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
	return false;
}
const bool CelestialBody::GetIsDrawingUpgradeButtons() const
{
	return isDrawingUpgradeButtons;
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

	std::string errorMessage = "no such satellite ";
	switch(type)
	{
	case SATELLITE_FIRE:
		errorMessage += "SATELLITE_FIRE";
		break;
	case SATELLITE_EARTH:
		errorMessage += "SATELLITE_EARTH";
		break;
	case SATELLITE_WATER:
		errorMessage += "SATELLITE_WATER";
		break;
	case SATELLITE_AIR:
		errorMessage += "SATELLITE_AIR";
		break;
	default:
		HandleUnexpectedError("invalid satellite type", __LINE__, __FILE__);
		break;
	}
	HandleUnexpectedError(errorMessage, __LINE__, __FILE__);
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

	HandleUnexpectedError("no outer satellite", __LINE__, __FILE__);
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
std::vector<std::shared_ptr<Skill>> CelestialBody::GetSatelliteSkills(SatelliteType type)
{
	std::vector<std::shared_ptr<Skill>> allSkills;

	for(int i = 0; i < satellites.size(); i++)
	{
		if(satellites[i]->satType == type)
		{
			allSkills = satellites[i]->GetAllSkills();
		}
	}

	return allSkills;
}
std::vector<std::shared_ptr<Skill>> CelestialBody::GetSunSkills()
{
	return skills;
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
		return satelliteStats[satType].satelliteOffsetFromSun;
	}
}
SatelliteType CelestialBody::GetSatelliteType()
{
	if(!isSun)
	{
		return satType;
	}
	else return SatelliteType(-1);
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

void CelestialBody::SetProgress(float progress)
{
	if(!isSun)
	{
		revolutionDuration.Fastforward(progress);
	}
}
float CelestialBody::GetProgress()
{
	if(!isSun)
	{
		return revolutionDuration.GetProgression();
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