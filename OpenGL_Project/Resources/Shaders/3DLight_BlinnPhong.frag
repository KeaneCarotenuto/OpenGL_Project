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

struct Sphere {
	vec3 Position;
	float rad;
};

#define MAX_POINT_LIGHTS 4
#define MAX_SPHERES 20

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

uniform Sphere Spheres[MAX_SPHERES];

uniform float RimExponent = 0.0f;
uniform vec3 RimColour;

uniform vec2 mousePos;
uniform float CurrentTime;

uniform float offset;
uniform int frameCount;

out vec4 FinalColor;

#define PI 3.1415926538

//Caluclate the effect of a single point light on this fragment
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

	vec3 lightOutput = (diffuse + specular + rim);

	//Basic shadows for spheres
	for (int i = 0; i < MAX_SPHERES; i++){
		vec3 sPos = Spheres[i].Position;

		float d = distance(sPos, FragPos);
		vec3 line = normalize(-lightDir) * d + FragPos;
		if (distance(sPos, line) < Spheres[i].rad && d < distance(FragPos, _pLight.Position)) lightOutput = vec3(0);
	}

	lightOutput = (ambient + lightOutput) / Attenuation;

	if (Attenuation < 1f) {
		lightOutput = vec3(0,0,0);
	}

	return lightOutput;
}

//Caluclate the effect of the directional light on this fragment
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

	vec3 lightOutput = (diffuse + specular);

	//Basic shadows for spheres
	for (int i = 0; i < MAX_SPHERES; i++){
		vec3 sPos = Spheres[i].Position;

		float d = distance(sPos, FragPos);
		vec3 line = normalize(-lightDir) * d + FragPos;
		if (distance(sPos, line) < Spheres[i].rad) lightOutput *= min(d/30,1);
	}

	lightOutput = (ambient + lightOutput);

	return lightOutput;
}

//Calculate skybox reflection
vec4 CalcReflection() {
	
	vec3 normal = normalize(FragNormal);
	vec3 viewDir = normalize(FragPos - CameraPos);
	vec3 reflectDir = reflect(viewDir, normal);

	vec4 reflectColour = texture(Skybox, reflectDir);

	return reflectColour;
}

void main() 
{
	//New empty colour
	vec3 LightOutpt = vec3(0.0f, 0.0f, 0.0f);

	//Add all lights to the colour
	for (int i = 0; i < MAX_POINT_LIGHTS; i++){
		LightOutpt += CalcPointLight(PointLights[i]);
	}

	//Add the direct light to the colour
	LightOutpt += CalcDirLight(DirLight);

	float frameCountCopy = frameCount;
    if (frameCountCopy <= 0) frameCountCopy = 1;

	vec4 trueColour = vec4(LightOutpt, 1.0f) * texture(ImageTexture, vec2(FragTexCoords.x/frameCountCopy + offset, FragTexCoords.y));
	vec4 reflectColour = CalcReflection();
	float reflectionAmount = texture(ReflectionMap, FragTexCoords).r;
	if (!hasRefMap) reflectionAmount = 1;

	FinalColor = mix(trueColour, reflectColour, Reflectivity * reflectionAmount);

//	float d = distance(FragPos, CameraPos);
//	float lerp = (d - 5.0f)/20.f;
//	lerp = clamp(lerp, 0.0, 1.0);
//
//	vec4 vFogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
//	FinalColor = mix(FinalColor, vFogColor, lerp);
}