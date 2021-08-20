#version 400

layout (location = 0) out vec4 FragColor;

in vec3 Normal;
in vec4 EyePosition;

struct LightInfo {
	vec4 Position;
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
};
uniform LightInfo Light;

struct MaterialInfo {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};
uniform MaterialInfo Material;

vec3 diffuseSpecular(vec3 s, vec3 v, vec3 n)
{
	vec3 r = reflect(-s, n);  
	
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	float sDotN = dot(s, n);
	if (sDotN > 0.0) {
		diffuse = Light.Diffuse*Material.Diffuse*sDotN;
		specular = Light.Specular*Material.Specular*pow(max(dot(r, v), 0.0), Material.Shininess); 
	}
	return diffuse + specular;

}

void main() {
	vec3 s = normalize(vec3(Light.Position - EyePosition));
    vec3 v = normalize(-EyePosition.xyz);

	// Just making a dumb effect
	vec3 worldCoord = vec3(normalize(gl_FragCoord));
	vec3 ambient = vec3(worldCoord+Material.Ambient);

	ambient = Light.Ambient*ambient;
	FragColor = vec4(diffuseSpecular(s, v, Normal) + ambient, 1.0);
}
