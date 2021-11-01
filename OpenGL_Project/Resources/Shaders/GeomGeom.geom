#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 66) out;

out vec3 outColor;

in VS_GS_VERTEX{
	in vec4 position;
	in vec3 color;
	in mat4 PVMMat;
}gs_in[];

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {

	//make first half of star
	//gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0, 0, 0.0f, 0.0f); EmitVertex();
	//calculate the ten perimiter points of a five point star centered at the origin (0,0,0)
	for (int i = 0; i < 33; i++) {
		//find angle
		float angle = i * 2.0f * 3.14159f / 10.0f + 0.279253f;

		//calc pos and length based on angle
		float length = mod(i, 2) == 0 ? 1.0f : 0.5f;
		float x = cos(angle) * length;
		float y = sin(angle) * length;
		
		//random colour
		outColor = vec3(rand(vec2(i + 5,i * i)),rand(vec2(-i + 4,i * i)),rand(vec2(-i + 3,-i * i)));

		//if third point, make it zero
		if (mod(i,3) == 0){
			gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0, 0, 0.3f, 0.0f); EmitVertex();
			EndPrimitive();
		}
		else{
			gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(x, y, 0.0f, 0.0f); EmitVertex();
		}
	}
	EndPrimitive();

	//make another star, but flipped
	//gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0, 0, 0.0f, 0.0f); EmitVertex();
	//calculate the ten perimiter points of a five point star centered at the origin (0,0,0)
	for (int i = 32; i >= 0; i--) {
		//find angle
		float angle = i * 2.0f * 3.14159f / 10.0f + 0.279253f;

		//calc pos and length based on angle
		float length = mod(i, 2) == 0 ? 1.0f : 0.5f;
		float x = cos(angle) * length;
		float y = sin(angle) * length;
		
		//random colour
		outColor = vec3(rand(vec2(i + 2,i * i)),rand(vec2(-i + 1,i * i)),rand(vec2(-i + 0,-i * i)));

		//if third point, make it zero
		if (mod(i,3) == 0){
			gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0, 0, -0.3f, 0.0f); EmitVertex();
			EndPrimitive();
		}
		else{
			gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(x, y, 0.0f, 0.0f); EmitVertex();
		}
	}
}