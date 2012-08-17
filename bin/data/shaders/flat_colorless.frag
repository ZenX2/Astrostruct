#version 120

uniform sampler2D Texture;

varying vec2 UVCoords;

void main()
{
	gl_FragColor = texture2D(Texture, UVCoords);
}
