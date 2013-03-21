#version 330

uniform sampler2D _sampler;

uniform vec4 color;

in vec2 texCoord;
out vec4 outputColor;

void main()
{
	outputColor = texture2D(_sampler, texCoord) * color;//vec4(0.5, 0.4, 0.8, 1.0);

	// Making fake transparency
	//if(outputColor.a <= 0.1)
	//{
	//	discard;
	//}
}