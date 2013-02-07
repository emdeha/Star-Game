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
// TODO: Find a better way for the 'gamma' value to be distributed.
// TODO: Place 'CorrectGamma' in 'Utility'
// TODO: Make sure everything compiles with the include files removed from 'Microsoft SDKs'
// TODO: DATA DRIVEN DESIGN!!!
// TODO: Better error handling.
// TODO: Fix a bug with satellite selection. 
//		 When the sun is moved the selection doesn't work properly. Move the sun for an example.

#include <ctime>


#include "Main.h"
#include "ShaderManager.h"
#include "DisplayData.h"
#include "../framework/EventSystem.h"
#include "../Scene/Scene.h"
#include "../AssetLoader/GUILoader.h"
#include "../AssetLoader/MeshLoader.h"
#include "../ParticleEngine/Engine.h"
#include "../Entities/Skills.h"
#include "../GUISystem/GameSpecificGUI.h"


DisplayData displayData;

Scene scene = Scene(2.2f, 8.0f, 3.0f, 0.5f);


long long GetCurrentTimeMillis()
{
	return time(0) * 1000;
}


void HandleMouse()
{
	glm::vec4 cameraPosition = glm::vec4(scene.GetTopDownCamera().ResolveCamPosition(), 1.0f);
	
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	Utility::Ray mouseRay = 
		scene.GetMouse().GetPickRay(displayData.projectionMatrix, displayData.modelMatrix,
									cameraPosition, 
									windowWidth, windowHeight);


	if(scene.GetMouse().IsRightButtonDown())
	{		
		if(scene.HasSuns())
		{
			if(scene.GetSun()->IsClicked(mouseRay))
			{
				Event rightClickSunEvent = StockEvents::EventOnRightClick("sun");

				scene.GetSun()->OnEvent(rightClickSunEvent);
				scene.OnEvent(rightClickSunEvent);
				scene.OnEvent(scene.GetSun()->GetGeneratedEvent("satelliteRemoved"));
				scene.GetSun()->RemoveGeneratedEvent("satelliteRemoved");
			}
		}
	}

	if(scene.GetMouse().IsLeftButtonDown())
	{
		if(scene.IsLayoutOn(LAYOUT_IN_GAME))
		{/*
			if(
			   scene.GetLayout(LAYOUT_IN_GAME)->
			   GetControl("exit")->
			   IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("exitButton");

				scene.GetLayout(LAYOUT_IN_GAME)->GetControl("exit")->OnEvent(leftClickButtonEvent);

				scene.OnEvent(leftClickButtonEvent);

				scene.GetMouse().ReleaseLeftButton();
				return;
			}*/

			scene.OnEvent(StockEvents::EventOnLeftClick("deploySkill"));
		}

		if(scene.IsLayoutOn(LAYOUT_MENU))
		{
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("newGame")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("newGameButton");

				scene.GetLayout(LAYOUT_MENU)->GetControl("newGame")->OnEvent(leftClickButtonEvent);

				scene.OnEvent(leftClickButtonEvent);
			}

			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("saveGame")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("saveGameButton");

				scene.GetLayout(LAYOUT_MENU)->GetControl("saveGame")->OnEvent(leftClickButtonEvent);

				scene.OnEvent(leftClickButtonEvent);
			}

			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("quitGame")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("quitGameButton");

				scene.GetLayout(LAYOUT_MENU)->GetControl("quitGame")->OnEvent(leftClickButtonEvent);

				scene.OnEvent(leftClickButtonEvent);
			}
			/*
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("printCmd")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("printCmd");

				scene.GetLayout(LAYOUT_MENU)->GetControl("printCmd")->OnEvent(leftClickButtonEvent);

				scene.OnEvent(leftClickButtonEvent);
			}
			
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("sample")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickTextBoxEvent = StockEvents::EventOnLeftClick("sample");

				scene.GetLayout(LAYOUT_MENU)->GetControl("sample")->OnEvent(leftClickTextBoxEvent);

				
				if(scene.GetLayout(LAYOUT_MENU)->GetControl("sample") != 
				   scene.GetLayout(LAYOUT_MENU)->GetActiveControl())
				{
					Event unclickEvent = Event(EVENT_TYPE_UNCLICK);

					scene.GetLayout(LAYOUT_MENU)->GetActiveControl()->OnEvent(unclickEvent);
				}
			}
			
			if(
				scene.GetLayout(LAYOUT_MENU)->
				GetControl("sampleTwo")->
				IsMouseOn(glm::vec2(scene.GetMouse().GetCurrentPosition()))
			  )
			{
				Event leftClickTextBoxEvent = StockEvents::EventOnLeftClick("sampleTwo");

				scene.GetLayout(LAYOUT_MENU)->GetControl("sampleTwo")->OnEvent(leftClickTextBoxEvent);

				
				if(scene.GetLayout(LAYOUT_MENU)->GetControl("sampleTwo") != 
				   scene.GetLayout(LAYOUT_MENU)->GetActiveControl())
				{
					Event unclickEvent = Event(EVENT_TYPE_UNCLICK);

					scene.GetLayout(LAYOUT_MENU)->GetActiveControl()->OnEvent(unclickEvent);
				}
			}*/
		}

		if(scene.IsLayoutOn(LAYOUT_IN_GAME))
		{
			if(scene.HasSuns())
			{
				if(scene.GetSun()->IsClicked(mouseRay))
				{
					Event leftClickSunEvent = StockEvents::EventOnLeftClick("sun");

					scene.GetSun()->OnEvent(leftClickSunEvent);
					scene.OnEvent(leftClickSunEvent);
				}

				std::vector<std::shared_ptr<CelestialBody>> sunSatellites = scene.GetSun()->GetSatellites();
				for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin(); 
					iter != sunSatellites.end(); ++iter)
				{
					if((*iter)->IsClicked(mouseRay))
					{
						Event leftClickSatelliteEvent = StockEvents::EventOnLeftClick("satellite");

						(*iter)->OnEvent(leftClickSatelliteEvent);
						scene.OnEvent(leftClickSatelliteEvent);
					}
				}
			}
		}
	}

	if(scene.IsLayoutOn(LAYOUT_IN_GAME))
	{
		if(scene.HasSuns())
		{
			std::vector<std::shared_ptr<CelestialBody>> sunSatellites = scene.GetSun()->GetSatellites();
			for(std::vector<std::shared_ptr<CelestialBody>>::iterator iter = sunSatellites.begin(); 
				iter != sunSatellites.end(); ++iter)
			{
				if((*iter)->IsClicked(mouseRay))
				{
					Event satelliteHoveredEvent = StockEvents::EventOnHover();

					(*iter)->OnEvent(satelliteHoveredEvent);
					scene.OnEvent(satelliteHoveredEvent);
				}
			}
		}
	}

	if(scene.IsLayoutOn(LAYOUT_MENU))
	{
		/*if(
			universe->GetLayout(LAYOUT_MENU).
			GetButtonControl("newGame")->
			IsMouseOn(scene->GetMouse().GetClipSpacePosition(windowWidth, windowHeight))
			)
		{
			//Event hoverButtonEvent = StockEvents::EventOnHover();
			EventArg hoverButtonEventArg[1];
			hoverButtonEventArg[0].argType = "isBold";
			hoverButtonEventArg[0].argument.varType = TYPE_BOOL;
			hoverButtonEventArg[0].argument.varBool = true;

			Event hoverButtonEvent = Event(1, EVENT_TYPE_ON_HOVER, hoverButtonEventArg);

			universe->GetLayout(LAYOUT_MENU).GetButtonControl("newGame")->OnEvent(hoverButtonEvent);

			//universe->GetLayout(LAYOUT_MENU).GetButtonControl("newGame")->SetFont(true);
		}
		else
		{
			EventArg hoverButtonEventArg[1];
			hoverButtonEventArg[0].argType = "isBold";
			hoverButtonEventArg[0].argument.varType = TYPE_BOOL;
			hoverButtonEventArg[0].argument.varBool = false;

			Event hoverButtonEvent = Event(1, EVENT_TYPE_ON_HOVER, hoverButtonEventArg);

			universe->GetLayout(LAYOUT_MENU).GetButtonControl("newGame")->OnEvent(hoverButtonEvent);
		}

		/*if(
			universe->GetLayout(LAYOUT_MENU).
			GetButtonControl("saveGame").
			IsMouseOn(scene->GetMouse().GetClipSpacePosition(windowWidth, windowHeight))
			)
		{
			Event hoverButtonEvent = StockEvents::EventOnHover();

			universe->GetLayout(LAYOUT_MENU).GetButtonControl("saveGame").OnEvent(hoverButtonEvent);
		}*/
	}


	displayData.mousePosition = scene.GetMouse().GetCurrentPosition();


	scene.GetMouse().ReleaseRightButton();
	scene.GetMouse().ReleaseLeftButton();
}
void HandleMouseButtons(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		scene.GetMouse().PressLeftButton();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		scene.GetMouse().ReleaseLeftButton();
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		scene.GetMouse().PressRightButton();
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		scene.GetMouse().ReleaseRightButton();
	}
}
void HandleActiveMovement(int x, int y)
{
	scene.GetMouse().SetCurrentPosition(glm::ivec2(x, y));
}
void HandlePassiveMovement(int x, int y)
{
	scene.GetMouse().SetCurrentPosition(glm::ivec2(x, y));
}

void InitializePrograms()
{
	scene.GetShaderManager().LoadLitProgram("shaders/PN.vert", "shaders/GaussianLighting.frag");
	scene.GetShaderManager().LoadUnlitProgram("shaders/PosTransform.vert", "shaders/UniformColor.frag");
	scene.GetShaderManager().LoadSimpleProgram("shaders/PosColorLocalTransform.vert", "shaders/ColorPassThrough.frag");
	scene.GetShaderManager().LoadSimpleNoUBProgram("shaders/PosTransformNoUB.vert", "shaders/ColorPassThrough.frag");
	scene.GetShaderManager().LoadFontProgram("shaders/Font.vert", "shaders/Font.frag");
	scene.GetShaderManager().LoadSimpleTextureProgData("shaders/SimpleTexture.vert", "shaders/SimpleTexture.frag");
	scene.GetShaderManager().LoadTextureProgData("shaders/Texture.vert", "shaders/Texture.frag");
	scene.GetShaderManager().LoadPerspectiveTextureProgData("shaders/TexturePerspective.vert", "shaders/Texture.frag");
	scene.GetShaderManager().LoadBillboardProgData("shaders/BillboardShader.vert", "shaders/BillboardShader.geom", "shaders/BillboardShader.frag");
	scene.GetShaderManager().LoadParticleProgData("shaders/ParticleShader.vert", "shaders/ParticleShader.geom");
	scene.GetShaderManager().LoadBillboardProgDataNoTexture("shaders/BillboardShader.vert", "shaders/BillboardShaderNoTexture.geom", "shaders/BillboardShaderNoTexture.frag");
}

void InitializeGUI()
{
	GUILoader guiLoader("../data/gui-descriptor/descriptor.txt", 
						glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	scene.AddLayouts(guiLoader.GetAllLoadedLayouts());
	
	glm::vec4 mousePos_worldSpace = 
		scene.GetMouse().GetWorldSpacePosition(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
											   displayData.projectionMatrix, displayData.modelMatrix);
}

void InitializeScene()
{
	Mouse userMouse;
	TopDownCamera userCamera = TopDownCamera(glm::vec3(), 13.5f, 0.0f, 45.0f);


	std::shared_ptr<CelestialBody> 
		mainSun(new CelestialBody(glm::vec3(0.0f), glm::vec4(0.738f, 0.738f, 0.423f, 1.0f), 1.25f, 4, 50, 
								  200, 100));

	SunLight 
		mainSunLight(SunLight(glm::vec3(), glm::vec4(3.5f), glm::vec4(0.4f), 1.2f, 5.0f, displayData.gamma));

	mainSun->LoadMesh("mesh-files/UnitSphere.xml");

	scene.SetMouse(userMouse);
	scene.SetTopDownCamera(userCamera);

	scene.AddSun(mainSun);
	scene.AddSunLight(mainSunLight);

	scene.SetMusic("../data/music/background.mp3", MUSIC_BACKGROUND);
	scene.SetMusic("../data/music/onclick.wav", MUSIC_ON_SUN_CLICK);

	const float musicVolumeInteraction = 0.1f;
	const float musicVolumeMaster = 0.1f;

	scene.SetMusicVolume(musicVolumeInteraction, CHANNEL_INTERACTION);
	scene.SetMusicVolume(musicVolumeMaster, CHANNEL_MASTER);	

	scene.SetFusion(FusionInput('f'));
	scene.AddFusionSequence("fireSatellite", 'q', 'q', 'q');
	scene.AddFusionSequence("earthSatellite", 'e', 'e', 'e');
	scene.AddFusionSequence("waterSatellite", 'w', 'w', 'w');
	scene.AddFusionSequence("airSatellite", 'q', 'w', 'e');
	scene.AddFusionSequence("aoeSkill", 'q', 'q', 'w');
	scene.AddFusionSequence("passiveAoeSkill", 'q', 'q', 'e');
	scene.AddFusionSequence("sunNovaSkill", 'w', 'w', 'e');
	scene.AddFusionSequence("satFrostNova", 'q', 'w', 'q');
	scene.AddFusionSequence("satShieldSkill", 'w', 'e', 'w');
	scene.AddFusionSequence("burnSkill", 'w', 'e', 'q');


	std::shared_ptr<AOESkill> testAOESkill =
		std::shared_ptr<AOESkill>(new AOESkill(glm::vec3(),
											   20, 2.0f, 
											   "aoeSkill",
											   'q', 'q', 'w',
											   10));
	std::shared_ptr<PassiveAOESkill> testPassiveAOESkill =
		std::shared_ptr<PassiveAOESkill>(new PassiveAOESkill(glm::vec3(),
															 20, 1, 4,
															 2.0f,
															 "passiveAOESkill",
															 'q', 'q', 'e', 
															 10));
	std::shared_ptr<SunNovaSkill> testSunNovaSkill =
		std::shared_ptr<SunNovaSkill>(new SunNovaSkill(glm::vec3(), 40, 6.0f, 
													   0.05f, 
													   "sunNovaSkill", 
													   'w', 'w', 'e', 
													   50));
	std::shared_ptr<BurnSkill> testBurnSkill =
		std::shared_ptr<BurnSkill>(new BurnSkill(glm::vec3(), 
												 30, 1, 3,
												 2.0f, 
												 "burnSkill",
												 'w', 'e', 'q',
												 20));

	scene.GetSun()->AddSkill(testPassiveAOESkill);
	scene.GetSun()->AddSkill(testAOESkill);
	scene.GetSun()->AddSkill(testSunNovaSkill);
	scene.GetSun()->AddSkill(testBurnSkill);


	glUseProgram(scene.GetShaderManager().GetTextureProgData().theProgram);
	glUniform1i(scene.GetShaderManager().GetTextureProgData().colorTextureUnif, 0);
	glUseProgram(0);


	InitializeGUI();
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

//Mesh sampleMesh;

void Init()
{
	currentTime_milliseconds = GetCurrentTimeMillis();

	glutTimerFunc(0, TimerFunction, 0);


	InitializePrograms();
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

	GLuint lightUniformBuffer = 0;
	glGenBuffers(1, &lightUniformBuffer);
	scene.GetShaderManager().SetUniformBuffer(UBT_LIGHT, lightUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlockGamma), NULL, GL_DYNAMIC_DRAW);

	GLuint projectionUniformBuffer = 0;
	glGenBuffers(1, &projectionUniformBuffer);
	scene.GetShaderManager().SetUniformBuffer(UBT_PROJECTION, projectionUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, projectionUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

	GLuint orthographicUniformBuffer = 0;
	glGenBuffers(1, &orthographicUniformBuffer);
	scene.GetShaderManager().SetUniformBuffer(UBT_ORTHOGRAPHIC, orthographicUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, orthographicUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

	// Bind the static buffers.
	glBindBufferRange(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetBlockIndex(BT_LIGHT), 
		lightUniformBuffer, 
		0, sizeof(LightBlockGamma));

	glBindBufferRange(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetBlockIndex(BT_PROJECTION), 
		projectionUniformBuffer,
		0, sizeof(glm::mat4));

	glBindBufferRange(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetBlockIndex(BT_ORTHOGRAPHIC),
		orthographicUniformBuffer,
		0, sizeof(glm::mat4));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	nextGameTick = GetTickCount();

	/*
	if(!sampleMesh.LoadMesh("../data/mesh-files/spaceship.obj"))
	{
		std::printf("Problem loading texture\n");
	}*/
}

void Display()
{
	frameCount++;

	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(scene.IsLayoutOn(LAYOUT_IN_GAME))
	{		
		scene.SetDisplayData(displayData);
		
		glutil::MatrixStack modelMatrix;

		modelMatrix.SetMatrix(scene.GetTopDownCamera().CalcMatrix());
		displayData.modelMatrix = modelMatrix.Top();

		int loops = 0;
		while(GetTickCount() > nextGameTick && loops < MAX_FRAMESKIP)
		{
			scene.UpdateScene();

			nextGameTick += SKIP_TICKS;
			loops++;
		}


		float interpolation = float(GetTickCount() + SKIP_TICKS - nextGameTick) / float(SKIP_TICKS);
		scene.RenderScene(modelMatrix, interpolation);	
		
		scene.RenderCurrentLayout();
		

		//sampleMesh.Render(modelMatrix, shaderManager.GetSimpleTextureProgData());		
	}
	else //if(scene->IsLayoutOn(LAYOUT_MENU))
	{
		scene.RenderCurrentLayout();
	}

	HandleMouse();
	scene.GetMouse().OverrideLastPosition(scene.GetMouse().GetCurrentPosition());
	
	glutSwapBuffers();
	glutPostRedisplay();
}


void Reshape(int width, int height)
{
	glutil::MatrixStack projMatrix;
	float aspectRatio = width / (float)height;
	projMatrix.Perspective(45.0f, aspectRatio, displayData.zNear, displayData.zFar);

	displayData.projectionMatrix = projMatrix.Top();
	
	glUseProgram(scene.GetShaderManager().GetBillboardProgData().theProgram);
	glUniformMatrix4fv(scene.GetShaderManager().GetBillboardProgData().cameraToClipMatrixUnif, 
					   1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
	glUseProgram(0);

	glUseProgram(scene.GetShaderManager().GetBillboardProgDataNoTexture().theProgram);
	glUniformMatrix4fv(scene.GetShaderManager().GetBillboardProgDataNoTexture().cameraToClipMatrixUnif,		
					   1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
	glUseProgram(0);
	
	glBindBuffer(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetUniformBuffer(UBT_PROJECTION));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(displayData.projectionMatrix), &displayData.projectionMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	projMatrix.SetIdentity();
	projMatrix.Orthographic((float)width, 0.0f, (float)height, 0.0f, 1.0f, 1000.0f);

	//displayData.projectionMatrix = projMatrix.Top(); <- bugs the clicking mechanism


	glBindBuffer(GL_UNIFORM_BUFFER, scene.GetShaderManager().GetUniformBuffer(UBT_ORTHOGRAPHIC));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(displayData.projectionMatrix), &projMatrix.Top());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	
	scene.UpdateCurrentLayout(width, height);
	// TODO: Isolate in a separate function.
	/*if(width <= 800 || height <= 600)
	{
		scene->SetLayoutPreset(SMALL);
		scene->UpdateCurrentLayout(800, 600);
	}
	else if(width > 800 && width <= 1440 ||
			height > 600 && height <= 900)
	{
		scene->SetLayoutPreset(MEDIUM);
		scene->UpdateCurrentLayout(1440, 900);
	}
	else
	{
		scene->SetLayoutPreset(BIG);
		scene->UpdateCurrentLayout(1920, 1080);
	}*/

	displayData.windowHeight = height;
	displayData.windowWidth = width;

	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	// This needs to be passed as an event.
	if(scene.IsLayoutOn(LAYOUT_MENU) && 
	   scene.GetLayout(LAYOUT_MENU)->HasActiveControl())
	{
		scene.GetLayout(LAYOUT_MENU)->GetActiveControl()->InputChar(key);
	}

	switch (key)
	{
	case 27:
		// TODO: When exiting that way, if the scene is not a pointer, 
		//	     the application crashes.
		glutLeaveMainLoop();
		return;
	case 32:
		/*EventArg spaceClickedEventArg[1];
		spaceClickedEventArg[0].argType = "command";
		spaceClickedEventArg[0].argument.varType = TYPE_STRING;
		strcpy(spaceClickedEventArg[0].argument.varString, "playBackgroundMusic");
		Event spaceClickedEvent = Event(1, EVENT_TYPE_SPACE_BTN_CLICK, spaceClickedEventArg);

		universe->OnEvent(spaceClickedEvent);*/
		break;
	}

	if(scene.IsLayoutOn(LAYOUT_IN_GAME))
	{
		Event returnedFusionEvent = StockEvents::EmptyEvent();
		scene.UpdateFusion(key, returnedFusionEvent);
		scene.GetLayout(LAYOUT_IN_GAME)->GetControl("fusionOne")->OnEvent(returnedFusionEvent);
		scene.GetLayout(LAYOUT_IN_GAME)->GetControl("fusionTwo")->OnEvent(returnedFusionEvent);
		scene.GetLayout(LAYOUT_IN_GAME)->GetControl("fusionThree")->OnEvent(returnedFusionEvent);
	}

	glutPostRedisplay();
}


unsigned int Defaults(unsigned int displayMode, int &width, int &height) 
{ 
	return displayMode; 
}