#version 330

layout (location = 0) in vec4 position;

smooth out vec4 theColor;

uniform vec4 color;

void main()
{
	gl_Position = position;
	theColor = color;
}