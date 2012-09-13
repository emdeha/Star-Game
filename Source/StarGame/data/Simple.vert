#version 330

layout (location = 0) in vec4 position;

smooth out vec4 theColor;

uniform vec4 color;
uniform mat4 projectionMatrix;

void main()
{
	gl_Position = projectionMatrix * position;
	theColor = color;
}