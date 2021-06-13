#version 460 core

struct PointLight {
	vec3 Position;
	vec3 Colour;
	float AmbientStrength;
	float SpecularStrength;

	float AttenuationConstant;
	float AttenuationLinear;
	float AttenuationExponent;
};

#define MAX_POINT_LIGHTS 4

in vec2 FragTexCoords;
in vec3 FragNormal;
in vec3 FragPos;

uniform sampler2D ImageTexture;
uniform vec3 CameraPos;
uniform vec3 ObjectPos;
uniform float Shininess = 64.0f;
uniform PointLight PointLights[MAX_POINT_LIGHTS];

//uniform float AmbientStrength = 0.05f;
//uniform vec3 AmbientColour = vec3(1.0f, 1.0f, 1.0f);
//uniform vec3 LightColour = vec3(1.0f, 1.0f, 1.0f);
//uniform vec3 LightPos = vec3(-2.0f, 6.0f, 3.0f);
//uniform float LightSpecularStrength = 1.0f;

uniform float RimExponent = 4.0f;
uniform vec3 RimColour = vec3(1.0f, 0.0f, 0.0f);

uniform vec2 mousePos;
uniform float CurrentTime;

out vec4 FinalColor;

#define PI 3.1415926538

vec3 CalcPointLight(PointLight _pLight) {
	
	vec3 normal = normalize(FragNormal);
	vec3 lightDir = normalize(FragPos - _pLight.Position);

	vec3 ambient = _pLight.AmbientStrength * _pLight.Colour;

	float diffuseStrength = max(dot(normal, -lightDir), 0.0f);
	vec3 diffuse = diffuseStrength * _pLight.Colour;

	vec3 reverseViewDir = normalize(CameraPos - FragPos);
	vec3 halfWayVector = normalize(-lightDir + reverseViewDir);
	float specularReflecitivity = pow(max(dot(normal, halfWayVector), 0.0f), Shininess);
	vec3 specular = _pLight.SpecularStrength * specularReflecitivity * _pLight.Colour;

	float rimFactor = 1.0f - dot(normal, reverseViewDir);
	rimFactor = smoothstep(0.0f, 1.0f, rimFactor);
	rimFactor = pow(rimFactor, RimExponent);
	vec3 rim = rimFactor * RimColour * _pLight.Colour;

	float Distance = length(_pLight.Position - FragPos);
	float Attenuation =  _pLight.AttenuationConstant + (_pLight.AttenuationLinear * Distance) + (_pLight.AttenuationExponent * pow(Distance, 2));

	vec3 lightOutput = (ambient +  diffuse + specular + rim) / Attenuation;

	if (Attenuation < 1f) {
		lightOutput = vec3(0,0,0);
	}

	return lightOutput;
}

void main() 
{
//	//Clips the colour to black based on mouse position relative to fragment (multiply this with light to show)
//	float clip = dot(vec2(0,1), normalize(vec2(CameraPos.xz - FragPos.xz)));
//	float minDot = cos(radians(45));
//	float maxDot = 1.0f;
//	float dotDiff = maxDot - minDot;
//	float mouseXrat = (1 - (abs(mousePos.x - 400) / 400));
//	mouseXrat = sin(mouseXrat * PI/2);
//	if (clip > minDot + dotDiff * mouseXrat) clip = 1;
//	else clip = 0;

//	//Highlights the verts (multiply this with light to show)
//	float dist = 0.45;
//
//	float rad = (distance(ObjectPos, FragPos) - dist) * 0.5f/(0.5f - dist) * 2;

	vec3 LightOutpt = vec3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < MAX_POINT_LIGHTS; i++){
		LightOutpt += CalcPointLight(PointLights[i]);
	}

	FinalColor = vec4(LightOutpt, 1.0f) * texture(ImageTexture, FragTexCoords);
}