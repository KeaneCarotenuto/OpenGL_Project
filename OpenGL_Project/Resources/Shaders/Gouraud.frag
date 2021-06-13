#version 460 core

in vec3 FragColor;
in vec2 FragTexCoords;
in vec4 Light;

uniform sampler2D ImageTexture;

out vec4 FinalColor;
 

void main() 
{
    FinalColor = Light * texture(ImageTexture, FragTexCoords);
}