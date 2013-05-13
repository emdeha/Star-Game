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


#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <map>
#include <string>
#include "../fmod/fmod.hpp"
#include "../fmod/fmod_errors.h"
#include "../StarGame/AudioEnums.h"

/// \fn CheckForError
/// \brief Checks if FMOD has generated an error.
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

    /// The volume parameters are contained in an array. 
    /// Each element represents a channels volume. The first index is for
    /// the master volume, the second for the interactions and so on.
    float volumes[CHANNEL_COUNT];

public:
    Audio();
    //Audio(int newNumberOfChannels);
    
    /// \fn SetFileForPlay
    /// \brief Loads a sound for later playing.
    void SetFileForPlay(const std::string &newFileName, SoundType prevSoundType, bool isLooping = false);
    
    /// \fn SetVolume
    /// \brief Fills the `volumes` array which later will be passed to Play().
    void SetVolume(float volume, ChannelType chType = CHANNEL_MASTER);

    /// \fn Play
    /// \brief Plays a given song.
    void Play(SoundType soundName, ChannelType chType = CHANNEL_MASTER);

    void Stop(ChannelType chType = CHANNEL_MASTER);

    ~Audio();

private:
    FMOD::System *system;
    std::vector<FMOD::Channel *> channels;

    std::map<SoundType, FMOD::Sound *> audioFiles;
};

#endif