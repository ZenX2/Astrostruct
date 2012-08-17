#version 120

attribute vec2 VertexPosition;
attribute vec2 UVPosition;

varying vec2 UVCoords;

void main()
{
	UVCoords = UVPosition;
	gl_Position = vec4(VertexPosition,0,1);
}
