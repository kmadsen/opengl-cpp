#version 400

in vec3 EyeNormal;
in vec4 EyePosition;
in vec4 ProjTexCoord;

uniform sampler2D ProjectorTex;

struct MaterialInfo {
	vec3 Ambient;
	vec3 Diffuse;
};
uniform MaterialInfo Material;

struct LightInfo {
	vec3 Intensity;
	vec4 Position;
};
uniform LightInfo Light;

layout (location = 0) out vec4 FragColor;

void main() {
	vec3 s = normalize(vec3(Light.Position - EyePosition));
	vec3 n = normalize(EyeNormal);
	float sDotN = dot(s, n);
	vec3 diffuseShade = max(sDotN, 0.0) * Material.Diffuse;

	vec4 color = vec4(0.0);
	if (ProjTexCoord.z > 0) {
		color = textureProj(ProjectorTex, ProjTexCoord);
	}
	color +=  vec4(Light.Intensity*(Material.Ambient + diffuseShade), 1.0);
	FragColor = color;
}
