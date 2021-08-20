#version 400

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Tex1;

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

layout (location = 0) out vec4 FragColor;

void diffuseShade(vec3 pos, vec3 norm, out vec3 ambAndDiff) {
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(-pos.xyz);
    vec3 ambient = Light.Intensity * Material.Ambient;
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = Light.Intensity * Material.Diffuse * sDotN;
    ambAndDiff = ambient + diffuse;
}

void main() {
	
	vec3 color;

	if (gl_FrontFacing) {
		diffuseShade(Position, Normal, color);
	}
	else {
		diffuseShade(Position, -Normal, color);
	}

	FragColor = vec4(color,1.0) * texture(Tex1, TexCoord);
}
