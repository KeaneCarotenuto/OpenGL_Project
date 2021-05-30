#version 460 core

layout (location = 0) in vec4 Vertex;

uniform mat4 ProjectionMat;

out vec2 FragTexCoords;

void main()
{
	gl_Position = ProjectionMat * vec4(Vertex.xy, 0.0f, 1.0f);
	FragTexCoords = vec2(Vertex.z, Vertex.w);
}