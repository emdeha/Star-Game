#version 330

in vec2 colorCoord;
in vec3 vertexNormal;
in vec3 cameraSpacePosition;

out vec4 outputColor;

layout(std140) uniform;

uniform sampler2D _sampler;
uniform vec4 lightIntensity;
uniform vec3 modelSpaceLightPos;
uniform vec3 cameraSpaceLightPos;

uniform Material
{
	vec4 diffuseColor;
	vec4 specularColor;
	float shininessFactor;
} Mtl;

uniform Light
{
	vec4 ambientIntensity;
	float lightAttenuation;
	float maxIntensity;
	float gamma;
} Lgt;


float CalcAttenuation(in vec3 cameraSpacePosition, out vec3 lightDirection)
{
	vec3 lightDifference = cameraSpaceLightPos - cameraSpacePosition;
	float lightDistanceSqr = dot(lightDifference, lightDifference);
	lightDirection = lightDifference * inversesqrt(lightDistanceSqr);

	return (1 / (1.0 + Lgt.lightAttenuation * sqrt(lightDistanceSqr)));
}

vec4 ComputeLighting()
{
	vec3 lightDirection;

	float atten = CalcAttenuation(cameraSpacePosition, lightDirection);
	vec4 attenIntensity = atten * lightIntensity;

	vec3 surfaceNormal = normalize(vertexNormal);
	float cosAngIncidence = dot(surfaceNormal, lightDirection);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	vec3 viewDirection = normalize(-cameraSpacePosition);

	vec3 halfAngle = normalize(lightDirection + viewDirection);
	float angleNormalHalf = acos(dot(halfAngle, surfaceNormal));
	float exponent = angleNormalHalf / Mtl.shininessFactor;
	exponent = -(exponent * exponent);
	float gaussianTerm = exp(exponent);

	gaussianTerm = cosAngIncidence != 0.0 ? gaussianTerm : 0.0;

	vec4 lighting = texture2D(_sampler, colorCoord) /*Mtl.diffuseColor*/ * attenIntensity * cosAngIncidence;
	lighting += Mtl.specularColor * attenIntensity * gaussianTerm;

	return lighting;
}

void main()
{
	vec4 lighting = texture2D(_sampler, colorCoord)/* Mtl.diffuseColor */* Lgt.ambientIntensity;
	lighting += ComputeLighting();
	
	lighting = lighting / Lgt.maxIntensity;
	vec4 gamma = vec4(1.0 / Lgt.gamma);
	gamma.w = 1.0;
	outputColor = pow(lighting, gamma);
}