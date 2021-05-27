#version 460 core

in vec2 FragTexCoords;
in vec2 VertexCopy;
in float leftClip;
in float rightClip;

uniform sampler2D TextTexture;
uniform vec3 TextColor;
uniform float CurrentTime;

out vec4 FinalColor;

void main()
{
	float characterWidth = 50;

	

	float Alpha = texture(TextTexture, FragTexCoords).r;
	if (VertexCopy.x < (leftClip + characterWidth) || VertexCopy.x > (rightClip - characterWidth)){
		Alpha = 0;
	}
	FinalColor = vec4(TextColor, Alpha);
}