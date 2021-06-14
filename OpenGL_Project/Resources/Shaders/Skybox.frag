#version 460 core

in vec3 FragTexCoords;

uniform samplerCube ImageTexture;

out vec4 FinalColor;
 

void main() 
{
    FinalColor = texture(ImageTexture, FragTexCoords);
}