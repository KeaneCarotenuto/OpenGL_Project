#version 430 core
in vec3 outColor;
out vec4 color;

uniform float CurrentTime;

void main(){
	color = vec4(outColor * (sin(CurrentTime) + 2) / 3.0f, 1.0f);
}