#version 460 core

in vec3 FragColor;
in vec2 FragTexCoords;

uniform sampler2D ImageTexture;
uniform float offset;
uniform int frameCount;

out vec4 FinalColor;
 

void main() 
{
    float frameCountCopy = frameCount;
    if (frameCountCopy <= 0) frameCountCopy = 1;

    FinalColor = texture(ImageTexture, vec2(FragTexCoords.x/frameCountCopy + offset, FragTexCoords.y));
}