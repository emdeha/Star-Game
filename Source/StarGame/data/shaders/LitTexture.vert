#version 330

layout(std140) uniform;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoord;
layout(location = 2) in vec3 normal;

out vec2 colorCoord;
out vec3 vertexNormal;
out vec3 cameraSpacePosition;

uniform Projection
{
	mat4 cameraToClipMatrix;
};
uniform mat4 modelToCameraMatrix;
uniform mat3 normalModelToCameraMatrix;

void main()
{
	vec4 cameraPosition = (modelToCameraMatrix * vec4(position, 1.0));
	gl_Position = cameraToClipMatrix * cameraPosition;

	vertexNormal = normalize(normalModelToCameraMatrix * normal);
	cameraSpacePosition = vec3(cameraPosition);
}