#version 460 core

uniform vec3 Colour;

out vec4 FinalColor;

void main() {
    FinalColor = vec4(Colour, 1.0f);
}