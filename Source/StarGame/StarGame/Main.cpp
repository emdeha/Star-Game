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


// TODO: Use smart pointers where needed.
// TODO: Place 'CorrectGamma' in 'Utility'
// TODO: Make sure everything compiles with the include files removed from 'Microsoft SDKs'
// TODO: DATA DRIVEN DESIGN!!!
// TODO: Better error handling.
// TODO: Fix a bug with satellite selection. 
//		 When the sun is moved the selection doesn't work properly. Move the sun for an example.

#pragma warning(push, 0)
#include <ctime>
#include <sstream>
#include <string>
#include <iostream>

#include "Main.h"
#include "ShaderManager.h"
#include "DisplayData.h"
#include "../framework/ErrorAPI.h"
#pragma warning(pop)

#include "../Fusion_Scene/Scene.h"
#include "../Fusion_Scene/World.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_EntitySystem/FusionSystems.h"
#include "../Fusion_AssetLoader/AssetLoader.h"
#include "../Fusion_Renderer/Renderer.h"
#include "../Fusion_Renderer/ShaderEnums.h"
#include "../Fusion_Entities/CelestialBody.h"

#define FUSION_LOAD_FAST

namespace FE = FusionEngine;


long long GetCurrentTimeMillis()
{
    return time(0) * 1000;
}


void HandleMouse()
{
	unsigned int entityIndex = GetScene().GetEntity("sun")->GetIndex();
	FE::ComponentMapper<FE::Collidable> collidableData = 
		GetScene().GetEntityManager()->GetComponentList(entityIndex, FE::CT_COLLISION);
	if (GetWorld().GetMouse().IsLeftButtonDown())
	{
		collidableData[0]->isForCheck = true;

		auto guiLayouts = GetWorld().GetLayouts();
		for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
		{
			(*layout).second->DeactivateAllControls();
			auto layoutControls = (*layout).second->GetControls();
			for (auto control = layoutControls.begin(); control != layoutControls.end(); ++control)
			{
				if ((*layout).second->IsSet() &&
					(*control)->IsMouseOn(GetWorld().GetMouse().GetCurrentPosition()))
				{
					GetWorld().GetEventManager().FireEvent(FE::OnClickEvent(FE::EVENT_ON_CLICK, true, (*control)->GetName()));
				}
			}
		}
	}
	else
	{
		collidableData[0]->isForCheck = false;
	}

	GetWorld().GetMouse().ReleaseLeftButton();
	GetWorld().GetMouse().ReleaseRightButton();
}
void HandleMouseButtons(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
		GetWorld().GetMouse().PressLeftButton();
    }
    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
		GetWorld().GetMouse().ReleaseLeftButton();
    }

    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
		GetWorld().GetMouse().PressRightButton();
    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
		GetWorld().GetMouse().ReleaseRightButton();
    }
}
void HandleActiveMovement(int x, int y)
{
	GetWorld().GetMouse().SetCurrentPosition(glm::ivec2(x, y));
}
void HandlePassiveMovement(int x, int y)
{
	GetWorld().GetMouse().SetCurrentPosition(glm::ivec2(x, y));
}

void InitializeScene()
{
    TopDownCamera userCamera = TopDownCamera(glm::vec3(), 13.5f, 0.0f, 45.0f);

    SunLight 
        mainSunLight(SunLight(glm::vec3(), glm::vec4(3.5f), glm::vec4(0.4f), 1.2f, 5.0f, GetWorld().GetDisplayData().gamma));//displayData.gamma));

	GetWorld().GetCamera() = userCamera;
	GetWorld().GetSunLight() = mainSunLight;

	FE::ShaderManager worldShaderManager = GetWorld().GetShaderManager();
	FE::ProgramData textureProgData = worldShaderManager.GetProgram(FE::FE_PROGRAM_TEXTURE);
    glUseProgram(textureProgData.programId);
    glUniform1i(textureProgData.GetUniform(FE::FE_UNIFORM__SAMPLER), 0);
	FE::ProgramData perspectiveTextureProgData = worldShaderManager.GetProgram(FE::FE_PROGRAM_TEXTURE_PERSPECTIVE);
    glUseProgram(perspectiveTextureProgData.programId);
    glUniform1i(perspectiveTextureProgData.GetUniform(FE::FE_UNIFORM__SAMPLER), 0);
	FE::ProgramData simpleTextureProgData = worldShaderManager.GetProgram(FE::FE_PROGRAM_SIMPLE_TEXTURE);
    glUseProgram(simpleTextureProgData.programId);
    glUniform1i(simpleTextureProgData.GetUniform(FE::FE_UNIFORM_COLOR_TEXTURE), 0);
	FE::ProgramData litTextureProgData = worldShaderManager.GetProgram(FE::FE_PROGRAM_LIT_TEXTURE);
    glUseProgram(litTextureProgData.programId);
    glUniform1i(litTextureProgData.GetUniform(FE::FE_UNIFORM__SAMPLER), 0);
	FE::ProgramData spriteProgramData = worldShaderManager.GetProgram(FE::FE_PROGRAM_SPRITE_PARTICLE);
    glUseProgram(spriteProgramData.programId);
    glUniform1i(spriteProgramData.GetUniform(FE::FE_UNIFORM__SAMPLER), 0);
    glUseProgram(0);


	GetScene().Init(GetWorld().GetEventManager());

	FE::AssetLoader<FE::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FE::MeshLoader());
	FE::MeshAssetObject loadedMesh = meshLoader.LoadAssetObject("mesh-files", "sun.obj");

	FE::Render *sunRender = new FE::Render();

	std::vector<std::shared_ptr<FE::MeshEntry>> meshEntries = loadedMesh.GetMeshEntries();
	for(auto meshEntry = meshEntries.begin(); meshEntry != meshEntries.end(); ++meshEntry)
	{
		sunRender->mesh.AddEntry((*meshEntry));
	}
	std::vector<std::shared_ptr<Texture2D>> textures = loadedMesh.GetTextures();
	for(auto texture = textures.begin(); texture != textures.end(); ++texture)
	{
		sunRender->mesh.AddTexture((*texture));
	}
	sunRender->rendererType = FE::Render::FE_RENDERER_SIMPLE;
	sunRender->shaderProgram = worldShaderManager.GetProgram(FE::FE_PROGRAM_SIMPLE).programId;
	sunRender->vao = loadedMesh.vao;

	GetScene().AddEntity("sun");
	FE::FunctionalSystem<FE::CelestialBody> *sunFunctional =
		new FE::FunctionalSystem<FE::CelestialBody>(&GetWorld().GetEventManager(), GetScene().GetEntityManager());
	GetScene().AddSystem(sunFunctional);
	FE::CollisionSystem *sunClickable = 
		new FE::CollisionSystem(&GetWorld().GetEventManager(), GetScene().GetEntityManager());
	GetScene().AddSystem(sunClickable);
	GetScene().AddComponent("sun", sunRender);

	FE::Transform *sunTransform = new FE::Transform();
	sunTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);
	sunTransform->rotation = glm::vec3();
	sunTransform->scale = glm::vec3(0.5f);
	GetScene().AddComponent("sun", sunTransform);

	FE::Functional<FE::CelestialBody> *sunFuncComp = new FE::Functional<FE::CelestialBody>();
	sunFuncComp->updatedObject = 
		std::unique_ptr<FE::CelestialBody>(new FE::CelestialBody(FE::FE_CELESTIALBODY_SUN, 4, 0.5f, 0.0f));
	sunFuncComp->updatedObject->AddSkill(std::shared_ptr<FE::Skill>(
		new FE::SatelliteCreationSkill(FE::FE_CELESTIALBODY_WATER, 'q', 'q', 'q', 0, 0)));
	GetScene().AddComponent("sun", sunFuncComp);
	
	FE::Collidable *sunCollidable = new FE::Collidable();
	sunCollidable->isForCheck = false;
	GetScene().AddComponent("sun", sunCollidable);

	GetWorld().GetRenderer().SubscribeForRendering(GetScene().GetEntity("sun"));


	///////////////////////////////////////
	GetWorld().SetFusionInput('f', 'q', 'w', 'e');
	GetWorld().AddFusionSequence("waterSat", 'q', 'q', 'q');
}


const int TICKS_PER_SECOND = 25;
const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
const int MAX_FRAMESKIP = 5;

unsigned long nextGameTick = 0;

unsigned int frameCount = 0;
const char WINDOW_TITLE[] = "Test Playground: ";

void TimerFunction(int value)
{
    if(value != 0)
    {
        char *tempString = (char *)malloc(512 + strlen(WINDOW_TITLE));

        sprintf(tempString, "%s %i frames per second", WINDOW_TITLE, frameCount * 4);

        glutSetWindowTitle(tempString);
        free(tempString);
    }

    frameCount = 0;
    glutTimerFunc(250, TimerFunction, 1);
}

long long currentTime_milliseconds;

void Init()
{
    currentTime_milliseconds = GetCurrentTimeMillis();

    glutTimerFunc(0, TimerFunction, 0);

	GetWorld().GetDisplayData().windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	GetWorld().GetDisplayData().windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	GetWorld().Load("gui-config.yaml", "audio-config.yaml", "shader-config.yaml");
	InitializeScene();

    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    const float depthZNear = 0.0f;
    const float depthZFar = 1.0f;

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(depthZNear, depthZFar);
    glEnable(GL_DEPTH_CLAMP);

	FE::ShaderManager &worldShaderManager = GetWorld().GetShaderManager();

    GLuint lightUniformBuffer = 0;
    glGenBuffers(1, &lightUniformBuffer);
    worldShaderManager.SetUniformBuffer(FE::FE_UBT_LIGHT, lightUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlockGamma), NULL, GL_DYNAMIC_DRAW);

    GLuint projectionUniformBuffer = 0;
    glGenBuffers(1, &projectionUniformBuffer);
    worldShaderManager.SetUniformBuffer(FE::FE_UBT_PROJECTION, projectionUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, projectionUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

    GLuint orthographicUniformBuffer = 0;
    glGenBuffers(1, &orthographicUniformBuffer);
    worldShaderManager.SetUniformBuffer(FE::FE_UBT_ORTHOGRAPHIC, orthographicUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, orthographicUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

    // Bind the static buffers.
    glBindBufferRange(GL_UNIFORM_BUFFER, worldShaderManager.GetBlockIndex(FE::FE_BT_LIGHT), 
        lightUniformBuffer, 
        0, sizeof(LightBlockGamma));

    glBindBufferRange(GL_UNIFORM_BUFFER, worldShaderManager.GetBlockIndex(FE::FE_BT_PROJECTION), 
        projectionUniformBuffer,
        0, sizeof(glm::mat4));

    glBindBufferRange(GL_UNIFORM_BUFFER, worldShaderManager.GetBlockIndex(FE::FE_BT_ORTHOGRAPHIC),
        orthographicUniformBuffer,
        0, sizeof(glm::mat4));

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    nextGameTick = GetTickCount();
}

bool isEmitterStarted = false;

void Display()
{
    frameCount++;

    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    

	glutil::MatrixStack modelMatrix;

	modelMatrix.SetMatrix(GetWorld().GetCamera().CalcMatrix());
	GetWorld().GetDisplayData().modelMatrix = modelMatrix;

	int loops = 0;
	while (GetTickCount() > nextGameTick && loops < MAX_FRAMESKIP)
	{
		GetScene().ProcessSystems();
	    HandleMouse();
	
		nextGameTick += SKIP_TICKS;
		loops++;
	}
	// TODO: Use that
	float interpolation = float(GetTickCount() + SKIP_TICKS - nextGameTick) / float(SKIP_TICKS);
	GetWorld().interpolation = interpolation;
	GetWorld().Render(); // TODO: Optimize

	GetWorld().GetMouse().OverrideLastPosition();
    
    glutSwapBuffers();
    glutPostRedisplay();
}


void Reshape(int width, int height)
{
    glutil::MatrixStack projMatrix;
    float aspectRatio = width / (float)height;
    projMatrix.Perspective(45.0f, aspectRatio, GetWorld().GetDisplayData().zNear, 
											   GetWorld().GetDisplayData().zFar);

    GetWorld().GetDisplayData().projectionMatrix = projMatrix.Top();
	FE::ShaderManager worldShaderManager = GetWorld().GetShaderManager();

	FE::ProgramData billboardProgData = worldShaderManager.GetProgram(FE::FE_PROGRAM_BILLBOARD);
    glUseProgram(billboardProgData.programId);
    glUniformMatrix4fv(billboardProgData.GetUniform(FE::FE_UNIFORM_CAMERA_TO_CLIP_MATRIX), 
                       1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
    glUseProgram(0);

	FE::ProgramData billboardNoTextureProgData = worldShaderManager.GetProgram(FE::FE_PROGRAM_BILLBOARD_NO_TEXTURE);
    glUseProgram(billboardNoTextureProgData.programId);
    glUniformMatrix4fv(billboardNoTextureProgData.GetUniform(FE::FE_UNIFORM_CAMERA_TO_CLIP_MATRIX),		
                       1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
    glUseProgram(0);
    
    glBindBuffer(GL_UNIFORM_BUFFER, worldShaderManager.GetUniformBuffer(FE::FE_UBT_PROJECTION));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projMatrix.Top()), &projMatrix.Top());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    projMatrix.SetIdentity();
    projMatrix.Orthographic((float)width, 0.0f, (float)height, 0.0f, 1.0f, 1000.0f);
    
    glBindBuffer(GL_UNIFORM_BUFFER, worldShaderManager.GetUniformBuffer(FE::FE_UBT_ORTHOGRAPHIC));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projMatrix.Top()), &projMatrix.Top());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    
	GetWorld().GetDisplayData().windowHeight = height;
	GetWorld().GetDisplayData().windowWidth = width;
	GetWorld().GetEventManager().FireEvent(FE::OnReshapeEvent(FE::EVENT_ON_RESHAPE, width, height));

    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27:
		break;
        //glutLeaveMainLoop();
        //return;
	case 'r':
		if (!GetWorld().GetCurrentLayout()->HasActiveTextBox())
		{
			GetWorld().ReloadGUI("gui-config.yaml");
			return;
		}
		break;
   }

	std::string objName = "";
	unsigned int currentFusionInputIndex = GetWorld().GetCurrentFusionInputIndex();
	switch (currentFusionInputIndex)
	{
	case 0:
		objName = "fusionOne";
		break;
	case 1:
		objName = "fusionTwo";
		break;
	case 2:
		objName = "fusionThree";
		break;
	case 3:
		objName = "all";
		break;
	default:
		HandleUnexpectedError("Something is wrong with the fusion sequences!!!", __LINE__, __FILE__);
	}

	GetWorld().GetEventManager().FireEvent(FE::OnKeyPressedEvent(FE::EVENT_ON_KEY_PRESSED, key, objName));

    glutPostRedisplay();
}


unsigned int Defaults(unsigned int displayMode, int &width, int &height) 
{ 
    return displayMode; 
}