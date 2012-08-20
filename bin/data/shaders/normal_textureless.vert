#version 120

attribute vec3 VertexPosition;

uniform mat4 View;
uniform mat4 World3D;

void main()
{
	gl_Position = (World3D*View)*vec4(VertexPosition,1);
}
