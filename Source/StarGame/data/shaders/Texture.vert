#version 330

layout(std140) uniform;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;


uniform OrthographicProjection
{
	mat4 cameraToClipMatrix;
};
uniform mat4 modelToCameraMatrix;


out vec2 finalTexCoord;

void main()
{
	vec4 cameraPos = modelToCameraMatrix * position;
	gl_Position = cameraToClipMatrix * cameraPos;
	finalTexCoord = texCoord;
}