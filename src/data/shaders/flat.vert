#version 330 core

layout(location = 0) in vec2 VertexPosition;
layout(location = 1) in vec2 UVPosition;

uniform mat4 ModelView;
uniform vec2 Screen;

out vec2 UVCoords;

void main()
{
	UVCoords = UVPosition;
	vec4 TransformVertex = ModelView*vec4(VertexPosition/(Screen/2.f),0.f,1.f);
	vec4 TranslateVertex = vec4(TransformVertex.x-1.f,TransformVertex.y-1.f,TransformVertex.z,TransformVertex.w);
	gl_Position = TranslateVertex;
}
