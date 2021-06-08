#version 460 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;

uniform mat4 PVMMat;
uniform mat4 Model;

out vec2 FragTexCoords;
out vec3 FragNormal;
out vec3 FragPos;

void main() 
{
	gl_Position = PVMMat * vec4(Pos, 1.0);

	FragTexCoords = TexCoords;
	FragNormal = mat3(transpose(inverse(Model))) * Normal;
	FragPos = vec3(Model * vec4(Pos, 1.0f));
}