#version 330

uniform sampler2D Texture;
uniform vec4 Color;

out vec4 FinalColor;
in vec2 UVCoords;

void main()
{
	FinalColor = Color*vec4(1,1,1,texture2D(Texture, UVCoords).a);
}
