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

#include "Utility.h"


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void MouseCallback(GLFWwindow* window, int button, int action, int mods);

void TextInput(GLFWwindow* window, unsigned int codePoint);

bool Startup();
void InitialSetup();

bool AudioInit();

void GenTexture(GLuint& texture, const char* texPath);

void Update();
void CheckInput();
void Render();

GLFWwindow* window = nullptr;

CCamera* camera = new CCamera();

//Storing previous time step
float previousTimeStep = 0;

bool doInput = false;

//Move to map/vector later
GLuint Program_Texture;
GLuint Program_ClipSpace;
GLuint Program_ClipSpaceFractal;
GLuint Program_Text;
GLuint Program_TextScroll;


//Move to map/vector later
GLuint Texture_Rayman;
GLuint Texture_Awesome;
GLuint Texture_CapMan;
GLuint Texture_Frac;

std::map<GLuint, std::string> textures;

TextLabel* Text_Message;
TextLabel* Text_Message2;

//Make shapes (Add them to vector later)
CShape* g_Hexagon;
CShape* g_Rectangle;
CShape* g_Fractal;
CShape* g_Cube;
CShape* g_Cube2;

FMOD::System* AudioSystem;
FMOD::Sound* FX_Gunshot;
FMOD::Sound* Track_Dance;

float CurrentTime;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Quit if esc key pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		doInput = !doInput;
		std::cout << "Input is now " << (doInput ? "Enabled." : "Disabled.") << std::endl;
		(doInput == true) ? glfwSetCharCallback(window, TextInput) : glfwSetCharCallback(window, 0);
	}

	if (doInput && key == GLFW_KEY_BACKSPACE && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		Text_Message->SetText( Text_Message->GetText().substr(0, Text_Message->GetText().size()-1) );
	}
}

void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		AudioSystem->playSound(FX_Gunshot, 0, false, 0);
	}
}

void TextInput(GLFWwindow* window, unsigned int codePoint) {
	std::cout << "Text input detected: " << (unsigned char)codePoint << std::endl;
	unsigned char uc = (unsigned char)codePoint;
	std::string s(1, static_cast<char>(uc));
	Text_Message->SetText(Text_Message->GetText() + s);
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

	FX_Gunshot->release();
	Track_Dance->release();
	AudioSystem->release();

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
	window = glfwCreateWindow(utils::windowWidth, utils::windowHeight, "Keane Carotenuto - Summative  1", NULL, NULL);

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
	//Create cube mesh
	CMesh::NewCMesh(
		"cube",
		{
			// Index        // Position                     //Texture Coords
						//Front Quad
			/* 00 */        -0.5f,  0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 00 */
			/* 01 */        -0.5f, -0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 01 */
			/* 02 */         0.5f, -0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 02 */
			/* 03 */         0.5f,  0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 03 */
			//Back Quad
			/* 04 */         0.5f,  0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 04 */
			/* 05 */         0.5f, -0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 05 */
			/* 06 */        -0.5f, -0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 06 */
			/* 07 */        -0.5f,  0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 07 */
			//Right
			/* 08 */         0.5f,  0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 03 */
			/* 09 */         0.5f, -0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 02 */
			/* 10 */         0.5f, -0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 05 */
			/* 11 */         0.5f,  0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 04 */
			//Left
			/* 12 */        -0.5f,  0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 07 */
			/* 13 */        -0.5f, -0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 06 */
			/* 14 */        -0.5f, -0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 01 */
			/* 15 */        -0.5f,  0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 00 */
			//Top
			/* 16 */        -0.5f,  0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 07 */
			/* 17 */        -0.5f,  0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 00 */
			/* 18 */         0.5f,  0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 03 */
			/* 19 */         0.5f,  0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 04 */
			//Bottom
			/* 20 */        -0.5f, -0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 1.0f,     /* 01 */
			/* 21 */        -0.5f, -0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         0.0f, 0.0f,     /* 06 */
			/* 22 */         0.5f, -0.5f, -0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 0.0f,     /* 05 */
			/* 23 */         0.5f, -0.5f,  0.5f,	-1.0f,  1.0f,  1.0f,         1.0f, 1.0f,     /* 02 */
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

	g_Hexagon = new CShape(6, glm::vec3(0.25f, 0.25f, 0.0f), 0.0f, glm::vec3(0.5f, 0.5f, 1.0f), false);
	g_Rectangle = new CShape(4, glm::vec3(-0.25f, -0.25f, 0.0f), 0.0f, glm::vec3(0.5f, 1.0f, 1.0f), false);
	g_Fractal = new CShape(40, glm::vec3(0.5f, -0.5f, 0.0f), 0.0f, glm::vec3(0.5f, 0.5f, 1.0f), false);

	g_Cube = new CShape("cube", glm::vec3(5.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false);
	g_Cube2 = new CShape("cube", glm::vec3(-5.0f, 0.0f, 0.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f), false);

	//Set the clear colour as blue (used by glClear)
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	// Maps the range of the window size to NDC (-1 -> 1)
	glViewport(0, 0, utils::windowWidth, utils::windowHeight);

	//Create programs
	Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vert", "Resources/Shaders/Texture.frag");
	Program_ClipSpace = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vert", "Resources/Shaders/TextureMix.frag");
	Program_ClipSpaceFractal = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vert", "Resources/Shaders/Fractal.frag");
	Program_Text = ShaderLoader::CreateProgram("Resources/Shaders/Text.vert", "Resources/Shaders/Text.frag");
	Program_TextScroll = ShaderLoader::CreateProgram("Resources/Shaders/TextScroll.vert", "Resources/Shaders/TextScroll.frag");

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseCallback);

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

	//Create all textures and bind them
	GenTexture(Texture_Rayman, "Resources/Textures/Rayman.jpg");
	GenTexture(Texture_Awesome, "Resources/Textures/AwesomeFace.png");
	GenTexture(Texture_CapMan, "Resources/Textures/Capguy_Walk.png");
	GenTexture(Texture_Frac, "Resources/Textures/pal.png");

	Text_Message = new TextLabel("Press [ENTER] to edit me!", "Resources/Fonts/ARIAL.ttf", glm::ivec2(0,48), glm::vec2(100.0f, 100.0f));
	Text_Message->SetProgram(Program_TextScroll);
	Text_Message2 = new TextLabel("Bounce!", "Resources/Fonts/Roboto.ttf", glm::ivec2(0,48), glm::vec2(100.0f, 700.0f));
	Text_Message2->SetBouncing(true);

	//Set program and add uniforms to hexagon
	g_Hexagon->SetProgram(Program_ClipSpace);
	g_Hexagon->SetCamera(camera);

	g_Hexagon->AddUniform(new ImageUniform(Texture_Rayman), "ImageTexture");
	g_Hexagon->AddUniform(new ImageUniform(Texture_Awesome), "ImageTexture1");
	g_Hexagon->AddUniform(new FloatUniform(0), "CurrentTime");
	g_Hexagon->AddUniform(new Mat4Uniform(g_Hexagon->GetPVM()), "PVMMat");

	//Set program and add uniforms to Rectangle
	g_Rectangle->SetProgram(Program_Texture);
	g_Rectangle->SetCamera(camera);

	g_Rectangle->AddUniform(new AnimationUniform(Texture_CapMan, 8, 0.1f, g_Rectangle), "ImageTexture");
	g_Rectangle->AddUniform(new IntUniform(8), "frameCount");
	g_Rectangle->AddUniform(new FloatUniform(0), "offset");
	g_Rectangle->AddUniform(new Mat4Uniform(g_Rectangle->GetPVM()), "PVMMat");

	//Set program and add uniforms to Rectangle
	g_Fractal->SetProgram(Program_ClipSpaceFractal);
	g_Fractal->SetCamera(camera);

	g_Fractal->AddUniform(new ImageUniform(Texture_Frac), "ImageTexture");
	g_Fractal->AddUniform(new FloatUniform(0), "CurrentTime");
	g_Fractal->AddUniform(new Mat4Uniform(g_Rectangle->GetPVM()), "PVMMat");

	//Set program and add uniforms to Cube
	g_Cube->SetProgram(Program_ClipSpace);
	g_Cube->SetCamera(camera);

	g_Cube->AddUniform(new ImageUniform(Texture_Rayman), "ImageTexture");
	g_Cube->AddUniform(new ImageUniform(Texture_Awesome), "ImageTexture1");
	g_Cube->AddUniform(new FloatUniform(0), "CurrentTime");
	g_Cube->AddUniform(new Mat4Uniform(g_Cube->GetPVM()), "PVMMat");

	//Set program and add uniforms to Cube
	g_Cube2->SetProgram(Program_ClipSpace);
	g_Cube2->SetCamera(camera);

	g_Cube2->AddUniform(new ImageUniform(Texture_Awesome), "ImageTexture");
	g_Cube2->AddUniform(new ImageUniform(Texture_Rayman), "ImageTexture1");
	g_Cube2->AddUniform(new FloatUniform(0), "CurrentTime");
	g_Cube2->AddUniform(new Mat4Uniform(g_Cube2->GetPVM()), "PVMMat");

	AudioInit();
}

bool AudioInit()
{
	if (FMOD::System_Create(&AudioSystem) != FMOD_OK) {
		std::cout << "FMOD ERROR: Audio System failed to create." << std::endl;
		return false;
	}

	if (AudioSystem->init(100, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0) != FMOD_OK) {
		std::cout << "FMOD ERROR: Audio system failed to initialize." << std::endl;
		return false;
	}

	if (AudioSystem->createSound("Resources/Audio/Gunshot.wav", FMOD_DEFAULT, 0, &FX_Gunshot) != FMOD_OK) {
		std::cout << "FMOD ERROR: Failed to load sound using createsound(...)" << std::endl;
	}

	if (AudioSystem->createSound("Resources/Audio/DanceTrack.mp3", FMOD_LOOP_NORMAL, 0, &Track_Dance) != FMOD_OK) {
		std::cout << "FMOD ERROR: Failed to load sound using createsound(...)" << std::endl;
	}

	AudioSystem->playSound(Track_Dance, 0, false, 0);

	return true;
}

void CheckInput()
{
	//bool updated = false;

	double xPos;
	double yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {5,50});
	std::cout << "x: " << xPos << ", y: " << yPos << "          " << std::endl;

	//Move camera SWAD
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		camera->CameraPos += glm::normalize(glm::rotate(camera->CameraLookDir, glm::radians(90.0f), camera->CameraUpDir)) * 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		camera->CameraPos -= glm::normalize(glm::rotate(camera->CameraLookDir, glm::radians(90.0f), camera->CameraUpDir)) * 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		camera->CameraPos += glm::normalize(camera->CameraLookDir) * 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		camera->CameraPos -= glm::normalize(camera->CameraLookDir) * 0.05f;
	}

	//Scale Triangle SHIFT, CTRL
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
	{
		//g_Hexagon->m_scale -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		//g_Hexagon->m_scale += 0.01f;
	}


	//NOTE THIS STUFF NEESD TO BE CHANGED SO THAT LOOK DIR AND UP DIR ARE LOCKED TOGETHER AT 90 DEGREES


	//Rotate Triangle QE
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		//g_Hexagon->m_rotation += 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		//g_Hexagon->m_rotation -= 0.5f;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		camera->CameraLookDir = glm::rotate(camera->CameraLookDir, glm::radians(1.0f), camera->CameraUpDir);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		camera->CameraLookDir = glm::rotate(camera->CameraLookDir, glm::radians(-1.0f), camera->CameraUpDir);
	}

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		/*camera->CameraUpDir = glm::rotate(camera->CameraUpDir, glm::radians(-1.0f), camera->CameraLookDir);*/
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		
	}

	if (glfwGetKey(window, GLFW_KEY_KP_ADD))
	{
		camera->CameraPos += glm::normalize(camera->CameraUpDir) * 0.05f;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT))
	{
		camera->CameraPos -= glm::normalize(camera->CameraUpDir) * 0.05f;
	}
}

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

float accum = 0;
//Called each frame
void Update()
{
	//Get current time and calc delta time
	CurrentTime = (float)glfwGetTime();
	utils::time = CurrentTime;
	float DeltaTime = CurrentTime - previousTimeStep;
	previousTimeStep = CurrentTime;

	//Add up accumulator
	accum += DeltaTime;

	//Call update func for shapes (make into vector)
	g_Hexagon->Update(DeltaTime, CurrentTime);
	g_Rectangle->Update(DeltaTime, CurrentTime);
	g_Fractal->Update(DeltaTime, CurrentTime);

	g_Cube->Update(DeltaTime, CurrentTime);
	g_Cube2->Update(DeltaTime, CurrentTime);

	g_Cube->SetPosition(glm::vec3(sin(CurrentTime + glm::pi<float>())*2, 0, cos(CurrentTime + glm::pi<float>())*2));
	g_Cube2->SetPosition(glm::vec3(sin(CurrentTime)*2, 0, cos(CurrentTime)*2));

	Text_Message->Update(DeltaTime, CurrentTime);
	Text_Message2->Update(DeltaTime, CurrentTime);

	glUseProgram(Program_TextScroll);
	glUniform1f(glGetUniformLocation(Program_TextScroll, "CurrentTime"), CurrentTime);
	glUseProgram(0);

	//Check for user input at fixed rate
	if (accum >= 0.01) {
		//Reset accumulator
		accum = 0;
		
		//Check for input
		CheckInput();
	}

	//Update the FMOD Audio System
	AudioSystem->update();
}

/// <summary>
/// Renders objects in order
/// </summary>
void Render()
{
	//Clear Screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw rect
	g_Rectangle->Render();

	//Draw hex
	g_Hexagon->Render();

	//Draw fractal
	g_Fractal->Render();

	g_Cube->Render();
	g_Cube2->Render();

	Text_Message->Render();
	Text_Message2->Render();
	
	glfwSwapBuffers(window);
}