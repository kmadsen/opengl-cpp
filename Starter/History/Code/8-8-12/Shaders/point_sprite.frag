#version 400

in vec2 TexCoord; // From the geometry shader

uniform sampler2D SpriteTex;

layout (location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture(SpriteTex, TexCoord);
}