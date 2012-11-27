#version 330

layout (location = 0) in vec3 position;

uniform vec3 deltaPosition;

void main()
{
	gl_Position = vec4(position, 1.0) + vec4(deltaPosition, 0.0);
}