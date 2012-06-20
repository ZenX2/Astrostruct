#version 330

layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec2 UVPosition;

out vec2 UVCoords;

void main()
{
	UVCoords = UVPosition;
	gl_Position = vec4(VertexPosition,0,1);
}
