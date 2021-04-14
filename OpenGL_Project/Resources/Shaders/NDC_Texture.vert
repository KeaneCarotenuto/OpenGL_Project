#version 460 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Col;
layout (location = 2) in vec2 TexCoords;

out vec3 FragColor;
out vec2 FragTexCoords;

void main() 
{
	gl_Position = vec4(Pos, 1.0);
	FragColor = Col;
	FragTexCoords = TexCoords;
}