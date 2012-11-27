#version 330

uniform sampler2D _sampler;

in vec2 texCoord;
out vec4 outputColor;

void main()
{
	outputColor = texture2D(_sampler, texCoord) * vec4(0.5, 0.4, 0.8, 1.0);

	// Making fake transparency
	//if(outputColor.r >= 0.9 && outputColor.g >= 0.9 && outputColor.b >= 0.9)
	//{
	//	discard;
	//}
}