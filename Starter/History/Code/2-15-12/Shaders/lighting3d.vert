#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 LightIntensity;

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

uniform mat3 NormalMatrix;
uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat4 MVP;

void main()
{
	vec3 normal = normalize(NormalMatrix * VertexNormal);
	vec4 eyePosition = ModelView * vec4(VertexPosition,1.0);
	vec3 s = normalize(vec3(Light.Position - eyePosition));
    vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, normal);  
	
	vec3 ambient = Light.Ambient*Material.Ambient;
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	float sDotN = dot(s, normal);
	if (sDotN > 0.0) {
		diffuse = Light.Diffuse*Material.Diffuse*sDotN;
		specular = Light.Specular*Material.Specular*pow(max(dot(r, v), 0.0), Material.Shininess); 
	}
	LightIntensity = ambient + diffuse + specular;
	
	gl_Position = MVP * vec4(VertexPosition,1.0);
}
