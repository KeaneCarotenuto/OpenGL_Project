#version 460 core

in vec2 FragTexCoords;
in vec3 FragNormal;
in vec3 FragPos;

uniform sampler2D ImageTexture;
uniform vec3 CameraPos;
uniform float AmbientStrength = 0.1f;
uniform vec3 AmbientColour = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 LightColour = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 LightPos = vec3(-2.0f, 6.0f, 3.0f);
uniform float LightSpecularStrength = 1.0f;
uniform float Shininess = 32.0f;


uniform float CurrentTime;

out vec4 FinalColor;

void main() 
{
	vec3 normal = normalize(FragNormal);
	vec3 lightDir = normalize(FragPos - LightPos);

	vec3 ambient = AmbientStrength * AmbientColour;

	float diffuseStrength = max(dot(normal, -lightDir), 0.0f);
	vec3 diffuse = diffuseStrength * LightColour;

	vec3 reverseViewDir = normalize(CameraPos - FragPos);
	vec3 reflectedDir = reflect(lightDir, normal);
	float specularReflecitivity = pow(max(dot(reverseViewDir, reflectedDir), 0.0f), Shininess);
	vec3 specular = LightSpecularStrength * specularReflecitivity * LightColour;

	vec4 light = vec4(ambient + diffuse + specular, 1.0f);

	FinalColor = light * texture(ImageTexture, FragTexCoords);
}