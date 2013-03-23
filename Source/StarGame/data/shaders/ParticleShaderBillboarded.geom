#version 330

layout (points) in;
layout (points) out;
layout (max_vertices = 30) out; // Why 30?


// Why arrays?
in float typeToGS[];
in vec3 positionToGS[];
in vec3 velocityToGS[];
in float ageToGS[];


out float outType;
out vec3 outPosition;
out vec3 outVelocity;
out float outAge;


uniform float deltaTime_milliseconds;
uniform float time;
uniform float launcherLifetime;
uniform float shellLifetime;
uniform float secondaryShellLifetime;
uniform sampler1D randomDirections;


#define PARTICLE_TYPE_LAUNCHER 0.0f
#define PARTICLE_TYPE_SHELL 1.0f
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f


vec3 GetRandomDirection(float texCoord)
{
	vec3 randomDirection = texture(randomDirections, texCoord).xyz;
	randomDirection -= vec3(0.5f, 0.5f, 0.5f); // Why 0.5?
	return randomDirection;
}

void main()
{
	float age = ageToGS[0] + deltaTime_milliseconds;

	if(typeToGS[0] == PARTICLE_TYPE_LAUNCHER)
	{
		if(age >= launcherLifetime)
		{
			outType = PARTICLE_TYPE_SHELL;
			outPosition = positionToGS[0];
			vec3 randomDirection = GetRandomDirection(time / 1000.0f); // Is '/ 1000' for conversion to millisecs?
			randomDirection.y = max(randomDirection.y, 0.5f); // '0.5' ?
			outVelocity = normalize(randomDirection) / 20.0f; // div by 20.0 ?
			outAge = 0.0f;
			EmitVertex();
			EndPrimitive();
			age = 0.0f;
		}

		outType = PARTICLE_TYPE_LAUNCHER;
		outPosition = positionToGS[0];
		outVelocity = velocityToGS[0];
		outAge = age;
		EmitVertex();
		EndPrimitive();
	}
	else
	{
		float deltaTime_seconds = deltaTime_milliseconds / 1000.0f;
		float timeOne = ageToGS[0] / 1000.0f;
		float timeTwo = age / 1000.0f;
		vec3 deltaPosition = deltaTime_seconds * velocityToGS[0]; // ?
		vec3 deltaVelocity = vec3(deltaTime_seconds) * (0.0f, -9.81f, 0.0f); // -9.81 ?

		if(typeToGS[0] == PARTICLE_TYPE_SHELL)
		{
			if(age < shellLifetime)
			{
				outType = PARTICLE_TYPE_SHELL;
				outPosition = positionToGS[0] + deltaPosition;
				outVelocity = velocityToGS[0] + deltaVelocity;
				outAge = age;
				EmitVertex();
				EndPrimitive();
			}
			else
			{
				for(int i = 0; i < 10; i++)
				{
					outType = PARTICLE_TYPE_SECONDARY_SHELL;
					outPosition = positionToGS[0];
					vec3 randomDirection = GetRandomDirection((time + i) / 1000.0f); // ?
					outVelocity = normalize(randomDirection) / 20.0f;
					outAge = 0.0f;
					EmitVertex();
					EndPrimitive();
				}
			}
		}
		else
		{
			if(age < secondaryShellLifetime)
			{
				outType = PARTICLE_TYPE_SECONDARY_SHELL;
				outPosition = positionToGS[0] + deltaPosition;
				outVelocity = velocityToGS[0] + deltaVelocity;
				outAge = age;
				EmitVertex();
				EndPrimitive();
			}
		}
	}
}