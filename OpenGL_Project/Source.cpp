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

#include "Source.h"
#include "ShaderLoader.h"
#include "TextLabel.h"
#include "CCamera.h"

#include "CShape.h"
#include "CUniform.h"
#include "Sphere.h"

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

Sphere* g_sphere = nullptr;

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

	// Maps the range of the window size to NDC (-1 -> 1)
	glViewport(0, 0, utils::windowWidth, utils::windowHeight);

	glfwSetKeyCallback(g_window, KeyCallback);
	glfwSetMouseButtonCallback(g_window, MouseCallback);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Cull polygons not facing
	glCullFace(GL_BACK);

	//Set winding order of verticies (for front and back facing)
	glFrontFace(GL_CCW);

	//Enable Culling
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Flip Images
	stbi_set_flip_vertically_on_load(true);

	//Create textures
	TextureCreation();

	//Create default meshes
	MeshCreation();

	//Create objects
	ObjectCreation();

	//Set up shaders
	ProgramSetup();

	CLightManager::AddLight(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.05f, 1.0f, 100);
	CLightManager::AddLight(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.05f, 1.0f, 100);
	CLightManager::AddLight(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.05f, 1.0f, 100);
	CLightManager::UpdateUniforms(ShaderLoader::GetProgram("3DLight")->m_id);

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

/// <summary>
/// Create all shapes used in program
/// </summary>
void ObjectCreation()
{
	CObjectManager::AddShape("fractal", new CShape("square", glm::vec3(0.3f, 0.65f, 0.0f), 0.0f, glm::vec3(0.5f, 0.5f, 1.0f), true));
	CObjectManager::GetShape("fractal")->SetCamera(g_camera);

	CObjectManager::AddShape("floor", new CShape("floor-squareNorm", glm::vec3(0.0f, -0.5f, 0.0f), 0.0f, glm::vec3(100.0f, 1.0f, 100.0f), false));
	CObjectManager::GetShape("floor")->SetCamera(g_camera);

	CObjectManager::AddShape("sphere1", new CShape("sphere", glm::vec3(5.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("sphere1")->SetCamera(g_camera);

	CObjectManager::AddShape("cube2", new CShape("cubeNorm", glm::vec3(-5.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false));
	CObjectManager::GetShape("cube2")->SetCamera(g_camera);

	CObjectManager::AddShape("moveable", new CShape("cube", glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 2.0f, 1.0f), false));
	CObjectManager::GetShape("moveable")->SetCamera(g_camera);

	Text_Message = new TextLabel("Press [ENTER] to edit me!", "Resources/Fonts/ARIAL.ttf", glm::ivec2(0, 48), glm::vec2(100.0f, 100.0f));
	Text_Message2 = new TextLabel("Bounce!", "Resources/Fonts/Roboto.ttf", glm::ivec2(0, 48), glm::vec2(100.0f, 700.0f));
	Text_Message2->SetBouncing(true);
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

	CShape* _shape = nullptr;

	//Set program and add uniforms to fractal
	if (_shape = CObjectManager::GetShape("fractal")) {
		_shape->SetProgram(ShaderLoader::GetProgram("clipSpaceFractal")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Frac), "ImageTexture");
		_shape->AddUniform(new FloatUniform(0), "CurrentTime");
		_shape->AddUniform(new Vec4Uniform(glm::vec4(1, 1, 1, 1)), "FractalColour");
		_shape->AddUniform(new Mat4Uniform(glm::mat4()), "ModelMat");
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM()), "PVMMat");
	}
	

	//Set program and add uniforms to Rectangle
	if (_shape = CObjectManager::GetShape("floor")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Floor), "ImageTexture");
		_shape->AddUniform(new FloatUniform(0), "CurrentTime");
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM()), "PVMMat");
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM()), "Model");
	}
	
	
	//Set program and add uniforms to Cube
	if (_shape = CObjectManager::GetShape("sphere1")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Rayman), "ImageTexture");
		_shape->AddUniform(new FloatUniform(0), "CurrentTime");
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM()), "PVMMat");
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM()), "Model");
	}
	
	
	//Set program and add uniforms to Cube
	if (_shape = CObjectManager::GetShape("cube2")) {
		_shape->SetProgram(ShaderLoader::GetProgram("3DLight")->m_id);
		_shape->AddUniform(new ImageUniform(Texture_Awesome), "ImageTexture");
		_shape->AddUniform(new FloatUniform(0), "CurrentTime");
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM()), "PVMMat");
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM()), "Model");
	}
	
	
	//Set program and add uniforms to moveable
	if (_shape = CObjectManager::GetShape("moveable")) {
		_shape->SetProgram(ShaderLoader::GetProgram("clipSpaceFade")->m_id);
		_shape->AddUniform(new FloatUniform(0), "CurrentTime");
		_shape->AddUniform(new Mat4Uniform(_shape->GetPVM()), "PVMMat");
	}
	
	Text_Message->SetProgram(ShaderLoader::GetProgram("textScroll")->m_id);
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

	CAudioSystem::GetInstance().PlaySong("DanceTrack");

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

	//Enable text input if ENTER is pressed
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		doInput = !doInput;
		Print(5, 7, "Input is now " + (std::string)(doInput ? "Enabled. " : "Disabled. "), 15);
		CAudioSystem::GetInstance().PlaySong(doInput ? "Enabled" : "Disabled");
		(doInput == true) ? glfwSetCharCallback(window, TextInput) : glfwSetCharCallback(window, 0);
	}

	//If input is enabled, delete the last character with BACKPSACE
	if (doInput && key == GLFW_KEY_BACKSPACE && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		Text_Message->SetText(Text_Message->GetText().substr(0, Text_Message->GetText().size() - 1));
	}

	//Change line mode (fill or line) with F
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
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		CAudioSystem::GetInstance().PlaySong("Gunshot");
	}
}

/// <summary>
/// Callback for any characters pressed
/// </summary>
/// <param name="window">window to check on</param>
/// <param name="codePoint">"character" pressed</param>
void TextInput(GLFWwindow* window, unsigned int codePoint) {
	unsigned char uc = (unsigned char)codePoint;
	std::string s(1, static_cast<char>(uc));

	Print(5, 8, "Text input detected: " + s, 15);

	Text_Message->SetText(Text_Message->GetText() + s);
}

#pragma endregion

/// <summary>
/// Generates a texture and binds it to a GLuint
/// </summary>
/// <param name="texture">the GLuint to bind</param>
/// <param name="texPath">the folder pathing + name of file</param>
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

	//Unbind and free texture and image
	stbi_image_free(ImageData);
	glBindTexture(GL_TEXTURE_2D, 0);
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

	//Check mouse overlap with fractal shape, and change colour depending on distance
	CShape* _fractal = CObjectManager::GetShape("fractal");
	if	(_fractal != nullptr &&
		utils::mousePos.x > (_fractal->GetPosition().x - _fractal->GetScale().x / 2) * (utils::windowWidth / 2.0f)
		&&  utils::mousePos.x < (_fractal->GetPosition().x + _fractal->GetScale().x / 2) * (utils::windowWidth / 2.0f)
		&&  utils::mousePos.y > (_fractal->GetPosition().y - _fractal->GetScale().y / 2) * (utils::windowHeight / 2.0f)
		&&  utils::mousePos.y < (_fractal->GetPosition().y + _fractal->GetScale().y / 2) * (utils::windowHeight / 2.0f)
		) 
	{
		float dist = glm::distance(utils::mousePos, (glm::vec2)_fractal->GetPosition() * (utils::windowWidth / 2.0f)) / ((utils::windowHeight / 2.0f) * (_fractal->GetScale().x / 2));
		_fractal->UpdateUniform(new Vec4Uniform(glm::vec4(1, 1 - dist, dist, 1)), "FractalColour");
	}
	else {
		_fractal->UpdateUniform(new Vec4Uniform(glm::vec4(0, 1, 1, 1)), "FractalColour");
	}

	//Move shapes around world origin in circle
	CObjectManager::GetShape("sphere1")->SetPosition(glm::vec3(sin(utils::currentTime + glm::pi<float>())*2, 0, cos(utils::currentTime + glm::pi<float>())*2));
	CObjectManager::GetShape("cube2")->SetPosition(glm::vec3(sin(utils::currentTime)*2, 0, cos(utils::currentTime)*2));

	//Update all shapes
	CObjectManager::UpdateAll(DeltaTime, utils::currentTime);

	//Update text
	Text_Message->Update(DeltaTime, utils::currentTime);
	Text_Message2->Update(DeltaTime, utils::currentTime);

	//Update scrolling text program uniform
	glUseProgram(ShaderLoader::GetProgram("textScroll")->m_id);
	glUniform1f(glGetUniformLocation(ShaderLoader::GetProgram("textScroll")->m_id, "CurrentTime"), utils::currentTime);
	glUseProgram(0);

	glUseProgram(ShaderLoader::GetProgram("3DLight")->m_id);
	glUniform3fv(glGetUniformLocation(ShaderLoader::GetProgram("3DLight")->m_id, "CameraPos"), 1, glm::value_ptr(g_camera->GetCameraPos()));
	glUniform3fv(glGetUniformLocation(ShaderLoader::GetProgram("3DLight")->m_id, "ObjectPos"), 1, glm::value_ptr(CObjectManager::GetShape("sphere1")->GetPosition()));
	glUniform2fv(glGetUniformLocation(ShaderLoader::GetProgram("3DLight")->m_id, "mousePos"), 1, glm::value_ptr(utils::mousePos));
	glUniform1f(glGetUniformLocation(ShaderLoader::GetProgram("3DLight")->m_id, "mousePos"), utils::currentTime);
	glUseProgram(0);


	//Check for input
	CheckInput(DeltaTime, utils::currentTime);

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
	Print(5, 5, "Mouse Position (x: " + std::to_string((int)utils::mousePos.x) + " y:" + std::to_string((int)utils::mousePos.y) + ")", 15);

	if (cursorLocked) {
		//Update camera rotation with mouse movement
		g_camera->SetYaw(g_camera->GetYaw() - g_camera->GetSpeed() * (utils::mousePos.x - oldMouse.x) * _deltaTime);
		g_camera->SetPitch(g_camera->GetPitch() - g_camera->GetSpeed() * (utils::mousePos.y - oldMouse.y) * _deltaTime);
		Print(10, 10, "Mouse rotation (yaw: " + std::to_string((int)glm::degrees(g_camera->GetYaw())) + " pitch:" + std::to_string((int)glm::degrees(g_camera->GetPitch())) + ")", 15);

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
}

/// <summary>
/// Calls render function for objects
/// </summary>
void Render()
{
	//Clear Screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	CObjectManager::RenderAll();

	Text_Message->Render();
	Text_Message2->Render();
	
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