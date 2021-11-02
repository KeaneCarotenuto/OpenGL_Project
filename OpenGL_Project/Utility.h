#pragma once
#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/rotate_vector.hpp>

#include <fmod.hpp>

#include <iostream>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <map>
#include <string>
#include <vector>

namespace utils {
	//Width and height of window
	extern int windowWidth;
	extern int windowHeight;

	extern glm::vec2 mousePos;

	extern float currentTime;
	extern float deltaTime;
	extern float previousTimeStep;
}

