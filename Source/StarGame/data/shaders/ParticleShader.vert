#version 330

layout(std140) uniform;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 colorCoord;

uniform Projection
{
	mat4 cameraToClipMatrix;
};
uniform mat4 modelToCameraMatrix;

uniform vec3 deltaPosition;

void main()
{
	vec3 updatedPosition = position + deltaPosition;
	gl_Position = cameraToClipMatrix * (modelToCameraMatrix * vec4(updatedPosition, 1.0));
	colorCoord = texCoord;
}