#version 330

in vec2 colorCoord;

uniform sampler2D _sampler;
uniform vec4 color;

out vec4 outputColor;

void main()
{
	outputColor = texture2D(_sampler, colorCoord) * color;
}