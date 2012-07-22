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


#include "Main.h"
#include "../Entities/PlanetBodies.h"
#include "../Entities/Lights.h"
#include "../Universe/Universe.h"
#include "../Camera/TopDownCamera.h"


float g_fzNear = 1.0f;
float g_fzFar = 1000.0f;

ProgramData g_Lit;
UnlitProgData g_Unlit;
InterpProgData g_Interp;

const int g_projectionBlockIndex = 2;

GLuint positionAttrib;
//GLuint colorAttrib;
InterpProgData LoadInterpProgram(const std::string &strVertexShader, const std::string &strFragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, strFragmentShader));

	InterpProgData data;
	data.theProgram = Framework::CreateProgram(shaderList);
	data.modelToCameraMatrixUnif = glGetUniformLocation(data.theProgram, "modelToCameraMatrix");
	data.colorUnif = glGetUniformLocation(data.theProgram, "color");

	positionAttrib = glGetAttribLocation(data.theProgram, "position");
	//colorAttrib = glGetAttribLocation(data.theProgram, "color");
	/*data.startColorUnif = glGetUniformLocation(data.theProgram, "startColor");*/
	//data.endColorUnif = glGetUniformLocation(data.theProgram, "endColor");

	GLuint projectionBlock = glGetUniformBlockIndex(data.theProgram, "Projection");	
	glUniformBlockBinding(data.theProgram, projectionBlock, g_projectionBlockIndex);

	return data;
}

UnlitProgData LoadUnlitProgram(const std::string &strVertexShader, const std::string &strFragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, strFragmentShader));

	UnlitProgData data;
	data.theProgram = Framework::CreateProgram(shaderList);
	data.modelToCameraMatrixUnif = glGetUniformLocation(data.theProgram, "modelToCameraMatrix");
	data.objectColorUnif = glGetUniformLocation(data.theProgram, "objectColor");

	GLuint projectionBlock = glGetUniformBlockIndex(data.theProgram, "Projection");
	glUniformBlockBinding(data.theProgram, projectionBlock, g_projectionBlockIndex);

	return data;
}

ProgramData LoadLitProgram(const std::string &strVertexShader, const std::string &strFragmentShader)
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, strFragmentShader));

	ProgramData data;
	data.theProgram = Framework::CreateProgram(shaderList);
	data.modelToCameraMatrixUnif = glGetUniformLocation(data.theProgram, "modelToCameraMatrix");
	data.lightIntensityUnif = glGetUniformLocation(data.theProgram, "lightIntensity");
	data.ambientIntensityUnif = glGetUniformLocation(data.theProgram, "ambientIntensity");

	data.normalModelToCameraMatrixUnif = glGetUniformLocation(data.theProgram, "normalModelToCameraMatrix");
	data.cameraSpaceLightPosUnif = glGetUniformLocation(data.theProgram, "cameraSpaceLightPos");
	data.lightAttenuationUnif = glGetUniformLocation(data.theProgram, "lightAttenuation");
	data.shininessFactorUnif = glGetUniformLocation(data.theProgram, "shininessFactor");
	data.baseDiffuseColorUnif = glGetUniformLocation(data.theProgram, "baseDiffuseColor");

	GLuint projectionBlock = glGetUniformBlockIndex(data.theProgram, "Projection");
	glUniformBlockBinding(data.theProgram, projectionBlock, g_projectionBlockIndex);

	return data;
}

void InitializePrograms()
{
	g_Lit = LoadLitProgram("PN.vert", "GaussianLighting.frag");
	g_Unlit = LoadUnlitProgram("PosTransform.vert", "UniformColor.frag");
	g_Interp = LoadInterpProgram("Interp.vert", "Interp.frag");
}

GLuint g_projectionUniformBuffer = 0;

struct ProjectionBlock
{
	glm::mat4 cameraToClipMatrix;
};

glm::mat4 g_cameraToClipMatrix;
glm::mat4 g_modelMatrix;

Sun *mainSun = new Sun(glm::vec3(), 0.5f);
SunLight *mainSunLight = new SunLight(glm::vec3(), glm::vec4(1.0f), glm::vec4(0.2f), glm::vec4(1.0f),
								   1.2f, 0.5f);

Universe *universe = new Universe();

Mouse userMouse;

TopDownCamera userCamera = TopDownCamera(glm::vec3(), 6.25f, 90.0f, 0.0f);

float CalcFrustumScale(float fFovDeg)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fFovRad = fFovDeg * degToRad;
	return 1.0f / tan(fFovRad / 2.0f);
}

float initialOffset = 1.5f;

void HandleMouse()
{
	glm::vec3 cameraPosition = userCamera.ResolveCamPosition();

	float windowWidth = (float)glutGet(GLUT_WINDOW_HEIGHT);
	float windowHeight = (float)glutGet(GLUT_WINDOW_WIDTH);

	if(userMouse.IsLeftButtonDown())
	{
		if(mainSun->IsClicked
			(g_cameraToClipMatrix, g_modelMatrix, userMouse, 
			 glm::vec4(cameraPosition, 1.0f), windowWidth, windowHeight))
		{
			if(mainSun->AddSatellite("UnitSphere.xml", 1.5f, initialOffset, 10.0f, 0.5f))
			{
				initialOffset += 1.0f;
			}
			else std::printf("Satellite cap reached!\n");
		}

		std::vector<Satellite*> satellites = mainSun->GetSatellites();
		for(int satellite = 0; satellite < satellites.size(); satellite++)
		{
			if(satellites[satellite]->IsClicked
				(g_cameraToClipMatrix, g_modelMatrix, userMouse, 
				 glm::vec4(cameraPosition, 1.0f), windowWidth, windowHeight))
			{
				std::printf("Satellite %d clicked!!!\n", satellite);
			}
		}
	}

	std::vector<Satellite*> satellites = mainSun->GetSatellites();
	for(int satellite = 0; satellite < satellites.size(); satellite++)
	{
		if(satellites[satellite]->IsClicked
			(g_cameraToClipMatrix, g_modelMatrix, userMouse, 
				glm::vec4(cameraPosition, 1.0f), windowWidth, windowHeight))
		{
			std::printf("Satellite %d clicked!!!\n", satellite);
		}
	}

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

std::vector<float> torusData;
unsigned short indexData[181 * 4];

void Generate2DTorus(float innerRadius, float outerRadius, short resolution)
{
	short currentCoord = 0;
	unsigned short currentIndex = 0;
	int currentIndexPos = 0;

	//indexData.resize(((resolution / 2) + 1) * 4);
	//indexData = new unsigned short[181 * 4];

	for(int i = 0; i <= 360; i += 360 / resolution)
	{		
		torusData.push_back(cosf(i * (PI / 180.0f)) * outerRadius);
		torusData.push_back(0.0f);
		torusData.push_back(sinf(i * (PI / 180.0f)) * outerRadius);
		torusData.push_back(1.0f);
		
		torusData.push_back(cosf(i * (PI / 180.0f)) * innerRadius);
		torusData.push_back(0.0f);
		torusData.push_back(sinf(i * (PI / 180.0f)) * innerRadius);
		torusData.push_back(1.0f);

		indexData[currentIndexPos] = currentIndex + 1;
		indexData[currentIndexPos + 1] = currentIndex + 2;
		//indexData.insert(indexData.begin() + currentIndexPos, currentIndex + 1);
		//indexData.insert(indexData.begin() + (currentIndexPos + 1), currentIndex + 2);

		currentIndexPos += 2;
		currentCoord += 8;
		currentIndex += 2;
	}

	std::printf("Index: %i\n", currentIndex);

	indexData[currentIndexPos - 1] = 1;
	indexData[currentIndexPos] = 2;
	indexData[currentIndexPos + 1] = currentIndex - 2;
	//indexData.insert(indexData.begin() + (currentIndexPos - 1), 1);
	//indexData.insert(indexData.begin() + currentIndexPos, 2);
	//indexData.insert(indexData.begin() + (currentIndexPos + 1), currentIndex - 2);
	
	//std::printf("Size: %i\n", indexData.size());
	std::printf("Size: %i\n", sizeof(indexData));
}

GLuint vertexBO;
GLuint indexBO;
GLuint vao;

Utility::BasicMeshGeneration::Torus2D torus(1.0f, 2.0f, 360);

void InitializeVAO()
{
	//Generate2DTorus(1.0f, 2.0f, 360);
	//torus.Generate();

	glGenBuffers(1, &vertexBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
	glBufferData(GL_ARRAY_BUFFER, torus.vertexData.size() * sizeof(float), &torus.vertexData[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(torus.indexData), torus.indexData, GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBO);

	glBindVertexArray(0);

	torus.indexBO = indexBO;
	torus.vertexBO = vertexBO;
	torus.vao = vao;
}



//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void Init()
{
	InitializePrograms();
	torus.Init();
	//InitializeVAO();
	
	mainSun->LoadMesh("UnitSphere.xml");
	//mainSun->AddSatellite("UnitSphere.xml", 1.5f, 3.0f, 100.0f, 0.5f);
	//mainSun->AddSatellite("UnitSphere.xml", 0.0f, 2.0f, 110.0f, 0.5f);
	//mainSun->AddSatellite("UnitSphere.xml", 1.0f, 2.5f, 90.0f, 0.5f);
	//mainSun->AddSatellite("UnitSphere.xml", 1.0f, 4.0f, 60.0f, 0.5f);

	universe->AddSun(mainSun);
	universe->AddSunLight(mainSunLight);


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

	glGenBuffers(1, &g_projectionUniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, g_projectionUniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjectionBlock), NULL, GL_DYNAMIC_DRAW);

	//Bind the static buffers.
	glBindBufferRange(GL_UNIFORM_BUFFER, g_projectionBlockIndex, g_projectionUniformBuffer,
		0, sizeof(ProjectionBlock));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

int drawCMD = GL_TRIANGLES;

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void Display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutil::MatrixStack modelMatrix;
	modelMatrix.SetMatrix(userCamera.CalcMatrix());

	torus.Draw(modelMatrix, g_Interp, positionAttrib);
	/*glUseProgram(g_Interp.theProgram);
	glBindVertexArray(vao);
	{
		glutil::PushStack push(modelMatrix);

		modelMatrix.RotateX(90.0f);

		glUniformMatrix4fv(g_Interp.modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(modelMatrix.Top()));
		glUniform4f(g_Interp.colorUnif, 1.0f, 0.0f, 0.0f, 1.0f);
		
		glBindBuffer(GL_ARRAY_BUFFER, vertexBO);
		glEnableVertexAttribArray(positionAttrib);
		glVertexAttribPointer(positionAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawElements(GL_TRIANGLE_STRIP, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
	}
	glDisableVertexAttribArray(0);
	glUseProgram(0);*/

	universe->UpdateUniverse();
	universe->RenderUniverse(modelMatrix, g_Lit, g_Unlit, g_Interp);

	g_modelMatrix = modelMatrix.Top();

	HandleMouse();
	userMouse.OverrideLastPosition(userMouse.GetCurrentPosition());

	glutSwapBuffers();
	glutPostRedisplay();
}


//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void Reshape(int width, int height)
{
	glutil::MatrixStack persMatrix;
	float aspectRatio = width / (float)height;
	persMatrix.Perspective(45.0f, aspectRatio, g_fzNear, g_fzFar);

	//ProjectionBlock projData;
	//projData.cameraToClipMatrix = persMatrix.Top();
	g_cameraToClipMatrix = persMatrix.Top();

	glBindBuffer(GL_UNIFORM_BUFFER, g_projectionUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(g_cameraToClipMatrix), &g_cameraToClipMatrix);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	glutPostRedisplay();
}

char animationIDs[] =
{
	'a', 'b', 'c',
};

int currAnimID = 0;


//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		return;
	case 'a':
		std::vector<Satellite*> satellites = mainSun->GetSatellites();
		for(int i = 0; i < satellites.size(); i++)
		{
			satellites[i]->SetAnimationID(animationIDs[currAnimID]);
		}
		currAnimID++;
		if(currAnimID > ARRAY_COUNT(animationIDs)) currAnimID = 0;
		break;
	}

	glutPostRedisplay();
}


unsigned int Defaults(unsigned int displayMode, int &width, int &height) 
{ 
	return displayMode; 
}