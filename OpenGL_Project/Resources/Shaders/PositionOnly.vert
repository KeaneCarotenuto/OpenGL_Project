#version 460 core

layout (location = 0) in vec3 Pos;

uniform mat4 PVMMat;

void main() 
{
	gl_Position = PVMMat * vec4(Pos, 1.0);
}