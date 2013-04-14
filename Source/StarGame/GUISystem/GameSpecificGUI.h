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


#ifndef GAME_SPECIFIC_GUI
#define GAME_SPECIFIC_GUI


#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>
#include "../framework/EventSystem.h"
#include "../framework/Utility.h"


#define RESOLUTION 90
	

enum TextureTypeSun
{
	TEXTURE_TYPE_UPGRADE_AOE,
	TEXTURE_TYPE_UPGRADE_SUN_NOVA,
	TEXTURE_TYPE_UPGRADE_PASSIVE_AOE,
	TEXTURE_TYPE_UPGRADE_BURN,
	TEXTURE_TYPE_NO_UPGRADE
};

enum TextureTypeSat
{
	TEXTURE_TYPE_SAT_UPGRADE,
	TEXTURE_TYPE_SAT_NO_UPGRADE,
};


class AOESelector
{
private:
	Utility::Primitives::Circle selectorBody;
	glm::vec3 position;
	float radius;
	
public:
	AOESelector() {}
	AOESelector(glm::vec3 newPosition, float newRadius,
				glm::vec4 newColor);

	void Init();

	void Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &simpleData);

	void Update(glm::vec3 newPosition);
};


class SkillUpgradeButtons
{
private:	
	friend class SatelliteOrbit; // WARN: Should I? Maybe, they are in parent-child relationship.


	glm::vec3 orbitPosition;
	float orbitInnerRadius;
	float orbitOuterRadius;

	float width;
	float height;

	std::string textureFileNames[2];

	Utility::Primitives::Sprite3D skillButtons[3];
	bool buttonIndices[3];

public:
	SkillUpgradeButtons() {}
	SkillUpgradeButtons(float newWidth, float newHeight,
						const std::string &noUpgradeFileName, const std::string &upgradeFileName,
						glm::vec3 newOrbitPosition, float newOrbitInnerRadius, float newOrbitOuterRadius);

	void Init();

	void Draw(glutil::MatrixStack &modelMatrix, const SimpleTextureProgData &textureData);

	void ChangeTexture(TextureTypeSat type, int buttonIndex);

	bool IsClicked(Utility::Ray mouseRay, int &buttonIndex);
};


class SatelliteOrbit
{
private:
	glm::vec4 mainColor; 
	glm::vec4 outlineColor;

	glm::vec3 position;

	float outerRadius;
	float innerRadius;


	Utility::Primitives::Torus2D mainOrbit;
	Utility::Primitives::Torus2D orbitOutlineOne;
	Utility::Primitives::Torus2D orbitOutlineTwo;
	SkillUpgradeButtons upgradeButtons;

public:
	SatelliteOrbit();
	SatelliteOrbit(glm::vec4 newMainColor, glm::vec4 newOutlineColor,
				   glm::vec3 newPosition, 
				   const std::vector<std::string> &texturesFileButtons,
				   float newOuterRadius, float newInnerRadius,
				   float gamma);

	void Init();

	/// \fn Draw
	/// \brief Draws the orbit.
	void Draw(glutil::MatrixStack &modelMatrix, 
			  const SimpleProgData &simpleData, const SimpleTextureProgData &textureData,
			  bool isDrawingUpgradeButtons);

	void ChangeUpgradeButtonTexture(TextureTypeSat textType, int buttonIndex);

	bool IsUpgradeButtonClicked(Utility::Ray mouseRay, int &buttonIndex);
};


class SunSkillUpgradeButtons
{
private:
	glm::vec3 centerPosition;
	float width;
	float height;
	float radius;

	std::vector<std::string> textureFileNames;

	Utility::Primitives::Sprite3D skillButtons[4];

	struct UpgradedButton
	{
		int buttonIndex;
		TextureTypeSun buttonTextureType;

		UpgradedButton(int newButtonIndex, TextureTypeSun newButtonTextureType)
		{
			buttonIndex = newButtonIndex;
			buttonTextureType = newButtonTextureType;
		}
	};
	std::vector<UpgradedButton> upgradedButtons;

public:
	SunSkillUpgradeButtons() {}
	SunSkillUpgradeButtons(float newWidth, float newHeight, float newRadius,
						   glm::vec3 newCenterPosition,
						   const std::vector<std::string> &newTexturesFileNames);

	void Init();

	void Draw(glutil::MatrixStack &modelMatrix, const SimpleTextureProgData &textureData);

	void ChangeTexture(TextureTypeSun type, int buttonIndex);

	std::vector<UpgradedButton> GetUpgradedButtons();
	UpgradedButton GetUpgradedButtonBySkillType(const std::string &skillType);
	TextureTypeSun GetSkillTextureType(const std::string &skillType);
	TextureTypeSun GetSkillTextureTypeAtButton(int buttonIndex);
	int GetSkillButtonIndex(const std::string &skillType);

	bool IsClicked(Utility::Ray mouseRay, int &buttonIndex);
};


#endif