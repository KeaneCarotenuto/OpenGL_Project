#version 460 core

in vec2 FragTexCoords;
in vec3 FragNormal;
in vec3 FragPos;

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

uniform vec2 mousePos;
uniform float CurrentTime;

out vec4 FinalColor;

#define PI 3.1415926538

void main() 
{
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

	float clip = dot(vec2(0,1), normalize(vec2(CameraPos.xz - FragPos.xz)));
	float minDot = cos(radians(45));
	float maxDot = 1.0f;
	float dotDiff = maxDot - minDot;
	float mouseXrat = (1 - (abs(mousePos.x - 400) / 400));
	mouseXrat = sin(mouseXrat * PI/2);
	if (clip > minDot + dotDiff * mouseXrat) clip = 1;
	else clip = 0;

	//Highlights the verts (multiply this with light to show)
//	float dist = 0.45;
//
//	float rad = (distance(ObjectPos, FragPos) - dist) * 0.5f/(0.5f - dist) * 2;

	vec4 light = vec4(ambient + diffuse + specular + rim, 1.0f) * clip;

	FinalColor = light * texture(ImageTexture, FragTexCoords);
}