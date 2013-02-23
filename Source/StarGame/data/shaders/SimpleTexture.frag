#version 330

in vec2 colorCoord;

uniform sampler2D colorTexture;
uniform vec4 color;

out vec4 outputColor;

void main()
{
	outputColor = texture2D(colorTexture, colorCoord).rgba + color;
}