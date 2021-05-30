#version 460 core

layout (location = 0) in vec4 Vertex;

uniform mat4 ProjectionMat;
uniform float CurrentTime;
uniform vec2 XCliping;
uniform vec2 CharacterSize;

out vec2 FragTexCoords;
out vec2 VertexCopy;
out float leftClip;
out float rightClip;
out vec2 CharSize;



void main()
{
	CharSize = CharacterSize;

	leftClip = XCliping.x;
	rightClip = XCliping.y;

	vec2 newCoords = vec2(mod(Vertex.x + CurrentTime*CharSize.y, rightClip - leftClip) + leftClip, Vertex.y );

	gl_Position = ProjectionMat * vec4(newCoords, 0.0f, 1.0f);
	FragTexCoords = vec2(Vertex.z, Vertex.w);
	VertexCopy = newCoords;
}