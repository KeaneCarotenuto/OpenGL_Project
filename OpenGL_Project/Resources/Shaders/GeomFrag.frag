#version 430 core
in vec3 outColor;
out vec4 color;

in GS_FS_VERTEX{
	vec2 texcoord;
}fs_in;

uniform sampler2D Texture;


void main(){
	color = texture(Texture, vec2( fs_in.texcoord.x , 1 - fs_in.texcoord.y)) * vec4(1, 1, 1, 1.0f);
}