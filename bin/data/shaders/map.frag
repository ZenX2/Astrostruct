#version 120

uniform sampler2D Texture;
uniform vec4 Color;

varying vec2 UVCoords;
varying float FogDensity;

void main()
{
	vec4 FogColor = vec4(0.0,0.0,0.0,1.0)*FogDensity;
	float AntiFogDensity = 1.0-FogDensity;
	vec4 FragColor = Color*texture2D(Texture, UVCoords);
	gl_FragColor = FogColor+(FragColor*AntiFogDensity);
}
