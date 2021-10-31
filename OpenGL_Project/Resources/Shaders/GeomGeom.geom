#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 33) out;

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

	//gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(-2.0f, 0.0f, 0.0f, 0.0f); EmitVertex();
	//gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(2.0f, 0.0f, 0.0f, 0.0f); EmitVertex();
	//gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0.0f, 2.0f, 0.0f, 0.0f); EmitVertex();
	//EndPrimitive();

	gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0, 0, 0.0f, 0.0f); EmitVertex();
	//calculate the ten perimiter points of a five point star centered at the origin (0,0,0)
	for (int i = 0; i < 33; i++) {
		float angle = i * 2.0f * 3.14159f / 10.0f + 0.279253f;
		float length = mod(i, 2) == 0 ? 1.0f : 0.5f;
		float x = cos(angle) * length;
		float y = sin(angle) * length;

		if (mod(i,3) == 0){
			outColor = vec3(1,1,1);
			gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0, 0, 0.0f, 0.0f); EmitVertex();
			EndPrimitive();
		}
		else{
			if (mod(i,2) == 0) outColor = vec3(0,0,1);
			else outColor = vec3(1,0,0);
			gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(x, y, 0.0f, 0.0f); EmitVertex();
		}
	}


 
	// //create a five pointed star out of triangles and emit them
	// gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0.0f, 0.0f, 0.0f, 0.0f); EmitVertex();
	// gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(-0.5f, -1.0f, 0.0f, 0.0f); EmitVertex();
	// gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0.5f, -1.0f, 0.0f, 0.0f); EmitVertex();
	
	// gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0.5f, -2.0f, 0.0f, 0.0f); EmitVertex();
	// gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(1.5f, -1.0f, 0.0f, 0.0f); EmitVertex();

	// EndPrimitive();

	// gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0.5f, -2.0f, 0.0f, 0.0f); EmitVertex();
	// gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(-0.5f, -2.0f, 0.0f, 0.0f); EmitVertex();
	// gl_Position = gs_in[0].position + gs_in[0].PVMMat * vec4(0.0f, -3.0f, 0.0f, 0.0f); EmitVertex();

	// EndPrimitive();



}