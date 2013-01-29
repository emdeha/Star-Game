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
#include "Scene.h"

#include <algorithm>

#define DEPLOY_UNITS_COUNT 4
static int countedUnits = 0;

Scene::Scene(float newSceneGamma)
{
	lights.resize(0);
	suns.resize(0);
	//explosionEmitters.resize(0);
	enemies.resize(0);

	sceneLayouts.clear();

	//universeMusic.AddFileForPlay("../data/music/onclick.wav", SOUND_NAMES_ONSUNCLICK);
	//universeMusic.AddFileForPlay("../data/music/background.mp3", SOUND_NAMES_BACKGROUND);

	sceneGamma = newSceneGamma;
}


void Scene::RenderScene(glutil::MatrixStack &modelMatrix, 
						GLuint materialBlockIndex, GLuint lightUniformBuffer,
						const LitProgData &litData, 
						const UnlitProgData &unLitData, 
						const SimpleProgData &interpData,
						const BillboardProgDataNoTexture &billboardNoTextureData,
						float interpolation)
{
	int sizeLights = lights.size();
	for(int i = 0; i < sizeLights; i++)
	{
		lights[i].Render(modelMatrix, litData, lightUniformBuffer);
	}

	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		suns[i]->Render(modelMatrix, materialBlockIndex, sceneGamma, 
						litData, unLitData, interpData,
						interpolation);
	}

	for(int i = 0; i < enemies.size(); i++)
	{
		if(enemies[i]->IsSceneUpdated())
		{
			enemies[i]->Render(modelMatrix, sceneTopDownCamera.ResolveCamPosition(), billboardNoTextureData);
			enemies[i]->Render(modelMatrix, materialBlockIndex, sceneGamma, litData, interpolation);
		}
	}
}
void Scene::RenderCurrentLayout(const FontProgData &fontData,
								const SimpleProgData &simpleData,
								const TextureProgData &textureData)
{
	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = sceneLayouts.begin();
		iter != sceneLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->Draw(fontData, simpleData, textureData);
			break;
		}
	}
}
void Scene::UpdateScene()
{
	// Should be in the OnEvent function.
	if(!suns.empty())
	{
		if(suns.front()->GetHealth() <= 0)
		{
			suns.front()->RemoveSatellites();
			suns.pop_back();
			lights.pop_back(); // got to find the light connected to the sun
							   // even if I remove the light the ship is still lit
		}
	}

	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		suns[i]->Update();
	}

	/*
	int sizeExplosionEmitters = explosionEmitters.size();
	for(int i = 0; i < sizeExplosionEmitters; i++)
	{
		if(explosionEmitters[i].IsActive())
		{
			explosionEmitters[i].Update();
		}
		if(explosionEmitters[i].IsDead())
		{
			std::vector<ExplosionEmitter>::iterator currentEmitter = 
				explosionEmitters.begin() + i;
			explosionEmitters.erase(currentEmitter);
		}
	}*/

	for(std::vector<std::shared_ptr<Enemy>>::iterator iter = enemies.begin();
		iter != enemies.end();
		)
	{
		if((*iter)->IsSceneUpdated())
		{
			if(!suns.empty())
			{
				(*iter)->Update(false, *suns.front().get());
			}
			else
			{
				(*iter)->Update(true);
			}
		}
		if((*iter)->IsDestroyed())
		{
			enemies.erase(iter);
			break;
		}
		++iter;
	}
	
	if(!suns.empty())
	{
		std::vector<std::shared_ptr<Skill>> skills = suns[0]->GetAllSkills();
		int sizeSkills = skills.size();
		for(int skillIndex = 0; skillIndex < sizeSkills; skillIndex++)
		{
			if(skills[skillIndex]->GetSkillType() == "aoeSkill")
			{
				glm::vec4 mousePos_atZ = 
					sceneMouse.GetPositionAtDimension(currentDisplayData.windowWidth, 
													  currentDisplayData.windowHeight,
													  currentDisplayData.projectionMatrix, 
													  currentDisplayData.modelMatrix, 
													  glm::vec4(sceneTopDownCamera.ResolveCamPosition(), 1.0f), 
													  glm::comp::Y);

				skills[skillIndex]->SetParameter(PARAM_POSITION, 
												 mousePos_atZ.swizzle(glm::comp::X, glm::comp::Y, glm::comp::Z));
			}
			// TODO:
			// Maybe a design flaw - http://stackoverflow.com/a/307793/628873
			if(skills[skillIndex]->GetSkillType() == "passiveAOESkill")
			{
				for(int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++)
				{
					if(skills[skillIndex]->IsIntersectingObject(enemies[enemyIndex]->GetPosition()))
					{
						Event skillEvent = skills[skillIndex]->GetGeneratedEvent("timeended");
						if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
						{
							enemies[enemyIndex]->OnEvent(skillEvent);
							skills[skillIndex]->RemoveGeneratedEvent("timeended");
						}
					}
				}
			}

			if(skills[skillIndex]->GetSkillType() == "sunNovaSkill" ||
			   skills[skillIndex]->GetSkillType() == "satChainSkill" ||
			   skills[skillIndex]->GetSkillType() == "satFrostNova")
			{
				for(int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++)
				{
					if(skills[skillIndex]->IsIntersectingObject(enemies[enemyIndex]->GetPosition()))
					{
						if(skills[skillIndex]->GetSkillType() == "satFrostNova")
						{
							Event skillEvent = skills[skillIndex]->GetGeneratedEvent("stunskilldeployed");
							if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
							{
								enemies[enemyIndex]->OnEvent(skillEvent);
							}
						}
						else
						{
							Event skillEvent = skills[skillIndex]->GetGeneratedEvent("skilldeployed");
							if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
							{
								enemies[enemyIndex]->OnEvent(skillEvent);
							}
						}
					}

					if(skills[skillIndex]->GetSkillType() == "satFrostNova")
					{			
						Event skillEvent = skills[skillIndex]->GetGeneratedEvent("stuntimeended");
						if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
						{
							enemies[enemyIndex]->OnEvent(skillEvent);
							skills[skillIndex]->RemoveGeneratedEvent("stuntimeended");
						}
					}
				}
			}

			if(skills[skillIndex]->GetSkillType() == "satShieldSkill")
			{
				for(int enemyIndex = 0; enemyIndex < enemies.size(); enemyIndex++)
				{
					if(skills[skillIndex]->IsIntersectingObject(enemies[enemyIndex]->GetPosition()))
					{
						Event skillEvent = skills[skillIndex]->GetGeneratedEvent("shieldskilldeployed");
						if(skillEvent.GetType() != EventType::EVENT_TYPE_EMPTY)
						{
							enemies[enemyIndex]->OnEvent(skillEvent);
							skills[skillIndex]->OnEvent(Event(EVENT_TYPE_ATTACKED));
						}
					}
				}
			}
		}
	}
	else if(suns.empty())
	{
		sceneFusionInput.Clear();
	}
}
void Scene::UpdateFusion(char key, Event &returnedFusionEvent)
{
	returnedFusionEvent = sceneFusionInput.Update(key);
	if(returnedFusionEvent.GetType() != EVENT_TYPE_EMPTY)
	{
		OnEvent(returnedFusionEvent);
	}
}
void Scene::UpdateCurrentLayout(int windowWidth, int windowHeight)
{
	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = sceneLayouts.begin();
		iter != sceneLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->Update(windowWidth, windowHeight);
			break;
		}
	}
}

void Scene::AddFusionSequence(std::string sequenceName, 
							  char buttonA, char buttonB, char buttonC)
{
	sceneFusionInput.AddSequence(sequenceName, buttonA, buttonB, buttonC);
}
/*
void Scene::AddExplosionEmitter(const ExplosionEmitter &newExplosionEmitter)
{
	explosionEmitters.push_back(newExplosionEmitter);
}*/

void Scene::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ON_CLICK:
		if(strcmp(_event.GetArgument("object").varString, "sun") == 0)
		{
			sceneMusic.Play(MUSIC_ON_SUN_CLICK);
		}
		if(strcmp(_event.GetArgument("object").varString, "exitButton") == 0)
		{
			this->SetLayout(LAYOUT_MENU, true);
			this->SetLayout(LAYOUT_IN_GAME, false);
		}
		if(strcmp(_event.GetArgument("object").varString, "newGameButton") == 0)
		{
			this->SetLayout(LAYOUT_MENU, false);
			this->SetLayout(LAYOUT_IN_GAME, true);
		}
		if(strcmp(_event.GetArgument("object").varString, "saveGameButton") == 0)
		{
			this->SetLayout(LAYOUT_SAVE_GAME, true);
			this->SetLayout(LAYOUT_MENU, false);
		}
		if(strcmp(_event.GetArgument("object").varString, "printCmd") == 0)
		{
			std::printf("%s", 
				this->GetLayout(LAYOUT_MENU)->GetControl("sample")->GetContent().c_str());
			
			this->GetLayout(LAYOUT_MENU)->GetControl("sample")->Clear();
		}
		if(strcmp(_event.GetArgument("object").varString, "quitGameButton") == 0)
		{
			// Add clean-up if necessary.
			exit(EXIT_SUCCESS);
		}
		if(strcmp(_event.GetArgument("object").varString, "deploySkill") == 0)
		{
			if(!suns.empty())
			{
				std::vector<std::shared_ptr<Skill>> skills = suns[0]->GetAllSkills();
				if(!enemies.empty() && !skills.empty())
				{
					for(int i = 0; i < enemies.size(); i++)
					{
						for(int skillIndex = 0; skillIndex < skills.size(); skillIndex++)
						{
							if(skills[skillIndex]->IsIntersectingObject(enemies[i]->GetPosition()))
							{
								Event skillEvent = skills[skillIndex]->GetGeneratedEvent("skilldeployed");

								enemies[i]->OnEvent(skillEvent);
							}
						}
					}
				}
			}
		}
		break;
	case EVENT_TYPE_SPACE_BTN_CLICK:
		if(strcmp(_event.GetArgument("command").varString, "playBackgroundMusic") == 0)
		{
			sceneMusic.Play(MUSIC_BACKGROUND);
		}
		break;
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("what_event").varString, "fusion_seq") == 0)
		{
			if(strcmp(_event.GetArgument("buttons").varString, 
					  sceneFusionInput.GetSequenceButtons("fireSatellite").c_str()) == 0)
			{
				if(HasSuns())
				{
					suns[0]->AddSatellite("mesh-files/UnitSphere.xml", 
										  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
										  10.0f, 0.5f,
										  SatelliteType(SATELLITE_FIRE), 
										  5);
				}
			}
			if(strcmp(_event.GetArgument("buttons").varString, 
					  sceneFusionInput.GetSequenceButtons("waterSatellite").c_str()) == 0)
			{
				if(HasSuns())
				{
					suns[0]->AddSatellite("mesh-files/UnitSphere.xml", 
										  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
										  10.0f, 0.5f,
										  SatelliteType(SATELLITE_WATER), 
										  5);
				}
			}
			if(strcmp(_event.GetArgument("buttons").varString, 
					  sceneFusionInput.GetSequenceButtons("earthSatellite").c_str()) == 0)
			{
				if(HasSuns())
				{
					suns[0]->AddSatellite("mesh-files/UnitSphere.xml", 
										  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
										  10.0f, 0.5f,
										  SatelliteType(SATELLITE_EARTH), 
										  5);
				}
			}
			if(strcmp(_event.GetArgument("buttons").varString,
			   sceneFusionInput.GetSequenceButtons("airSatellite").c_str()) == 0)
			{
				if(HasSuns())
				{
					suns[0]->AddSatellite("mesh-files/UnitSphere.xml",
										  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
										  10.0f, 0.5f, 
										  SatelliteType(SATELLITE_AIR),
										  5);
				}
			}

			std::vector<std::shared_ptr<Skill>> skills = suns[0]->GetAllSkills();
			for(int i = 0; i < skills.size(); i++)
			{
				skills[i]->OnEvent(_event);
			}
		}
		break;
	default:
		break;
	};
}

void Scene::StartScene()
{
	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		std::vector<std::shared_ptr<CelestialBody>> sunSatellites = suns[i]->GetSatellites();
		for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin();
			iter != sunSatellites.end(); ++iter)
		{
			(*iter)->Start();
		}
	}
}
void Scene::StopScene()
{
	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		std::vector<std::shared_ptr<CelestialBody>> sunSatellites = suns[i]->GetSatellites();
		for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin();
			iter != sunSatellites.end(); ++iter)
		{
			(*iter)->Stop();
		}
	}
}


void Scene::AddSunLight(const SunLight &newSunLight)
{
	lights.push_back(newSunLight);
}
void Scene::AddSun(const std::shared_ptr<CelestialBody> newSun)
{
	suns.push_back(newSun);
}
void Scene::AddEnemy(const std::shared_ptr<Enemy> newEnemy)
{
	enemies.push_back(newEnemy);
}

void Scene::AddLayouts(const std::map<LayoutType, std::shared_ptr<Layout>> &newLayouts)
{
	sceneLayouts.insert(newLayouts.begin(), newLayouts.end());
}
void Scene::AddLayout(const std::shared_ptr<Layout> newLayout)
{
	sceneLayouts.insert(std::make_pair(newLayout->GetLayoutType(), newLayout));
}
void Scene::AddLayout(LayoutType layoutType, LayoutInfo layoutInfo)
{
	sceneLayouts.insert(std::make_pair(layoutType, std::shared_ptr<Layout>(new Layout(layoutType, layoutInfo))));
}
std::shared_ptr<Layout> Scene::GetLayout(LayoutType layoutType)
{
	if(this->HasLayout(layoutType))
	{
		return sceneLayouts[layoutType];
	}

	// TODO: Better error handling.
	std::printf("No such layout\n");
}
bool Scene::HasLayout(LayoutType layoutType)
{
	return sceneLayouts.find(layoutType) != sceneLayouts.end();
}
bool Scene::IsLayoutOn(LayoutType layoutType)
{
	if(this->HasLayout(layoutType))
	{
		return sceneLayouts[layoutType]->IsSet();
	}
	return false;
}
void Scene::SetLayout(LayoutType layoutType, bool isSet)
{
	if(this->HasLayout(layoutType))
	{
		sceneLayouts[layoutType]->Set(isSet);
	}

	// TODO: Better error handling.
	else std::printf("No such layout\n");
}


void Scene::SetMusic(const std::string &fileName, SoundTypes soundType)
{
	sceneMusic.SetFileForPlay(fileName, soundType);
}
void Scene::SetMusicVolume(float volume, ChannelType chType)
{
	sceneMusic.SetVolume(volume, chType);
}

void Scene::SetDisplayData(const DisplayData &newDisplayData)
{
	currentDisplayData.gamma = newDisplayData.gamma;
	currentDisplayData.modelMatrix = newDisplayData.modelMatrix;
	currentDisplayData.projectionMatrix = newDisplayData.projectionMatrix;
	currentDisplayData.mousePosition = newDisplayData.mousePosition;
	currentDisplayData.windowHeight = newDisplayData.windowHeight;
	currentDisplayData.windowWidth = newDisplayData.windowWidth;
	currentDisplayData.zFar = newDisplayData.zFar;
	currentDisplayData.zNear = newDisplayData.zNear;
}

void Scene::PlayMusic(SoundTypes soundType)
{
	sceneMusic.Play(soundType);
}

void Scene::SetTopDownCamera(const TopDownCamera &newCamera)
{
	sceneTopDownCamera = newCamera;
}
void Scene::SetMouse(const Mouse &newMouse)
{
	sceneMouse = newMouse;
}

void Scene::SetGamma(float newSceneGamma)
{
	sceneGamma = newSceneGamma;
}

void Scene::SetFusion(const FusionInput &newFusionInput)
{
	sceneFusionInput = newFusionInput;
}

void Scene::SetLayoutPreset(LayoutPreset layoutPreset)
{
	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = sceneLayouts.begin();
		iter != sceneLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->SetCurrentPreset(layoutPreset);
			break;
		}
	}
}

TopDownCamera &Scene::GetTopDownCamera()
{
	return sceneTopDownCamera;
}
Mouse &Scene::GetMouse()
{
	return sceneMouse;
}


std::shared_ptr<CelestialBody> Scene::GetSun()
{
	return suns.front(); // replace 0 with an index
}
SunLight Scene::GetSunLight()
{
	return lights.front();
}

bool Scene::HasSuns()
{
	if(!suns.empty())
		return true;
	return false;
}

#include <ctime>

void Scene::GenerateRandomSwarms(int count, const BillboardProgDataNoTexture &progData)
{
	if(enemies.size() <= 0)
	{
		srand(time(0));

		for(int i = 0; i < count; i++)
		{
			float range = ((float)rand() / (float)RAND_MAX) * 4.0f + 4.0f;
			float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

			float posX = cosf(posOnCircle * (2.0f * PI)) * range;
			float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

			glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
			glm::vec3 frontVector = glm::normalize((glm::vec3(suns[0]->GetPosition()) - position));
			float speed = 0.05f;
		
			std::shared_ptr<Swarm> randSwarm = 
				std::shared_ptr<Swarm>(new Swarm(100, 1, 5, progData, 
												 glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 
												 position, frontVector, speed, 3.0f, 50));

			enemies.push_back(randSwarm);
		}
	}
}

void Scene::GenerateRandomSpaceships(int count)
{
	if(enemies.size() <= 0)
	{
		srand(time(0));

		for(int i = 0; i < count; i++)
		{
			float range = ((float)rand() / (float)RAND_MAX) * 4.0f + 2.0f;
			float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

			float posX = cosf(posOnCircle * (2.0f * PI)) * range;
			float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

			glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
			glm::vec3 frontVector = glm::normalize((glm::vec3(suns[0]->GetPosition()) - position));
			float speed = 0.05f;

			std::shared_ptr<Spaceship> randSpaceship =
				std::shared_ptr<Spaceship>(new Spaceship(0.3f, 20, 1, 
														 glm::vec4(0.21f, 0.42f, 0.34f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
														 position, frontVector, speed, 
														 2.0f, 50));
			randSpaceship->LoadMesh("mesh-files/Ship.xml");
			randSpaceship->LoadProjectileMesh("mesh-files/UnitSphere.xml");
			

			enemies.push_back(randSpaceship);
		}
	}
}

void Scene::GenerateRandomSuicideBombers(int count)
{
	if(enemies.size() <= 0)
	{
		srand(time(0));

		for(int i = 0; i < count; i++)
		{
			float range = ((float)rand() / (float)RAND_MAX) * 4.0f + 2.0f;
			float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

			float posX = cosf(posOnCircle * (2.0f * PI)) * range;
			float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

			glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
			glm::vec3 frontVector = glm::normalize((glm::vec3(suns[0]->GetPosition()) - position));
			float speed = 0.02f;
			float chargeSpeed = 0.1f;

			
			std::shared_ptr<FastSuicideBomber> randBomber = 
				std::shared_ptr<FastSuicideBomber>(new FastSuicideBomber(50, chargeSpeed,
																		 glm::vec4(0.5f, 0.5f, 0.7f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
																		 position, frontVector, speed,
																		 2.0f, 50));
			randBomber->LoadMesh("mesh-files/UnitSphere.xml");

			enemies.push_back(randBomber);
		}
	}
}

void Scene::GenerateRandomMothership()
{
	if(enemies.size() <= 0)
	{
		srand(time(0));

		float range = ((float)rand() / (float)RAND_MAX) * 8.0f + 6.0f;
		float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

		float posX = cosf(posOnCircle * (2.0f * PI)) * range;
		float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

		glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
		glm::vec3 frontVector = glm::normalize((glm::vec3(suns[0]->GetPosition()) - position));
		float speed = 0.03f;

		std::shared_ptr<Mothership> randMothership =
			std::shared_ptr<Mothership>(new Mothership(glm::vec4(0.21f, 0.42f, 0.34f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
													   position, frontVector, speed, 5.0f, 300));
		randMothership->LoadMesh("mesh-files/Ship.xml");

		int deployUnitsCount = 4;
		randMothership->InitDeployUnits("mesh-files/Ship.xml", deployUnitsCount, 
										0.3f, 20, 1, 
										glm::vec4(0.21f, 0.42f, 0.34f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
										0.05f, 2.0f, 50);
		enemies.push_back(randMothership);
		
		std::vector<std::shared_ptr<DeployUnit>> deployUnits = randMothership->GetDeployUnits();
		for(int i = 0; i < deployUnits.size(); i++)
		{
			enemies.push_back(deployUnits[i]);
		}
	}
}

void Scene::GenerateRandomAsteroids(int count)
{
	if(enemies.size() <= 0)
	{
		srand(time(0));

		for(int i = 0; i < count; i++)
		{
			float range = ((float)rand() / (float)RAND_MAX) * 8.0f + 6.0f;
			float posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;

			float posX = cosf(posOnCircle * (2.0f * PI)) * range;
			float posZ = sinf(posOnCircle * (2.0f * PI)) * range;

			posOnCircle = ((float)rand() / (float)RAND_MAX) * 360;
			float dirX = cosf(posOnCircle * (2.0f * PI)) * range;
			float dirZ = sinf(posOnCircle * (2.0f * PI)) * range;

			glm::vec3 position = glm::vec3(posX, 0.0f, posZ);
			glm::vec3 frontVector = glm::normalize(glm::vec3(dirX, 0.0f, dirZ));
			float speed = 0.03f;

			std::shared_ptr<Asteroid> randAsteroid = 
				std::shared_ptr<Asteroid>(new Asteroid(20, 
													   glm::vec4(0.57, 0.37, 0.26, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
													   position, frontVector, speed, 0.0f, 50));
			randAsteroid->LoadMesh("mesh-files/UnitTetrahedron.xml");

			enemies.push_back(randAsteroid);
		}
	}
}