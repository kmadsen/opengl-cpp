#version 400

in vec3 LightDir;
in vec2 TexCoord;
in vec3 ViewDir;

uniform sampler2D ColorTex;
uniform sampler2D NormalTex;

struct LightInfo {
	vec4 Position;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};
uniform LightInfo Light;

struct MaterialInfo {
	vec3 Specular;
	float Shininess;
};
uniform MaterialInfo Material;

layout (location = 0) out vec4 FragColor;

vec3 diffuseShade(vec3 norm, vec3 diffColor) {
    vec3 ambient = Light.Ambient * diffColor;
    float sDotN = max(dot(LightDir,norm), 0.0);
    vec3 diffuse = Light.Diffuse * diffColor * sDotN;

	vec3 specular = vec3(0.0);
	if (sDotN > 0.0) {
		vec3 r = reflect(-LightDir, norm);
		float rDotV = dot(r,ViewDir);
		specular = Light.Specular * Material.Specular *
			pow(max(rDotV,0.0), Material.Shininess);
	}
    return ambient + diffuse + specular;
}

void main() {
	
	vec3 calculatedColor;
	vec3 color = vec3(texture(ColorTex, TexCoord));
	vec3 normal = vec3(texture(NormalTex, TexCoord));

	if (gl_FrontFacing) {
		calculatedColor = diffuseShade(normal, color);
	}
	else {
		calculatedColor = diffuseShade(-normal, color);
	}

	FragColor = vec4(calculatedColor, 1.0);
}
