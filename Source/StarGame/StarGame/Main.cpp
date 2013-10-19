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
#include "../Fusion_Entities/CelestialBody.h"

#define FUSION_LOAD_FAST


long long GetCurrentTimeMillis()
{
    return time(0) * 1000;
}


void HandleMouse()
{
	unsigned int entityIndex = FusionEngine::Scene::GetScene().GetEntity("sun")->GetIndex();
	FusionEngine::ComponentMapper<FusionEngine::Collidable> collidableData = 
		FusionEngine::Scene::GetScene().GetEntityManager()->GetComponentList(entityIndex, FusionEngine::CT_COLLISION);
	if (FusionEngine::World::GetWorld().GetMouse().IsLeftButtonDown())
	{
		collidableData[0]->isForCheck = true;
	}
	else
	{
		collidableData[0]->isForCheck = false;
	}

	FusionEngine::World::GetWorld().GetMouse().ReleaseLeftButton();
	FusionEngine::World::GetWorld().GetMouse().ReleaseRightButton();
}
void HandleMouseButtons(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
		FusionEngine::World::GetWorld().GetMouse().PressLeftButton();
    }
    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
		FusionEngine::World::GetWorld().GetMouse().ReleaseLeftButton();
    }

    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
		FusionEngine::World::GetWorld().GetMouse().PressRightButton();
    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
		FusionEngine::World::GetWorld().GetMouse().ReleaseRightButton();
    }
}
void HandleActiveMovement(int x, int y)
{
	FusionEngine::World::GetWorld().GetMouse().SetCurrentPosition(glm::ivec2(x, y));
}
void HandlePassiveMovement(int x, int y)
{
	FusionEngine::World::GetWorld().GetMouse().SetCurrentPosition(glm::ivec2(x, y));
}

void InitializePrograms()
{
	FusionEngine::World::GetWorld().GetShaderManager().LoadSimpleTextureProgData("shaders/SimpleTexture.vert", "shaders/SimpleTexture.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadUnlitProgram("shaders/PosTransform.vert", "shaders/UniformColor.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadSimpleProgram("shaders/PosColorLocalTransform.vert", "shaders/ColorPassThrough.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadLitProgram("shaders/PN.vert", "shaders/GaussianLighting.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadSimpleNoUBProgram("shaders/PosTransformNoUB.vert", "shaders/ColorPassThrough.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadFontProgram("shaders/Font.vert", "shaders/Font.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadTextureProgData("shaders/Texture.vert", "shaders/Texture.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadPerspectiveTextureProgData("shaders/TexturePerspective.vert", "shaders/Texture.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadBillboardProgData("shaders/BillboardShader.vert", "shaders/BillboardShader.geom", "shaders/BillboardShader.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadParticleProgData("shaders/ParticleShaderBillboarded.vert", "shaders/ParticleShaderBillboarded.geom");
	FusionEngine::World::GetWorld().GetShaderManager().LoadBillboardProgDataNoTexture("shaders/BillboardShader.vert", "shaders/BillboardShaderNoTexture.geom", "shaders/BillboardShaderNoTexture.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadSpriteParticleProgData("shaders/ParticleShader.vert", "shaders/ParticleShader.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadLitTextureProgram("shaders/LitTexture.vert", "shaders/LitTexture.frag");
	FusionEngine::World::GetWorld().GetShaderManager().LoadSpriteParticleProgData("shaders/SpriteParticleShader.vert", "shaders/SpriteParticleShader.frag");
}

void InitializeScene()
{
    TopDownCamera userCamera = TopDownCamera(glm::vec3(), 13.5f, 0.0f, 45.0f);

    SunLight 
        mainSunLight(SunLight(glm::vec3(), glm::vec4(3.5f), glm::vec4(0.4f), 1.2f, 5.0f, FusionEngine::World::GetWorld().GetDisplayData().gamma));//displayData.gamma));

	FusionEngine::World::GetWorld().GetCamera() = userCamera;
	FusionEngine::World::GetWorld().GetSunLight() = mainSunLight;

	ShaderManager worldShaderManager = FusionEngine::World::GetWorld().GetShaderManager();
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


	FusionEngine::Scene::GetScene().Init(FusionEngine::World::GetWorld().GetEventManager());

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
	sunRender->shaderProgram = FusionEngine::World::GetWorld().GetShaderManager().GetSimpleProgData().theProgram;
	sunRender->vao = loadedMesh.vao;

	FusionEngine::Scene::GetScene().AddEntity("sun");
	FusionEngine::FunctionalSystem<FusionEngine::NewCelestialBody> *sunFunctional =
		new FusionEngine::FunctionalSystem<FusionEngine::NewCelestialBody>(&FusionEngine::World::GetWorld().GetEventManager(), FusionEngine::Scene::GetScene().GetEntityManager());
	FusionEngine::Scene::GetScene().AddSystem(sunFunctional);
	FusionEngine::CollisionSystem *sunClickable = 
		new FusionEngine::CollisionSystem(&FusionEngine::World::GetWorld().GetEventManager(), FusionEngine::Scene::GetScene().GetEntityManager());
	FusionEngine::Scene::GetScene().AddSystem(sunClickable);
	FusionEngine::Scene::GetScene().AddComponent("sun", sunRender);

	FusionEngine::Transform *sunTransform = new FusionEngine::Transform();
	sunTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);
	sunTransform->rotation = glm::vec3();
	sunTransform->scale = glm::vec3(0.5f);
	FusionEngine::Scene::GetScene().AddComponent("sun", sunTransform);

	FusionEngine::Functional<FusionEngine::NewCelestialBody> *sunFuncComp = 
		new FusionEngine::Functional<FusionEngine::NewCelestialBody>();
	sunFuncComp->updatedObject = std::unique_ptr<FusionEngine::NewCelestialBody>(new FusionEngine::NewCelestialBody(4, 0.5f, 0.0f, 1.0f));
	FusionEngine::Scene::GetScene().AddComponent("sun", sunFuncComp);
	
	FusionEngine::Collidable *sunCollidable = new FusionEngine::Collidable();
	sunCollidable->isForCheck = false;
	FusionEngine::Scene::GetScene().AddComponent("sun", sunCollidable);

	FusionEngine::World::GetWorld().GetRenderer().SubscribeForRendering(FusionEngine::Scene::GetScene().GetEntity("sun"));
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
	FusionEngine::World::GetWorld().Load("test-gui.yaml");

    
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

	ShaderManager &worldShaderManager = FusionEngine::World::GetWorld().GetShaderManager();

    GLuint lightUniformBuffer = 0;
    glGenBuffers(1, &lightUniformBuffer);
    worldShaderManager.SetUniformBuffer(UBT_LIGHT, lightUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlockGamma), NULL, GL_DYNAMIC_DRAW);

    GLuint projectionUniformBuffer = 0;
    glGenBuffers(1, &projectionUniformBuffer);
    worldShaderManager.SetUniformBuffer(UBT_PROJECTION, projectionUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, projectionUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

    GLuint orthographicUniformBuffer = 0;
    glGenBuffers(1, &orthographicUniformBuffer);
    worldShaderManager.SetUniformBuffer(UBT_ORTHOGRAPHIC, orthographicUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, orthographicUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

    // Bind the static buffers.
    glBindBufferRange(GL_UNIFORM_BUFFER, worldShaderManager.GetBlockIndex(BT_LIGHT), 
        lightUniformBuffer, 
        0, sizeof(LightBlockGamma));

    glBindBufferRange(GL_UNIFORM_BUFFER, worldShaderManager.GetBlockIndex(BT_PROJECTION), 
        projectionUniformBuffer,
        0, sizeof(glm::mat4));

    glBindBufferRange(GL_UNIFORM_BUFFER, worldShaderManager.GetBlockIndex(BT_ORTHOGRAPHIC),
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

	modelMatrix.SetMatrix(FusionEngine::World::GetWorld().GetCamera().CalcMatrix());
	FusionEngine::World::GetWorld().GetDisplayData().modelMatrix = modelMatrix;

	int loops = 0;
	while (GetTickCount() > nextGameTick && loops < MAX_FRAMESKIP)
	{
		FusionEngine::Scene::GetScene().ProcessSystems();
	    HandleMouse();
	
		nextGameTick += SKIP_TICKS;
		loops++;
	}
	// TODO: Use that
	float interpolation = float(GetTickCount() + SKIP_TICKS - nextGameTick) / float(SKIP_TICKS);

	FusionEngine::World::GetWorld().Render();


	FusionEngine::World::GetWorld().GetMouse().OverrideLastPosition();
    
    glutSwapBuffers();
    glutPostRedisplay();
}


void Reshape(int width, int height)
{
    glutil::MatrixStack projMatrix;
    float aspectRatio = width / (float)height;
    projMatrix.Perspective(45.0f, aspectRatio, FusionEngine::World::GetWorld().GetDisplayData().zNear, 
											   FusionEngine::World::GetWorld().GetDisplayData().zFar);

    FusionEngine::World::GetWorld().GetDisplayData().projectionMatrix = projMatrix.Top();
	ShaderManager worldShaderManager = FusionEngine::World::GetWorld().GetShaderManager();

    glUseProgram(worldShaderManager.GetBillboardProgData().theProgram);
    glUniformMatrix4fv(worldShaderManager.GetBillboardProgData().cameraToClipMatrixUnif, 
                       1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
    glUseProgram(0);

    glUseProgram(worldShaderManager.GetBillboardProgDataNoTexture().theProgram);
    glUniformMatrix4fv(worldShaderManager.GetBillboardProgDataNoTexture().cameraToClipMatrixUnif,		
                       1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
    glUseProgram(0);
    
    glBindBuffer(GL_UNIFORM_BUFFER, worldShaderManager.GetUniformBuffer(UBT_PROJECTION));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projMatrix.Top()), &projMatrix.Top());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    projMatrix.SetIdentity();
    projMatrix.Orthographic((float)width, 0.0f, (float)height, 0.0f, 1.0f, 1000.0f);
    
    glBindBuffer(GL_UNIFORM_BUFFER, worldShaderManager.GetUniformBuffer(UBT_ORTHOGRAPHIC));
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projMatrix.Top()), &projMatrix.Top());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    
	FusionEngine::World::GetWorld().GetDisplayData().windowHeight = height;
	FusionEngine::World::GetWorld().GetDisplayData().windowWidth = width;
	FusionEngine::World::GetWorld().GetEventManager().
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