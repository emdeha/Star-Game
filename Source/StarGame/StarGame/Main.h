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


/// \defgroup module_StarGame Star Game
/// \ingroup module_StarGame
/// @{
/// \file Main.h
/// \brief Provides methods which are crucial to the program's functioning.
/// @}


#pragma warning(push, 1)
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <math.h>
#include <stdio.h>

#include "../glsdk/glload/gl_3_3.h"
#include "../glsdk/glutil/glutil.h"
#include "../glsdk/glload/gll.hpp"
#include "../glsdk/glutil/Shader.h"
#include "../glsdk/freeglut/include/GL/freeglut.h"

#include "../glsdk/glm/glm.hpp"
#include "../glsdk/glm/gtc/type_ptr.hpp"

#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/MousePole.h"
#include "../framework/Timer.h"
#include "../framework/Text.h"

#include "../ProgramData/ProgramData.h"
#include "../Audio/Audio.h"
#pragma warning(pop)


#ifdef LOAD_X11
#define APIENTRY
#endif


/// \fn Init
/// \brief Used for initialization.

/// Initializes shaders, game objects.
/// Enables OpenGL enumerators. Sets OpenGL enumerators.
/// Generates buffers.
void Init();

/// \fn Display
/// \brief The main loop of the game.

/// Renders and updates objects.
/// Handles mouse events.
void Display();

/// \fn Reshape
/// \brief Handles screen reshaping.

/// Calculates the new perspective matrix, applies it to all objects in the scene.
/// Calls glViewport to set the new viewport dimensions.
void Reshape(int width, int height);

/// \fn Keyboard
/// \brief Handles key press events.
void Keyboard(unsigned char key, int x, int y);


/// \fn HandleMouse
/// \brief Handles mouse events.
void HandleMouse();

/// \fn HandleMouseButtons
/// \brief Handles the pressing of the mouse buttons.
void HandleMouseButtons(int button, int state, int x, int y);

/// \fn HandleActiveMovement
/// \brief Handles the active movement of the mouse.
void HandleActiveMovement(int x, int y);

/// \fn HandlePassiveMovement
/// \brief Handles the passive movement of the mouse.
void HandlePassiveMovement(int x, int y);


unsigned int Defaults(unsigned int displayMode, int &width, int &height);

/// \fn DebugFunc
/// \brief Outputs debug information.
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

	int width = 1280;
	int height = 768;
	unsigned int displayMode = GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL;
	displayMode = Defaults(displayMode, width, height);

	glutInitDisplayMode(displayMode);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
#ifdef DEBUG
	glutInitContextFlags(GLUT_DEBUG);
#endif
	glutInitWindowSize(width, height); 
	glutInitWindowPosition(0, 0);
	int window = glutCreateWindow(argv[0]);

	glload::LoadFunctions();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	if(!glload::IsVersionGEQ(3, 3))
	{
		std::wstringstream messageBoxText;
		messageBoxText<<"Your OpenGL version is "<<glload::GetMajorVersion()<<", "<<glload::GetMinorVersion()
					  <<". You must have at least OpenGL 3.3 to run this game.\n\n"
					  <<"If your videocard is bought after year 2008, updating its drivers might fix this problem. "
					  <<"Else you\'ll have to upgrade your video.";
		MessageBox(NULL, messageBoxText.str().c_str(), L"OpenGL initialization error", MB_ICONERROR);
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
	glutMouseFunc(HandleMouseButtons);
	glutMotionFunc(HandleActiveMovement);
	glutPassiveMotionFunc(HandlePassiveMovement);
	glutMainLoop();
	return 0;
}
