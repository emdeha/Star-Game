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
#include "../framework/ErrorAPI.h"

#include <iostream>
#include <assert.h>


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
        std::printf("You are using an old version of FMOD %08x. This program requires %08x\n", version, FMOD_VERSION);
        HandleUnexpectedError("", __LINE__, __FILE__);
    }

    result = system->init(numberOfChannels, FMOD_INIT_NORMAL, 0);
    CheckForError(result);
}

void Audio::SetFileForPlay(const std::string &newFileName, SoundType prevSoundType, bool isLooping)
{
    FMOD_RESULT result;

    FMOD::Sound *newSound;
    result = system->createSound(newFileName.c_str(), FMOD_HARDWARE | FMOD_CREATESTREAM, 0, &newSound);
    CheckForError(result);
    if(isLooping)
    {
        result = newSound->setMode(FMOD_LOOP_NORMAL);
        CheckForError(result);
    }

    if(audioFiles.find(prevSoundType) != audioFiles.end())
	{
        audioFiles[prevSoundType] = newSound;
	}
    else 
	{
		audioFiles.insert(std::make_pair(prevSoundType, newSound));
	}
}

void Audio::SetVolume(float volume, ChannelType chType)
{
    volumes[chType] = volume;
}

void Audio::Play(SoundType soundType, ChannelType chType)
{
    if(audioFiles.find(soundType) == audioFiles.end())
    {
        HandleUnexpectedError("there is no sound at this index", __LINE__, __FILE__);
    }
        
    FMOD_RESULT result;
    
    result = system->playSound(FMOD_CHANNEL_FREE, audioFiles[soundType], false, &channels[chType]);
    CheckForError(result);
        
    if(volumes[chType] != 0.0f)
    {
        result = channels[chType]->setVolume(volumes[chType]);
        CheckForError(result);
    }

    system->update();
}

void Audio::Stop(ChannelType chType)
{	
    FMOD_RESULT result;
    result = channels[chType]->stop();
    CheckForError(result);
}

Audio::~Audio()
{
    FMOD_RESULT result;

    for(std::map<SoundType, FMOD::Sound *>::iterator iter = audioFiles.begin();
        iter != audioFiles.end(); ++iter)
    {
        result = iter->second->release();
        CheckForError(result);
    }

    result = system->release();
    CheckForError(result);

    channels.clear();
}