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
#include "AudioLoader.h"
#include "../framework/ErrorAPI.h"

#include <fstream>


AudioLoader::AudioLoader(const std::string &fileName)
{
	YAML::Node audio = YAML::LoadFile(fileName);

	AudioData audioData;
	//audioData.channel = ChannelType(audio["channel-master"]["index"].as<int>());
	//audioData.channelVolume = audio["channel-master"]["volume"].as<float>();
	
	for(YAML::Node::const_iterator channel = audio.begin();
		channel != audio.end(); ++channel)
	{	
		audioData.channel = ChannelType((*channel).second["index"].as<int>());
		audioData.channelVolume = (*channel).second["volume"].as<float>();

		loadedAudio.push_back(std::pair<ChannelType, AudioData>(audioData.channel, audioData));
		
		AudioFile audioFile;

		std::printf("channel: %i, %f: \n", audioData.channel, audioData.channelVolume);

		for(YAML::Node::const_iterator music = (*channel).second["music"].begin();
			music != (*channel).second["music"].end(); ++music)
		{
			audioFile.soundType = SoundType((*music).second["sound-type"].as<int>());
			audioFile.path = (*music).second["sound-file"].as<std::string>();
			loadedAudio[audioData.channel].second.audioFiles.push_back(audioFile);
			std::printf("	audio: %i %s \n", audioFile.soundType, audioFile.path.c_str());
		}
	}
}

const std::vector<std::pair<ChannelType, AudioData>> AudioLoader::GetAllLoadedAudios()
{
	return loadedAudio;
}