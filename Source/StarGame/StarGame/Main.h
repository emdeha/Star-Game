#include <string>
#include <vector>
#include <stack>
#include <math.h>
#include <stdio.h>
#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <glload/gll.hpp>
#include <glutil/Shader.h>
#include <GL/freeglut.h>
#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/MousePole.h"
#include "../framework/Timer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ProgramData.h"

#ifdef LOAD_X11
#define APIENTRY
#endif

void Init();
void Display();
void Reshape(int width, int height);
void Keyboard(unsigned char key, int x, int y);

unsigned int Defaults(unsigned int displayMode, int &width, int &height);

void APIENTRY DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
			   const GLchar* message, GLvoid* userParam)
{
	std::string srcName;
	switch(source)
	{
	case GL_DEBUG_SOURCE_API_ARB: srcName = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: srcName = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: srcName = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: srcName = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB: srcName = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER_ARB: srcName = "Other"; break;
	}

	std::string errorType;
	switch(type)
	{
	case GL_DEBUG_TYPE_ERROR_ARB: errorType = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: errorType = "Deprecated Functionality"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: errorType = "Undefined Behavior"; break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB: errorType = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB: errorType = "Performance"; break;
	case GL_DEBUG_TYPE_OTHER_ARB: errorType = "Other"; break;
	}

	std::string typeSeverity;
	switch(severity)
	{
	case GL_DEBUG_SEVERITY_HIGH_ARB: typeSeverity = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB: typeSeverity = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW_ARB: typeSeverity = "Low"; break;
	}

	printf("%s from %s,\t%s priority\nMessage: %s\n",
		errorType.c_str(), srcName.c_str(), typeSeverity.c_str(), message);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	int width = 500;
	int height = 500;
	unsigned int displayMode = GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL;
	displayMode = Defaults(displayMode, width, height);

	glutInitDisplayMode (displayMode);
	glutInitContextVersion (3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
#ifdef DEBUG
	glutInitContextFlags(GLUT_DEBUG);
#endif
	glutInitWindowSize (width, height); 
	glutInitWindowPosition (300, 200);
	int window = glutCreateWindow (argv[0]);

	glload::LoadFunctions();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	if(!glload::IsVersionGEQ(3, 3))
	{
		printf("Your OpenGL version is %i, %i. You must have at least OpenGL 3.3 to run this tutorial.\n",
			glload::GetMajorVersion(), glload::GetMinorVersion());
		glutDestroyWindow(window);
		return 0;
	}

	if(glext_ARB_debug_output)
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageCallbackARB(DebugFunc, (void*)15);
	}

	Init();

	glutDisplayFunc(Display); 
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
	return 0;
}