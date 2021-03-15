#version 460 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Col;

out vec3 FragColor;

void main() 
{
	gl_Position = vec4(Pos, 1.0);
	FragColor = Col;
}