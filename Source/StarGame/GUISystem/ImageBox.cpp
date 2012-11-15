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


ImageBox::ImageBox()
{
	name = "";

	isActive = false;

	index = 0;
}
ImageBox::ImageBox(LayoutPreset newCurrentPreset,
			       const std::string &newName, 
				   glm::vec2 newPosition,
				   float newWidth, float newHeight,
				   int newIndex)
{
	currentPreset = newCurrentPreset;
	presetPosition[currentPreset] = newPosition;
	width = newWidth;
	height = newHeight;
	name = newName;
	isActive = false;

	index = newIndex;

	boxSprite = 
		Utility::Primitives::Sprite(glm::vec4(newPosition, 0.0f, 1.0f), 
									glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
									newWidth, newHeight, false);
}

void ImageBox::Init()
{
	boxSprite.Init("../data/images/fusion-empty.png");
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
					boxSprite.ChangeTexture("../data/images/fusion-q-.png");
				}
				if(strcmp(button.c_str(), "w") == 0)
				{
					boxSprite.ChangeTexture("../data/images/fusion-w.png");
				}
				if(strcmp(button.c_str(), "e") == 0)
				{
					boxSprite.ChangeTexture("../data/images/fusion-e.png");
				}
			}
		}
		if(strcmp(_event.GetArgument("what_event").varString, "fusion_seq") == 0)
		{
			boxSprite.ChangeTexture("../data/images/fusion-empty.png");
		}
		break;
	}
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