#version 120

uniform sampler2D Texture;
uniform vec4 Color;

varying vec2 UVCoords;

void main()
{
	gl_FragColor = Color*vec4(1,1,1,texture2D(Texture, UVCoords).a);
}
