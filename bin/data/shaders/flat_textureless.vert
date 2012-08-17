#version 120

attribute vec2 VertexPosition;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP*vec4(VertexPosition,0,1);
}
