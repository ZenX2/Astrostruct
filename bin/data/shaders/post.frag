#version 120

uniform sampler2D Texture;
uniform float Time;

varying vec2 UVCoords;

void main()
{
	gl_FragColor = texture2D(Texture, UVCoords);
}
