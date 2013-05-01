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
#include "GUISystem.h"
#include "../framework/ErrorAPI.h"


void ImageBox::Init(const std::string &imageFileName,
					int windowWidth, int windowHeight)
{
	if(isUsingPercentage)
	{
		position = glm::vec2((percentagedPosition.x / 100) * windowWidth,
								(percentagedPosition.y / 100) * windowHeight);
	}

	image = 
		Utility::Primitives::Sprite(glm::vec3(position, 0.0f),
									glm::vec4(1.0f), width, height, false);
	image.Init(imageFileName);
}

void ImageBox::ComputeNewAttributes()
{
	if(isUsingPercentage)
	{
		position = glm::vec2((percentagedPosition.x / 100) * windowWidth,
								(percentagedPosition.y / 100) * windowHeight);
	}
	image.Update(width, height, position);
}

void ImageBox::Update(int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;

	ComputeNewAttributes();
}

void ImageBox::Draw(const FontProgData &fontData, const TextureProgData &textureData)
{
	glutil::MatrixStack identityMat;
	identityMat.SetIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	image.Draw(identityMat, textureData);

	glDisable(GL_BLEND);
}

std::string ImageBox::GetType()
{
	return "ImageBox";
}

/*
void ImageBox::Init()
{
	boxSprite.Init(fusionTextures[0]);
}


void ImageBox::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("what_event").varString, "fusion_button") == 0)
		{
			std::string button = _event.GetArgument("what_button").varString;

			if(_event.GetArgument("button_pos").varInteger == index)
			{
				if(strcmp(button.c_str(), "q") == 0)
				{
					boxSprite.ChangeTexture(fusionTextures[1]);
				}
				if(strcmp(button.c_str(), "w") == 0)
				{
					boxSprite.ChangeTexture(fusionTextures[2]);
				}
				if(strcmp(button.c_str(), "e") == 0)
				{
					boxSprite.ChangeTexture(fusionTextures[3]);
				}
			}
		}
		if(strcmp(_event.GetArgument("what_event").varString, "fusion_seq") == 0)
		{
			boxSprite.ChangeTexture(fusionTextures[0]);
		}
		break;
	default:
		HandleUnexpectedError("invalid event type", __LINE__, __FILE__);
		break;
	}
}

void ImageBox::SetPosition(glm::vec2 newPosition)
{
	presets[SMALL].position = newPosition;
	presets[MEDIUM].position = newPosition;
	presets[BIG].position = newPosition;
}

void ImageBox::AddPreset(LayoutPreset newPreset, glm::vec2 newPosition)
{
	presets[newPreset].position = newPosition;
}


void ImageBox::Update(int newWindowWidth, int newWindowHeight)
{
	windowWidth = newWindowWidth;
	windowHeight = newWindowHeight;
}
void ImageBox::Draw(const TextureProgData &textureData)
{
	glutil::MatrixStack identityMat;
	identityMat.SetIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	boxSprite.Draw(identityMat, textureData);

	glDisable(GL_BLEND);
}


void ImageBox::SetTextures(std::string textures[])
{
	for(int i = 0; i < 4; i++)
	{
		fusionTextures[i] = textures[i];
	}
}*/