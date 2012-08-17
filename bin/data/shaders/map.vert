#version 120

attribute vec3 VertexPosition;
attribute vec2 UVPosition;

uniform mat4 MVP;

varying vec2 UVCoords;
varying float FogDensity;

void main()
{
	vec4 TranslatedVert = MVP*vec4(VertexPosition,1);
	float FogStart = 400;
	float FogEnd = 600;
	float Distance = TranslatedVert.z;
	if (Distance<=FogStart)
	{
		FogDensity = 0;
	} else if (Distance>FogEnd)
	{
		FogDensity = 1;
	} else {
		FogDensity = (Distance-FogStart)/(FogEnd-FogStart);
	}
	UVCoords = UVPosition;
	gl_Position = TranslatedVert;
}
