#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
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
void CheckInput();
void Render();

void Triangle(vector3 v1, vector3 c1, vector3 v2, vector3 c2, vector3 v3, vector3 c3);
void EquiTriangle(vector3 position, float height, float angle);

GLFWwindow* window = nullptr;

const int width = 800;
const int height = 800;

//GLuint Program_Tri;
//GLuint Program_ColorFadeTri;
GLuint Program_Texture;

GLuint VBO_Tri;
GLuint VAO_Tri;

vector3 pos;
float ang = 0;
float size = 1.0f;

GLfloat Vert_Quad[] = {
	//Pos					//Col					//Texture Coords
	-0.5f, 0.5f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		//Top - Left
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		//Bot - Left
	0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,		//Bot - Right
	0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		//Top - Right
};

GLuint Indices_Quad[] = {
	0, 1, 2,
	0, 2, 3
};

GLuint VBO_Quad;
GLuint VAO_Quad;
GLuint EBO_Quad;

GLuint Texture_Rayman;

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

	//Program_Tri = ShaderLoader::CreateProgram("Resources/Shaders/Triangle.vs", "Resources/Shaders/Color.fs");
	//GLuint test_ProgramTri = ShaderLoader::CreateProgram("Resources/Shaders/Triangle.vs", "Resources/Shaders/Color.fs");
	//Program_ColorFadeTri = ShaderLoader::CreateProgram("Resources/Shaders/Triangle.vs", "Resources/Shaders/VertexColorFade.fs");
	Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/NDC_Texture.vs", "Resources/Shaders/Texture.fs");

	glfwSetKeyCallback(window, key_callback);

	//Cull polygons not facing
	glCullFace(GL_BACK);

	//Set winding order of verticies (for front and back facing)
	glFrontFace(GL_CCW);

	//Enable Culling
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	//Flip Images
	stbi_set_flip_vertically_on_load(true);

	//Load the image data
	int ImageWidth;
	int ImageHeight;
	int ImageComponents;
	unsigned char* ImageData = stbi_load("Resources/Textures/Rayman.jpg", &ImageWidth, &ImageHeight, &ImageComponents, 0);

	//Gen and bind texture
	glGenTextures(1, &Texture_Rayman);
	glBindTexture(GL_TEXTURE_2D, Texture_Rayman);

	//Check how many components in image (RGBA or RGB)
	GLint LoadedComponents = ((ImageComponents == 4) ? GL_RGBA : GL_RGB);

	//Populate the texture with the image data
	glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponents, ImageWidth, ImageHeight, 0, LoadedComponents, GL_UNSIGNED_BYTE, ImageData);

	//Generating the mipmaps, free the memory and unbind the texture
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	stbi_image_free(ImageData);
	glBindTexture(GL_TEXTURE_2D, 0);


	//Gen VAO for quad
	glGenVertexArrays(1, &VAO_Quad);
	glBindVertexArray(VAO_Quad);

	//Gen EBO for quad
	glGenBuffers(1, &EBO_Quad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Quad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices_Quad), Indices_Quad, GL_STATIC_DRAW);

	//Gen VBO for quad
	glGenBuffers(1, &VBO_Quad);
	//copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert_Quad), Vert_Quad, GL_STATIC_DRAW);

	//Set the vertex attributes pointers (How to interperet Vertex Data)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
}

//Called each frame
void Update()
{
	CurrentTime = (float)glfwGetTime();

	CheckInput();
}

void CheckInput()
{
	bool updated = false;

	//Move Triangle
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		pos.x += 0.01f;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		pos.x -= 0.01f;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		pos.y += 0.01f;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		pos.y -= 0.01f;
		updated = true;
	}

	//Scale Triangle
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
	{
		size -= 0.01f;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		size += 0.01f;
		updated = true;
	}

	//Rotate Triangle
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		ang -= 0.5f;
		updated = true;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		ang += 0.5f;
		updated = true;
	}

	if (updated) {
		EquiTriangle(pos, size, ang);
	}
}

//Render window
void Render()
{
	//Clear Screen
	glClear(GL_COLOR_BUFFER_BIT);

	//
	/*glUseProgram(Program_ColorFadeTri);
	glBindVertexArray(VAO_Quad);

	GLint CurrentTimeLoc = glGetUniformLocation(Program_ColorFadeTri, "CurrentTime");
	glUniform1f(CurrentTimeLoc, CurrentTime);*/

	glUseProgram(Program_Texture);
	glBindVertexArray(VAO_Quad);

	//Activate and bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture_Rayman);
	glUniform1i(glGetUniformLocation(Program_Texture, "ImageTexture"), 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	/*glUseProgram(Program_ColorFadeTri);
	glBindVertexArray(VAO_Quad);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	*/

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

	//copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Tri);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}