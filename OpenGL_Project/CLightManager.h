#pragma once
#include <iostream>
#include <string>

#include <glew.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

struct PointLight 
{
	glm::vec3 Position;
	glm::vec3 Colour;
	float AmbientStrength;
	float SpecularStrength;

	float AttenuationConstant;
	float AttenuationLinear;
	float AttenuationExponent;
};

struct DirectionalLight {
	glm::vec3 Direction;
	glm::vec3 Colour;
	float AmbientStrength;
	float SpecularStrength;
};

class CLightManager
{
private:
	static const int MAX_POINT_LIGHTS = 4;

	static PointLight PointLights[MAX_POINT_LIGHTS];

	static DirectionalLight directionalLight;

	static int currentLightNum;

public:
	static void AddLight(glm::vec3 _pos, glm::vec3 _col, float _ambientStrength, float _specularStrength, float _attenDist);
	static void UpdateUniforms(GLuint _program);

	static int GetMaxPointLights() { return MAX_POINT_LIGHTS; };

	static PointLight* GetPointLights() { return PointLights; };
	static PointLight GetPointLight(int i) { return PointLights[i]; };
};

