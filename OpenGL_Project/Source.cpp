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

void Triangle(vector3 v1, vector3 c1, vector3 v2, vector3 c2, vector3 v3, vector3 c3);
void EquiTriangle(vector3 position, float height, float angle);

GLFWwindow* window = nullptr;

const int width = 800;
const int height = 800;

GLuint Program_FixedTri;
GLuint Program_FixedTri2;
GLuint Program_ColorFade;

GLuint VBO_Tri;
GLuint VAO_Tri;

// Vert //Indices
GLfloat Vertices_Tri[] = {
	// Position				// Color 
	0.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,	// Top Right
	-0.5f, 0.8f, 0.0f,		0.0f, 1.0f, 0.0f,	// Top Left
	0.5f, 0.8f, 0.0f,		0.0f, 0.0f, 1.0f,	// Bottom Center
};

vector3 pos;
float ang = 0;
float size = 1.0f;

float CurrentTime;

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Maps the range of the window size to NDC (-1 -> 1)
	glViewport(0, 0, width, height);

	Program_FixedTri = ShaderLoader::CreateProgram("Resources/Shaders/TriangleVS.txt", "Resources/Shaders/FixedColor.txt");
	//Program_FixedTri2 = ShaderLoader::CreateProgram("Resources/Shaders/TriangleVS.txt", "Resources/Shaders/FixedColor.txt");
	//Program_ColorFade = ShaderLoader::CreateProgram("Resources/Shaders/VertexColorFade.txt", "Resources/Shaders/FixedColor.txt");

	glfwSetKeyCallback(window, key_callback);

	//Gen VAO for triangle
	glGenVertexArrays(1, &VAO_Tri);
	glBindVertexArray(VAO_Tri);

	//Gen VBO for triangle
	glGenBuffers(1, &VBO_Tri);
	//copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Tri);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices_Tri), Vertices_Tri, GL_STATIC_DRAW);

	//Set the vertex attributes pointers (How to interperet Vertex Data)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
}

//Called each frame
void Update()
{
	CurrentTime = (float)glfwGetTime();

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

	glUseProgram(Program_FixedTri);
	glBindVertexArray(VAO_Tri);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glUseProgram(0);

	//EquiTriangle(pos, size, ang);
	//EquiTriangle(vector3{pos.x + 1, pos.y + 1, pos.z}, size, ang);

	glfwSwapBuffers(window);
}

void EquiTriangle(vector3 position, float height, float angle) {
	Triangle(	vector3{ position.x + (float)sin(angle * M_PI / 180) * (height / 2), position.y + (float)cos(angle * M_PI / 180) * (height / 2) , 0.0f },
				vector3{ 1.0f,0.0f,0.0f },

				vector3{ position.x + (float)sin((angle + 120) * M_PI / 180) * (height/2), position.y + (float)cos((angle + 120) * M_PI / 180) * (height/2) , 0.0f },	
				vector3{ 0.0f,1.0f,0.0f },

				vector3{ position.x + (float)sin((angle + 240) * M_PI / 180) * (height/2), position.y + (float)cos((angle + 240) * M_PI / 180) * (height/2) , 0.0f },	
				vector3{ 0.0f,0.0f,1.0f }) ;
}

void Triangle(vector3 v1, vector3 c1, vector3 v2 , vector3 c2, vector3 v3 , vector3 c3)
{
	vector3 vertices[] = {
		v1,  c1,
		v2,  c2,
		v3,  c3,
	};

	//Gen VAO for triangle
	glGenVertexArrays(1, &VAO_Tri);
	glBindVertexArray(VAO_Tri);

	//Gen VBO for triangle
	glGenBuffers(1, &VBO_Tri);
	//copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Tri);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Set the vertex attributes pointers (How to interperet Vertex Data)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glUseProgram(Program_FixedTri);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(0);
}