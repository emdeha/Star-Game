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


#ifndef AUDIO_ENUMS_H
#define AUDIO_ENUMS_H


/// \enum ChannelType
/// \brief Holds the types of channels on which music is played.
enum ChannelType
{
	CHANNEL_MASTER, ///< The 'master' music channel.
	CHANNEL_INTERACTION, ///< The 'interaction' music channel.
	CHANNEL_GAME, ///< for game behaviors - fusion, explosion, etc.

	CHANNEL_COUNT,
};

/// \enum SoundTypes
/// \brief Holds the types of sounds which will be played.
enum SoundTypes
{
	MUSIC_BACKGROUND, ///< The background music.
	MUSIC_MENU,
	MUSIC_ON_SUN_CLICK, ///< The on-click music.
	MUSIC_EXPLOSION,
	MUSIC_FUSION,

	MUSIC_COUNT,
};


#endif