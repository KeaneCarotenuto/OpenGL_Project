#version 460 core

layout (location = 0) in vec4 Vertex;

out vec2 FragTexCoords;
out vec2 VertexCopy;
out float leftClip;
out float rightClip;

uniform mat4 ProjectionMat;
uniform float CurrentTime;

void main()
{
	leftClip = 100;
	rightClip = 700;
	vec2 newCoords = vec2(mod(Vertex.x + CurrentTime*50, rightClip - leftClip) + leftClip, Vertex.y + sin(Vertex.x/100 + CurrentTime)* 10);
	gl_Position = ProjectionMat * vec4(newCoords, 0.0f, 1.0f);
	FragTexCoords = vec2(Vertex.z, Vertex.w);
	VertexCopy = newCoords;
}