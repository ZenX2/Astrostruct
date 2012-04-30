#version 330 core

uniform sampler2D Texture;

out vec4 FinalColor;
in vec2 UVCoords;

void main()
{
	FinalColor = vec4(1,1,1,texture2D(Texture, UVCoords).a);
}
