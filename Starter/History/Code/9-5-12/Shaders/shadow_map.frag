#version 400

in vec3 EyeNormal;
in vec3 EyePosition;
in vec4 ShadowCoord;

uniform struct MaterialInfo {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
} Material;

uniform struct LightInfo {
	vec3 Intensity;
	vec4 Position;
} Light; 

uniform sampler2DShadow ShadowMap;

layout (location = 0) out vec4 FragColor;

vec3 phongModelDiffAndSpec()
{
    vec3 n = EyeNormal;
    if( !gl_FrontFacing ) n = -n;
    vec3 s = normalize(vec3(Light.Position) - EyePosition);
    vec3 v = normalize(-EyePosition.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = Light.Intensity * Material.Diffuse * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Specular *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );

    return diffuse + spec;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
    float shadow = textureProj(ShadowMap, ShadowCoord);

	FragColor = vec4(phongModelDiffAndSpec() * shadow + Material.Ambient, 1.0);
}


subroutine (RenderPassType)
void recordDepth()
{
	// Don't need to do anything
}

void main()
{
	RenderPass();
}
