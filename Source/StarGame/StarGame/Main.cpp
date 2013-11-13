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

#include <ctime>
#include <sstream>
#include <string>
#include <iostream>

#include "Main.h"
#include "ShaderManager.h"
#include "DisplayData.h"
#include "../framework/ErrorAPI.h"

#include "../Fusion_Scene/Scene.h"
#include "../Fusion_Scene/World.h"
#include "../Fusion_EntitySystem/FusionComponents.h"
#include "../Fusion_EntitySystem/FusionSystems.h"
#include "../Fusion_AssetLoader/AssetLoader.h"
#include "../Fusion_Renderer/Renderer.h"
#include "../Fusion_Renderer/ShaderEnums.h"
#include "../Fusion_Entities/CelestialBody.h"

#define FUSION_LOAD_FAST


long long GetCurrentTimeMillis()
{
    return time(0) * 1000;
}


void HandleMouse()
{
	unsigned int entityIndex = GetScene().GetEntity("sun")->GetIndex();
	FusionEngine::ComponentMapper<FusionEngine::Collidable> collidableData = 
		GetScene().GetEntityManager()->GetComponentList(entityIndex, FusionEngine::CT_COLLISION);
	if (GetWorld().GetMouse().IsLeftButtonDown())
	{
		collidableData[0]->isForCheck = true;

		auto guiLayouts = GetWorld().GetLayouts();
		for (auto layout = guiLayouts.begin(); layout != guiLayouts.end(); ++layout)
		{
			if ((*layout).second->IsSet())
			{
				auto layoutControls = (*layout).second->GetControls();
				for (auto control = layoutControls.begin(); control != layoutControls.end(); ++control)
				{
					if ((*control)->IsMouseOn(GetWorld().GetMouse().GetCurrentPosition()))
					{
						GetWorld().GetEventManager().FireEvent(FusionEngine::OnClickEvent(FusionEngine::EVENT_ON_CLICK, true, (*control)->GetName()));
					}
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

void InitializePrograms()
{
	GetWorld().GetShaderManager().LoadSimpleTextureProgData("shaders/SimpleTexture.vert", "shaders/SimpleTexture.frag");
	GetWorld().GetShaderManager().LoadUnlitProgram("shaders/PosTransform.vert", "shaders/UniformColor.frag");
	GetWorld().GetShaderManager().LoadSimpleProgram("shaders/PosColorLocalTransform.vert", "shaders/ColorPassThrough.frag");
	GetWorld().GetShaderManager().LoadLitProgram("shaders/PN.vert", "shaders/GaussianLighting.frag");
	GetWorld().GetShaderManager().LoadSimpleNoUBProgram("shaders/PosTransformNoUB.vert", "shaders/ColorPassThrough.frag");
	GetWorld().GetShaderManager().LoadFontProgram("shaders/Font.vert", "shaders/Font.frag");
	GetWorld().GetShaderManager().LoadTextureProgData("shaders/Texture.vert", "shaders/Texture.frag");
	GetWorld().GetShaderManager().LoadPerspectiveTextureProgData("shaders/TexturePerspective.vert", "shaders/Texture.frag");
	GetWorld().GetShaderManager().LoadBillboardProgData("shaders/BillboardShader.vert", "shaders/BillboardShader.geom", "shaders/BillboardShader.frag");
	GetWorld().GetShaderManager().LoadParticleProgData("shaders/ParticleShaderBillboarded.vert", "shaders/ParticleShaderBillboarded.geom");
	GetWorld().GetShaderManager().LoadBillboardProgDataNoTexture("shaders/BillboardShader.vert", "shaders/BillboardShaderNoTexture.geom", "shaders/BillboardShaderNoTexture.frag");
	GetWorld().GetShaderManager().LoadSpriteParticleProgData("shaders/ParticleShader.vert", "shaders/ParticleShader.frag");
	GetWorld().GetShaderManager().LoadLitTextureProgram("shaders/LitTexture.vert", "shaders/LitTexture.frag");
	GetWorld().GetShaderManager().LoadSpriteParticleProgData("shaders/SpriteParticleShader.vert", "shaders/SpriteParticleShader.frag");
}

void InitializeScene()
{
    TopDownCamera userCamera = TopDownCamera(glm::vec3(), 13.5f, 0.0f, 45.0f);

    SunLight 
        mainSunLight(SunLight(glm::vec3(), glm::vec4(3.5f), glm::vec4(0.4f), 1.2f, 5.0f, GetWorld().GetDisplayData().gamma));//displayData.gamma));

	GetWorld().GetCamera() = userCamera;
	GetWorld().GetSunLight() = mainSunLight;

	ShaderManager worldShaderManager = GetWorld().GetShaderManager();
    glUseProgram(worldShaderManager.GetTextureProgData().theProgram);
    glUniform1i(worldShaderManager.GetTextureProgData().colorTextureUnif, 0);
    glUseProgram(0);
    glUseProgram(worldShaderManager.GetPerspectiveTextureProgData().theProgram);
    glUniform1i(worldShaderManager.GetPerspectiveTextureProgData().colorTextureUnif, 0);
    glUseProgram(0);
    glUseProgram(worldShaderManager.GetSimpleTextureProgData().theProgram);
    glUniform1i(worldShaderManager.GetSimpleTextureProgData().textureUnif, 0);
    glUseProgram(0);
    glUseProgram(worldShaderManager.GetLitTextureProgData().theProgram);
    glUniform1i(worldShaderManager.GetLitTextureProgData().textureUnif, 0);
    glUseProgram(0);
    glUseProgram(worldShaderManager.GetSpriteParticleProgData().theProgram);
    glUniform1i(worldShaderManager.GetSpriteParticleProgData().samplerUnif, 0);
    glUseProgram(0);


	GetScene().Init(GetWorld().GetEventManager());

	FusionEngine::AssetLoader<FusionEngine::MeshAssetObject> meshLoader;
	meshLoader.RegisterType("mesh-files", new FusionEngine::MeshLoader());
	FusionEngine::MeshAssetObject loadedMesh = meshLoader.LoadAssetObject("mesh-files", "sun.obj");

	FusionEngine::Render *sunRender = new FusionEngine::Render();

	std::vector<std::shared_ptr<FusionEngine::MeshEntry>> meshEntries = loadedMesh.GetMeshEntries();
	for(auto meshEntry = meshEntries.begin(); meshEntry != meshEntries.end(); ++meshEntry)
	{
		sunRender->mesh.AddEntry((*meshEntry));
	}
	std::vector<std::shared_ptr<Texture2D>> textures = loadedMesh.GetTextures();
	for(auto texture = textures.begin(); texture != textures.end(); ++texture)
	{
		sunRender->mesh.AddTexture((*texture));
	}
	sunRender->rendererType = FusionEngine::Render::FE_RENDERER_SIMPLE;
	sunRender->shaderProgram = GetWorld().GetShaderManager().GetSimpleProgData().theProgram;
	sunRender->vao = loadedMesh.vao;

	GetScene().AddEntity("sun");
	FusionEngine::FunctionalSystem<FusionEngine::NewCelestialBody> *sunFunctional =
		new FusionEngine::FunctionalSystem<FusionEngine::NewCelestialBody>(&GetWorld().GetEventManager(), GetScene().GetEntityManager());
	GetScene().AddSystem(sunFunctional);
	FusionEngine::CollisionSystem *sunClickable = 
		new FusionEngine::CollisionSystem(&GetWorld().GetEventManager(), GetScene().GetEntityManager());
	GetScene().AddSystem(sunClickable);
	GetScene().AddComponent("sun", sunRender);

	FusionEngine::Transform *sunTransform = new FusionEngine::Transform();
	sunTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);
	sunTransform->rotation = glm::vec3();
	sunTransform->scale = glm::vec3(0.5f);
	GetScene().AddComponent("sun", sunTransform);

	FusionEngine::Functional<FusionEngine::NewCelestialBody> *sunFuncComp = 
		new FusionEngine::Functional<FusionEngine::NewCelestialBody>();
	sunFuncComp->updatedObject = std::unique_ptr<FusionEngine::NewCelestialBody>(new FusionEngine::NewCelestialBody(4, 0.5f, 0.0f));
	GetScene().AddComponent("sun", sunFuncComp);
	
	FusionEngine::Collidable *sunCollidable = new FusionEngine::Collidable();
	sunCollidable->isForCheck = false;
	GetScene().AddComponent("sun", sunCollidable);

	GetWorld().GetRenderer().SubscribeForRendering(GetScene().GetEntity("sun"));
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


    InitializePrograms();
    InitializeScene();
	GetWorld().Load("test-gui.yaml", "audio-config.yaml");

    
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

	ShaderManager &worldShaderManager = GetWorld().GetShaderManager();

    GLuint lightUniformBuffer = 0;
    glGenBuffers(1, &lightUniformBuffer);
    worldShaderManager.SetUniformBuffer(FusionEngine::UBT_LIGHT, lightUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlockGamma), NULL, GL_DYNAMIC_DRAW);

    GLuint projectionUniformBuffer = 0;
    glGenBuffers(1, &projectionUniformBuffer);
    worldShaderManager.SetUniformBuffer(FusionEngine::UBT_PROJECTION, projectionUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, projectionUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

    GLuint orthographicUniformBuffer = 0;
    glGenBuffers(1, &orthographicUniformBuffer);
    worldShaderManager.SetUniformBuffer(FusionEngine::UBT_ORTHOGRAPHIC, orthographicUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, orthographicUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

    // Bind the static buffers.
    glBindBufferRange(GL_UNIFORM_BUFFER, worldShaderManager.GetBlockIndex(FusionEngine::BT_LIGHT), 
        lightUniformBuffer, 
        0, sizeof(LightBlockGamma));

    glBindBufferRange(GL_UNIFORM_BUFFER, worldShaderManager.GetBlockIndex(FusionEngine::BT_PROJECTION), 
        projectionUniformBuffer,
        0, sizeof(glm::mat4));

    glBindBufferRange(GL_UNIFORM_BUFFER, worldShaderManager.GetBlockIndex(FusionEngine::BT_ORTHOGRAPHIC),
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
	GetWorld().Render();


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
	ShaderManager worldShaderManager = GetWorld().GetShaderManager();

    glUseProgram(worldShaderManager.GetBillboardProgData().theProgram);
    glUniformMatrix4fv(worldShaderManager.GetBillboardProgData().cameraToClipMatrixUnif, 
                       1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
    glUseProgram(0);

    glUseProgram(worldShaderManager.GetBillboardProgDataNoTexture().theProgram);
    glUniformMatrix4fv(worldShaderManager.GetBillboardProgDataNoTexture().cameraToClipMatrixUnif,		
                       1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
    glUseProgram(0);
    
    glBindBuffer(GL_UNIFORM_BUFFER, worldShaderManager.GetUniformBuffer(FusionEngine::UBT_PROJECTION));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projMatrix.Top()), &projMatrix.Top());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    projMatrix.SetIdentity();
    projMatrix.Orthographic((float)width, 0.0f, (float)height, 0.0f, 1.0f, 1000.0f);
    
    glBindBuffer(GL_UNIFORM_BUFFER, worldShaderManager.GetUniformBuffer(FusionEngine::UBT_ORTHOGRAPHIC));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projMatrix.Top()), &projMatrix.Top());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    
	GetWorld().GetDisplayData().windowHeight = height;
	GetWorld().GetDisplayData().windowWidth = width;
	GetWorld().GetEventManager().
		FireEvent(FusionEngine::OnReshapeEvent(FusionEngine::EVENT_ON_RESHAPE, width, height));

    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27:
        glutLeaveMainLoop();
        return;
   }

    glutPostRedisplay();
}


unsigned int Defaults(unsigned int displayMode, int &width, int &height) 
{ 
    return displayMode; 
}