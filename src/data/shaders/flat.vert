#version 330 core

layout(location = 0) in vec2 VertexPosition;

uniform mat4 ModelView;

void main()
{
	vec4 TransformVertex = vec4(VertexPosition,0,1);
	gl_Position = TransformVertex*ModelView;
}
