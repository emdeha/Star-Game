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

#include <fstream>


AudioLoader::AudioLoader(const std::string &fileName)
{
	std::string line;
	std::ifstream data(fileName);

	loadedAudio.clear();

	if(data.is_open())
	{
		while(!data.eof())
		{
			getline(data, line);
			char command[50];
			sscanf(line.c_str(), "%s ", &command);

			if(strcmp(command, "channel") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				AudioData audioData;
				sscanf(line.c_str(), "%i %f ", &audioData.channel, &audioData.channelVolume);
				loadedAudio.push_back(std::pair<ChannelType, AudioData>(audioData.channel, audioData));
			}
			if(strcmp(command, "audio") == 0)
			{
				line.erase(line.begin(), line.begin() + strlen(command));
				line[0] = ' ';

				AudioFile audioFile;
				char path[50];
				int chType;
				sscanf(line.c_str(), "%i %i %s ", &chType, &audioFile.soundType, &path);
				audioFile.path = path;
				for(std::vector<std::pair<ChannelType, AudioData>>::iterator iter = loadedAudio.begin();
					iter != loadedAudio.end(); ++iter)
				{
					if(iter->first == chType)
					{
						iter->second.audioFiles.push_back(audioFile);
						break;
					}
				}
			}
			if(strcmp(command, "**") == 0)
			{
				continue;
			}
			if(strcmp(command, "endfile") == 0)
			{
				data.close();
				return;
			}
		}
		data.close();
	}
	else
	{
		// TODO: Better error handling
		std::printf("Cannot open audio config file.\n");
		return;
	}
}

const std::vector<std::pair<ChannelType, AudioData>> AudioLoader::GetAllLoadedAudios()
{
	return loadedAudio;
}