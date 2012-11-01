#version 330

layout (location = 0) in vec4 position;

smooth out vec4 theColor;

uniform vec4 color;

uniform mat4 modelToCameraMatrix;
uniform mat4 cameraToClipMatrix;

void main()
{
	gl_Position = cameraToClipMatrix * (modelToCameraMatrix * vec4(position.xy, 0.0, 1.0));
	theColor = color;
}