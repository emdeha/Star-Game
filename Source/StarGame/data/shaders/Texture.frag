#version 330

in vec2 finalTexCoord;

out vec4 outputColor;

uniform vec4 color;

uniform sampler2D _sampler;

void main()
{
	outputColor = texture2D(_sampler, finalTexCoord).rgba * color.rgba;
}