#version 120

uniform vec4 Color;

varying vec4 FinalColor;

void main()
{
	gl_FragColor = Color;
}
