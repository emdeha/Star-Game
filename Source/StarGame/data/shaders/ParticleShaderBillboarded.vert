#version 330

layout (location = 0) in float type;
layout (location = 1) in vec3 position;
layout (location = 2) in vec3 velocity;
layout (location = 3) in float age;


out float typeToGS;
out vec3 positionToGS;
out vec3 velocityToGS;
out float ageToGS;


void main()
{
	typeToGS = type;
	positionToGS = position;
	velocityToGS = velocity;
	ageToGS = age;
}