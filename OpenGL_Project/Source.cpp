// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// (c) 2021 Media Design School
//
// File Name   : Source.cpp
// Description : Main file containing most/all logic
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#include <glew.h>
#include <glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

#include "Source.h"
#include "ShaderLoader.h"
#include "TextLabel.h"
#include "CCamera.h"

#include "CShape.h"
#include "CUniform.h"

#include "CAudioSystem.h"

#include "Utility.h"
#include "CObjectManager.h"
#include "CLightManager.h"

#pragma region Function Headers
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void MouseCallback(GLFWwindow* window, int button, int action, int mods);

void TextInput(GLFWwindow* window, unsigned int codePoint);

bool Startup();
void InitialSetup();

void TextureCreation();
void MeshCreation();
void ObjectCreation();

void ProgramSetup();

bool AudioInit();

void GenTexture(GLuint& texture, const char* texPath);
void GenCubemap(GLuint& texture, std::string texPath[6]);

void Update();
void CheckInput(float _deltaTime, float _currentTime);
void Render();

void Print(int x, int y, std::string str, int effect);
void SlowPrint(int x, int y, std::string _message, int effect, int _wait);
void GotoXY(int x, int y);

#pragma endregion

//Program render window
GLFWwindow* g_window = nullptr;

//Main camera
CCamera* g_camera = new CCamera();

//Storing previous time step
float previousTimeStep = 0;

//Enable and disable input
bool doInput = false;

bool cursorLocked = false;

//Textures
GLuint Texture_Rayman;
GLuint Texture_Awesome;
GLuint Texture_CapMan;
GLuint Texture_Frac;
GLuint Texture_Floor;
GLuint Texture_Crate;

GLuint Texture_Cubemap;

GLuint Texture_CrateReflectionMap;

//Text objects
TextLabel* Text_Message;
TextLabel* Text_Message2;

int main() {

	//Setup for use of OpenGL
	if (!Startup()) return -1;

	//Setup project specific settings
	InitialSetup();

	//Main Loop
	while (!glfwWindowShouldClose(g_window)) {
		glfwPollEvents();

		//Update all objects and run processes
		Update();

		//Render all the objects
		Render();
	}

	//Release all audio before quitting
	CAudioSystem::GetInstance().ReleaseAll();

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
	glfwWindowHint(GLFW_SAMPLES, 4);

	//Setting console cursor visibilty
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO     cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(out, &cursorInfo);

	//Create a GLFW controlled context window
	g_window = glfwCreateWindow(utils::windowWidth, utils::windowHeight, "Keane Carotenuto - Summative  1", NULL, NULL);

	//Check for failure
	if (g_window == NULL) {
		std::cout << "GLFW failed to init. Exiting" << std::endl;
		system("pause");

		glfwTerminate();
		return false;
	}

	//Set current context to the new window
	glfwMakeContextCurrent(g_window);


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

	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);

	// Maps the range of the window size to NDC (-1 -> 1)
	glViewport(0, 0, utils::windowWidth, utils::windowHeight);

	glfwSetKeyCallback(g_window, KeyCallback);
	glfwSetMouseButtonCallback(g_window, MouseCallback);

	//Fill polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Multi sampling
	glEnable(GL_MULTISAMPLE);

	//Cull polygons not facing
	glCullFace(GL_BACK);

	//Set winding order of verticies (for front and back facing)
	glFrontFace(GL_CCW);

	//Enable Culling
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Create textures
	TextureCreation();

	//Create default meshes
	MeshCreation();

	//Create objects
	ObjectCreation();

	//Set up shaders
	ProgramSetup();

	CLightManager::AddLight(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.05f, 1.0f, 35);
	CLightManager::AddLight(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.05f, 1.0f, 20);
	CLightManager::AddLight(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.05f, 1.0f, 10);
	CLightManager::UpdateUniforms(ShaderLoader::GetProgram("3DLight")->m_id);

	CShape* _shape = nullptr;

	for (int i = 0; i < CLightManager::GetMaxPointLights(); i++) {

		if (CLightManager::GetPointLight(i).Colour == glm::vec3(0,0,0)) continue;

		std::string name = "lSphere" + std::to_string(i);
		CObjectManager::AddShape(name, new CShape("sphere", CLightManager::GetPointLight(i).Position, 0.0f, glm::vec3(0.2f, 0.2f, 0.2f), false));
		CObjectManager::GetShape(name)->SetCamera(g_camera);

		//Set program and add uniforms to Cube
		if (_shape = CObjectManager::GetShape(name)) {
			_shape->SetProgram(ShaderLoader::GetProgram("solidColour")->m_id);
			_shape->AddUniform(new Vec3Uniform(CLightManager::GetPointLight(i).Colour, "Colour"));
			_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		}
	}

	//Set up Audio files
	AudioInit();

	system("CLS");
}

#pragma region Creation Functions

/// <summary>
/// Vind all textures used
/// </summary>
void TextureCreation()
{
	//Create all textures and bind them
	GenTexture(Texture_Rayman, "Resources/Textures/Rayman.jpg");
	GenTexture(Texture_Awesome, "Resources/Textures/AwesomeFace.png");
	GenTexture(Texture_CapMan, "Resources/Textures/Capguy_Walk.png");
	GenTexture(Texture_Frac, "Resources/Textures/pal.png");
	GenTexture(Texture_Floor, "Resources/Textures/Floor.jpg");
	GenTexture(Texture_Crate, "Resources/Textures/Crate.jpg");
	GenTexture(Texture_CrateReflectionMap, "Resources/Textures/Crate-Reflection.png");

	std::string cubemapPaths[6] = {
		"MountainOutpost/Right.jpg",
		"MountainOutpost/Left.jpg",
		"MountainOutpost/Top.jpg",
		"MountainOutpost/Bottom.jpg",
		"MountainOutpost/Back.jpg",
		"MountainOutpost/Front.jpg",
	};
	GenCubemap(Texture_Cubemap, cubemapPaths);
}

/// <summary>
/// Create default meshes to be used by shapes
/// </summary>
void MeshCreation()
{
	//Create cube mesh
	CMesh::NewCMesh(
		"cube",
		VertType::Pos_Col_Tex,
		{
			// Index        // Position                     //Texture Coords
			//Front Quad
			/* 00 */        -0.5f,  0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 00 */
			/* 01 */        -0.5f, -0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 01 */
			/* 02 */         0.5f, -0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 02 */
			/* 03 */         0.5f,  0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 03 */
			//Back Quad
			/* 04 */         0.5f,  0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 04 */
			/* 05 */         0.5f, -0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 05 */
			/* 06 */        -0.5f, -0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 06 */
			/* 07 */        -0.5f,  0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 07 */
			//Right
			/* 08 */         0.5f,  0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 03 */
			/* 09 */         0.5f, -0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 02 */
			/* 10 */         0.5f, -0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 05 */
			/* 11 */         0.5f,  0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 04 */
			//Left
			/* 12 */        -0.5f,  0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 07 */
			/* 13 */        -0.5f, -0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 06 */
			/* 14 */        -0.5f, -0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 01 */
			/* 15 */        -0.5f,  0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 00 */
			//Top
			/* 16 */        -0.5f,  0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 07 */
			/* 17 */        -0.5f,  0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 00 */
			/* 18 */         0.5f,  0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 03 */
			/* 19 */         0.5f,  0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 04 */
			//Bottom
			/* 20 */        -0.5f, -0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 01 */
			/* 21 */        -0.5f, -0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 06 */
			/* 22 */         0.5f, -0.5f, -0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 05 */
			/* 23 */         0.5f, -0.5f,  0.5f,	1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 02 */
		},
		{
			0, 1, 2, // Front Tri 1
			0, 2, 3, // Front Tri 2

			4, 5, 6, // Back Tri 1
			4, 6, 7, // Back Tri 2

			8, 9,  10, // Right Tri 1
			8, 10, 11, // Right Tri 2

			12, 13, 14, // Left Tri 1
			12, 14, 15, // Left Tri 2

			16, 17, 18, // Top Tri 1
			16, 18, 19, // Top Tri 2

			20, 21, 22, // Bottom Tri 1
			20, 22, 23, // Bottom Tri 2
		}
		);

	CMesh::NewCMesh(
		"cubeNorm",
		VertType::Pos_Tex_Norm,
		{
			// Index        // Position					//Texture Coords	//Normal
			//Front Quad
			/* 00 */        -0.5f,  0.5f,  0.5f,		0.0f, 1.0f,			0.0f,  0.0f,  1.0f,   /* 00 */
			/* 02 */        -0.5f, -0.5f,  0.5f,		0.0f, 0.0f,			0.0f,  0.0f,  1.0f,   /* 02 */
			/* 03 */         0.5f, -0.5f,  0.5f,		1.0f, 0.0f,			0.0f,  0.0f,  1.0f,   /* 03 */
			/* 01 */         0.5f,  0.5f,  0.5f,		1.0f, 1.0f,			0.0f,  0.0f,  1.0f,   /* 01 */
			//Back Quad
			/* 04 */         0.5f,  0.5f, -0.5f,		0.0f, 1.0f,			0.0f,  0.0f,  -1.0f,   /* 04 */
			/* 05 */         0.5f, -0.5f, -0.5f,		0.0f, 0.0f,			0.0f,  0.0f,  -1.0f,   /* 05 */
			/* 06 */        -0.5f, -0.5f, -0.5f,		1.0f, 0.0f,			0.0f,  0.0f,  -1.0f,   /* 06 */
			/* 07 */        -0.5f,  0.5f, -0.5f,		1.0f, 1.0f,			0.0f,  0.0f,  -1.0f,   /* 07 */
			//Right
			/* 08 */         0.5f,  0.5f,  0.5f,		0.0f, 1.0f,			1.0f,  0.0f,  0.0f,   /* 03 */
			/* 09 */         0.5f, -0.5f,  0.5f,		0.0f, 0.0f,			1.0f,  0.0f,  0.0f,   /* 02 */
			/* 10 */         0.5f, -0.5f, -0.5f,		1.0f, 0.0f,			1.0f,  0.0f,  0.0f,   /* 05 */
			/* 11 */         0.5f,  0.5f, -0.5f,		1.0f, 1.0f,			1.0f,  0.0f,  0.0f,   /* 04 */
			//Left
			/* 12 */        -0.5f,  0.5f, -0.5f,		0.0f, 1.0f,			-1.0f,  0.0f,  0.0f,   /* 07 */
			/* 13 */        -0.5f, -0.5f, -0.5f,		0.0f, 0.0f,			-1.0f,  0.0f,  0.0f,   /* 06 */
			/* 14 */        -0.5f, -0.5f,  0.5f,		1.0f, 0.0f,			-1.0f,  0.0f,  0.0f,   /* 01 */
			/* 15 */        -0.5f,  0.5f,  0.5f,		1.0f, 1.0f,			-1.0f,  0.0f,  0.0f,   /* 00 */
			//Top
			/* 16 */        -0.5f,  0.5f, -0.5f,		0.0f, 1.0f,			0.0f,  1.0f,  0.0f,   /* 07 */
			/* 17 */        -0.5f,  0.5f,  0.5f,		0.0f, 0.0f,			0.0f,  1.0f,  0.0f,   /* 00 */
			/* 18 */         0.5f,  0.5f,  0.5f,		1.0f, 0.0f,			0.0f,  1.0f,  0.0f,   /* 03 */
			/* 19 */         0.5f,  0.5f, -0.5f,		1.0f, 1.0f,			0.0f,  1.0f,  0.0f,   /* 04 */
			//Bottom
			/* 20 */        -0.5f, -0.5f,  0.5f,		0.0f, 1.0f,			0.0f,  -1.0f,  0.0f,   /* 01 */
			/* 21 */        -0.5f, -0.5f, -0.5f,		0.0f, 0.0f,			0.0f,  -1.0f,  0.0f,   /* 06 */
			/* 22 */         0.5f, -0.5f, -0.5f,		1.0f, 0.0f,			0.0f,  -1.0f,  0.0f,   /* 05 */
			/* 23 */         0.5f, -0.5f,  0.5f,		1.0f, 1.0f,			0.0f,  -1.0f,  0.0f,   /* 02 */
		},
		{
			0, 1, 2, // Front Tri 1
			0, 2, 3, // Front Tri 2

			4, 5, 6, // Back Tri 1
			4, 6, 7, // Back Tri 2

			8, 9,  10, // Right Tri 1
			8, 10, 11, // Right Tri 2

			12, 13, 14, // Left Tri 1
			12, 14, 15, // Left Tri 2

			16, 17, 18, // Top Tri 1
			16, 18, 19, // Top Tri 2

			20, 21, 22, // Bottom Tri 1
			20, 22, 23, // Bottom Tri 2
		}
	);

	CMesh::NewCMesh(
		"skybox",
		VertType::Pos,
		{
			// Index        // Position			
			//Front 
			/* 00 */        -0.5f,  0.5f,  0.5f,
			/* 02 */        -0.5f, -0.5f,  0.5f,
			/* 03 */         0.5f, -0.5f,  0.5f,
			/* 01 */         0.5f,  0.5f,  0.5f,
			//Back 
			/* 04 */         0.5f,  0.5f, -0.5f,
			/* 05 */         0.5f, -0.5f, -0.5f,
			/* 06 */        -0.5f, -0.5f, -0.5f,
			/* 07 */        -0.5f,  0.5f, -0.5f,
			//
			/* 08 */         0.5f,  0.5f,  0.5f,
			/* 09 */         0.5f, -0.5f,  0.5f,
			/* 10 */         0.5f, -0.5f, -0.5f,
			/* 11 */         0.5f,  0.5f, -0.5f,
			//
			/* 12 */        -0.5f,  0.5f, -0.5f,
			/* 13 */        -0.5f, -0.5f, -0.5f,
			/* 14 */        -0.5f, -0.5f,  0.5f,
			/* 15 */        -0.5f,  0.5f,  0.5f,
			//
			/* 16 */        -0.5f,  0.5f, -0.5f,
			/* 17 */        -0.5f,  0.5f,  0.5f,
			/* 18 */         0.5f,  0.5f,  0.5f,
			/* 19 */         0.5f,  0.5f, -0.5f,
			//
			/* 20 */        -0.5f, -0.5f,  0.5f,
			/* 21 */        -0.5f, -0.5f, -0.5f,
			/* 22 */         0.5f, -0.5f, -0.5f,
			/* 23 */         0.5f, -0.5f,  0.5f,
		},
		{
			0,	2,	1,	// Front Tri 1
			0,	3,	2,	// Front Tri 2

			4,	6,	5,	// Back Tri 1
			4,	7,	6,	// Back Tri 2

			8,	10,	9,   // Right Tri 1
			8,	11,	10,  // Right Tri 2

			12,	14,	13,  // Left Tri 1
			12, 15,	14,  // Left Tri 2

			16, 18,	17,  // Top Tri 1
			16, 19,	18,  // Top Tri 2

			20, 22,	21,  // Bottom Tri 1
			20, 23,	22,  // Bottom Tri 2
		}
		);

	//Create cube mesh
	CMesh::NewCMesh(
		"square",
		VertType::Pos_Col_Tex,
		{
			// Index        // Position                     //Texture Coords
			//Front Quad
			/* 00 */        -0.5f,  0.5f,  0.0f,	-1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 00 */
			/* 01 */        -0.5f, -0.5f,  0.0f,	-1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 01 */
			/* 02 */         0.5f, -0.5f,  0.0f,	-1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 02 */
			/* 03 */         0.5f,  0.5f,  0.0f,	-1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 03 */
		},
		{
			0, 1, 2, // Front Tri 1
			0, 2, 3, // Front Tri 2
		}
		);

	//Create cube mesh
	CMesh::NewCMesh(
		"floor-square",
		VertType::Pos_Col_Tex,
		{
			// Index        // Position                     //Texture Coords
			//Front Quad
			/* 00 */        -0.5f,  0.0f,  0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 50.0f,     /* 00 */
			/* 01 */        -0.5f,  0.0f, -0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 01 */
			/* 02 */         0.5f,  0.0f, -0.5f,	-1.0f,  1.0f,  1.0f,         50.0f, 0.0f,     /* 02 */
			/* 03 */         0.5f,  0.0f,  0.5f,	-1.0f,  1.0f,  1.0f,         50.0f, 50.0f,     /* 03 */
		},
		{
			0, 2, 1, // Front Tri 1
			0, 3, 2, // Front Tri 2
		}
		);

	//Create cube mesh
	CMesh::NewCMesh(
		"floor-squareNorm",
		VertType::Pos_Tex_Norm,
		{
			// Index        // Position				//Texture Coords
			//Front Quad
			/* 00 */        -0.5f,  0.0f,  0.5f,	0.0f, 50.0f,			0.0f,  1.0f,  0.0f,             /* 00 */
			/* 01 */        -0.5f,  0.0f, -0.5f,	0.0f, 0.0f,				0.0f,  1.0f,  0.0f,            /* 01 */
			/* 02 */         0.5f,  0.0f, -0.5f,	50.0f, 0.0f,			0.0f,  1.0f,  0.0f,             /* 02 */
			/* 03 */         0.5f,  0.0f,  0.5f,	50.0f, 50.0f,			0.0f,  1.0f,  0.0f,              /* 03 */
		},
		{
			0, 2, 1, // Front Tri 1
			0, 3, 2, // Front Tri 2
		}
		);

	CMesh::NewCMesh("sphere", 0.5f, 15);
}

int randSphereAmount = 10;

/// <summary>
/// Create all shapes used in program
/// </summary>
void ObjectCreation()
{

	CObjectManager::AddShape("floor", new CShape("floor-squareNorm", glm::vec3(0.0f, -0.5f, 0.0f), 0.0f, glm::vec3(100.0f, 1.0f, 100.0f), false));
	CObjectManager::GetShape("floor")->SetCamera(g_camera);

	CObjectManager::AddShape("sphere1", new CShape("sphere", glm::vec3(5.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("sphere1")->SetCamera(g_camera);

	CObjectManager::AddShape("sphere2", new CShape("sphere", glm::vec3(0.0f, 2.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("sphere2")->SetCamera(g_camera);

	CObjectManager::AddShape("sphere3", new CShape("sphere", glm::vec3(0.0f, 4.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("sphere3")->SetCamera(g_camera);

	for (int i = 4; i < 4 + randSphereAmount; i++) {
		float randScale = (float)(rand() % 200)/100 + 0.5f;
		CObjectManager::AddShape("sphere" + std::to_string(i), new CShape("sphere", glm::vec3(rand() % 10 + 5, rand() % 10 + 5, rand() % 10 + 5), 0.0f, glm::vec3(randScale), false));
		CObjectManager::GetShape("sphere" + std::to_string(i))->SetCamera(g_camera);
	}

	CObjectManager::AddShape("cube2", new CShape("cubeNorm", glm::vec3(-5.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("cube2")->SetCamera(g_camera);

	CObjectManager::AddShape("skybox", new CShape("skybox", glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(2000.0f, 2000.0f, 2000.0f), false));
	CObjectManager::GetShape("skybox")->SetCamera(g_camera);
}

#pragma endregion

/// <summary>
/// Set up shaders and shader programs
/// </summary>
void ProgramSetup()
{
	//Create programs
	ShaderLoader::CreateProgram("texture", "Resources/Shaders/ClipSpace.vert", "Resources/Shaders/Texture.frag" );
	ShaderLoader::CreateProgram("clipSpace", "Resources/Shaders/ClipSpace.vert", "Resources/Shaders/TextureMix.frag" );
	ShaderLoader::CreateProgram("clipSpaceFade", "Resources/Shaders/ClipSpace.vert", "Resources/Shaders/VertexColorFade.frag" );
	ShaderLoader::CreateProgram("clipSpaceFractal", "Resources/Shaders/WorldSpace.vert", "Resources/Shaders/Fractal.frag" );
	ShaderLoader::CreateProgram("text", "Resources/Shaders/Text.vert", "Resources/Shaders/Text.frag" );
	ShaderLoader::CreateProgram("textScroll", "Resources/Shaders/TextScroll.vert", "Resources/Shaders/TextScroll.frag" );
	ShaderLoader::CreateProgram("3DLight", "Resources/Shaders/3D_Normals.vert", "Resources/Shaders/3DLight_BlinnPhong.frag" );
	ShaderLoader::CreateProgram("skybox", "Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag" );
	ShaderLoader::CreateProgram("solidColour", "Resources/Shaders/PositionOnly.vert", "Resources/Shaders/ColourOnly.frag");

	CShape* _shape = nullptr;
	

	//Set program and add uniforms to Rectangle
	if (_shape = CObjectManager::GetShape("floor")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Floor, "ImageTexture"));
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
		_shape->AddUniform(new FloatUniform(0.02f, "Reflectivity"));
		_shape->AddUniform(new BoolUniform(false, "hasRefMap"));
		_shape->AddUniform(new FloatUniform(0, "RimExponent"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}

	//Set program and add uniforms to Cube
	if (_shape = CObjectManager::GetShape("sphere1")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Rayman, "ImageTexture"));
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
		_shape->AddUniform(new FloatUniform(0.5f, "Reflectivity"));
		_shape->AddUniform(new BoolUniform(false, "hasRefMap"));
		_shape->AddUniform(new FloatUniform(5, "RimExponent"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(1.0f,0.0f,0.0f), "RimColour"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(1.0f,0.0f,0.0f), "Colour"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}

	//Set program and add uniforms to Cube
	if (_shape = CObjectManager::GetShape("sphere2")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Rayman, "ImageTexture"));
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
		_shape->AddUniform(new FloatUniform(0.5f, "Reflectivity"));
		_shape->AddUniform(new BoolUniform(false, "hasRefMap"));
		_shape->AddUniform(new FloatUniform(5, "RimExponent"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(0.0f, 1.0f, 0.0f), "RimColour"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}

	//Set program and add uniforms to Cube
	if (_shape = CObjectManager::GetShape("sphere3")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Rayman, "ImageTexture"));
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
		_shape->AddUniform(new FloatUniform(0.5f, "Reflectivity"));
		_shape->AddUniform(new BoolUniform(false, "hasRefMap"));
		_shape->AddUniform(new FloatUniform(5, "RimExponent"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(0.0f, 0.0f, 1.0f), "RimColour"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}

	for (int i = 4; i < 4 + randSphereAmount; i++) {
		//Set program and add uniforms to Cube
		if (_shape = CObjectManager::GetShape("sphere" + std::to_string(i))) {
			_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
			_shape->AddUniform(new ImageUniform(Texture_Rayman, "ImageTexture"));
			_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
			_shape->AddUniform(new FloatUniform(((float)(rand() % 100)) / 100, "Reflectivity"));
			_shape->AddUniform(new BoolUniform(false, "hasRefMap"));
			_shape->AddUniform(new FloatUniform(0, "RimExponent"));
			_shape->AddUniform(new Vec3Uniform(glm::vec3(0.0f, 0.0f, 0.0f), "RimColour"));
			_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
			_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
			_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
		}
	}
	
	
	//Set program and add uniforms to Cube
	if (_shape = CObjectManager::GetShape("cube2")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Crate, "ImageTexture"));
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
		_shape->AddUniform(new BoolUniform(true, "hasRefMap"));
		_shape->AddUniform(new ImageUniform(Texture_CrateReflectionMap, "ReflectionMap"));
		_shape->AddUniform(new FloatUniform(0.5f, "Reflectivity"));
		_shape->AddUniform(new FloatUniform(0, "RimExponent"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}
	
	//Set program and add uniforms to skybox
	if (_shape = CObjectManager::GetShape("skybox")) {
		_shape->SetProgram(ShaderLoader::GetProgram("skybox")->m_id);
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "ImageTexture"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
	}
}

/// <summary>
/// Bind audio files
/// </summary>
/// <returns></returns>
bool AudioInit()
{
	CAudioSystem::GetInstance().AddSong("DanceTrack", "Resources/Audio/DanceTrack.mp3", FMOD_LOOP_NORMAL);
	CAudioSystem::GetInstance().AddSong("Gunshot", "Resources/Audio/Gunshot.wav", FMOD_DEFAULT);
	CAudioSystem::GetInstance().AddSong("Enabled", "Resources/Audio/Enabled.wav", FMOD_DEFAULT);
	CAudioSystem::GetInstance().AddSong("Disabled", "Resources/Audio/Disabled.wav", FMOD_DEFAULT);

	return true;
}

#pragma region Callback Functions

/// <summary>
/// Callback for individual keys
/// </summary>
/// <param name="window">window to check on</param>
/// <param name="key">key pressed</param>
/// <param name="scancode"></param>
/// <param name="action">how is the key being used</param>
/// <param name="mods"></param>
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Quit if ESC key pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	//Change line mode (fill or line) with F
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		GLboolean isCull = glIsEnabled(GL_CULL_FACE);

		if (isCull) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
		}
	}

	//Change backface mode (cull or no cull) with B
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		GLint polygonMode[2];
		glGetIntegerv(GL_POLYGON_MODE, polygonMode);

		glPolygonMode(GL_FRONT_AND_BACK, (polygonMode[0] == GL_FILL ? GL_LINE : GL_FILL));
	}

	//Hide or show cursor
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		GLuint mode = glfwGetInputMode(window, GLFW_CURSOR);

		cursorLocked = (mode == GLFW_CURSOR_DISABLED ? false : true);

		glfwSetInputMode(window,  GLFW_CURSOR, (mode == GLFW_CURSOR_NORMAL ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
	}
}

/// <summary>
/// Mouse click callback
/// </summary>
/// <param name="window">window to check on</param>
/// <param name="button">button pressed</param>
/// <param name="action">action performed</param>
/// <param name="mods"></param>
void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
}

/// <summary>
/// Callback for any characters pressed
/// </summary>
/// <param name="window">window to check on</param>
/// <param name="codePoint">"character" pressed</param>
void TextInput(GLFWwindow* window, unsigned int codePoint) {
	
}

#pragma endregion

/// <summary>
/// Generates a texture and binds it to a GLuint
/// </summary>
/// <param name="texture">the GLuint to bind</param>
/// <param name="texPath">the folder pathing + name of file</param>
void GenTexture(GLuint& texture, const char* texPath)
{
	stbi_set_flip_vertically_on_load(true);

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

	//Unbind and free texture and image
	stbi_image_free(ImageData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/// <summary>
/// Generates a cubemap texture and binds it to a GLuint
/// </summary>
/// <param name="texture">the GLuint to bind</param>
/// <param name="texPath">(name of folder +) name of cubemap file</param>
void GenCubemap(GLuint& texture, std::string texPath[6])
{
	stbi_set_flip_vertically_on_load(false);

	//Load the image data
	int ImageWidth;
	int ImageHeight;
	int ImageComponents;

	//Gen and bind texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	for (int i = 0; i < 6; i++) {
		std::string fullFilePath = "Resources/Textures/Cubemaps/" + texPath[i];
		unsigned char* ImageData = stbi_load(fullFilePath.c_str(), &ImageWidth, &ImageHeight, &ImageComponents, 0);

		//Check how many components in image (RGBA or RGB)
		GLint LoadedComponents = ((ImageComponents == 4) ? GL_RGBA : GL_RGB);

		//Populate the texture with the image data
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, LoadedComponents, ImageWidth, ImageHeight, 0, LoadedComponents, GL_UNSIGNED_BYTE, ImageData);

		//Unbind and free texture and image
		stbi_image_free(ImageData);
	}

	//Clamp to edge to make sure no seams appear
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Generating the mipmaps, free the memory and unbind the texture
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


/// <summary>
/// Called every frame
/// </summary>
void Update()
{
	//Get current time and calc delta time
	utils::currentTime = (float)glfwGetTime();
	float DeltaTime = utils::currentTime - previousTimeStep;
	previousTimeStep = utils::currentTime;

	//Move shapes around world origin in circle
	CObjectManager::GetShape("sphere1")->SetPosition(glm::vec3(sin(utils::currentTime + glm::pi<float>())*2, 0, cos(utils::currentTime + glm::pi<float>())*2));
	CObjectManager::GetShape("sphere2")->SetPosition(glm::vec3(0, sin(utils::currentTime) * 2 + 2, 0));
	CObjectManager::GetShape("sphere3")->SetPosition(glm::vec3(0, sin(utils::currentTime) * 2 + 4, 0));

	//Update all shapes
	CObjectManager::UpdateAll(DeltaTime, utils::currentTime);

	glUseProgram(ShaderLoader::GetProgram("3DLight")->m_id);
	glUniform3fv(glGetUniformLocation(ShaderLoader::GetProgram("3DLight")->m_id, "CameraPos"), 1, glm::value_ptr(g_camera->GetCameraPos()));
	glUseProgram(0);

	//Check for input
	CheckInput(DeltaTime, utils::currentTime);

	CLightManager::UpdateUniforms(ShaderLoader::GetProgram("3DLight")->m_id);

	//Update the FMOD Audio System
	CAudioSystem::GetInstance().Update();
}

/// <summary>
/// Check keys pressed for input during game during/after update call
/// </summary>
/// <param name="_deltaTime"></param>
/// <param name="_currentTime"></param>
void CheckInput(float _deltaTime, float _currentTime)
{
	//Update mouse data
	glm::vec2 oldMouse = utils::mousePos;

	double xPos;
	double yPos;
	glfwGetCursorPos(g_window, &xPos, &yPos);
	utils::mousePos = glm::vec2(xPos, utils::windowHeight - yPos);
	if (oldMouse == glm::vec2()) oldMouse = utils::mousePos;
	Print(5, 5, "Mouse Position (x: " + std::to_string((int)utils::mousePos.x) + " y:" + std::to_string((int)utils::mousePos.y) + ")    ", 15);

	if (cursorLocked) {
		//Update camera rotation with mouse movement
		g_camera->SetYaw(g_camera->GetYaw() - g_camera->GetSpeed() * (utils::mousePos.x - oldMouse.x) * _deltaTime);
		g_camera->SetPitch(g_camera->GetPitch() - g_camera->GetSpeed() * (utils::mousePos.y - oldMouse.y) * _deltaTime);
		Print(5, 10, "Mouse rotation (yaw: " + std::to_string((int)glm::degrees(g_camera->GetYaw())) + " pitch:" + std::to_string((int)glm::degrees(g_camera->GetPitch())) + ")    ", 15);

		g_camera->UpdateRotation();
	}

	//Camera movement below

	glm::vec3 camMovement = glm::vec3(0, 0, 0);
	float camSpeed = 5.0f;

	//Move camera SWAD
	if (glfwGetKey(g_window, GLFW_KEY_A))
	{
		camMovement += glm::normalize(g_camera->GetCameraRightDir());
	}
	if (glfwGetKey(g_window, GLFW_KEY_D))
	{
		camMovement -= glm::normalize(g_camera->GetCameraRightDir());
	}
	if (glfwGetKey(g_window, GLFW_KEY_W))
	{
		camMovement += glm::normalize(g_camera->GetCameraForwardDir());
	}
	if (glfwGetKey(g_window, GLFW_KEY_S))
	{
		camMovement -= glm::normalize(g_camera->GetCameraForwardDir());
	}
	if (glfwGetKey(g_window, GLFW_KEY_LEFT_SHIFT))
	{
		camMovement += glm::normalize(g_camera->GetCameraUpDir());
	}
	if (glfwGetKey(g_window, GLFW_KEY_LEFT_CONTROL))
	{
		camMovement -= glm::normalize(g_camera->GetCameraUpDir());
	}
	if (glm::length(camMovement) >= 0.01f) {
		camMovement = glm::normalize(camMovement) * camSpeed * _deltaTime;
		g_camera->SetCameraPos(g_camera->GetCameraPos() + camMovement);
	}

	if (glfwGetKey(g_window, GLFW_KEY_R)) {
		glClearStencil(1);
		glClear(GL_STENCIL_BUFFER_BIT);
		glClearStencil(0);
	}
}

/// <summary>
/// Calls render function for objects
/// </summary>
void Render()
{
	//Clear Screen
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_SCISSOR_TEST);
	//glScissor(100, 100, 600, 600);

	////enable stencil and set stencil operation
	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glStencilMask(0xFF); // Enable writing again for next time

	////** 1st pass ** - set current stencil value
	//glStencilFunc(GL_ALWAYS, // test function
	//	1,// current value to set
	//	0xFF);//mask value,
	//glStencilMask(0xFF);//enable writing to stencil buffer
	//CObjectManager::GetShape("sphere1")->Render();

	//glStencilFunc(GL_ALWAYS, // test function
	//	0,// current value to set
	//	0xFF);//mask value,
	//CObjectManager::GetShape("sphere2")->Render();

	//glDisable(GL_SCISSOR_TEST);

	//// ** 2nd pass **
	//glStencilFunc(GL_EQUAL, 1, 0xFF); // write to areas where value is equal to 1
	//glStencilMask(0x00); //disable writing to stencil buffer
	//CObjectManager::RenderAll();

	//glStencilMask(0x00); //disable writing to stencil mask
	//glDisable(GL_STENCIL_TEST); // Disable stencil test
	//glStencilMask(0xFF); // Enable writing again for next time

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	CObjectManager::GetShape("floor")->Render();

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	CObjectManager::GetShape("sphere1")->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
	CObjectManager::GetShape("sphere1")->Render();

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	CObjectManager::GetShape("sphere1")->Scale(1.1f);
	CObjectManager::GetShape("sphere1")->SetProgram(ShaderLoader::GetProgram("solidColour")->m_id);
	CObjectManager::GetShape("sphere1")->UpdateUniform(new Vec3Uniform({ 1,0,0 }, "Colour"));
	CObjectManager::GetShape("sphere1")->Render();
	CObjectManager::GetShape("sphere1")->Scale(1.0 / 1.1f);

	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);


	glfwSwapBuffers(g_window);
}

#pragma region "Printing Functions"
/// <summary>
/// Prints out text at position with colour
/// </summary>
/// <param name="x"> pos</param>
/// <param name="y"> pos</param>
/// <param name="str"> string to print</param>
/// <param name="effect"> colour to print with</param>
void Print(int x, int y, std::string str, int effect) {
	GotoXY(x, y);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), effect);
	std::cout << str;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
};

/// <summary>
/// Slowly prints out text at pos with colour 
/// </summary>
/// <param name="x">pos</param>
/// <param name="y">pos</param>
/// <param name="_message">string to pring</param>
/// <param name="effect">colour to print with</param>
/// <param name="_wait">deltay after each character</param>
void SlowPrint(int x, int y, std::string _message, int effect, int _wait) {
	GotoXY(x, y);
	for (wchar_t _char : _message) {
		Sleep(_wait);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), effect);
		std::cout << _char;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}
}

/// <summary>
/// Moves console cursor to specific position
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
void GotoXY(int x, int y) {
	COORD point = { (short)x,(short)y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), point);
};
#pragma endregion