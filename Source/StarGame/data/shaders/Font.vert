#version 330

layout (location = 0) in vec4 position;

out vec2 texCoord;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(position.xy, 0.0, 1.0);
	texCoord =  position.zw;
}