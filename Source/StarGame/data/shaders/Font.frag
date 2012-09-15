#version 330

in vec2 texCoord;

uniform sampler2D fontTexture;
uniform vec4 fontColor;

out vec4 outputColor;

void main()
{
	outputColor = vec4(1.0, 1.0, 1.0, 
	texture(fontTexture, texCoord).r) * fontColor;
}