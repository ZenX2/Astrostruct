#version 120

uniform vec4 Color;

varying float FogDensity;

void main()
{
	vec4 FogColor = vec4(0.0,0.0,0.0,1.0)*FogDensity;
	float AntiFogDensity = 1.0-FogDensity;
	gl_FragColor = FogColor+(Color*AntiFogDensity);
}
