#version 330

layout (location = 0) in vec4 position;

smooth out vec4 theColor;

uniform vec4 color;

uniform mat4 modelToCameraMatrix;
uniform Projection
{
	mat4 cameraToClipMatrix;
};

void main()
{
	vec4 cameraPos = modelToCameraMatrix * position;
	gl_Position = cameraToClipMatrix * cameraPos;
	theColor = color;
}