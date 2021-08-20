#version 400

// Input values
in vec3 Position;
in vec3 Normal;

// Uniform input
struct LightInfo {
	vec4 Position;
	vec3 Intensity;
};
uniform LightInfo Light;

struct MaterialInfo {
	vec3 Ambient;
	vec3 Diffuse;
};
uniform MaterialInfo Material;

// Constants
const int levels = 4;
const float scaleFactor = 1.0 / levels;

// Output
layout (location = 0) out vec4 FragColor;

void main() {
	vec3 s = normalize(Light.Position.xyz - Position);
	float cosine = max(dot(s,Normal), 0.0);
	vec3 diffuse = Material.Diffuse*ceil(cosine*levels)*scaleFactor;

	vec3 outColor = Light.Intensity*(Material.Ambient + diffuse);
	FragColor = vec4(outColor, 1.0);
}
