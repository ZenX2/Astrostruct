#version 120

attribute vec2 VertexPosition;
attribute vec2 UVPosition;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 World3D;

varying vec2 UVCoords;

void main()
{
    UVCoords = UVPosition;
	gl_Position = (World3D*View*Model)*vec4(VertexPosition,0,1);
}