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
	leftClip = 0;
	rightClip = 800;
	//vec2 newCoords = vec2(mod(Vertex.x + CurrentTime*50, rightClip - leftClip) + leftClip, Vertex.y ) ;
	vec2 newCoords = Vertex.xy;
	gl_Position = ProjectionMat * vec4(newCoords, 0.0f, 1.0f);
	FragTexCoords = vec2(Vertex.z, Vertex.w);
	VertexCopy = newCoords;
}