#version 400

in vec3 TexCoord;
layout (location = 0) out vec4 FragColor;

// This will have 48 bytes. Think, that's different than what a CPU struct would be like.
// There are 10 float values - 40 bytes. The GPU alignes to 4 floats (a vector of course!).
layout(row_major, shared) uniform BlobSettings {
  vec4 InnerColor;
  vec4 OuterColor;
  float RadiusInner;
  float RadiusOuter;
};

void main() {
    float dx = TexCoord.x - 0.5;
    float dy = TexCoord.y - 0.5;
    float dist = sqrt(dx * dx + dy * dy);
    FragColor =
       mix(InnerColor, OuterColor, 
	       smoothstep(RadiusInner, RadiusOuter, dist ));
}
