#version 460 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;

uniform mat4 PVMMat;
uniform mat4 Model;

uniform sampler2D ImageTexture;
uniform vec3 CameraPos;
uniform vec3 ObjectPos;
uniform float AmbientStrength = 0.05f;
uniform vec3 AmbientColour = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 LightColour = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 LightPos = vec3(-2.0f, 6.0f, 3.0f);
uniform float LightSpecularStrength = 1.0f;
uniform float Shininess = 64.0f;
uniform float RimExponent = 4.0f;
uniform vec3 RimColour = vec3(1.0f, 0.0f, 0.0f);

out vec2 FragTexCoords;
out vec3 FragNormal;
out vec3 FragPos;
out vec4 FinalColorIN;

void main() 
{
	gl_Position = PVMMat * vec4(Pos, 1.0);

	FragTexCoords = TexCoords;
	FragNormal = mat3(transpose(inverse(Model))) * Normal;
	FragPos = vec3(Model * vec4(Pos, 1.0f));

	vec3 normal = normalize(FragNormal);
	vec3 lightDir = normalize(FragPos - LightPos);

	vec3 ambient = AmbientStrength * AmbientColour;

	float diffuseStrength = max(dot(normal, -lightDir), 0.0f);
	vec3 diffuse = diffuseStrength * LightColour;

	vec3 reverseViewDir = normalize(CameraPos - FragPos);
	vec3 halfWayVector = normalize(-lightDir + reverseViewDir);
	float specularReflecitivity = pow(max(dot(normal, halfWayVector), 0.0f), Shininess);
	vec3 specular = LightSpecularStrength * specularReflecitivity * LightColour;

	float rimFactor = 1.0f - dot(normal, reverseViewDir);
	rimFactor = smoothstep(0.0f, 1.0f, rimFactor);
	rimFactor = pow(rimFactor, RimExponent);
	vec3 rim = rimFactor * RimColour * LightColour;
	vec4 light = vec4(ambient + diffuse + specular + rim, 1.0f);

	FinalColorIN = light * texture(ImageTexture, FragTexCoords);

}