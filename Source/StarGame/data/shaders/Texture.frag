#version 330

uniform sampler2D colorTexture;

out vec4 outputColor;

void main()
{
	outputColor = texture(colorTexture, colorCoord);
}