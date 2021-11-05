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
#include <gtx/vector_angle.hpp>

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

void InitShapes();

void GenTexture(GLuint& texture, const char* texPath);
void GenRenderTexture(GLuint& texture, GLuint& frameBuffer, int width, int height);
void GenCubemap(GLuint& texture, std::string texPath[6]);

void Update();
void CheckInput(float _deltaTime, float _currentTime);
float GetTerrainHeight(CShape* floor, float _worldX, float _worldZ);
void Render();

void DrawCirlce(float _rad, glm::vec3 _screenPos, glm::vec3 _colour = glm::vec3(1, 1, 1), int _points = 10);

void RenderOutline();

void Print(int x, int y, std::string str, int effect);
void SlowPrint(int x, int y, std::string _message, int effect, int _wait);
void GotoXY(int x, int y);

#pragma endregion

glm::vec2 utils::mousePos = glm::vec2();
float utils::currentTime = 0.0f;
float utils::deltaTime = 0.0f;
float utils::previousTimeStep = 0.0f;

//Program render window
GLFWwindow* g_window = nullptr;

//Main camera
CCamera* g_camera = new CCamera();

//Enable and disable input
bool doInput = false;

bool cursorLocked = false;

bool outlineSphere = false;
bool holdSphere = false;

//Textures
GLuint Texture_Rayman;
GLuint Texture_Awesome;
GLuint Texture_CapMan;
GLuint Texture_Frac;
GLuint Texture_Floor;
GLuint Texture_Crate;
GLuint Texture_Water;
GLuint Texture_HeightMap;
GLuint Texture_Terrain;

GLuint Texture_RenderTexture;
GLuint frameBuffer;

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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

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

	//Set up shapes
	InitShapes();

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
	GenTexture(Texture_Water, "Resources/Textures/Water.png");
	GenTexture(Texture_HeightMap, "Resources/Textures/HeightMap.png");
	GenTexture(Texture_Terrain, "Resources/Textures/Terrain_Texture.png");
	GenTexture(Texture_CrateReflectionMap, "Resources/Textures/Crate-Reflection.png");

	//Create render texture
	GenRenderTexture(Texture_RenderTexture, frameBuffer, utils::windowWidth, utils::windowHeight);

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
			/* 01 */        -0.5f, -0.5f,  0.5f,		0.0f, 0.0f,			0.0f,  0.0f,  1.0f,   /* 02 */
			/* 02 */         0.5f, -0.5f,  0.5f,		1.0f, 0.0f,			0.0f,  0.0f,  1.0f,   /* 03 */
			/* 03 */         0.5f,  0.5f,  0.5f,		1.0f, 1.0f,			0.0f,  0.0f,  1.0f,   /* 01 */
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
	
	
	CMesh::NewCMesh(
		"squareNorm",
		VertType::Pos_Tex_Norm,
		{
			// Index        // Position			    //Texture Coords	//Normal
			//Front Quad
			/* 00 */        -0.5f,  0.0f,  0.5f,	0.0f,  1.0f,  		0.0f,  1.0f,  0.0f,   /* 00 */
			/* 01 */        -0.5f,  0.0f, -0.5f,	0.0f,  0.0f,  		0.0f,  1.0f,  0.0f,   /* 02 */
			/* 02 */         0.5f,  0.0f, -0.5f,	1.0f,  0.0f,  		0.0f,  1.0f,  0.0f,   /* 03 */
			/* 03 */         0.5f,  0.0f,  0.5f,	1.0f,  1.0f,  		0.0f,  1.0f,  0.0f,   /* 01 */
		},
		{
			0, 2, 1, // Front Tri 1
			0, 3, 2, // Front Tri 2
		}
		);

	CMesh::NewCMesh(
		"quadNorm",
		VertType::Pos_Tex_Norm,
		{
			// Index        // Position			    //Texture Coords	//Normal
			//Front Quad
			/* 00 */        -1.0f,  1.0f, 0.0f, 	0.0f,  1.0f,  		0.0f,  1.0f,  0.0f,   /* 00 */
			/* 01 */        -1.0f, -1.0f, 0.0f, 	0.0f,  0.0f,  		0.0f,  1.0f,  0.0f,   /* 02 */
			/* 02 */         1.0f, -1.0f, 0.0f, 	1.0f,  0.0f,  		0.0f,  1.0f,  0.0f,   /* 03 */
			/* 03 */         1.0f,  1.0f, 0.0f, 	1.0f,  1.0f,  		0.0f,  1.0f,  0.0f,   /* 01 */
		},
		{
			0, 1, 2, // Front Tri 1
			0, 2, 3, // Front Tri 2
		}
		);

	CMesh::NewCMesh(
		"patches",
		VertType::Patches,
		{
			// Index        // Position		
			//Front Quad
			/* 00 */        -1.0f,  1.0f, 0.0f,
			/* 01 */        -1.0f, -1.0f, 0.0f,
			/* 02 */         1.0f, -1.0f, 0.0f,
			/* 03 */         1.0f,  1.0f, 0.0f,
		},
		{
			0, 1, 2, 3
		}
		);

	//Create cube mesh
	CMesh::NewCMesh(
		"floor-square",
		VertType::Pos_Col_Tex,
		{
			// Index        // Position											//Texture Coords
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

	//Create Colour Point Mesh
		CMesh::NewCMesh(
		"point-col",
		VertType::Pos_Col,
		{
			// Index        // Position				//Colour
			//Front Quad
			/* 00 */        0.0f,  0.0f,  0.0f,		0.0f,  1.0f,  0.0f,             /* 00 */
		},
		{
			0
		}
		);

	CMesh::NewCMesh("sphere", 0.5f, 15);

	CMesh::NewPlane("terrain", 200.0f, 200.0f, 500, 500	);
}

int randSphereAmount = 10;

/// <summary>
/// Create all shapes used in program
/// </summary>
void ObjectCreation()
{

	CObjectManager::AddShape("floor", new CShape("terrain", glm::vec3(-100.0f, -10.0f, -100.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("floor")->SetCamera(g_camera);

	CObjectManager::AddShape("sphere1", new CShape("sphere", glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("sphere1")->SetCamera(g_camera);

	CObjectManager::AddShape("cube1", new CShape("cubeNorm", glm::vec3(5.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("cube1")->SetCamera(g_camera);

	CObjectManager::AddShape("point", new CShape("point-col", glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("point")->SetCamera(g_camera);

	CObjectManager::AddShape("renderTexture", new CShape("quadNorm", glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("renderTexture")->m_orthoProject = true;
	CObjectManager::GetShape("renderTexture")->SetCamera(g_camera);

	CObjectManager::AddShape("tess", new CShape("patches", glm::vec3(0.0f, -3.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("tess")->SetCamera(g_camera);

	CObjectManager::AddShape("water1", new CShape("squareNorm", glm::vec3(0.0f, -5.5f, 0.0f), 0.0f, glm::vec3(325.0f, 1.0f, 325.0f), false));
	CObjectManager::GetShape("water1")->SetCamera(g_camera);

	CObjectManager::AddShape("skybox", new CShape("skybox", glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(2000.0f, 2000.0f, 2000.0f), false));
	CObjectManager::GetShape("skybox")->SetCamera(g_camera);
}

#pragma endregion

#pragma region Setup Functions
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
	ShaderLoader::CreateProgram("ScanlineBlur", "Resources/Shaders/3D_Normals.vert", "Resources/Shaders/TextureScanlineBlur.frag" );
	ShaderLoader::CreateProgram("3Dtexture", "Resources/Shaders/3D_Normals.vert", "Resources/Shaders/TextureOnly.frag" );
	ShaderLoader::CreateProgram("skybox", "Resources/Shaders/Skybox.vert", "Resources/Shaders/Skybox.frag" );
	ShaderLoader::CreateProgram("solidColour", "Resources/Shaders/PositionOnly.vert", "Resources/Shaders/ColourOnly.frag");
	ShaderLoader::CreateProgram("tesShader", "Resources/Shaders/PositionOnlyNoPVM.vert", "Resources/Shaders/ColourOnly.frag", nullptr, "Resources/Shaders/QuadTes.tesc", "Resources/Shaders/QuadTes.tese");
	ShaderLoader::CreateProgram("geom", "Resources/Shaders/GeomVert.vert", "Resources/Shaders/GeomFrag.frag", "Resources/Shaders/GeomGeom.geom");
}

void InitShapes()
{
	CShape* _shape = nullptr;

	//Set program and add uniforms to Rectangle
	if (_shape = CObjectManager::GetShape("floor")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Terrain, "ImageTexture"));
		_shape->AddUniform(new IntUniform(0, "frameCount"));
		_shape->AddUniform(new FloatUniform(0, "offset"));
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
		_shape->AddUniform(new FloatUniform(0.0f, "Reflectivity"));
		_shape->AddUniform(new FloatUniform(5.0f, "Shininess"));
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
		_shape->AddUniform(new IntUniform(0, "frameCount"));
		_shape->AddUniform(new FloatUniform(0, "offset"));
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
		_shape->AddUniform(new FloatUniform(0.5f, "Reflectivity"));
		_shape->AddUniform(new FloatUniform(64.0f, "Shininess"));
		_shape->AddUniform(new BoolUniform(false, "hasRefMap"));
		_shape->AddUniform(new FloatUniform(5, "RimExponent"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(1.0f, 0.0f, 0.0f), "RimColour"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(1.0f, 0.0f, 0.0f), "Colour"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}

	//Set program and add uniforms to Cube
	if (_shape = CObjectManager::GetShape("cube1")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Rayman, "ImageTexture"));
		_shape->AddUniform(new IntUniform(0, "frameCount"));
		_shape->AddUniform(new FloatUniform(0, "offset"));
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
		_shape->AddUniform(new FloatUniform(0.0f, "Reflectivity"));
		_shape->AddUniform(new BoolUniform(false, "hasRefMap"));
		_shape->AddUniform(new FloatUniform(5, "RimExponent"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(1.0f, 0.0f, 0.0f), "RimColour"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(1.0f, 0.0f, 0.0f), "Colour"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}

	//Set program and add uniforms to point
	if (_shape = CObjectManager::GetShape("point")) {
		_shape->SetProgram(ShaderLoader::GetProgram("geom")->m_id);
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
	}

	//Set program and add uniforms to water
	if (_shape = CObjectManager::GetShape("water1")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new AnimationUniform(Texture_Water, 100, 0.1f, _shape,  "ImageTexture"));
		_shape->AddUniform(new IntUniform(100, "frameCount"));
		_shape->AddUniform(new FloatUniform(0, "offset"));
		_shape->AddUniform(new CubemapUniform(Texture_Cubemap, "Skybox"));
		_shape->AddUniform(new FloatUniform(0.1f, "Reflectivity"));
		_shape->AddUniform(new BoolUniform(false, "hasRefMap"));
		_shape->AddUniform(new FloatUniform(0, "RimExponent"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(0.0f, 0.0f, 0.0f), "RimColour"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}
	
	//Set program and add uniforms to renderTexture
	if (_shape = CObjectManager::GetShape("renderTexture")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3Dtexture")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_RenderTexture, "ImageTexture"));
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}

	//Set program and add uniforms to renderTexture
	if (_shape = CObjectManager::GetShape("tess")) {
		_shape->SetProgram(ShaderLoader::GetProgram("tesShader")->m_id);
		_shape->AddUniform(new FloatUniform(0, "CurrentTime"));
		_shape->AddUniform(new FloatUniform(0, "Distance"));
		_shape->AddUniform(new Vec3Uniform(glm::vec3(0,1,0), "Colour"));
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
#pragma endregion

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
	/*if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}*/

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


	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		if (!holdSphere && outlineSphere) {
			holdSphere = true;
		}
		else if (holdSphere) {
			holdSphere = false;
		}
	}

	//Change line mode (fill or line) with F
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		CObjectManager::DeleteAll();

		//Create objects
		ObjectCreation();
		//Set up shapes
		InitShapes();

		g_camera->SetCameraPos({ 0,0,0 });
		g_camera->SetYaw(0);
		g_camera->SetPitch(0);
		g_camera->UpdateRotation();
	}

	//Change backface mode (cull or no cull) with B
	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		g_camera->followTerrain = !g_camera->followTerrain;
		CShape* _shape = CObjectManager::GetShape("renderTexture");
		_shape->SetProgram(ShaderLoader::GetProgram(g_camera->followTerrain ? "ScanlineBlur" : "3Dtexture")->m_id);
		_shape->UpdateUniform(new ImageUniform(Texture_RenderTexture, "ImageTexture"));
		_shape->UpdateUniform(new FloatUniform(0, "CurrentTime"));
		_shape->UpdateUniform(new Mat4Uniform(_shape->GetPVM(), "PVMMat"));
		_shape->UpdateUniform(new Mat4Uniform(_shape->GetPVM(), "Model"));
	}

	//Change backface mode (cull or no cull) with B
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		GLint polygonMode[2];
		glGetIntegerv(GL_POLYGON_MODE, polygonMode);

		glPolygonMode(GL_FRONT_AND_BACK, (polygonMode[0] == GL_FILL ? GL_LINE : GL_FILL));
	}

	//Hide or show cursor
	if ((key == GLFW_KEY_TAB || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
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

	double xPos;
	double yPos;
	glfwGetCursorPos(g_window, &xPos, &yPos);
	utils::mousePos = glm::vec2(xPos, utils::windowHeight - yPos);

	//get mouse ray from camera
	glm::vec3 worldRay = g_camera->GetWorldRay();
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


void GenRenderTexture(GLuint& texture, GLuint& frameBuffer, int width, int height){
	//Generate texture and bind it to a GLuint (texture) and framebuffer (frameBuffer) 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glGenerateMipmap(GL_TEXTURE_2D);

	//Generate the framebuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	//Attach the texture to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	//Generate the renderbuffer and attach it to the framebuffer (for depth) 
	GLuint renderBuffer;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	//Check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete" << std::endl;
	}

	//Unbind framebuffer and texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
	utils::deltaTime = utils::currentTime - utils::previousTimeStep;
	utils::previousTimeStep = utils::currentTime;

	//Update all shapes
	CObjectManager::UpdateAll(utils::deltaTime, utils::currentTime);

	glUseProgram(ShaderLoader::GetProgram("3DLight")->m_id);
	glUniform3fv(glGetUniformLocation(ShaderLoader::GetProgram("3DLight")->m_id, "CameraPos"), 1, glm::value_ptr(g_camera->GetCameraPos()));
	glUseProgram(0);

	CObjectManager::GetShape("tess")->UpdateUniform(new FloatUniform(glm::distance(CObjectManager::GetShape("tess")->GetPosition(), g_camera->GetCameraPos()), "Distance"));

	//glUseProgram(ShaderLoader::GetProgram("3DTexture")->m_id);
	//glUniform1f(glGetUniformLocation(ShaderLoader::GetProgram("3DTexture")->m_id, "CurrentTime"), utils::currentTime);
	//glUseProgram(0);

	CShape* sphere = CObjectManager::GetShape("sphere1");
	CShape* floor = CObjectManager::GetShape("floor");

	//Sphere "Rolling" on the terrain
	if (sphere && floor) {
		//temp static velocity and acceleration
		static glm::vec3 vel = glm::vec3(0,0,0);
		static glm::vec3 acc = glm::vec3(0,0,0);

		//If holidng sphere, dont do movement, follow camera
		if (holdSphere) {
			sphere->SetPosition(g_camera->GetCameraPos() + g_camera->GetCameraForwardDir() * 2.0f);
			vel = glm::vec3(0, 0, 0);
			acc = glm::vec3(0, 0, 0);
		}
		else {
			//Gravity
			acc = glm::vec3(0, -9.81f, 0) / 5.0f;

			//Terrain height at sphere pos
			float terrainHeight = floor->GetPosition().y + GetTerrainHeight(floor, sphere->GetPosition().x, sphere->GetPosition().z);

			//If sphere is below terrain, set velocity to 0	
			if (sphere->GetPosition().y - sphere->GetScale().y / 2.0f < terrainHeight) {
				vel.y = 0;
				sphere->SetPosition(glm::vec3(sphere->GetPosition().x, terrainHeight + sphere->GetScale().y / 2.0f, sphere->GetPosition().z));

				int wdivs = CObjectManager::GetShape("floor")->GetMesh()->GetWidthDivs();
				int ldivs = CObjectManager::GetShape("floor")->GetMesh()->GetLengthDivs();
				
				//Calc relevant data position data for normals
				float relativeX = ((sphere->GetPosition().x - floor->GetPosition().x) / floor->GetScale().x) * wdivs / CObjectManager::GetShape("floor")->GetMesh()->GetWidth();
				float relativeZ = ((sphere->GetPosition().z - floor->GetPosition().z) / floor->GetScale().z)* ldivs / CObjectManager::GetShape("floor")->GetMesh()->GetLength();
				if (relativeX < 1) relativeX = 1;
				if (relativeX > wdivs - 1) relativeX = wdivs - 1;
				if (relativeZ < 1) relativeZ = 1;
				if (relativeZ > ldivs - 1) relativeZ = ldivs - 1;

				//get normal data from verticies
				float xNorm = CObjectManager::GetShape("floor")->GetMesh()->GetVertices()[(int(relativeZ) * (CObjectManager::GetShape("floor")->GetMesh()->GetWidthDivs() + 1) + int(relativeX)) * 8 + 5];
				float zNorm = CObjectManager::GetShape("floor")->GetMesh()->GetVertices()[(int(relativeZ) * (CObjectManager::GetShape("floor")->GetMesh()->GetWidthDivs() + 1) + int(relativeX)) * 8 + 7];

				//Apply force in direction of normal
				acc = glm::vec3(xNorm, 0, zNorm);
			}
			
			//Update position from vel and accel
			vel += acc * 5.0f * utils::deltaTime;
			sphere->SetPosition(sphere->GetPosition() + vel * utils::deltaTime);

			//rotate sphere in direction of velocity (Not very good)
			sphere->SetRotation(glm::vec3(sphere->GetRotation().x + vel.z * utils::deltaTime, 0, sphere->GetRotation().z + vel.x * utils::deltaTime));
		}
	}

	//pickup sphere
	if (sphere) {
		glm::vec3 camDir = glm::normalize(g_camera->GetCameraForwardDir());
		glm::vec3 objDir = glm::normalize(sphere->GetPosition() - g_camera->GetCameraPos());
		float angle = glm::degrees(glm::angle(camDir, objDir));

		if (!holdSphere && angle <= 10.0f) {
			outlineSphere = true;
		}
		else {
			outlineSphere = false;
		}
	}

	//Check for input
	CheckInput(utils::deltaTime, utils::currentTime);

	CLightManager::UpdateUniforms(ShaderLoader::GetProgram("3DLight")->m_id);
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
		g_camera->SetYaw(g_camera->GetYaw() - g_camera->GetSpeed() * (utils::mousePos.x - oldMouse.x) * 0.002f);
		g_camera->SetPitch(g_camera->GetPitch() - g_camera->GetSpeed() * (utils::mousePos.y - oldMouse.y) * 0.002f);
		Print(5, 10, "Mouse rotation (yaw: " + std::to_string((int)glm::degrees(g_camera->GetYaw())) + " pitch:" + std::to_string((int)glm::degrees(g_camera->GetPitch())) + ")    ", 15);

		g_camera->UpdateRotation();
	}
	

	//Camera movement below

	glm::vec3 camMovement = glm::vec3(0, 0, 0);
	float camSpeed = 5.0f; 

	if (glfwGetKey(g_window, GLFW_KEY_U))
	{
		CShape* sphere = CObjectManager::GetShape("sphere1");
		sphere->SetPosition(sphere->GetPosition() + glm::vec3(0, 1, 0));
	}

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
		camMovement += glm::vec3(0,1,0);
		if (g_camera->followTerrain) camSpeed *= 2.0f;
	}
	if (glfwGetKey(g_window, GLFW_KEY_LEFT_CONTROL))
	{
		camMovement -= glm::vec3(0, 1, 0);
		if (g_camera->followTerrain) camSpeed *= 0.5f;
	}

	if (glm::length(camMovement) >= 0.01f) {
		//If following terrain
		if (g_camera->followTerrain) {
			
			//Get floor
			CShape* floor = CObjectManager::GetShape("floor");
			
			//if floor exists
			if (floor) {
				float camX = g_camera->GetCameraPos().x;
				float camZ = g_camera->GetCameraPos().z;
				
				//Get terrain height at camera locaiton
				float terrainHeight = GetTerrainHeight(CObjectManager::GetShape("floor"), camX, camZ);
				
				float relative_camY = terrainHeight + 1.0f;

				//Walk above terrain
				g_camera->SetCameraPos(glm::vec3(camX, floor->GetPosition().y + relative_camY * floor->GetScale().y, camZ));

				camMovement.y = 0;
			}

			if (glm::length(camMovement) >= 0.01f) {
				camMovement = glm::normalize(camMovement) * camSpeed * _deltaTime;
				g_camera->SetCameraPos(g_camera->GetCameraPos() + camMovement);
			}
		}
		else {
			camMovement = glm::normalize(camMovement) * camSpeed * _deltaTime;
			g_camera->SetCameraPos(g_camera->GetCameraPos() + camMovement);
		}
	}
}

/// <summary>
/// Gets the terrain height at the given x and z position, of the given terrain
/// </summary>
float GetTerrainHeight(CShape* floor,float _worldX, float _worldZ) {
	if (floor) {
		CMesh* mesh = floor->GetMesh();
		std::vector<float> vertices = mesh->GetVertices();

		int widthDivs = mesh->GetWidthDivs();
		int lengthDivs = mesh->GetLengthDivs();

		float relativeX = ((_worldX - floor->GetPosition().x) / floor->GetScale().x) * widthDivs / mesh->GetWidth();
		//check that the camera is not outside the mesh bounds (if it is, clamp it) 
		if (relativeX < 1) relativeX = 1;
		if (relativeX > widthDivs - 1) relativeX = widthDivs - 1;

		float relativeZ = ((_worldZ - floor->GetPosition().z) / floor->GetScale().z) * lengthDivs / mesh->GetLength();
		//check that the camera is not outside the mesh bounds (if it is, clamp it)
		if (relativeZ < 1) relativeZ = 1;
		if (relativeZ > lengthDivs - 1) relativeZ = lengthDivs - 1;

		float nearestVertHeight = vertices[(int(relativeZ) * (widthDivs + 1) + int(relativeX)) * 8 + 1];

		//get the surrounding vertices to calculate the height of the terrain at the camera position
		float vertHeight1 = vertices[(int(relativeZ) * (widthDivs + 1) + int(relativeX)) * 8 + 1];
		float vertHeight2 = vertices[(int(relativeZ) * (widthDivs + 1) + int(relativeX) + 1) * 8 + 1];
		float vertHeight3 = vertices[((int(relativeZ) + 1) * (widthDivs + 1) + int(relativeX)) * 8 + 1];
		float vertHeight4 = vertices[((int(relativeZ) + 1) * (widthDivs + 1) + int(relativeX) + 1) * 8 + 1];

		//calculate the height of the terrain at the camera position by interpolating the surrounding vertices and the relative position on the terrain to get the height 
		float h12 = utils::Lerp(vertHeight1, vertHeight2, relativeX - int(relativeX));
		float h34 = utils::Lerp(vertHeight3, vertHeight4, relativeX - int(relativeX));

		float terrainHeight = utils::Lerp(h12, h34, relativeZ - int(relativeZ));

		return terrainHeight;
	}
}

/// <summary>
/// Calls render function for objects
/// </summary>
void Render()
{
	GLint polygonMode[2];
	glGetIntegerv(GL_POLYGON_MODE, polygonMode);

	//Only enable frame buffer if not on wireframe mode, as buffer hides all other wireframes
	if (polygonMode[0] == GL_FILL) {
		//bind framebuffer, enable depth test, and clear screen 
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//Enable blending for textures with opacity
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Clear screen, and stenctils
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Render normal objects
	CObjectManager::GetShape("skybox")->Render();
	CObjectManager::GetShape("floor")->Render();
	CObjectManager::GetShape("cube1")->Render();
	
	//Enable stencil, and set function
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//Render normal sphere
	//Also write to stencil, so that coloured sphere does not overlap
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	CShape* sphere = CObjectManager::GetShape("sphere1");
	sphere->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
	sphere->UpdateUniform(new Vec3Uniform({ 1,0,0 }, "Colour"));
	sphere->UpdateUniform(new Mat4Uniform(sphere->GetPVM(), "PVMMat"));
	sphere->UpdateUniform(new Mat4Uniform(sphere->GetPVM(), "Model"));
	sphere->Render();


	if (outlineSphere) {
		//Render Outlined
		RenderOutline();
	}

	glStencilMask(0xFF);
	glDisable(GL_STENCIL_TEST);

	//Render water and star with backface enabled
	GLboolean cull = glIsEnabled(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	CObjectManager::GetShape("water1")->Render();
	CObjectManager::GetShape("point")->Render();
	CObjectManager::GetShape("tess")->Render();
	if (cull) glEnable(GL_CULL_FACE);

	DrawCirlce(0.01f, glm::vec3(0,0,0));

	//Render buffer if wireframe mode is not on
	if (polygonMode[0] == GL_FILL) {
		//unbind framebuffer 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		//render the renderTexture object
		CObjectManager::GetShape("renderTexture")->Render();
	}

	glfwSwapBuffers(g_window);
}

/// <summary>
/// Draws a circle on the screen
/// </summary>
/// <param name="floor"></param>
/// <param name="_worldX"></param>
/// <param name="_worldZ"></param>
/// <returns></returns>
void DrawCirlce(float _rad, glm::vec3 _screenPos, glm::vec3 _colour, int _points) {
	glShadeModel(GL_FLAT);    // as opposed to GL_FLAT
	glBegin(GL_LINE_LOOP);

	glColor3f(_colour.r, _colour.g, _colour.b);

	for (int i = 0; i < _points; i++) {
		float theta = 2.0f * glm::pi<float>() * float(i) / float(_points);//get the current angle 

		float ratio = float(utils::windowHeight) / float(utils::windowWidth);

		float x = _rad * cosf(theta) * ratio;//calculate the x component 
		float y = _rad * sinf(theta);//calculate the y component 

		glVertex3f(x, y, 0);//output vertex 
	}

	glEnd();
}

/// <summary>
/// Renders outline of sphere
/// </summary>
/// <param name="floor"></param>
/// <param name="_worldX"></param>
/// <param name="_worldZ"></param>
/// <returns></returns>
void RenderOutline()
{
	//Render scaled up and colour only sphere
	//Only render where stencil value is not 1 (aka where original sphere is)
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	CShape* sphere = CObjectManager::GetShape("sphere1");
	sphere->Scale(1.1f);
	sphere->SetProgram(ShaderLoader::GetProgram("solidColour")->m_id);
	sphere->UpdateUniform(new Vec3Uniform({ 1,0,0 }, "Colour"));
	sphere->UpdateUniform(new Mat4Uniform(sphere->GetPVM(), "PVMMat"));
	sphere->UpdateUniform(new Mat4Uniform(sphere->GetPVM(), "Model"));
	sphere->Render();
	sphere->Scale(1.0f / 1.1f);
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