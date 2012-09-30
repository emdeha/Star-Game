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


#include "Main.h"
#include "ShaderManager.h"
#include "DisplayData.h"
#include "../framework/EventSystem.h"
#include "../Universe/Universe.h"
#include "../Camera/TopDownCamera.h"
#include "../AssetLoader/GUILoader.h"
#include "../AssetLoader/MeshLoader.h"


ShaderManager shaderManager;
DisplayData displayData;

std::shared_ptr<Sun> mainSun(new Sun(glm::vec3(0.0f), glm::vec4(0.738f, 0.738f, 0.423f, 1.0f), 
									 1.25f, 4));
SunLight mainSunLight(SunLight(glm::vec3(), glm::vec4(3.5f), glm::vec4(0.4f), 
					  1.2f,
					  5.0f, displayData.gamma));

Universe *universe(new Universe());

Mouse userMouse;

TopDownCamera userCamera = TopDownCamera(glm::vec3(), 12.5f, 90.0f, 135.0f);

float g_initialOffset = 1.0f;


float g_musicVolumeMaster = 0.1f;
float g_musicVolumeInteraction = 0.1f;



void HandleMouse()
{
	glm::vec3 cameraPosition = userCamera.ResolveCamPosition();

	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	if(userMouse.IsRightButtonDown())
	{
		if(mainSun->IsClicked
			(displayData.projectionMatrix, displayData.modelMatrix, userMouse, 
			 glm::vec4(cameraPosition, 1.0f), windowWidth, windowHeight))
		{
			Event rightClickSunEvent = StockEvents::EventOnRightClick("sun");

			mainSun->OnEvent(rightClickSunEvent);
			universe->OnEvent(rightClickSunEvent);
		}
	}

	if(userMouse.IsLeftButtonDown())
	{
		if(universe->IsLayoutOn(LAYOUT_IN_GAME))
		{
			if(
			   universe->GetLayout(LAYOUT_IN_GAME)->
			   GetControl("exit")->
			   IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("exitButton");

				universe->GetLayout(LAYOUT_IN_GAME)->GetControl("exit")->OnEvent(leftClickButtonEvent);

				universe->OnEvent(leftClickButtonEvent);

				userMouse.ReleaseLeftButton();
				return;
			}
		}

		if(universe->IsLayoutOn(LAYOUT_MENU))
		{
			if(
				universe->GetLayout(LAYOUT_MENU)->
				GetControl("newGame")->
				IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("newGameButton");

				universe->GetLayout(LAYOUT_MENU)->GetControl("newGame")->OnEvent(leftClickButtonEvent);

				universe->OnEvent(leftClickButtonEvent);
			}

			if(
				universe->GetLayout(LAYOUT_MENU)->
				GetControl("saveGame")->
				IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("saveGameButton");

				universe->GetLayout(LAYOUT_MENU)->GetControl("saveGame")->OnEvent(leftClickButtonEvent);

				universe->OnEvent(leftClickButtonEvent);
			}

			if(
				universe->GetLayout(LAYOUT_MENU)->
				GetControl("quitGame")->
				IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("quitGameButton");

				universe->GetLayout(LAYOUT_MENU)->GetControl("quitGame")->OnEvent(leftClickButtonEvent);

				universe->OnEvent(leftClickButtonEvent);
			}

			if(
				universe->GetLayout(LAYOUT_MENU)->
				GetControl("printCmd")->
				IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("printCmd");

				universe->GetLayout(LAYOUT_MENU)->GetControl("printCmd")->OnEvent(leftClickButtonEvent);

				universe->OnEvent(leftClickButtonEvent);
			}
			
			if(
				universe->GetLayout(LAYOUT_MENU)->
				GetControl("sample")->
				IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			  )
			{
				Event leftClickTextBoxEvent = StockEvents::EventOnLeftClick("sample");

				universe->GetLayout(LAYOUT_MENU)->GetControl("sample")->OnEvent(leftClickTextBoxEvent);

				
				if(universe->GetLayout(LAYOUT_MENU)->GetControl("sample") != 
				   universe->GetLayout(LAYOUT_MENU)->GetActiveControl())
				{
					Event unclickEvent = Event(EVENT_TYPE_UNCLICK);

					universe->GetLayout(LAYOUT_MENU)->GetActiveControl()->OnEvent(unclickEvent);
				}
			}
			
			if(
				universe->GetLayout(LAYOUT_MENU)->
				GetControl("sampleTwo")->
				IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			  )
			{
				Event leftClickTextBoxEvent = StockEvents::EventOnLeftClick("sampleTwo");

				universe->GetLayout(LAYOUT_MENU)->GetControl("sampleTwo")->OnEvent(leftClickTextBoxEvent);

				
				if(universe->GetLayout(LAYOUT_MENU)->GetControl("sampleTwo") != 
				   universe->GetLayout(LAYOUT_MENU)->GetActiveControl())
				{
					Event unclickEvent = Event(EVENT_TYPE_UNCLICK);

					universe->GetLayout(LAYOUT_MENU)->GetActiveControl()->OnEvent(unclickEvent);
				}
			}
		}

		if(mainSun->IsClicked
			(displayData.projectionMatrix, displayData.modelMatrix, userMouse, 
			 glm::vec4(cameraPosition, 1.0f), windowWidth, windowHeight))
		{
			Event leftClickSunEvent = StockEvents::EventOnLeftClick("sun");

			mainSun->OnEvent(leftClickSunEvent);
			universe->OnEvent(leftClickSunEvent);
		}

		std::vector<Satellite*> sunSatellites = mainSun->GetSatellites();
		for(std::vector<Satellite*>::iterator iter = sunSatellites.begin(); 
			iter != sunSatellites.end(); ++iter)
		{
			if((*iter)->IsClicked
				(displayData.projectionMatrix, displayData.modelMatrix, userMouse, 
					glm::vec4(cameraPosition, 1.0f), windowWidth, windowHeight))
			{
				Event leftClickSatelliteEvent = StockEvents::EventOnLeftClick("satellite");

				(*iter)->OnEvent(leftClickSatelliteEvent);
				universe->OnEvent(leftClickSatelliteEvent);
			}
		}
	}

	std::vector<Satellite*> sunSatellites = mainSun->GetSatellites();
	for(std::vector<Satellite*>::iterator iter = sunSatellites.begin(); 
		iter != sunSatellites.end(); ++iter)
	{
		if((*iter)->IsClicked
			(displayData.projectionMatrix, displayData.modelMatrix, userMouse, 
				glm::vec4(cameraPosition, 1.0f), windowWidth, windowHeight))
		{
			Event satelliteHoveredEvent = StockEvents::EventOnHover();

			(*iter)->OnEvent(satelliteHoveredEvent);
			universe->OnEvent(satelliteHoveredEvent);
		}
	}


	if(universe->IsLayoutOn(LAYOUT_MENU))
	{
		/*if(
			universe->GetLayout(LAYOUT_MENU).
			GetButtonControl("newGame")->
			IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
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
			IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			)
		{
			Event hoverButtonEvent = StockEvents::EventOnHover();

			universe->GetLayout(LAYOUT_MENU).GetButtonControl("saveGame").OnEvent(hoverButtonEvent);
		}*/
	}


	userMouse.ReleaseRightButton();
	userMouse.ReleaseLeftButton();
}
void HandleMouseButtons(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		userMouse.PressLeftButton();
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		userMouse.ReleaseLeftButton();
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		userMouse.PressRightButton();
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		userMouse.ReleaseRightButton();
	}
}
void HandleActiveMovement(int x, int y)
{
	userMouse.SetCurrentPosition(glm::ivec2(x, y));
}
void HandlePassiveMovement(int x, int y)
{
	userMouse.SetCurrentPosition(glm::ivec2(x, y));
}

void InitializePrograms()
{
	shaderManager.LoadLitProgram("shaders/PN.vert", "shaders/GaussianLighting.frag");
	shaderManager.LoadUnlitProgram("shaders/PosTransform.vert", "shaders/UniformColor.frag");
	shaderManager.LoadSimpleProgram("shaders/Interp.vert", "shaders/Interp.frag");
	shaderManager.LoadSimpleProgramOrtho("shaders/Simple.vert", "shaders/Simple.frag");
	shaderManager.LoadFontProgram("shaders/Font.vert", "shaders/Font.frag");
	shaderManager.LoadSimpleTextureProgData("shaders/SimpleTexture.vert", "shaders/SimpleTexture.frag");
}

void InitializeGUI()
{
	GUILoader guiLoader("../data/gui-descriptor/descriptor.txt", 
						glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	universe->AddLayouts(guiLoader.GetAllLoadedLayouts());
}

void InitializeScene()
{
	mainSun->LoadMesh("mesh-files/UnitSphere.xml");

	universe->AddSun(mainSun);
	universe->AddSunLight(mainSunLight);

	universe->SetMusic("../data/music/background.mp3", MUSIC_BACKGROUND);
	universe->SetMusic("../data/music/onclick.wav", MUSIC_ON_SUN_CLICK);

	universe->SetMusicVolume(g_musicVolumeInteraction, CHANNEL_INTERACTION);
	universe->SetMusicVolume(g_musicVolumeMaster, CHANNEL_MASTER);	

	InitializeGUI();
}

Mesh myMesh;

void Init()
{
	InitializePrograms();
	InitializeScene();

	myMesh.LoadMesh("../data/mesh-files/phoenix_ugv.md2");

	glutMouseFunc(HandleMouseButtons);
	glutMotionFunc(HandleActiveMovement);
	glutPassiveMotionFunc(HandlePassiveMovement);

	
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
	shaderManager.SetUniformBuffer(UBT_LIGHT, lightUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBlockGamma), NULL, GL_DYNAMIC_DRAW);

	GLuint projectionUniformBuffer = 0;
	glGenBuffers(1, &projectionUniformBuffer);
	shaderManager.SetUniformBuffer(UBT_PROJECTION, projectionUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, projectionUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

	// Bind the static buffers.
	glBindBufferRange(GL_UNIFORM_BUFFER, shaderManager.GetBlockIndex(BT_LIGHT), 
		lightUniformBuffer, 
		0, sizeof(LightBlockGamma));

	glBindBufferRange(GL_UNIFORM_BUFFER, shaderManager.GetBlockIndex(BT_PROJECTION), 
		projectionUniformBuffer,
		0, sizeof(glm::mat4));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Display()
{
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(universe->IsLayoutOn(LAYOUT_IN_GAME))
	{
		glutil::MatrixStack modelMatrix;
		modelMatrix.SetMatrix(userCamera.CalcMatrix());
	
		displayData.modelMatrix = modelMatrix.Top();

		universe->UpdateUniverse();
		universe->RenderUniverse(modelMatrix, 
								 shaderManager.GetBlockIndex(BT_MATERIAL),
								 shaderManager.GetUniformBuffer(UBT_LIGHT), 
								 shaderManager.GetLitProgData(),
								 shaderManager.GetUnlitProgData(),
								 shaderManager.GetSimpleProgData());
		
		universe->RenderCurrentLayout(shaderManager.GetFontProgData(),
									  shaderManager.GetSimpleProgDataOrtho());	


		myMesh.Render(modelMatrix, shaderManager.GetSimpleTextureProgData());
	}
	else /*if(universe->IsLayoutOn(LAYOUT_MENU))*/
	{
		universe->RenderCurrentLayout(shaderManager.GetFontProgData(),
									  shaderManager.GetSimpleProgDataOrtho());
	}

	HandleMouse();
	userMouse.OverrideLastPosition(userMouse.GetCurrentPosition());

	glutSwapBuffers();
	glutPostRedisplay();
}


void Reshape(int width, int height)
{
	glutil::MatrixStack projMatrix;
	float aspectRatio = width / (float)height;
	projMatrix.Perspective(45.0f, aspectRatio, displayData.zNear, displayData.zFar);

	displayData.projectionMatrix = projMatrix.Top();
	

	glBindBuffer(GL_UNIFORM_BUFFER, shaderManager.GetUniformBuffer(UBT_PROJECTION));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(displayData.projectionMatrix), &displayData.projectionMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	glutPostRedisplay();


	projMatrix.SetIdentity();
	glUseProgram(shaderManager.GetFontProgData().theProgram);
		glUniformMatrix4fv(shaderManager.GetFontProgData().projectionMatrixUnif,
						   1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
	glUseProgram(0);

	glUseProgram(shaderManager.GetSimpleProgDataOrtho().theProgram);
		glUniformMatrix4fv(shaderManager.GetSimpleProgDataOrtho().projectionMatrixUnif, 
						   1, GL_FALSE, glm::value_ptr(projMatrix.Top()));
	glUseProgram(0);

	//universe->UpdateCurrentLayout(width, height);
	if(width <= 800 || height <= 600)
	{
		universe->SetLayoutPreset(SMALL);
		universe->UpdateCurrentLayout(800, 600);
	}
	else if(width > 800 && width <= 1440 ||
			height > 600 && height <= 900)
	{
		universe->SetLayoutPreset(MEDIUM);
		universe->UpdateCurrentLayout(1440, 900);
	}
	else
	{
		universe->SetLayoutPreset(BIG);
		universe->UpdateCurrentLayout(1920, 1080);
	}
}


void Keyboard(unsigned char key, int x, int y)
{
	// This needs to be passed as an event.
	if(universe->IsLayoutOn(LAYOUT_MENU) && 
	   universe->GetLayout(LAYOUT_MENU)->HasActiveControl())
	{
		universe->GetLayout(LAYOUT_MENU)->GetActiveControl()->InputChar(key);
	}

	switch (key)
	{
	case 27:
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

	glutPostRedisplay();
}


unsigned int Defaults(unsigned int displayMode, int &width, int &height) 
{ 
	return displayMode; 
}