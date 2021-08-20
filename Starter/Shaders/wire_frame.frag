#version 400

uniform struct LineInfo {
	float Width;
	vec4 Color;
} Line;

uniform struct LightInfo {
	vec4 Position;
	float AmbientIntensity;
	float DiffuseIntensity;
	float SpecularIntensity;
} Light;

uniform struct MaterialInfo {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
} Material;

in vec3 GPosition;
in vec3 GNormal;
noperspective in vec3 GEdgeDistance;

layout(location = 0) out vec4 FragColor;

vec3 phongModel( vec3 pos, vec3 norm )
{
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(-pos.xyz);
    vec3 r = reflect( -s, norm );
    vec3 ambient = Light.AmbientIntensity * Material.Ambient;
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = Light.DiffuseIntensity * Material.Diffuse * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.SpecularIntensity * Material.Specular *
               pow( max( dot(r,v), 0.0 ), Material.Shininess );

    return ambient + diffuse + spec;
}

void main()
{
	// Shade the surface.
	vec4 color = vec4(phongModel(GPosition, GNormal), 1.0);

	// Get the smallest distance.
	float d = min(GEdgeDistance.x, GEdgeDistance.y);
	d = min(d, GEdgeDistance.z);

	float mixValue = smoothstep(Line.Width - 1, Line.Width + 1, d);

	FragColor = mix(Line.Color, color, mixValue);
}