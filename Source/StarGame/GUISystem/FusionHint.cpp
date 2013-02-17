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


void FusionHint::Init()
{
	boxSprite.Init(skillTextures[0]);
	
	textToDisplay = Text("../data/fonts/AGENCYR.TTF");
	textToDisplay.Init(800, 600);
	
	controlSquare.Init(800, 600);

	ComputeNewAttributes();
}

void FusionHint::OnEvent(Event &_event)
{
	switch(_event.GetType())
	{
	case EVENT_TYPE_OTHER:
		if(strcmp(_event.GetArgument("what_event").varString, "show_skill") == 0)
		{
			boxSprite.ChangeTexture(skillTextures[_event.GetArgument("skillIndex").varInteger]);
			text = skillDescriptions[_event.GetArgument("skillIndex").varInteger];
			isVisible = true;
		}
		break;
	default:
		break;
	}
}

void FusionHint::AddPreset(LayoutPreset newPreset, glm::vec2 newPosition)
{
	presets[newPreset].position = newPosition;
}

void FusionHint::Update(int newWindowWidth, int newWindowHeight)
{
	windowHeight = newWindowHeight;
	windowWidth = newWindowWidth;

	textToDisplay.UpdateWindowDimensions(windowWidth, windowHeight);
	ComputeNewAttributes();
}
void FusionHint::Draw(const FontProgData &fontData, const TextureProgData &textureData)
{		
	textToDisplay.Print(text.c_str(), fontData,
						glm::vec2(presets[currentPreset].position.x + width, 600 - presets[currentPreset].position.y),
						glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
						presets[currentPreset].textSize);

	if(isVisible)
	{
		glutil::MatrixStack identityMat;
		identityMat.SetIdentity();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/*
		// TODO: Add simpleData
		if(hasBackground)
		{
			controlSquare.Draw(modelMatrix, simpleData);
		}
		*/
	

		boxSprite.Draw(identityMat, textureData);

		glDisable(GL_BLEND);
	}
}

void FusionHint::SetTextures(std::vector<std::string> textures)
{
	for(int i = 0; i < textures.size(); i++)
	{
		skillTextures.push_back(textures[i]);
	}
}
void FusionHint::SetDescriptions(std::vector<std::string> descriptions)
{
	for(int i = 0; i < descriptions.size(); i++)
	{
		skillDescriptions.push_back(descriptions[i]);
	}
}