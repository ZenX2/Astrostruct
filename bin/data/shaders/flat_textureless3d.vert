#version 120

attribute vec2 VertexPosition;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 World3D;

void main()
{
	gl_Position = (World3D*View*Model)*vec4(VertexPosition,0,1);
}