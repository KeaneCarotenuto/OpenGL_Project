#version 460 core

in vec3 FragColor;
in vec2 FragTexCoords;

uniform sampler2D ImageTexture;
uniform sampler2D ImageTexture1;
uniform float CurrentTime;

out vec4 FinalColor;

void main() 
{
    FinalColor = mix(texture(ImageTexture, FragTexCoords + vec2((sin(CurrentTime)+1)/10, (cos(CurrentTime)+1)/10)), texture(ImageTexture1, FragTexCoords + vec2((cos(CurrentTime)+1)/10, (sin(CurrentTime)+1)/10)), (sin(CurrentTime)+1)/2);
}