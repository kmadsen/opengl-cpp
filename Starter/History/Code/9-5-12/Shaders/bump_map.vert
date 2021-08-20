#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

struct LightInfo {
	vec4 Position;
};
uniform LightInfo Light;

out vec3 LightDir;
out vec2 TexCoord;
out vec3 ViewDir;

uniform mat4 ModelView;
uniform mat3 NormalMatrix;
uniform mat4 Projection;
uniform mat4 MVP;

void main()
{
	// Compute the 3 basis vectors for the model space
	vec3 norm = normalize(NormalMatrix * VertexNormal);
	vec3 tang = normalize(NormalMatrix * vec3(VertexTangent));
	vec3 binormal = cross(norm,tang) * VertexTangent.w;

	// Put into a matrix for transformation
	mat3 toModelLocal = mat3(
		tang.x, binormal.x, norm.x,
		tang.y, binormal.y, norm.y,
		tang.z, binormal.z, norm.z);

	// Get the position in eye coordinates
	vec3 pos = vec3(ModelView * vec4(VertexPosition, 1.0));
	
	// Transform light and view directions to tangent space
	LightDir = normalize(toModelLocal * (Light.Position.xyz - pos));
	ViewDir = toModelLocal * normalize(-pos);

	// Pass the tex coordinates along
	TexCoord = VertexTexCoord;

	gl_Position = MVP * vec4(VertexPosition,1.0);
}
