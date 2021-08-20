#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 ReflectDir;
out vec3 RefractDirR;
out vec3 RefractDirG;
out vec3 RefractDirB;

struct MaterialInfo {
	float Eta; // Ratio of indices of refration
	float ReflectionFactor;
};
uniform MaterialInfo Material;

uniform bool DrawSkyBox;
uniform vec3 WorldCameraPosition;

uniform mat3 NormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	if (DrawSkyBox) {
		ReflectDir = VertexPosition;
	}
	else {
		// Calculate the reflection direction in world coordinates
		vec3 worldPosition = vec3(ModelMatrix * vec4(VertexPosition, 1.0));
		vec3 worldNormal =	vec3(ModelMatrix * vec4(VertexNormal, 1.0));
		vec3 worldView = normalize(WorldCameraPosition - worldPosition);
		ReflectDir = reflect(-worldView, worldNormal);
		RefractDirR = refract(-worldView, worldNormal, Material.Eta);
		RefractDirG = refract(-worldView, worldNormal, Material.Eta + 0.002);
		RefractDirB = refract(-worldView, worldNormal, Material.Eta + 0.004);
	}
	gl_Position = MVP * vec4(VertexPosition,1.0);
}
