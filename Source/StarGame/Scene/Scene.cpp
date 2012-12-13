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

Scene::Scene()
{
	lights.resize(0);
	suns.resize(0);
	spaceships.resize(0);
	fastSuicideBombers.resize(0);
	explosionEmitters.resize(0);

	sceneLayouts.clear();

	//universeMusic.AddFileForPlay("../data/music/onclick.wav", SOUND_NAMES_ONSUNCLICK);
	//universeMusic.AddFileForPlay("../data/music/background.mp3", SOUND_NAMES_BACKGROUND);

	sceneGamma = 2.2f;
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

	int sizeSpaceships = spaceships.size();
	for(int i = 0; i < sizeSpaceships; i++)
	{
		spaceships[i]->Render(modelMatrix, materialBlockIndex, sceneGamma, litData,
							  interpolation);
	}

	int sizeFastSuicideBombers = fastSuicideBombers.size();
	for(int i = 0; i < sizeFastSuicideBombers; i++)
	{
		fastSuicideBombers[i]->Render(modelMatrix, litData, materialBlockIndex, interpolation, 
									  sceneGamma);
	}

	int sizeExplosionEmitters = explosionEmitters.size();
	for(int i = 0; i < sizeExplosionEmitters; i++)
	{
		if(explosionEmitters[i].IsActive())
		{
			explosionEmitters[i].Render(modelMatrix, sceneTopDownCamera.ResolveCamPosition(), 
										billboardNoTextureData);
		}
	}
}
void Scene::RenderCurrentLayout(const FontProgData &fontData,
								const SimpleProgData &simpleData)
{
	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = sceneLayouts.begin();
		iter != sceneLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->Draw(fontData, simpleData);
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
		//if(suns[i]->GetIsClicked())
		//{
		//	universeMusic.Play(MUSIC_ON_SUN_CLICK);
		//}
	}

	int sizeSpaceships = spaceships.size();
	for(int i = 0; i < sizeSpaceships; i++)
	{
		if(!suns.empty())
		{
			spaceships[i]->Update(false, *suns.front().get());
		}
		else 
		{
			spaceships[i]->Update(true);
		}
	}

	int sizeFastSuicideBombers = fastSuicideBombers.size();
	for(int i = 0; i < sizeFastSuicideBombers; i++)
	{
		Event returnedEvent = StockEvents::EmptyEvent();

		if(!suns.empty())
		{
			returnedEvent = fastSuicideBombers[i]->Update(false, *suns.front().get());
		}
		else
		{
			returnedEvent = fastSuicideBombers[i]->Update(true);
		}

		this->OnEvent(returnedEvent);
	}

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

void Scene::AddExplosionEmitter(const ExplosionEmitter &newExplosionEmitter)
{
	explosionEmitters.push_back(newExplosionEmitter);
}

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
										  10.0f, 0.25f,
										  SatelliteType(SATELLITE_FIRE), 
										  5);
				}
			}
			if(strcmp(_event.GetArgument("buttons").varString, 
					  //"www"
					  sceneFusionInput.GetSequenceButtons("waterSatellite").c_str()) == 0)
			{
				if(HasSuns())
				{
					suns[0]->AddSatellite("mesh-files/UnitSphere.xml", 
										  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
										  10.0f, 0.25f,
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
										  10.0f, 0.25f,
										  SatelliteType(SATELLITE_EARTH), 
										  5);
				}
			}
		}
		if(strcmp(_event.GetArgument("what_event").varString, "explStarted") == 0)
		{
			int explosionIndex = _event.GetArgument("expl_index").varInteger;
			// TODO: add bounds check
			explosionEmitters[explosionIndex].SetPosition(
				fastSuicideBombers[explosionIndex]->GetPosition());
			explosionEmitters[explosionIndex].Activate();
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
		std::vector<std::shared_ptr<Satellite>> sunSatellites = suns[i]->GetSatellites();
		for(std::vector<std::shared_ptr<Satellite>>::iterator iter = sunSatellites.begin();
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
		std::vector<std::shared_ptr<Satellite>> sunSatellites = suns[i]->GetSatellites();
		for(std::vector<std::shared_ptr<Satellite>>::iterator iter = sunSatellites.begin();
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
void Scene::AddSun(const std::shared_ptr<Sun> newSun)
{
	suns.push_back(newSun);
}
void Scene::AddSpaceship(const std::shared_ptr<Spaceship> newSpaceship)
{
	spaceships.push_back(newSpaceship);
}
void Scene::AddFastSuicideBomber(const std::shared_ptr<FastSuicideBomber> newFastSuicideBomber)
{
	fastSuicideBombers.push_back(newFastSuicideBomber);
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


std::shared_ptr<Sun> Scene::GetSun()
{
	return suns.front(); // replace 0 with an index
}
std::shared_ptr<Spaceship> Scene::GetSpaceship()
{
	return spaceships.front();
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