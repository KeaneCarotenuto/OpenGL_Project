// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// (c) 2021 Media Design School
//
// File Name   : CLightManager.h
// Description : Manages all light objects, as well as uniforms for lights
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#pragma once
#include <iostream>
#include <string>

#include <glew.h>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "CObjectManager.h"

struct PointLight 
{
	glm::vec3 Position = glm::vec3();
	glm::vec3 Colour = glm::vec3();
	float AmbientStrength = 0;
	float SpecularStrength = 0;

	float AttenuationConstant = 0;
	float AttenuationLinear = 0;
	float AttenuationExponent = 0;
};

struct DirectionalLight {
	glm::vec3 Direction = glm::vec3();
	glm::vec3 Colour = glm::vec3();
	float AmbientStrength = 0;
	float SpecularStrength = 0;
};

class CLightManager
{
private:
	static const int MAX_POINT_LIGHTS = 4;

	static PointLight PointLights[MAX_POINT_LIGHTS];

	static int currentLightNum;


	static const int MAX_SPHERES = 20;

public:
	static DirectionalLight directionalLight;

	static void AddLight(glm::vec3 _pos, glm::vec3 _col, float _ambientStrength, float _specularStrength, float _attenDist);
	static void UpdateUniforms(GLuint _program);

	static int GetMaxPointLights() { return MAX_POINT_LIGHTS; };

	static PointLight* GetPointLights() { return PointLights; };
	static PointLight GetPointLight(int i) { return PointLights[i]; };
};

