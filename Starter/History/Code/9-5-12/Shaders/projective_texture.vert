#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 EyeNormal;
out vec4 EyePosition;
out vec4 ProjTexCoord;

uniform mat4 ProjectorMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	vec4 pos = vec4(VertexPosition, 1.0);

	EyeNormal = normalize(NormalMatrix * VertexNormal);
	EyePosition = ModelViewMatrix * pos;
	ProjTexCoord = ProjectorMatrix * (ModelMatrix * pos);

	gl_Position = MVP * pos;
}
