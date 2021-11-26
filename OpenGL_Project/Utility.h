#pragma once
#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/vector_angle.hpp>

#include <fmod.hpp>

#include <iostream>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <map>
#include <vector>
#include <string>

namespace utils {
	//Width and height of window
	extern int windowWidth;
	extern int windowHeight;

	extern glm::vec2 mousePos;

	extern float currentTime;
	extern float deltaTime;
	extern float previousTimeStep;

	static float RandomFloat(float min, float max) {
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = max - min;
		float r = random * diff;
		return min + r;
	}

	//Linearly interpolate between two values
	static float Lerp(float a, float b, float f) {
		return a + f * (b - a);
	}

    static double CosInterp(double a, double b, double x) {
        double ft = x * 3.1415927;
        double f = (1 - cos(ft)) * 0.5;
        return  a * (1 - f) + b * f;
    }

    static double RawNoise(int x, int y, int seed) {
        int n = x + y * seed;
        n = (n << 13) ^ n;
        int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
        return 1.0 - (double)(t) * 0.931322574615478515625e-9;
    }


    static double SmoothedNoise(int x, int y, int seed) {
        double corners = (RawNoise(x - 1, y - 1, seed) + RawNoise(x + 1, y - 1, seed) + RawNoise(x - 1, y + 1, seed) + RawNoise(x + 1, y + 1, seed)) / 16;
        double sides = (RawNoise(x - 1, y, seed) + RawNoise(x + 1, y, seed) + RawNoise(x, y - 1, seed) + RawNoise(x, y + 1, seed)) / 8;
        double center = RawNoise(x, y, seed) / 4;
		return corners + sides + center;
    }

    static double InterpolatedNoise(double x, double y, int seed) {
        int integer_X = (int)x;
        double fractional_X = x - (double)integer_X;
        int integer_Y = (int)y;
        double fractional_Y = y - (double)integer_Y;

        double v1 = SmoothedNoise(integer_X, integer_Y, seed);
        double v2 = SmoothedNoise(integer_X + 1, integer_Y, seed);
        double v3 = SmoothedNoise(integer_X, integer_Y + 1, seed);
        double v4 = SmoothedNoise(integer_X + 1, integer_Y + 1, seed);

        double i1 = Lerp((float)v1, (float)v2, (float)fractional_X);
        double i2 = Lerp((float)v3, (float)v4, (float)fractional_X);
        return CosInterp(i1, i2, fractional_Y);
    }

    static double FinalNoise2D(double x, double y, int seed = 57, int octaves = 8, double persistence = 0.5, double amplitude = 1.0) {
        double frequency = pow(2, octaves);
        double total = 0;

        for (int i = 0; i < octaves; ++i) {
            frequency /= 2;
            amplitude *= persistence;
            total += InterpolatedNoise(x / frequency, y / frequency, seed) * amplitude;
        }
        return total / frequency;
    }
	
}

