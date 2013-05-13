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

/// \file PlanetBodies.h
/// \brief Implements the planets.

#ifndef PLANET_BODIES_H
#define PLANET_BODIES_H


#include <glload/gl_3_3.h>
#include <glutil/glutil.h>

#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/Timer.h"
#include "../framework/Utility.h"
#include "../framework/EventSystem.h"

#include "../ProgramData/ProgramData.h"
#include "../GUISystem/GameSpecificGUI.h"
#include "MaterialBlock.h"
#include "../Mouse/Mouse.h"
#include "Skills.h"
#include "../AssetLoader/MeshLoader.h"


// NOTE: Maybe the meshes of both the sun and satellites must be loaded in the constructors.
//		 This means removing the LoadMesh function. For now, I will keep it that way in case I
//		 decide to use different meshes in the future.


enum SatelliteType
{
    SATELLITE_FIRE,
    SATELLITE_WATER,
    SATELLITE_AIR,
    SATELLITE_EARTH,

    SATELLITE_COUNT,
};

struct SatelliteStats
{
    float satelliteOffsetFromSun;
    float speed;
    float diameter;
    float resourceGainTime;
    int resourceGain_perTime;
    int health;	
};

struct ResourceData
{
    int resourceGain_perTime;
    float resourceGainTime;
    Framework::Timer resourceTimer;
};

struct SkillsStats
{
    bool isResearched;
    int upgradeBoxIndex;
    int skillApplyCost;
    int skillResearchCost;
    int damage;
    int defensePoints;
    float stunTime_secs;
    float duration_secs;	
    float damageApplyTime_secs;
    float range;
    float scaleRate;
    SatelliteType forWhichSatellite;
    std::string skillUpgradedTexture;
    std::string skillToolTipText;
    std::string skillHintText;
    std::string skillHintImage;
};



static bool typesTable[] = { false, false, false, false };


class CelestialBody
{
private:
    // WARN: Is it a good practice?
    friend class Scene;

    Mesh mesh;
    
    std::unique_ptr<CelestialBody> parent;
    std::vector<std::shared_ptr<CelestialBody>> satellites;

    std::vector<std::shared_ptr<Skill>> skills; // TODO: Is a pointer needed?

    SatelliteOrbit hoverOrbit; // TODO: Not sure if this should be here or somewhere more
                               //	    GUI specific.
    SunSkillUpgradeButtons sunSkillUpgradeBtns; // TODO: Not sure if this should be here or somewhere more
                                                //       GUI specific.

    std::vector<Event> generatedEvents;

    SatelliteStats satelliteStats[SATELLITE_COUNT];
    SatelliteType satType;
    ResourceData resource;

    Framework::Timer revolutionDuration;

    glm::vec4 color;
    glm::vec3 position;

    float diameter;
    float clickableRadius;

    bool isClicked;
    bool isSun;

    bool isDrawingUpgradeButtons;

    int health;
    int satelliteCap;
    int satelliteConstructionCost; // WARN: Can't tell if this is the right approach.

    int currentResource;
    int initialResource;

    SkillsStats satSkillStats[SKILL_TYPE_COUNT];


    int materialBlockSize;
    GLuint materialUniformBuffer;

private:
    void InitSatelliteStats();

public:
    struct SkillButtonClickedData
    {
        bool isClicked;
        int buttonIndex;
        int skillId;

        SkillButtonClickedData(bool newIsClicked, int newButtonIndex, int newSkillId)
        {
            isClicked = newIsClicked;
            buttonIndex = newButtonIndex;
            skillId = newSkillId;
        }
    };

public:
    CelestialBody() {}
    CelestialBody(const CelestialBody &other);
    ~CelestialBody();
    CelestialBody(glm::vec3 newPosition, glm::vec4 newColor, float newDiameter,
                  int newSatelliteCap, int newHealth, 
                  int newCurrentResource, int newSatelliteConstructionCost,
                  bool _isSun = true);												// isSun = true means that a
                                                                                    // sun will be created
    CelestialBody(float speed, float newOffsetFromParent, float newDiameter, int newHealth,
                  float resourceGainTime, int resourceGain_perTime, SatelliteType newSatType, 
                  glm::vec4 newColor, bool _isSun = false);						   // isSun = false means that a 
                                                                                   // satellite will be created

    void InitSatelliteOrbit(const std::vector<std::string> &textureFileNames);
    void InitSunSkillUpgradeButtons(const std::vector<std::string> &textureFileNames);

    void LoadMesh(const std::string &fileName);
    
    void Update();
    void Render(glutil::MatrixStack &modelMatrix, GLuint materialBlockIndex,
                float gamma, 
                const LitProgData &litData,
                const UnlitProgData &unlitData,
                const SimpleProgData &simpleData,
                const SimpleTextureProgData &textureData,
                const LitTextureProgData &litTextureData,
                const SpriteParticleProgData &spriteParticleProgData,
                float interpolation);

    void OnEvent(Event &_event);

    bool AddSatellite(const std::string &fileName,
                      glm::vec4 satelliteColor,
                      SatelliteType type, bool drainResource = true, 
                      bool addAtProgress = false, float progress = 0.0f,
                      bool addHealth = false, int newHealth = 0);
    void AddSkill(const std::shared_ptr<Skill> newSkill);

    bool RemoveSatellite();
    bool RemoveSatellite(std::vector<std::shared_ptr<CelestialBody>>::iterator index_iterator);
    bool RemoveSatellite(SatelliteType type);
    void RemoveSatellites();	

    bool IsClicked(Utility::Ray mouseRay);
    SkillButtonClickedData IsSkillUpgradeButtonClicked(Utility::Ray mouseRay, int &buttonIndex);
    bool IsSatelliteClicked(Utility::Ray mouseRay);

    bool IsSun();

    bool HasSatellites();

    // Gets the generated events and DESTROYS them.
    std::vector<Event> GetGeneratedEvents();

    Event GetGeneratedEvent(const std::string &eventName);
    void RemoveGeneratedEvent(const std::string &eventName);

    const bool GetIsClicked() const;
    const bool GetIsSatelliteClicked(SatelliteType type) const;
    const bool GetIsDrawingUpgradeButtons() const;

    const float GetRadius() const;
    const float GetDiameter() const;

    const int GetHealth() const;
    
    std::shared_ptr<CelestialBody> GetSatellite(SatelliteType type);
    std::shared_ptr<CelestialBody> GetOuterSatellite();
    std::vector<std::shared_ptr<CelestialBody>> GetSatellites();
    
    // Gets the sun's and its satellites' skills.
    std::vector<std::shared_ptr<Skill>> GetAllSkills();
    std::vector<std::shared_ptr<Skill>> GetSatelliteSkills(SatelliteType type);
    std::vector<std::shared_ptr<Skill>> GetSunSkills();

    const glm::vec3 GetPosition() const;
    const int GetCurrentResource() const;


    // Satellite-specific methods
    float GetOffsetFromSun();
    SatelliteType GetSatelliteType();

    void SetParent(CelestialBody *newParent);
    void SetIsClicked(bool newIsClicked);

    void SetProgress(float progress);
    float GetProgress();

    void Stop();
    void Start();
};


#endif