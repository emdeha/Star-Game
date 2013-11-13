#include "stdafx.h"
#include "AssetLoader.h"

#include "../framework/ErrorAPI.h"


using namespace FusionEngine;


const std::vector<std::pair<ChannelType, AudioData>> AudioAssetObject::GetAllLoadedAudios() const
{
	return loadedAudio;
}


AudioAssetObject CAudioLoader::Load(const std::string &type, const std::string &name)
{
	YAML::Node audio = YAML::LoadFile("../data/" + type + "/" + name);

	AudioData audioData;

	std::vector<std::pair<ChannelType, AudioData>> loadedAudio;

	for(YAML::Node::const_iterator channel = audio.begin();
		channel != audio.end(); ++channel)
    {
		if(channel->first.as<std::string>() == "channel-master")
        {
			audioData.channel = CHANNEL_MASTER;
        }
        else if(channel->first.as<std::string>() == "channel-interaction")
        {
			audioData.channel = CHANNEL_INTERACTION;
        }
        else if(channel->first.as<std::string>() == "channel-game")
        {
			audioData.channel = CHANNEL_GAME;
        }
		audioData.channelVolume = channel->second["volume"].as<float>();

		AudioFile audioFile;
		for(YAML::Node::const_iterator music = channel->second["music"].begin();
			music != channel->second["music"].end(); ++music)
        {
			audioFile.soundType = SoundType(music->second["sound-type"].as<int>());
			audioFile.path = music->second["sound-file"].as<std::string>();
            if(music->second["sound-loop"])
            {
				audioFile.isLooping = true;
            }
            else
            {
				audioFile.isLooping = false;
            }
			audioData.audioFiles.push_back(audioFile);
        }

		loadedAudio.push_back(std::pair<ChannelType, AudioData>(audioData.channel, audioData));
    }

	return AudioAssetObject(loadedAudio);
}