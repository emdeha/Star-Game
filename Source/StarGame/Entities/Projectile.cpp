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
#include "Enemy.h"
#include "../framework/ErrorAPI.h"


static void GenerateUniformBuffers(int &materialBlockSize, 
                                   glm::vec4 diffuseColor,
                                   GLuint &materialUniformBuffer)
{
    MaterialBlock material;
    material.diffuseColor = diffuseColor;
    material.specularColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    material.shininessFactor = 0.3f;


    int uniformBufferAlignSize = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);

    materialBlockSize = sizeof(MaterialBlock);
    materialBlockSize += uniformBufferAlignSize -
        (materialBlockSize % uniformBufferAlignSize);

    
    glGenBuffers(1, &materialUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, materialUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, materialBlockSize, &material, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


Projectile::Projectile(glm::vec3 newPosition, glm::vec3 newVelocity,
                       int newDamage, int newLifeSpan)
{
    position = newPosition;
    velocity = newVelocity;
    damage = newDamage;

    lifeSpan = newLifeSpan;

    isDestroyed = false;
}

void Projectile::LoadMesh(const std::string &meshFile)
{
    // TODO: Change mesh
    // TODO: Check for errors
    try
    {
        mesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(meshFile));
    }
    catch(std::exception &except)
    {
        printf("%s\n", except.what());
        throw;
    }

    GenerateUniformBuffers(materialBlockSize, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), materialUniformBuffer);
}

void Projectile::LoadMesh(const std::auto_ptr<Framework::Mesh> newMesh) // WARN: Why auto_ptr???
{
    // TODO: Change mesh
    // TODO: Check for errors
    mesh = std::unique_ptr<Framework::Mesh>(new Framework::Mesh(*newMesh.get()));

    GenerateUniformBuffers(materialBlockSize, glm::vec4(0.0f), materialUniformBuffer);
}

void Projectile::Update(CelestialBody &sun)
{
    if(sun.GetHealth() <= 0)
    {
        isDestroyed = true;
    }

    if(lifeSpan <= 0)
    {
        isDestroyed = true;
    }

    if(!isDestroyed)
    {
        position += velocity;
        lifeSpan--;

        CheckTargetHit(sun);
    }
}

void Projectile::Render(glutil::MatrixStack &modelMatrix, int materialBlockIndex,
                        float gamma, 
                        const LitProgData &litData,
                        float interpolation)
{
    {
        glutil::PushStack push(modelMatrix);

        glm::vec3 viewPosition = position + velocity * interpolation;
        modelMatrix.Translate(viewPosition);
        modelMatrix.Scale(0.05f);

        glBindBufferRange(GL_UNIFORM_BUFFER, materialBlockIndex, materialUniformBuffer,
                          0, sizeof(MaterialBlock));

        glm::mat3 normMatrix(modelMatrix.Top());
        normMatrix = glm::transpose(glm::inverse(normMatrix));

        glUseProgram(litData.theProgram);
        glUniformMatrix4fv(litData.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
        glUniformMatrix3fv(litData.normalModelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(normMatrix));

        mesh->Render("lit");

        glUseProgram(0);

        glBindBufferBase(GL_UNIFORM_BUFFER, materialBlockIndex, 0);
    }
}

void Projectile::CheckTargetHit(CelestialBody &sun)
{
    std::vector<std::shared_ptr<CelestialBody>> sunSatellites = sun.GetSatellites();
    for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin();
        iter != sunSatellites.end();
        ++iter)
    {
        glm::vec3 satellitePosition = glm::vec3((*iter)->GetPosition());
        float satelliteRadius = (*iter)->GetRadius() * 2.0f;

        glm::vec3 distance = position - satellitePosition;
        float distanceLength = glm::length(distance);

        if(distanceLength <= satelliteRadius * satelliteRadius)
        {
            EventArg satelliteHitEventArg[2];
            satelliteHitEventArg[0].argType = "damage";
            satelliteHitEventArg[0].argument.varType = TYPE_INTEGER;
            satelliteHitEventArg[0].argument.varInteger = damage;
            satelliteHitEventArg[1].argType = "bodyIndex";
            satelliteHitEventArg[1].argument.varType = TYPE_FLOAT;
            satelliteHitEventArg[1].argument.varFloat = (*iter)->GetOffsetFromSun(); 
            Event satelliteHitEvent(2, EVENT_TYPE_ATTACKED, satelliteHitEventArg);

            OnTargetHit(sun, satelliteHitEvent);
        }
    }

    glm::vec3 sunPosition = glm::vec3(sun.GetPosition());
    float sunRadius = sun.GetRadius();

    glm::vec3 distance = position - sunPosition;
    float distanceLength = glm::length(distance);

    if(distanceLength <= sunRadius * sunRadius)
    {
        EventArg sunHitEventArg[2];
        sunHitEventArg[0].argType = "damage";
        sunHitEventArg[0].argument.varType = TYPE_INTEGER;
        sunHitEventArg[0].argument.varInteger = damage;
        sunHitEventArg[1].argType = "bodyIndex";
        sunHitEventArg[1].argument.varType = TYPE_INTEGER;
        sunHitEventArg[1].argument.varInteger = -1; // the body index of the sun.
        Event sunHitEvent(2, EVENT_TYPE_ATTACKED, sunHitEventArg);

        OnTargetHit(sun, sunHitEvent);
    }
}


void Projectile::Recreate(glm::vec3 newPosition, 
                          glm::vec3 newVelocity, 
                          int newLifeSpan, int newDamage)
{
    position = newPosition;
    velocity = newVelocity;
    lifeSpan = newLifeSpan;
    isDestroyed = false;
    damage = newDamage;
}
void Projectile::OnTargetHit(CelestialBody &sun, Event &_event)
{
    isDestroyed = true;

    sun.OnEvent(_event);	
}