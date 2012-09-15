#version 330

layout (location = 0) in vec4 position;
//layout (location = 1) in vec4 color;

smooth out vec4 theColor;

uniform vec4 color;

uniform mat4 modelToCameraMatrix;
uniform Projection
{
	mat4 cameraToClipMatrix;
};

void main()
{	
	vec4 tempPos = modelToCameraMatrix * position;
	gl_Position = cameraToClipMatrix * tempPos;
	theColor = color;
}