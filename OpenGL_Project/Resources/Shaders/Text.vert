#version 460 core

layout(location = 0) in vec4 Vertex;

out vec2 FragTexCoords;

uniform mat4 ProjectionMat;

void main()
{
	gl_Position = ProjectionMat * vec4(Vertex.xy, 0.0F, 1.0F);
	FragTexCoords = Vertex.zw;
}