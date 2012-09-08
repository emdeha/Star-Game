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

	float windowWidth = (float)glutGet(GLUT_WINDOW_WIDTH);
	float windowHeight = (float)glutGet(GLUT_WINDOW_HEIGHT);

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
			   universe->GetLayout(LAYOUT_IN_GAME).
			   GetButtonControl("exit").
			   IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("exitButton");

				universe->GetLayout(LAYOUT_IN_GAME).GetButtonControl("exit").OnEvent(leftClickButtonEvent);

				universe->OnEvent(leftClickButtonEvent);

				userMouse.ReleaseLeftButton();
				return;
			}
		}

		if(universe->IsLayoutOn(LAYOUT_MENU))
		{
			if(
				universe->GetLayout(LAYOUT_MENU).
				GetButtonControl("newGame").
				IsMouseOn(userMouse.GetClipSpacePosition(windowWidth, windowHeight))
			  )
			{
				Event leftClickButtonEvent = StockEvents::EventOnLeftClick("newGameButton");

				universe->GetLayout(LAYOUT_MENU).GetButtonControl("newGame").OnEvent(leftClickButtonEvent);

				universe->OnEvent(leftClickButtonEvent);
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

	/*if(myButton.IsMouseOn(
		glm::vec2(userMouse.GetClipSpacePosition(windowWidth, windowHeight))))
	{
		return;
	}*/

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
	shaderManager.LoadLitProgram("PN.vert", "GaussianLighting.frag");
	shaderManager.LoadUnlitProgram("PosTransform.vert", "UniformColor.frag");
	shaderManager.LoadSimpleProgram("Interp.vert", "Interp.frag");
	shaderManager.LoadSimpleProgramOrtho("Simple.vert", "Simple.frag");
	shaderManager.LoadFontProgram("Font.vert", "Font.frag");
}

void InitializeGUI()
{
	LayoutInfo inGameLayoutInfo;
	inGameLayoutInfo.backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	Layout inGameLayout(LAYOUT_IN_GAME, inGameLayoutInfo);

	Button exitButton("exit", "Exit", glm::vec2(0.5f, 0.5f),
					  3.0f, 3.0f, 11.8f, 12.0f,
					  58,
					  glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
					  MEDIUM);
	exitButton.Init("../data/fonts/AGENCYR.TTF",
				    glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	exitButton.AddPreset(SMALL,
						 3.0f, 3.0f, 11.8f, 12.0f,
						 48,
						 glm::vec2(0.5f, 0.5f));
	exitButton.AddPreset(BIG,
						 3.0f, 3.0f, 11.8f, 12.0f,
						 68,
						 glm::vec2(0.5f, 0.5f));

	inGameLayout.AddButtonControl(exitButton);

	universe->AddLayout(inGameLayout);
	universe->SetLayout(LAYOUT_IN_GAME, false);
	


	LayoutInfo menuLayoutInfo;
	menuLayoutInfo.textSize = 48;
	menuLayoutInfo.backgroundColor = glm::vec4(0.0f, 0.5f, 0.6f, 1.0f);

	Layout menuLayout(LAYOUT_MENU, menuLayoutInfo);

	Button newGameButton("newGame", "New Game", glm::vec2(-0.9f, -0.6f),
						 0.0f, 0.0f, 0.0f, 0.0f,
						 48,
						 glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
						 MEDIUM);
	newGameButton.Init("../data/fonts/AGENCYR.TTF",
					   glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	
	newGameButton.AddPreset(SMALL,
							0.0f, 0.0f, 0.0f, 0.0f,
							28,
							glm::vec2(-0.9f, -0.62f));
	newGameButton.AddPreset(BIG,
							0.0f, 0.0f, 0.0f, 0.0f,
							68,
							glm::vec2(-0.9f, -0.58f));

	Button saveGameButton("saveGame", "Save Game", glm::vec2(-0.9f, -0.7f),
						  0.0f, 0.0f, 0.0f, 0.0f,
						  48,
						  glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
						  MEDIUM);
	saveGameButton.Init("../data/fonts/AGENCYR.TTF",
					    glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	saveGameButton.AddPreset(SMALL,
							 0.0f, 0.0f, 0.0f, 0.0f,
							 28,
							 glm::vec2(-0.9f, -0.71f));
	saveGameButton.AddPreset(BIG, 
							 0.0f, 0.0f, 0.0f, 0.0f,
							 68,
							 glm::vec2(-0.9f, -0.69f));


	Button quitGameButton("quitGame", "Quit", glm::vec2(-0.9f, -0.8f),
						  0.0f, 0.0f, 0.0f, 0.0f,
						  48, 
						  glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT),
						  MEDIUM);
	quitGameButton.Init("../data/fonts/AGENCYR.TTF",
					    glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	quitGameButton.AddPreset(SMALL,
							 0.0f, 0.0f, 0.0f, 0.0f,
							 28,
							 glm::vec2(-0.9f, -0.8f));
	quitGameButton.AddPreset(BIG,
							 0.0f, 0.0f, 0.0f, 0.0f,
							 68,
							 glm::vec2(-0.9f, -0.8f));



	menuLayout.AddButtonControl(newGameButton);
	menuLayout.AddButtonControl(saveGameButton);
	menuLayout.AddButtonControl(quitGameButton);

	universe->AddLayout(menuLayout);
	universe->SetLayout(LAYOUT_MENU, true);
}

void InitializeScene()
{
	mainSun->LoadMesh("UnitSphere.xml");

	universe->AddSun(mainSun);
	universe->AddSunLight(mainSunLight);

	universe->SetMusic("../data/music/background.mp3", MUSIC_BACKGROUND);
	universe->SetMusic("../data/music/onclick.wav", MUSIC_ON_SUN_CLICK);

	universe->SetMusicVolume(g_musicVolumeInteraction, CHANNEL_INTERACTION);
	universe->SetMusicVolume(g_musicVolumeMaster, CHANNEL_MASTER);	

	InitializeGUI();
}


void Init()
{
	InitializePrograms();
	InitializeScene();

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

		//myButton.Draw(shaderManager.GetFontProgData(), shaderManager.GetSimpleProgDataOrtho());
		universe->RenderCurrentLayout(shaderManager.GetFontProgData(),
									  shaderManager.GetSimpleProgDataOrtho());					 
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
	glutil::MatrixStack persMatrix;
	float aspectRatio = width / (float)height;
	persMatrix.Perspective(45.0f, aspectRatio, displayData.zNear, displayData.zFar);

	displayData.projectionMatrix = persMatrix.Top();
	

	glBindBuffer(GL_UNIFORM_BUFFER, shaderManager.GetUniformBuffer(UBT_PROJECTION));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(displayData.projectionMatrix), &displayData.projectionMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	glutPostRedisplay();

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
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		return;
	case 32:
		EventArg spaceClickedEventArg[1];
		spaceClickedEventArg[0].argType = "command";
		spaceClickedEventArg[0].argument.varType = TYPE_STRING;
		strcpy(spaceClickedEventArg[0].argument.varString, "playBackgroundMusic");
		Event spaceClickedEvent = Event(1, EVENT_TYPE_SPACE_BTN_CLICK, spaceClickedEventArg);

		universe->OnEvent(spaceClickedEvent);
		break;
	}

	glutPostRedisplay();
}


unsigned int Defaults(unsigned int displayMode, int &width, int &height) 
{ 
	return displayMode; 
}