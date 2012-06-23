#version 330

uniform sampler2D Texture;

out vec4 FinalColor;
in vec2 UVCoords;

void main()
{
	FinalColor = texture2D(Texture, UVCoords);
}
