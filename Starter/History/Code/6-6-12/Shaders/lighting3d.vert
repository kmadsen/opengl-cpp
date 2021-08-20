#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 FrontColor;
out vec3 BackColor;
out vec2 TexCoord;

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

void main()
{
	vec3 normal = normalize(NormalMatrix * VertexNormal);
	vec4 eyePosition = ModelView * vec4(VertexPosition,1.0);
	vec3 s = normalize(vec3(Light.Position - eyePosition));
    vec3 v = normalize(-eyePosition.xyz);

	vec3 ambient = Light.Ambient*Material.Ambient;
	
	FrontColor = diffuseSpecular(s, v, normal);
	BackColor = diffuseSpecular(s, v, -normal);
	TexCoord = VertexTexCoord;
	gl_Position = MVP * vec4(VertexPosition,1.0);
}
