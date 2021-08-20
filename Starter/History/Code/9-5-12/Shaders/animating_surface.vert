#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Normal;
out vec4 EyePosition;

uniform mat3 NormalMatrix;
uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat4 MVP;

uniform float Time;
uniform float Amplitude;
uniform float WaveLength;
uniform float Velocity;

#define TWOPI 6.2831853071795862

void main()
{
	EyePosition = ModelView * vec4(VertexPosition,1.0);

	gl_Position = MVP * vec4(VertexPosition,1.0);

	float posY = Amplitude * sin(TWOPI/WaveLength * (VertexPosition.z - Velocity*Time));
	float normZ = -Amplitude * TWOPI/WaveLength * 
		cos(TWOPI/WaveLength * (VertexPosition.z - Velocity*Time));

	vec3 waveNorm = VertexNormal;
	waveNorm.z = normZ;


	Normal = normalize(NormalMatrix * waveNorm);

	gl_Position.y += posY;
}
