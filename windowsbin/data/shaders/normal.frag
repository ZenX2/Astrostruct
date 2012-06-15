#version 330

uniform vec4 Color;

out vec4 FinalColor;
in float FogDensity;

void main()
{
	vec4 FogColor = vec4(0.0,0.0,0.0,1.0)*FogDensity;
	float AntiFogDensity = 1.0-FogDensity;
	FinalColor = FogColor+(Color*AntiFogDensity);
}
