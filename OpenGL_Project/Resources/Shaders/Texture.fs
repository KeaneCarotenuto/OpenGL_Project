#version 460 core

in vec3 FragColor;
in vec2 FragTexCoords;

uniform sampler2D ImageTexture;

out vec4 FinalColor;

void main() 
{
	FinalColor = texture(ImageTexture, FragTexCoords);
}