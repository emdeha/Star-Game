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
#include "Entities.h"

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))

float g_fzNear = 1.0f;
float g_fzFar = 1000.0f;

enum LightingModel
{
	LM_PHONG_SPECULAR = 0,
	LM_BLINN_SPECULAR,
	LM_GAUSSIAN_SPECULAR,
	LM_BECKMAN_SPECULAR,
	LM_CTBECK_SPECULAR,
	LM_CTGAUSS_SPECULAR,

	LM_MAX_LIGHTING_MODEL,
};

struct ProgramPairs
{
	ProgramData whiteProg;
	ProgramData colorProg;
};

struct ShaderPairs
{
	const char *strWhiteVertShader;
	const char *strColorVertShader;
	const char *strFragmentShader;
};

ProgramPairs g_Programs[LM_MAX_LIGHTING_MODEL];
ShaderPairs g_ShaderFiles[LM_MAX_LIGHTING_MODEL] =
{
	{"PN.vert", "PCN.vert", "PhongLighting.frag"},
	{"PN.vert", "PCN.vert", "BlinnLighting.frag"},
	{"PN.vert", "PCN.vert", "GaussianLighting.frag"},
	{"PN.vert", "PCN.vert", "BeckmannLighting.frag"},
	{"PN.vert", "PCN.vert", "CTBeckLighting.frag"},
	{"PN.vert", "PCN.vert", "CTGaussLighting.frag"},
};

UnlitProgData g_Unlit;

const int g_projectionBlockIndex = 2;

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
	for(int iProg = 0; iProg < LM_MAX_LIGHTING_MODEL; iProg++)
	{
		g_Programs[iProg].whiteProg = LoadLitProgram(
			g_ShaderFiles[iProg].strWhiteVertShader, g_ShaderFiles[iProg].strFragmentShader);
		g_Programs[iProg].colorProg = LoadLitProgram(
			g_ShaderFiles[iProg].strColorVertShader, g_ShaderFiles[iProg].strFragmentShader);
	}

	g_Unlit = LoadUnlitProgram("PosTransform.vert", "UniformColor.frag");
}

Framework::Mesh *g_pCylinderMesh = NULL;
Framework::Mesh *g_pPlaneMesh = NULL;
Framework::Mesh *g_pCubeMesh = NULL;

///////////////////////////////////////////////
// View/Object Setup
glutil::ViewData g_initialViewData =
{
	glm::vec3(0.0f, 0.5f, 0.0f),
	glm::fquat(0.92387953f, 0.3826834f, 0.0f, 0.0f),
	5.0f,
	0.0f
};

glutil::ViewScale g_viewScale =
{
	3.0f, 50.0f,
	1.5f, 0.5f,
	0.0f, 0.0f,		//No camera movement.
	90.0f/250.0f
};

glutil::ObjectData g_initialObjectData =
{
	glm::vec3(0.0f, 0.5f, 0.0f),
	glm::fquat(1.0f, 0.0f, 0.0f, 0.0f),
};

glutil::ViewPole g_viewPole = glutil::ViewPole(g_initialViewData,
											   g_viewScale, glutil::MB_LEFT_BTN);
glutil::ObjectPole g_objtPole = glutil::ObjectPole(g_initialObjectData,
												   90.0f/250.0f, glutil::MB_RIGHT_BTN, &g_viewPole);

namespace
{
	void MouseMotion(int x, int y)
	{
		Framework::ForwardMouseMotion(g_viewPole, x, y);
		Framework::ForwardMouseMotion(g_objtPole, x, y);
		glutPostRedisplay();
	}

	void MouseButton(int button, int state, int x, int y)
	{
		Framework::ForwardMouseButton(g_viewPole, button, state, x, y);
		Framework::ForwardMouseButton(g_objtPole, button, state, x, y);
		glutPostRedisplay();
	}

	void MouseWheel(int wheel, int direction, int x, int y)
	{
		Framework::ForwardMouseWheel(g_viewPole, wheel, direction, x, y);
		Framework::ForwardMouseWheel(g_objtPole, wheel, direction, x, y);
		glutPostRedisplay();
	}
}


GLuint g_projectionUniformBuffer = 0;

struct ProjectionBlock
{
	glm::mat4 cameraToClipMatrix;
};

Sun *newSun = new Sun();

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void Init()
{
	InitializePrograms();

	newSun->LoadMesh("UnitSphere.xml");
	newSun->AddSatellite("UnitSphere.xml", 1.5f, 1.0f, 5.0f);
	newSun->AddSatellite("UnitSphere.xml", 0.0f, 2.0f, 2.0f);
	newSun->AddSatellite("UnitSphere.xml", 1.0f, 2.5f, 10.0f);
	newSun->AddSatellite("UnitSphere.xml", 1.0f, 3.0f, 6.0f);

	try
	{
		g_pCylinderMesh = new Framework::Mesh("UnitSphere.xml");
		g_pPlaneMesh = new Framework::Mesh("LargePlane.xml");
		g_pCubeMesh = new Framework::Mesh("UnitCube.xml");
	}
	catch(std::exception &except)
	{
		printf("%s\n", except.what());
		throw;
	}

	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutMouseWheelFunc(MouseWheel);

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

static float g_fLightHeight = 1.5f;
static float g_fLightRadius = 5.0f;
using Framework::Timer;
Timer g_LightTimer(Timer::TT_LOOP, 5.0f);

glm::vec4 CalcLightPosition()
{
	float fCurrTimeThroughLoop = g_LightTimer.GetAlpha();

	glm::vec4 ret(0.0f, g_fLightHeight, 0.0f, 1.0f);

	ret.x = cosf(fCurrTimeThroughLoop * (3.14159f * 2.0f)) * g_fLightRadius;
	ret.z = sinf(fCurrTimeThroughLoop * (3.14159f * 2.0f)) * g_fLightRadius;

	return ret;
}

static int g_eLightModel = LM_BLINN_SPECULAR;

static bool g_bUseFragmentLighting = true;
static bool g_bDrawColoredCyl = false;
static bool g_bDrawLightSource = false;
static bool g_bScaleCyl = false;
static bool g_bDrawDark = false;

const float g_fLightAttenuation = 1.2f;

const glm::vec4 g_darkColor(0.2f, 0.2f, 0.2f, 1.0f);
const glm::vec4 g_lightColor(1.0f);

class MaterialParams
{
public:
	MaterialParams()
		: m_fPhongExponent(4.0f)
		, m_fBlinnExponent(4.0f)
	{}

	operator float() const
	{
		return GetSpecularValue();
	}

	void Increment(bool bIsLarge)
	{
		float &theParam = GetSpecularValue();

		if(bIsLarge)
			theParam += 0.5f;
		else
			theParam += 0.1f;

		ClampParam();
	}

	void Decrement(bool bIsLarge)
	{
		float &theParam = GetSpecularValue();

		if(bIsLarge)
			theParam -= 0.5f;
		else
			theParam -= 0.1f;

		ClampParam();
	}

private:
	float m_fPhongExponent;
	float m_fBlinnExponent;

	float &GetSpecularValue()
	{
		static float fStopComplaint = 0.0f;
		return fStopComplaint;
	}

	const float &GetSpecularValue() const
	{
		static float fStopComplaint = 0.0f;
		return fStopComplaint;
	}

	void ClampParam()
	{
		float &theParam = GetSpecularValue();

		if(theParam <= 0.0f)
			theParam = 0.0001f;
	}
};

static MaterialParams g_matParams;

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void Display()
{
	g_LightTimer.Update();

	glClearColor(0.0f, 0.0f, 0.24f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(g_pPlaneMesh && g_pCylinderMesh && g_pCubeMesh)
	{
		glutil::MatrixStack modelMatrix;
		modelMatrix.SetMatrix(g_viewPole.CalcMatrix());

		const glm::vec4 &worldLightPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);//CalcLightPosition();
		const glm::vec4 &lightPosCameraSpace = modelMatrix.Top() * worldLightPos;

		ProgramData &whiteProg = g_Programs[g_eLightModel].whiteProg;
		ProgramData &colorProg = g_Programs[g_eLightModel].colorProg;

		//mainSun->Render(modelMatrix, whiteProg, g_Unlit);
		newSun->Update();
		newSun->Render(modelMatrix, whiteProg, g_Unlit);
					
		glUseProgram(0);

		/*glUseProgram(whiteProg.theProgram);
		glUniform4f(whiteProg.lightIntensityUnif, 1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4f(whiteProg.ambientIntensityUnif, 0.2f, 0.2f, 0.2f, 1.0f);
		glUniform3fv(whiteProg.cameraSpaceLightPosUnif,1, glm::value_ptr(lightPosCameraSpace));
		glUniform1f(whiteProg.lightAttenuationUnif, g_fLightAttenuation);
		glUniform1f(whiteProg.shininessFactorUnif, g_matParams);
		glUniform4fv(whiteProg.baseDiffuseColorUnif, 1,
			g_bDrawDark ? glm::value_ptr(g_darkColor) : glm::value_ptr(g_lightColor));
		/*
		glUseProgram(colorProg.theProgram);
		glUniform4f(colorProg.lightIntensityUnif, 1.0f, 1.0f, 1.0f, 1.0f);
		glUniform4f(colorProg.ambientIntensityUnif, 0.2f, 0.2f, 0.2f, 1.0f);
		glUniform3fv(colorProg.cameraSpaceLightPosUnif, 1, glm::value_ptr(lightPosCameraSpace));
		glUniform1f(colorProg.lightAttenuationUnif, g_fLightAttenuation);
		glUniform1f(colorProg.shininessFactorUnif, g_matParams);
		glUseProgram(0);*/

		{
			glutil::PushStack push(modelMatrix);

			//Render the ground plane.
			/*{
				glutil::PushStack push(modelMatrix);

				glm::mat3 normMatrix(modelMatrix.Top());
				normMatrix = glm::transpose(glm::inverse(normMatrix));

				glUseProgram(whiteProg.theProgram);
				glUniformMatrix4fv(whiteProg.modelToCameraMatrixUnif, 1, GL_FALSE,
					glm::value_ptr(modelMatrix.Top()));

				glUniformMatrix3fv(whiteProg.normalModelToCameraMatrixUnif, 1, GL_FALSE,
					glm::value_ptr(normMatrix));
				g_pPlaneMesh->Render();
				glUseProgram(0);
			}*/

			//Render the Cylinder
			/*{
				glutil::PushStack push(modelMatrix);

				//modelMatrix.ApplyMatrix(g_objtPole.CalcMatrix());
				//modelMatrix.Translate(3.0f, 0.0f, 0.0f);
				//modelMatrix.RotateY(g_rotAngle_degs);
				glm::vec4 position = CalcLightPosition();
				modelMatrix.Translate(glm::vec3(position));

				if(g_bScaleCyl)
					modelMatrix.Scale(1.0f, 1.0f, 0.2f);

				glm::mat3 normMatrix(modelMatrix.Top());
				normMatrix = glm::transpose(glm::inverse(normMatrix));

				ProgramData &prog = g_bDrawColoredCyl ? colorProg : whiteProg;
				glUseProgram(prog.theProgram);
				glUniformMatrix4fv(prog.modelToCameraMatrixUnif, 1, GL_FALSE,
					glm::value_ptr(modelMatrix.Top()));

				glUniformMatrix3fv(prog.normalModelToCameraMatrixUnif, 1, GL_FALSE,
					glm::value_ptr(normMatrix));

				if(g_bDrawColoredCyl)
					g_pCylinderMesh->Render("lit-color");
				else
					g_pCylinderMesh->Render("lit");

				glUseProgram(0);
			}*/
		}
	}

	glutPostRedisplay();
	glutSwapBuffers();
}


//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void Reshape(int width, int height)
{
	glutil::MatrixStack persMatrix;
	float aspectRatio = width / (float)height;
	persMatrix.Perspective(45.0f, aspectRatio, g_fzNear, g_fzFar);

	ProjectionBlock projData;
	projData.cameraToClipMatrix = persMatrix.Top();

	glBindBuffer(GL_UNIFORM_BUFFER, g_projectionUniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ProjectionBlock), &projData);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	glutPostRedisplay();
}

static const char *strLightModelNames[] =
{
	"Phong Specular.",
	"Blinn Specular.",
	"Gaussian Specular.",
	"Beckmann Specular.",
	"Cook-Torrance with Beckmann Specular.",
	"Cook-Torrance with Gaussian Specular.",
};


//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
void Keyboard(unsigned char key, int x, int y)
{
	bool bChangedShininess = false;
	bool bChangedLightModel = false;
	switch (key)
	{
	case 27:
		delete g_pPlaneMesh;
		delete g_pCylinderMesh;
		delete g_pCubeMesh;
		glutLeaveMainLoop();
		return;

	case 32:
		g_bDrawColoredCyl = !g_bDrawColoredCyl;
		break;

	case 'i': g_fLightHeight += 0.2f; break;
	case 'k': g_fLightHeight -= 0.2f; break;
	case 'l': g_fLightRadius += 0.2f; break;
	case 'j': g_fLightRadius -= 0.2f; break;
	case 'I': g_fLightHeight += 0.05f; break;
	case 'K': g_fLightHeight -= 0.05f; break;
	case 'L': g_fLightRadius += 0.05f; break;
	case 'J': g_fLightRadius -= 0.05f; break;

	case 'o': g_matParams.Increment(true); bChangedShininess = true; break;
	case 'u': g_matParams.Decrement(true); bChangedShininess = true; break;
	case 'O': g_matParams.Increment(false); bChangedShininess = true; break;
	case 'U': g_matParams.Decrement(false); bChangedShininess = true; break;

	case 'y': g_bDrawLightSource = !g_bDrawLightSource; break;
	case 't': g_bScaleCyl = !g_bScaleCyl; break;
	case 'b': g_LightTimer.TogglePause(); break;
	case 'g': g_bDrawDark = !g_bDrawDark; break;
	case 'h':
		g_eLightModel += 1;
		g_eLightModel %= LM_MAX_LIGHTING_MODEL;
		bChangedLightModel = true;
		break;
	}

	if(g_fLightRadius < 0.2f)
		g_fLightRadius = 0.2f;

	if(bChangedShininess)
		printf("Shiny: %f\n", (float)g_matParams);

	if(bChangedLightModel)
		printf("%s\n", strLightModelNames[g_eLightModel]);

	glutPostRedisplay();
}


unsigned int Defaults(unsigned int displayMode, int &width, int &height) 
{ 
	return displayMode; 
}