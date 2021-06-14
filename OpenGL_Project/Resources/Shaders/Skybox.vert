#version 460 core

layout (location = 0) in vec3 Pos;

uniform mat4 PVMMat;

out vec3 FragTexCoords;

void main() 
{
	gl_Position = PVMMat * vec4(Pos, 1.0f);
	FragTexCoords = Pos;
}