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

struct DirectionalLight {
	vec3 Direction;
	vec3 Colour;
	float AmbientStrength;
	float SpecularStrength;
};

#define MAX_POINT_LIGHTS 4

in vec2 FragTexCoords;
in vec3 FragNormal;
in vec3 FragPos;
in vec2 screenPos;

uniform sampler2D ImageTexture;
uniform sampler2D ReflectionMap;
uniform samplerCube Skybox;
uniform vec3 CameraPos;
uniform vec3 ObjectPos;
uniform float Shininess = 64.0f;
uniform float Reflectivity;
uniform bool hasRefMap = true;
uniform PointLight PointLights[MAX_POINT_LIGHTS];
uniform DirectionalLight DirLight;

uniform float RimExponent = 0.0f;
uniform vec3 RimColour;

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

	vec3 rim = vec3(0,0,0);
	if (RimExponent > 0 ){
		float rimFactor = 1.0f - dot(normal, reverseViewDir);
		rimFactor = smoothstep(0.0f, 1.0f, rimFactor);
		rimFactor = pow(rimFactor, RimExponent);
		rim = rimFactor * RimColour;
	}
	

	float Distance = length(_pLight.Position - FragPos);
	float Attenuation =  _pLight.AttenuationConstant + (_pLight.AttenuationLinear * Distance) + (_pLight.AttenuationExponent * pow(Distance, 2));

	vec3 lightOutput = (ambient +  diffuse + specular + rim) / Attenuation;

	if (Attenuation < 1f) {
		lightOutput = vec3(0,0,0);
	}

	return lightOutput;
}

vec3 CalcDirLight(DirectionalLight _dLight) {
	
	vec3 normal = normalize(FragNormal);
	vec3 lightDir = normalize(_dLight.Direction);

	vec3 ambient = _dLight.AmbientStrength * _dLight.Colour;

	float diffuseStrength = max(dot(normal, -lightDir), 0.0f);
	vec3 diffuse = diffuseStrength * _dLight.Colour;

	vec3 reverseViewDir = normalize(CameraPos - FragPos);
	vec3 halfWayVector = normalize(-lightDir + reverseViewDir);
	float specularReflecitivity = pow(max(dot(normal, halfWayVector), 0.0f), Shininess);
	vec3 specular = _dLight.SpecularStrength * specularReflecitivity * _dLight.Colour;

	vec3 lightOutput = (ambient +  diffuse + specular);

	return lightOutput;
}

vec4 CalcReflection() {
	
	vec3 normal = normalize(FragNormal);
	vec3 viewDir = normalize(FragPos - CameraPos);
	vec3 reflectDir = reflect(viewDir, normal);

	vec4 reflectColour = texture(Skybox, reflectDir);

	return reflectColour;
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

	//New empty colour
	vec3 LightOutpt = vec3(0.0f, 0.0f, 0.0f);

	//Add all lights to the colour
	for (int i = 0; i < MAX_POINT_LIGHTS; i++){
		LightOutpt += CalcPointLight(PointLights[i]);
	}

	//Add the direct light to the colour
	LightOutpt += CalcDirLight(DirLight);

	vec4 trueColour = vec4(LightOutpt, 1.0f) * texture(ImageTexture, FragTexCoords);
	vec4 reflectColour = CalcReflection();
	float reflectionAmount = texture(ReflectionMap, FragTexCoords).r;
	if (!hasRefMap) reflectionAmount = 1;

	FinalColor = mix(trueColour, reflectColour, Reflectivity * reflectionAmount);
}