#version 330

layout(std140) uniform;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 colorCoord;

uniform Projection
{
	mat4 cameraToClipMatrix;
};
uniform mat4 modelToCameraMatrix;

void main()
{
	gl_Position = cameraToClipMatrix * (modelToCameraMatrix * vec4(position, 1.0));
	colorCoord = texCoord;
}