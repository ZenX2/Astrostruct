#version 120

attribute vec3 VertexPosition;

uniform mat4 MVP;
varying float FogDensity;

void main()
{
	vec4 TranslatedVert = MVP*vec4(VertexPosition,1);
	float FogStart = 500;
	float FogEnd = 628;
	float Distance = TranslatedVert.z;
	if (Distance<=FogStart)
	{
		FogDensity = 0;
	} else if (Distance>=FogEnd)
	{
		FogDensity = 1;
	} else {
		FogDensity = (Distance-FogStart)/(FogEnd-FogStart);
	}
	gl_Position = TranslatedVert;
}
