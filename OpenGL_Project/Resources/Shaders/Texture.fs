#version 460 core

in vec3 FragColor;
in vec2 FragTexCoords;

uniform sampler2D ImageTexture;
uniform float offset;

out vec4 FinalColor;


void main() 
{
    FinalColor = texture(ImageTexture, vec2(FragTexCoords.x + offset, FragTexCoords.y));
}