#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Source.h"
#include "ShaderLoader.h"

struct vector3 {
	float x;
	float y;
	float z;
};

bool Startup();
void InitialSetup();

void Update();
void Render();

void Triangle(vector3 v1, vector3 v2, vector3 v3);
void EquiTriangle(vector3 position, float height, float angle);

GLFWwindow* window = nullptr;

const int width = 800;
const int height = 800;

GLuint Program_FixedTri;

unsigned int VBO;

vector3 pos;
float ang = 0;
float size = 1.0f;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main() {

	//Setup for use of OpenGL
	if (!Startup()) return -1;

	InitialSetup();

	//Main Loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//Update all objects and run processes
		Update();

		//Render all the objects
		Render();
	}

	//Close GLFW correctly
	glfwTerminate();
	return 0;
}

bool Startup()
{
	//Init GLFW and set the version to 4.6
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	//Create a GLFW controlled context window
	window = glfwCreateWindow(width, height, "First OpenGL Window", NULL, NULL);

	//Check for failure
	if (window == NULL) {
		std::cout << "GLFW failed to init. Exiting" << std::endl;
		system("pause");

		glfwTerminate();
		return false;
	}

	//Set current context to the new window
	glfwMakeContextCurrent(window);


	//Init GLEW to propulate OpenGL function pointers
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW failed to init. Exiting" << std::endl;
		system("pause");

		glfwTerminate();
		return false;
	}

	return true;
}

void InitialSetup()
{
	//Set the clear colour as blue (used by glClear)
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	// Maps the range of the window size to NDC (-1 -> 1)
	glViewport(0, 0, width, height);

	Program_FixedTri = ShaderLoader::CreateProgram("Resources/Shaders/TriangleVS.txt", "Resources/Shaders/FixedColor.txt");

	glfwSetKeyCallback(window, key_callback);
}

//Called each frame
void Update()
{
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		pos.x += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		pos.x -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		pos.y += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		pos.y -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
	{
		size -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		size += 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		ang -= 0.5f;
	}

	if (glfwGetKey(window, GLFW_KEY_E))
	{
		ang += 0.5f;
	}
}

//Render window
void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	EquiTriangle(pos, size, ang);
	//EquiTriangle(vector3{pos.x + 1, pos.y + 1, pos.z}, size, ang);

	glfwSwapBuffers(window);
}

void EquiTriangle(vector3 position, float height, float angle) {
	Triangle(	vector3{ position.x + (float)sin(angle * M_PI / 180) * (height/2), position.y + (float)cos(angle * M_PI / 180) * (height/2) , 0.0f },
				vector3{ position.x + (float)sin((angle + 120) * M_PI / 180) * (height/2), position.y + (float)cos((angle + 120) * M_PI / 180) * (height/2) , 0.0f },
				vector3{ position.x + (float)sin((angle + 240) * M_PI / 180) * (height/2), position.y + (float)cos((angle + 240) * M_PI / 180) * (height/2) , 0.0f });
}

void Triangle(vector3 v1, vector3 v2, vector3 v3)
{
	float vertices[] = {
		v1.x, v1.y, v1.z,
		v2.x, v2.y, v2.z,
		v3.x, v3.y, v3.z,
	};

	glGenBuffers(1, &VBO);
	// 0. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 1. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glUseProgram(Program_FixedTri);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(0);
}