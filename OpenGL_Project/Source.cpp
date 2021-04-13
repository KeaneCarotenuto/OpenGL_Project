#include <glew.h>
#include <glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Source.h"
#include "ShaderLoader.h"

#include "CShape.h"
#include "CUniform.h"

struct vector3 {
	float x;
	float y;
	float z;
};

bool Startup();
void InitialSetup();

void GenTexture(GLuint& texture, const char* texPath);

void Update();
void UpdatePVM(CShape& _shape);
void CheckInput();
void Render();

void Triangle(vector3 v1, vector3 c1, vector3 v2, vector3 c2, vector3 v3, vector3 c3);
void EquiTriangle(vector3 position, float height, float angle);

GLFWwindow* window = nullptr;

const int width = 800;
const int height = 800;

//GLuint Program_Tri;
GLuint Program_ColorFadeTri;
GLuint Program_Texture;
GLuint Program_TextureMix;
GLuint Program_WorldSpace;
GLuint Program_ClipSpace;
GLuint Program_ClipSpaceColour;

// Camera Variables 
glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 CameraLookDir = glm::vec3(0.0f, 0.0f, -1.0f); 
glm::vec3 CameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f); 
glm::vec3 CameraUpDir = glm::vec3(0.0f, 1.0f, 0.0f); 
glm::mat4 ViewMat; 
glm::mat4 ProjectionMat;

GLuint Texture_Rayman;
GLuint Texture_Awesome;
GLuint Texture_NoCap;

CShape g_Hexagon;
CShape g_Rectangle;

float CurrentTime;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
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

	//Program_FixedColour = ShaderLoader::CreateProgram("Resources/Shaders/Triangle.vs", "Resources/Shaders/Color.fs");
	//GLuint test_ProgramTri = ShaderLoader::CreateProgram("Resources/Shaders/Triangle.vs", "Resources/Shaders/Color.fs");
	Program_ColorFadeTri = ShaderLoader::CreateProgram("Resources/Shaders/Triangle.vs", "Resources/Shaders/VertexColorFade.fs");
	Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vs", "Resources/Shaders/Texture.fs");
	Program_TextureMix = ShaderLoader::CreateProgram("Resources/Shaders/NDC_Texture.vs", "Resources/Shaders/TextureMix.fs");
	Program_WorldSpace = ShaderLoader::CreateProgram("Resources/Shaders/WorldSpace.vs", "Resources/Shaders/TextureMix.fs");
	Program_ClipSpace = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vs", "Resources/Shaders/TextureMix.fs");
	Program_ClipSpaceColour = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vs", "Resources/Shaders/VertexColorFade.fs");

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

	GenTexture(Texture_Rayman, "Resources/Textures/Rayman.jpg");
	GenTexture(Texture_Awesome, "Resources/Textures/AwesomeFace.png");
	GenTexture(Texture_NoCap, "Resources/Textures/Capguy_Walk.png");

	g_Hexagon.m_VertexArray.vertices = {
		//Pos					//Col					//Texture Coords
		-0.5f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		0.25f, 1.0f,		//Top - Left
		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.5f,		//Mid - Left
		-0.5f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.25f, 0.0f,		//Bot - Left
		0.5f, -1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.75f, 0.0f,		//Bot - Right
		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.5f,		//Mid - Right
		0.5f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,		0.75f, 1.0f,		//Top - Right
	};

	g_Hexagon.m_VertexArray.indices = {
		0, 1, 2,
		0, 2, 5,
		5, 2, 4,
		4, 2, 3,
	};

	g_Hexagon.m_position = glm::vec3(0.25f, 0.25f, 0.0f);
	g_Hexagon.m_rotation = 90.0f;
	g_Hexagon.m_scale = glm::vec3(0.5f, 0.5f, 1.0f);
	g_Hexagon.m_useScreenScale = true;

	g_Hexagon.m_program = Program_ClipSpace;

	g_Hexagon.GenBindVerts();

	g_Hexagon.AddUniform(new ImageUniform(Texture_Rayman), "ImageTexture");
	g_Hexagon.AddUniform(new ImageUniform(Texture_Awesome), "ImageTexture1");
	g_Hexagon.AddUniform(new FloatUniform(CurrentTime), "CurrentTime");
	g_Hexagon.AddUniform(new Mat4Uniform(g_Hexagon.m_PVMMat), "PVMMat");

	g_Rectangle.m_VertexArray.vertices = {
		//Pos					//Col					//Texture Coords
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,		//Top - Left
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,		//Bot - Left
		0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,		//Bot - Right
		0.5f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,		//Top - Right
	};

	g_Rectangle.m_VertexArray.indices = {
		0, 1, 2,
		0, 2, 3
	};

	g_Rectangle.m_position = glm::vec3(-0.25f, -0.25f, 0.0f);
	g_Rectangle.m_rotation = 0.0f;
	g_Rectangle.m_scale = glm::vec3(0.5f, 1.0f, 1.0f);
	g_Rectangle.m_useScreenScale = true;

	g_Rectangle.m_program = Program_Texture;

	g_Rectangle.AddUniform(new AnimationUniform(Texture_NoCap, 8, 0.1f, &g_Rectangle), "ImageTexture");
	g_Rectangle.AddUniform(new FloatUniform(0), "offset");
	g_Rectangle.AddUniform(new Mat4Uniform(g_Rectangle.m_PVMMat), "PVMMat");

	g_Rectangle.GenBindVerts();
}

void GenTexture(GLuint& texture, const char* texPath)
{
	//Load the image data
	int ImageWidth;
	int ImageHeight;
	int ImageComponents;
	unsigned char* ImageData = stbi_load(texPath, &ImageWidth, &ImageHeight, &ImageComponents, 0);
	//Gen and bind texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Check how many components in image (RGBA or RGB)
	GLint LoadedComponents = ((ImageComponents == 4) ? GL_RGBA : GL_RGB);

	//Populate the texture with the image data
	glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponents, ImageWidth, ImageHeight, 0, LoadedComponents, GL_UNSIGNED_BYTE, ImageData);

	//Generating the mipmaps, free the memory and unbind the texture
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	stbi_image_free(ImageData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//Called each frame
void Update()
{
	CurrentTime = (float)glfwGetTime();
	g_Hexagon.currentTime = CurrentTime;
	g_Rectangle.currentTime = CurrentTime;

	//std::cout << CurrentTime << std::endl;

	UpdatePVM(g_Hexagon);

	UpdatePVM(g_Rectangle);

	g_Hexagon.UpdateUniform(new Mat4Uniform(g_Hexagon.m_PVMMat), "PVMMat");
	g_Hexagon.UpdateUniform(new FloatUniform(g_Hexagon.currentTime), "CurrentTime");

	//g_Rectangle.UpdateUniform(new FloatUniform(CurrentTime), "offset");
	g_Rectangle.UpdateUniform(new FloatUniform(g_Rectangle.currentTime), "CurrentTime");
	g_Rectangle.UpdateUniform(new Mat4Uniform(g_Rectangle.m_PVMMat), "PVMMat");

	CheckInput();
}

void UpdatePVM(CShape &_shape)
{
	_shape.m_translationMat = glm::translate(glm::mat4(), _shape.m_position);
	_shape.m_rotationMat = glm::rotate(glm::mat4(), glm::radians(_shape.m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	_shape.m_scaleMat = glm::scale(glm::mat4(), _shape.m_scale);

	glm::mat4 pixelScale = (_shape.m_useScreenScale ? glm::scale(glm::mat4(), glm::vec3(width / 2, height / 2, 1)) : glm::scale(glm::mat4(), glm::vec3(1, 1, 1)));

	_shape.m_modelMat = pixelScale * _shape.m_translationMat * _shape.m_rotationMat * _shape.m_scaleMat;

	//Ortho project
	float halfWindowWidth = (float)width / 2.0f;
	float halfWindowHeight = (float)height / 2.0f;
	ProjectionMat = glm::ortho(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, 0.1f, 100.0f);

	//Perspective project
	//ProjectionMat = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	ViewMat = glm::lookAt(CameraPos, CameraPos + CameraLookDir, CameraUpDir);

	_shape.m_PVMMat = ProjectionMat * ViewMat * _shape.m_modelMat;
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

	g_Hexagon.Render();

	g_Rectangle.Render();
	
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