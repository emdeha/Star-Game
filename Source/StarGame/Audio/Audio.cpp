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
#include "Audio.h"

#include <iostream>


Audio::Audio()
{
	audioFiles.clear();
	numberOfChannels = 32;

	channels.resize(32);

	for(int i = 0; i < CHANNEL_COUNT; i++)
		volumes[i] = 0.0f;

	FMOD_RESULT result;

	result = FMOD::System_Create(&system);
	CheckForError(result);

	unsigned int version = 0;
	result = system->getVersion(&version);
	CheckForError(result);

	if(version < FMOD_VERSION)
	{
		std::printf("You are using an old version of FMOD %08x. This program requires %08x\n", 
			version, FMOD_VERSION);
	}

	result = system->init(numberOfChannels, FMOD_INIT_NORMAL, 0);
	CheckForError(result);
}
/*Audio::Audio(int newNumberOfChannels)
{
	audioFiles.clear();
	numberOfChannels = newNumberOfChannels;

	FMOD_RESULT result;

	result = FMOD::System_Create(&system);
	CheckForError(result);

	unsigned int version = 0;
	result = system->getVersion(&version);
	CheckForError(result);

	if(version < FMOD_VERSION)
	{
		std::printf("You are using an old version of FMOD %08x. This program requires %08x\n", 
			version, FMOD_VERSION);
	}

	result = system->init(numberOfChannels, FMOD_INIT_NORMAL, 0);
	CheckForError(result);
}*/

void Audio::SetFileForPlay(const std::string &newFileName, SoundTypes prevSoundType)
{
	FMOD_RESULT result;

	FMOD::Sound *newSound;
	result = system->createSound(newFileName.c_str(), FMOD_HARDWARE, 0, &newSound);
	CheckForError(result);

	if(audioFiles.find(prevSoundType) != audioFiles.end())
		audioFiles[prevSoundType] = newSound;
	else audioFiles.insert(std::make_pair(prevSoundType, newSound));
}

void Audio::SetVolume(float volume, ChannelType chType)
{
	volumes[chType] = volume;
	std::printf("%f\n", volumes[chType]);
}

void Audio::Play(SoundTypes soundType, ChannelType chType)
{
	// TODO: Handle errors.
	if(audioFiles.find(soundType) == audioFiles.end())
	{
		std::printf("There is no sound at this index. Check Audio.h.\n");
		return;
	}

	FMOD_RESULT result;
	result = system->playSound(FMOD_CHANNEL_FREE, audioFiles[soundType], false, &channels[chType]);
	CheckForError(result);

	system->update();

	if(volumes[chType] != 0.0f)
	{
		result = channels[chType]->setVolume(volumes[chType]);
		CheckForError(result);
	}
}

Audio::~Audio()
{
	FMOD_RESULT result;

	for(std::map<SoundTypes, FMOD::Sound *>::iterator iter = audioFiles.begin();
		iter != audioFiles.end(); ++iter)
	{
		result = iter->second->release();
		CheckForError(result);

		delete(*iter).second;
	}

	result = system->close();
	CheckForError(result);
	result = system->release();
	CheckForError(result);

	delete system;
	for(std::vector<FMOD::Channel *>::iterator iter = channels.begin();
		iter != channels.end(); ++iter)
	{
		delete *iter;
	}
	channels.clear();
}