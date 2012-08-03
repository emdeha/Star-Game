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


/// \ingroup module_StarGame

/// \file Audio.h
/// \brief Header for the music handling class.


#include <vector>
#include <map>
#include "../fmod/fmod.hpp"
#include "../fmod/fmod_errors.h"


static void CheckForError(FMOD_RESULT result)
{
	if(result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return;
	}
}

/// \class Audio
/// \brief Handles music, sounds, etc.

/// This class uses the FMOD api for handling music.

// TODO: Use smart ptrs.
class Audio
{
private:
	int numberOfChannels;

public:
	Audio(int newNumberOfChannels);
	
	void AddFileForPlay(const char *fileName);

	void Play(const char *fileName);

	~Audio();

private:
	FMOD::System *system;
	FMOD::Channel *channel;

	std::map<const char *, FMOD::Sound *> audioFiles;
};