#version 120

attribute vec2 VertexPosition;
attribute vec2 UVPosition;

uniform mat4 MVP;

varying vec2 UVCoords;

void main()
{
	UVCoords = UVPosition;
	gl_Position = MVP*vec4(VertexPosition,0,1);
}
