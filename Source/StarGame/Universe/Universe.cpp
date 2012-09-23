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
#include "Universe.h"

Universe::Universe()
{
	lights.resize(0);
	suns.resize(0);

	universeLayouts.clear();

	//universeMusic.AddFileForPlay("../data/music/onclick.wav", SOUND_NAMES_ONSUNCLICK);
	//universeMusic.AddFileForPlay("../data/music/background.mp3", SOUND_NAMES_BACKGROUND);

	universeGamma = 2.2f;
}


void Universe::RenderUniverse(glutil::MatrixStack &modelMatrix, 
							  GLuint materialBlockIndex, GLuint lightUniformBuffer,
							  const LitProgData &litData, 
							  const UnlitProgData &unLitData, 
							  const SimpleProgData &interpData)
{
	int sizeLights = lights.size();
	for(int i = 0; i < sizeLights; i++)
	{
		lights[i].Render(modelMatrix, litData, lightUniformBuffer);
	}

	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		suns[i]->Render(modelMatrix, materialBlockIndex, universeGamma, litData, unLitData, interpData);
	}
}
void Universe::RenderCurrentLayout(const FontProgData &fontData,
								   const SimpleProgData &simpleData)
{
	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = universeLayouts.begin();
		iter != universeLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->Draw(fontData, simpleData);
			break;
		}
	}
}
void Universe::UpdateUniverse()
{
	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		suns[i]->Update();
		//if(suns[i]->GetIsClicked())
		//{
		//	universeMusic.Play(MUSIC_ON_SUN_CLICK);
		//}
	}
}
void Universe::UpdateCurrentLayout(int windowWidth, int windowHeight)
{
	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = universeLayouts.begin();
		iter != universeLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->Update(windowWidth, windowHeight);
			break;
		}
	}
}

void Universe::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_ON_CLICK:
		if(strcmp(_event.GetArgument("object").varString, "sun") == 0)
		{
			universeMusic.Play(MUSIC_ON_SUN_CLICK);
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
			universeMusic.Play(MUSIC_BACKGROUND);
		}
		break;
	default:
		break;
	};
}

void Universe::StartUniverse()
{
	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		std::vector<Satellite*> sunSatellites = suns[i]->GetSatellites();
		for(std::vector<Satellite*>::iterator iter = sunSatellites.begin();
			iter != sunSatellites.end(); ++iter)
		{
			(*iter)->Start();
			delete *iter;
		}
		sunSatellites.clear();
	}
}
void Universe::StopUniverse()
{
	int sizeSuns = suns.size();
	for(int i = 0; i < sizeSuns; i++)
	{
		std::vector<Satellite*> sunSatellites = suns[i]->GetSatellites();
		for(std::vector<Satellite*>::iterator iter = sunSatellites.begin();
			iter != sunSatellites.end(); ++iter)
		{
			(*iter)->Stop();
			delete *iter;
		}
		sunSatellites.clear();
	}
}


void Universe::AddSunLight(const SunLight &newSunLight)
{
	lights.push_back(newSunLight);
}
void Universe::AddSun(const std::shared_ptr<Sun> newSun)
{
	suns.push_back(newSun);
}


void Universe::AddLayouts(const std::map<LayoutType, std::shared_ptr<Layout>> &newLayouts)
{
	universeLayouts.insert(newLayouts.begin(), newLayouts.end());
}
void Universe::AddLayout(const std::shared_ptr<Layout> newLayout)
{
	universeLayouts.insert(std::make_pair(newLayout->GetLayoutType(), newLayout));
}
void Universe::AddLayout(LayoutType layoutType, LayoutInfo layoutInfo)
{
	universeLayouts.insert(std::make_pair(layoutType, std::shared_ptr<Layout>(new Layout(layoutType, layoutInfo))));
}
std::shared_ptr<Layout> Universe::GetLayout(LayoutType layoutType)
{
	if(this->HasLayout(layoutType))
	{
		return universeLayouts[layoutType];
	}

	// TODO: Better error handling.
	std::printf("No such layout\n");
}
bool Universe::HasLayout(LayoutType layoutType)
{
	return universeLayouts.find(layoutType) != universeLayouts.end();
}
bool Universe::IsLayoutOn(LayoutType layoutType)
{
	if(this->HasLayout(layoutType))
	{
		return universeLayouts[layoutType]->IsSet();
	}
	return false;
}
void Universe::SetLayout(LayoutType layoutType, bool isSet)
{
	if(this->HasLayout(layoutType))
	{
		universeLayouts[layoutType]->Set(isSet);
	}

	// TODO: Better error handling.
	else std::printf("No such layout\n");
}


void Universe::SetMusic(const std::string &fileName, SoundTypes soundType)
{
	universeMusic.SetFileForPlay(fileName, soundType);
}
void Universe::SetMusicVolume(float volume, ChannelType chType)
{
	universeMusic.SetVolume(volume, chType);
}
void Universe::PlayMusic(SoundTypes soundType)
{
	universeMusic.Play(soundType);
}


void Universe::SetGamma(float newUniverseGamma)
{
	universeGamma = newUniverseGamma;
}

void Universe::SetLayoutPreset(LayoutPreset layoutPreset)
{
	for(std::map<LayoutType, std::shared_ptr<Layout>>::iterator iter = universeLayouts.begin();
		iter != universeLayouts.end(); ++iter)
	{
		if(iter->second->IsSet())
		{
			iter->second->SetCurrentPreset(layoutPreset);
			break;
		}
	}
}


Universe::~Universe()
{
	if(!lights.empty())
		lights.clear();
}	