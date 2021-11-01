#version 430 core
//size of output patch & no. of times the TCS will be executed
layout (vertices = 4) out;

uniform float Distance;

void main() {
	float innerDivs = Distance < 10.0f ? 3 : 2;
	float outerDivs = Distance < 10.0f ? 5 : 2;

	if (gl_InvocationID == 0) {
		gl_TessLevelInner[0] = innerDivs;
		gl_TessLevelInner[1] = innerDivs;

		gl_TessLevelOuter[0] = outerDivs;
		gl_TessLevelOuter[1] = outerDivs;
		gl_TessLevelOuter[2] = outerDivs;
		gl_TessLevelOuter[3] = outerDivs;
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}