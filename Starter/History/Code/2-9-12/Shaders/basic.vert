#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;

/* Above is the alternative set for this - you have to also call
   glBindAttribLocation in the program if you use the method below. */
//in vec3 VertexPosition;
//in vec3 VertexColor;

out vec3 Color;

uniform mat4 RotationMatrix;

void main()
{
    Color = VertexColor;

    gl_Position = RotationMatrix * vec4(VertexPosition,1.0);
}
