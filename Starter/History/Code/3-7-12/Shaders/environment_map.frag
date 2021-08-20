#version 400

in vec3 ReflectDir;
in vec3 RefractDir;

uniform samplerCube CubeMapTex;

uniform bool DrawSkyBox;

struct MaterialInfo {
	float Eta; // Ratio of indices of refration
	float ReflectionFactor;
};
uniform MaterialInfo Material;

layout (location = 0) out vec4 FragColor;

void main() {
	
	vec4 reflectColor = texture(CubeMapTex, ReflectDir);
	vec4 refractColor = texture(CubeMapTex, RefractDir);

	if (DrawSkyBox) {
		FragColor = reflectColor;
	}
	else {
		FragColor = mix(refractColor, reflectColor, Material.ReflectionFactor);
	}
}
