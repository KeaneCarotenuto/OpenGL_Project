#version 460 core

in vec3 FragColor;
in vec2 FragTexCoords;

uniform sampler2D ImageTexture;
uniform sampler2D ImageTexture1;
uniform sampler2D currentTime;

out vec4 FinalColor;

void main() 
{
	FinalColor = mix(texture(ImageTexture,  FragTexCoords), texture(ImageTexture1, FragTexCoords), 0.5f);
}