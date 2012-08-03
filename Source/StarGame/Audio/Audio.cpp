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


Audio::Audio(int newNumberOfChannels)
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
}

void Audio::AddFileForPlay(const char *fileName)
{
	FMOD_RESULT result;

	FMOD::Sound *newSound;
	result = system->createSound(fileName, FMOD_HARDWARE, 0, &newSound);
	CheckForError(result);

	audioFiles.insert(std::make_pair(fileName, newSound));
}

void Audio::Play(const char *fileName)
{
	// TODO: Handle errors.

	FMOD_RESULT result;
	result = system->playSound(FMOD_CHANNEL_FREE, audioFiles[fileName], false, &channel);
	CheckForError(result);

	system->update();
}


Audio::~Audio()
{
	FMOD_RESULT result;

	for(std::map<const char *, FMOD::Sound *>::iterator iter = audioFiles.begin();
		iter != audioFiles.end(); ++iter)
	{
		result = iter->second->release();
		CheckForError(result);
	}

	result = system->close();
	CheckForError(result);
	result = system->release();
	CheckForError(result);
}