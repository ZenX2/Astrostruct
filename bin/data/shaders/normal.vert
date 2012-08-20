#version 120

attribute vec3 VertexPosition;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 World3D;
varying float FogDensity;

void main()
{
	vec4 TranslatedVert = (World3D*View*Model)*vec4(VertexPosition,1);
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
