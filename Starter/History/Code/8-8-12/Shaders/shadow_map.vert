#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 EyeNormal;
out vec3 EyePosition;

// Coordinate to be used for shadow map lookup
out vec4 ShadowCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
uniform mat4 ShadowMatrix;

void main()
{
    EyePosition = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;
    EyeNormal = normalize( NormalMatrix * VertexNormal );

	// ShadowMatrix converts from modeling coordinates to shadow map coordinates
	ShadowCoord = ShadowMatrix * vec4(VertexPosition, 1.0);

    gl_Position = MVP * vec4(VertexPosition,1.0);
}
