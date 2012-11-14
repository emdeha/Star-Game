#version 330

layout(std140) uniform;

layout (location = 0) in vec4 position;

out vec2 texCoord;

uniform OrthographicProjection
{
	mat4 cameraToClipMatrix;
};

void main()
{
	gl_Position = cameraToClipMatrix * vec4(position.xy, 0.0, 1.0);
	texCoord =  position.zw;
}