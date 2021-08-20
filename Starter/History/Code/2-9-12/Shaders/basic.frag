#version 400

layout (location = 0) out vec4 FragColor;

/* Above is the alternative set for this - you have to also call
   glBindAttribLocation in the program if you use the method below. */

in vec3 Color;
//out vec4 FragColor;

void main() {
    FragColor = vec4(Color, 1.0);
}
