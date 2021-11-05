#pragma once
#include <iostream>

namespace utils {
	//Width and height of window
	extern int windowWidth;
	extern int windowHeight;

	extern glm::vec2 mousePos;

	extern float currentTime;
	extern float deltaTime;
	extern float previousTimeStep;

	static float Lerp(float a, float b, float f) {
		return a + f * (b - a);
	}
}

