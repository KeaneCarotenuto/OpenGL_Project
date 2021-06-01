#version 460 core

in vec3 FragColor;
uniform float CurrentTime;

out vec4 FinalColor;

#define PI 3.1415926538

void main() 
{
    FinalColor = vec4(FragColor.x * (sin(CurrentTime+(2*PI)/(3))+1)/(2), FragColor.y * (sin(CurrentTime+(4*PI)/(3))+1)/(2), FragColor.z * (sin(CurrentTime+(6*PI)/(3))+1)/(2), 1.0f) ;
}