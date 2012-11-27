#version 330

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 cameraToClipMatrix;
uniform mat4 modelToCameraMatrix;

uniform vec3 cameraPosition;

uniform float billboardSize;

out vec2 texCoord;

void main()
{
	vec3 _position = gl_in[0].gl_Position.xyz;
	vec3 positionToCameraVector = normalize(cameraPosition - _position);
	vec3 upVector = vec3(0.0, 1.0, 0.0);
	vec3 rightVector = cross(upVector, positionToCameraVector) * billboardSize;

	_position -= rightVector;
	gl_Position = (cameraToClipMatrix * (modelToCameraMatrix * vec4(_position, 1.0)));
	texCoord = vec2(0.0, 0.0);
	EmitVertex();

	_position.y += billboardSize;
	gl_Position = (cameraToClipMatrix * (modelToCameraMatrix * vec4(_position, 1.0)));
	texCoord = vec2(0.0, 1.0);
	EmitVertex();

	_position.y -= billboardSize;
	_position += rightVector;
	gl_Position = (cameraToClipMatrix * (modelToCameraMatrix * vec4(_position, 1.0)));
	texCoord = vec2(1.0, 0.0);
	EmitVertex();

	_position.y += billboardSize;
	gl_Position = (cameraToClipMatrix * (modelToCameraMatrix * vec4(_position, 1.0)));
	texCoord = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}