#version 400

in vec3 ReflectDir;
in vec3 RefractDirR;
in vec3 RefractDirG;
in vec3 RefractDirB;


uniform samplerCube CubeMapTex;

uniform bool DrawSkyBox;

struct MaterialInfo {
	float Eta; // Ratio of indices of refration
	float ReflectionFactor;
};
uniform MaterialInfo Material;

layout (location = 0) out vec4 FragColor;

void main() {
	
	vec4 reflectColor = texture(CubeMapTex, ReflectDir);

	if (DrawSkyBox) {
		FragColor = reflectColor;
	}
	else {		
		vec4 refractColorR = texture(CubeMapTex, RefractDirR);
		vec4 refractColorG = texture(CubeMapTex, RefractDirG);
		vec4 refractColorB = texture(CubeMapTex, RefractDirB);
		vec4 refractColor = vec4(refractColorR.r, refractColorG.g, refractColorB.b, 1.0); 
		//FragColor = refractColor;
		FragColor = mix(refractColor, reflectColor, Material.ReflectionFactor);
	}
}
