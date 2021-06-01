#pragma once

#include <glew.h>
#include <glfw3.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

class Sphere
{
public:
	Sphere(float Radius, int Fidelity);
	~Sphere();

	void Render();
	 
private:
	GLuint VAO;

	int IndexCount;
	int DrawType;
};

