#version 330

uniform sampler2D Texture;
uniform vec4 Color;

out vec4 FinalColor;
in vec2 UVCoords;
in float FogDensity;

void main()
{
	vec4 FogColor = vec4(0.0,0.0,0.0,1.0)*FogDensity;
	float AntiFogDensity = 1.0-FogDensity;
	vec4 FragColor = Color*texture2D(Texture, UVCoords);
	FinalColor = FogColor+(FragColor*AntiFogDensity);
}
