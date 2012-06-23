#version 330

uniform sampler2D Texture;
uniform vec4 Color;

out vec4 FinalColor;
in vec2 UVCoords;

void main()
{
	FinalColor = Color*texture2D(Texture, UVCoords);
}
