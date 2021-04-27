#version 460 core

layout (location = 0) in vec4 Vertex;

out vec2 FragTexCoords;
out vec4 VertexCopy;

uniform mat4 ProjectionMat;
uniform float CurrentTime;

void main()
{
	gl_Position = ProjectionMat * vec4(mod(Vertex.x + CurrentTime*50, 600), Vertex.y + sin(Vertex.x/100 + CurrentTime) * 10, 0.0f, 1.0f);
	FragTexCoords = vec2(Vertex.z, Vertex.w);
	VertexCopy = Vertex;
}