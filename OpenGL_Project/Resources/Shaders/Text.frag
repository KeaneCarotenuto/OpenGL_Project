#version 460 core

in vec2 FragTexCoords;
in vec4 VertexCopy;

uniform sampler2D TextTexture;
uniform vec3 TextColor;
uniform float CurrentTime;

out vec4 FinalColor;

void main()
{
	float Alpha = texture(TextTexture, FragTexCoords).r;
	if (mod(VertexCopy.x + CurrentTime*50, 600) > 500 || mod(VertexCopy.x + CurrentTime*50, 600) < 100 ){
		Alpha = 0;
	}
	FinalColor = vec4(TextColor.x, TextColor.yz, Alpha);
}