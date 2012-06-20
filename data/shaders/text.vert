#version 330

layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec2 UVPosition;

uniform mat4 MVP;

out vec2 UVCoords;

void main()
{
	UVCoords = UVPosition;
	gl_Position = MVP*vec4(VertexPosition,0,1);
}
