#version 460 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Col;
layout (location = 2) in vec2 TexCoords;

uniform mat4 ModelMat;

out vec3 FragColor;
out vec2 FragTexCoords;

void main() 
{
	mat4 aMat4 = mat4(	1.0, 0.0, 0.0, 0.0,  // 1. column
						0.0, 1.0, 0.0, 0.0,  // 2. column
						0.0, 0.0, 1.0, 0.0,  // 3. column
						0.5, 0.0, 0.0, 1.0); // 4. column
	gl_Position =  ModelMat * vec4(Pos.xy, 0, 1.0);
	FragColor = Col;
	FragTexCoords = TexCoords;
}