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
#include "GameSpecificGUI.h"
#include "../framework/ErrorAPI.h"


AOESelector::AOESelector(glm::vec3 newPosition, float newRadius,
						 glm::vec4 newColor)
{
	position = newPosition;
	radius = newRadius;

	selectorBody = 
		Utility::Primitives::Circle(newColor, position, newRadius, RESOLUTION);
}

void AOESelector::Init()
{
	selectorBody.Init();
}

void AOESelector::Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &simpleData)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	selectorBody.Draw(modelMatrix, simpleData);

	glDisable(GL_BLEND);
}

void AOESelector::Update(glm::vec3 newPosition)
{
	position = newPosition;
}


SkillUpgradeButtons::SkillUpgradeButtons(float newWidth, float newHeight,
										 const std::string &noUpgradeFileName, const std::string &upgradeFileName,
										 glm::vec3 newOrbitPosition, float newOrbitInnerRadius, float newOrbitOuterRadius)
{
	width = newWidth;
	height = newHeight;

	orbitPosition = newOrbitPosition;
	orbitInnerRadius = newOrbitInnerRadius;
	orbitOuterRadius = newOrbitOuterRadius;

	textureFileNames[TEXTURE_TYPE_SAT_NO_UPGRADE] = noUpgradeFileName;
	textureFileNames[TEXTURE_TYPE_SAT_UPGRADE] = upgradeFileName;

	skillButtons[0] = 
		Utility::Primitives::Sprite3D(glm::vec3(width / 2.0f, 0.01f, orbitPosition.z - orbitInnerRadius), width, height);
	skillButtons[1] = 
		Utility::Primitives::Sprite3D(glm::vec3(orbitPosition.x - orbitInnerRadius, 0.01f, height / 2.0f), width, height);
	skillButtons[2] = 
		Utility::Primitives::Sprite3D(glm::vec3(width / 2.0f, 0.01f, orbitPosition.z + orbitOuterRadius), width, height);
}

void SkillUpgradeButtons::Init()
{
	skillButtons[0].Init(textureFileNames[TEXTURE_TYPE_SAT_NO_UPGRADE]);
	skillButtons[1].Init(textureFileNames[TEXTURE_TYPE_SAT_NO_UPGRADE]);
	skillButtons[2].Init(textureFileNames[TEXTURE_TYPE_SAT_NO_UPGRADE]);
}

void SkillUpgradeButtons::Draw(glutil::MatrixStack &modelMatrix, const SimpleTextureProgData &textureData)
{
	skillButtons[0].Draw(modelMatrix, textureData);
	skillButtons[1].Draw(modelMatrix, textureData);
	skillButtons[2].Draw(modelMatrix, textureData);
}

void SkillUpgradeButtons::ChangeTexture(TextureTypeSat type, int buttonIndex)
{
	skillButtons[buttonIndex].ChangeTexture(textureFileNames[type]);
}

bool SkillUpgradeButtons::IsClicked(Utility::Ray mouseRay, int &buttonIndex)
{
	for(int i = 0; i < 3; i++)
	{
		std::vector<glm::vec3> buttonVertices = skillButtons[i].GetVertices();
		if(Utility::Intersections::RayIntersectsSquare(mouseRay, 
													   buttonVertices[0], buttonVertices[1], 
													   buttonVertices[2], buttonVertices[3]))
		{
			buttonIndex = i;
			return true;
		}
	}
	return false;
}


SatelliteOrbit::SatelliteOrbit()
{
	mainColor = glm::vec4();
	outlineColor = glm::vec4();

	position = glm::vec3();

	outerRadius = 0.0f;
	innerRadius = 0.0f;
}

SatelliteOrbit::SatelliteOrbit(glm::vec4 newMainColor, glm::vec4 newOutlineColor,
							   glm::vec3 newPosition, 
							   const std::vector<std::string> &texturesFileButtons,
							   float newOuterRadius, float newInnerRadius,
							   float gamma)
{
	mainColor = Utility::CorrectGamma(newMainColor, gamma);
	outlineColor = Utility::CorrectGamma(newOutlineColor, gamma);
	position = newPosition;
	outerRadius = newOuterRadius;
	innerRadius = newInnerRadius;

	mainOrbit = Utility::Primitives::Torus2D(mainColor,
											 position,
											 innerRadius,
											 outerRadius,
											 90);
	orbitOutlineOne = Utility::Primitives::Torus2D(outlineColor,
												   position,
												   innerRadius - 0.05f,
												   innerRadius,
												   90);
	orbitOutlineTwo = Utility::Primitives::Torus2D(outlineColor,
												   position,
												   outerRadius,
												   outerRadius + 0.05f,
												   90);

	upgradeButtons = SkillUpgradeButtons(1.0f, 1.0f,
										 texturesFileButtons[TEXTURE_TYPE_SAT_NO_UPGRADE], 
										 texturesFileButtons[TEXTURE_TYPE_SAT_UPGRADE], 
										 position, innerRadius, outerRadius);
}

void SatelliteOrbit::Init()
{
	mainOrbit.Init();
	orbitOutlineOne.Init();
	orbitOutlineTwo.Init();
	upgradeButtons.Init();
}

void SatelliteOrbit::Draw(glutil::MatrixStack &modelMatrix, 
						  const SimpleProgData &simpleData, const SimpleTextureProgData &textureData)
{
	// TODO: Play with blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mainOrbit.Draw(modelMatrix, simpleData);

	glDisable(GL_BLEND);

	orbitOutlineOne.Draw(modelMatrix, simpleData);
	orbitOutlineTwo.Draw(modelMatrix, simpleData);

	upgradeButtons.Draw(modelMatrix, textureData);
}

void SatelliteOrbit::ChangeUpgradeButtonTexture(TextureTypeSat type, int buttonIndex)
{
	upgradeButtons.ChangeTexture(type, buttonIndex);
}

bool SatelliteOrbit::IsUpgradeButtonClicked(Utility::Ray mouseRay, int &buttonIndex)
{
	bool isUpgrButtonClicked = upgradeButtons.IsClicked(mouseRay, buttonIndex);
	return isUpgrButtonClicked;
}


SunSkillUpgradeButtons::SunSkillUpgradeButtons(float newWidth, float newHeight, float newRadius,
											   glm::vec3 newCenterPosition, 
											   const std::vector<std::string> &texturesFileNames)
{
	width = newWidth;
	height = newHeight;
	radius = newRadius;
	centerPosition = newCenterPosition;
	textureFileNames[TEXTURE_TYPE_NO_UPGRADE] = texturesFileNames[TEXTURE_TYPE_NO_UPGRADE];
	textureFileNames[TEXTURE_TYPE_UPGRADE_AOE] = texturesFileNames[TEXTURE_TYPE_UPGRADE_AOE];
	textureFileNames[TEXTURE_TYPE_UPGRADE_SUN_NOVA] = texturesFileNames[TEXTURE_TYPE_UPGRADE_SUN_NOVA];
	textureFileNames[TEXTURE_TYPE_UPGRADE_PASSIVE_AOE] = texturesFileNames[TEXTURE_TYPE_UPGRADE_PASSIVE_AOE];
	textureFileNames[TEXTURE_TYPE_UPGRADE_BURN] = texturesFileNames[TEXTURE_TYPE_UPGRADE_BURN];

	skillButtons[0] = 
		Utility::Primitives::Sprite3D(glm::vec3(width / 2.0f, 0.01f, centerPosition.z + 2.5f * radius), width, height);
	skillButtons[1] = 
		Utility::Primitives::Sprite3D(glm::vec3(width / 2.0f, 0.01f, centerPosition.z - radius), width, height);
	skillButtons[2] = 
		Utility::Primitives::Sprite3D(glm::vec3(centerPosition.x + 2.5f * radius, 0.01f, height / 2.0f), width, height);
	skillButtons[3] = 
		Utility::Primitives::Sprite3D(glm::vec3(centerPosition.x - radius, 0.01f, height / 2.0f), width, height);
}

void SunSkillUpgradeButtons::Init()
{
	skillButtons[0].Init(textureFileNames[TEXTURE_TYPE_NO_UPGRADE]);
	skillButtons[1].Init(textureFileNames[TEXTURE_TYPE_NO_UPGRADE]);
	skillButtons[2].Init(textureFileNames[TEXTURE_TYPE_NO_UPGRADE]);
	skillButtons[3].Init(textureFileNames[TEXTURE_TYPE_NO_UPGRADE]);
}

void SunSkillUpgradeButtons::Draw(glutil::MatrixStack &modelMatrix, const SimpleTextureProgData &textureData)
{
	skillButtons[0].Draw(modelMatrix, textureData);
	skillButtons[1].Draw(modelMatrix, textureData);
	skillButtons[2].Draw(modelMatrix, textureData);
	skillButtons[3].Draw(modelMatrix, textureData);
}

void SunSkillUpgradeButtons::ChangeTexture(TextureTypeSun type, int buttonIndex)
{
	skillButtons[buttonIndex].ChangeTexture(textureFileNames[type]);
}

bool SunSkillUpgradeButtons::IsClicked(Utility::Ray mouseRay, int &buttonIndex)
{
	for(int i = 0; i < 4; i++)
	{
		std::vector<glm::vec3> buttonVertices = skillButtons[i].GetVertices();
		if(Utility::Intersections::RayIntersectsSquare(mouseRay, 
													   buttonVertices[0], buttonVertices[1], 
													   buttonVertices[2], buttonVertices[3]))
		{
			buttonIndex = i;
			return true;
		}
	}
	return false;
}