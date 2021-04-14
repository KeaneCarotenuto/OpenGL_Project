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

class CCamera {
public:
	// Camera Variables 
	glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 CameraLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 CameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraUpDir = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 ViewMat;
	glm::mat4 ProjectionMat;

	void Draw(CShape* _shape);
}camera;


bool Startup();
void InitialSetup();

void GenTexture(GLuint& texture, const char* texPath);

void Update();
void UpdatePVM(CShape* _shape);
void CheckInput();
void Render();

GLFWwindow* window = nullptr;

const int width = 800;
const int height = 800;

float previousTimeStep = 0;

//Move to map/vector later
GLuint Program_ColorFadeTri;
GLuint Program_Texture;
GLuint Program_TextureMix;
GLuint Program_WorldSpace;
GLuint Program_ClipSpace;
GLuint Program_ClipSpaceColour;
GLuint Program_ClipSpaceFractal;

//Move to map/vector later
GLuint Texture_Rayman;
GLuint Texture_Awesome;
GLuint Texture_CapMan;
GLuint Texture_Frac;

CShape* g_Hexagon = new CShape(6, glm::vec3(0.25f, 0.25f, 0.0f), 0.0f, glm::vec3(0.5f, 0.5f, 1.0f), true);
CShape* g_Rectangle = new CShape(4, glm::vec3(-0.25f, -0.25f, 0.0f), 0.0f, glm::vec3(0.5f, 1.0f, 1.0f), true);
CShape* g_Fractal = new CShape(40, glm::vec3(0.5f, -0.5f, 0.0f), 0.0f, glm::vec3(0.5f, 0.5f, 1.0f), true);

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Maps the range of the window size to NDC (-1 -> 1)
	glViewport(0, 0, width, height);

	//Program_FixedColour = ShaderLoader::CreateProgram("Resources/Shaders/Triangle.vs", "Resources/Shaders/Color.fs");
	//GLuint test_ProgramTri = ShaderLoader::CreateProgram("Resources/Shaders/Triangle.vs", "Resources/Shaders/Color.fs");
	//Program_ColorFadeTri = ShaderLoader::CreateProgram("Resources/Shaders/Triangle.vert", "Resources/Shaders/VertexColorFade.frag");
	Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vert", "Resources/Shaders/Texture.frag");
	//Program_TextureMix = ShaderLoader::CreateProgram("Resources/Shaders/NDC_Texture.vert", "Resources/Shaders/TextureMix.frag");
	//Program_WorldSpace = ShaderLoader::CreateProgram("Resources/Shaders/WorldSpace.vert", "Resources/Shaders/TextureMix.frag");
	Program_ClipSpace = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vert", "Resources/Shaders/TextureMix.frag");
	//Program_ClipSpaceColour = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vert", "Resources/Shaders/VertexColorFade.frag");
	Program_ClipSpaceFractal = ShaderLoader::CreateProgram("Resources/Shaders/ClipSpace.vert", "Resources/Shaders/Fractal.frag");

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

	//Create all textures and bind them
	GenTexture(Texture_Rayman, "Resources/Textures/Rayman.jpg");
	GenTexture(Texture_Awesome, "Resources/Textures/AwesomeFace.png");
	GenTexture(Texture_CapMan, "Resources/Textures/Capguy_Walk.png");
	GenTexture(Texture_Frac, "Resources/Textures/pal.png");


	//Set program and add uniforms to hexagon
	g_Hexagon->m_program = Program_ClipSpace;

	g_Hexagon->AddUniform(new ImageUniform(Texture_Rayman), "ImageTexture");
	g_Hexagon->AddUniform(new ImageUniform(Texture_Awesome), "ImageTexture1");
	g_Hexagon->AddUniform(new FloatUniform(g_Hexagon->m_currentTime), "CurrentTime");
	g_Hexagon->AddUniform(new FloatUniform(0), "offset");
	g_Hexagon->AddUniform(new Mat4Uniform(g_Hexagon->m_PVMMat), "PVMMat");

	g_Hexagon->GenBindVerts();


	//Set program and add uniforms to Rectangle
	g_Rectangle->m_program = Program_Texture;

	g_Rectangle->AddUniform(new AnimationUniform(Texture_CapMan, 8, 0.1f, g_Rectangle), "ImageTexture");
	g_Rectangle->AddUniform(new FloatUniform(0), "offset");
	g_Rectangle->AddUniform(new Mat4Uniform(g_Rectangle->m_PVMMat), "PVMMat");

	g_Rectangle->GenBindVerts();



	//Set program and add uniforms to Rectangle
	g_Fractal->m_program = Program_ClipSpaceFractal;

	g_Fractal->AddUniform(new ImageUniform(Texture_Frac), "ImageTexture");
	g_Fractal->AddUniform(new FloatUniform(g_Fractal->m_currentTime), "CurrentTime");
	g_Fractal->AddUniform(new Mat4Uniform(g_Rectangle->m_PVMMat), "PVMMat");

	g_Fractal->GenBindVerts();
}

void CheckInput()
{
	//bool updated = false;

	//Move Triangle
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		g_Hexagon->m_position.x += 0.01f;
		/*pos.x += 0.01f;
		updated = true;*/
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		g_Hexagon->m_position.x -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		g_Hexagon->m_position.y += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		g_Hexagon->m_position.y -= 0.01f;
	}

	//Scale Triangle
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
	{
		g_Hexagon->m_scale -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
	{
		g_Hexagon->m_scale += 0.01f;
	}

	//Rotate Triangle
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		g_Hexagon->m_rotation += 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		g_Hexagon->m_rotation -= 0.5f;
	}

	//if (updated) {
	//	EquiTriangle(pos, size, ang);
	//}
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
	float DeltaTime = CurrentTime - previousTimeStep;
	previousTimeStep = CurrentTime;

	g_Hexagon->Update(DeltaTime, CurrentTime);
	g_Rectangle->Update(DeltaTime, CurrentTime);
	g_Fractal->Update(DeltaTime, CurrentTime);

	CheckInput();
}

//Render window
void Render()
{
	//Clear Screen
	glClear(GL_COLOR_BUFFER_BIT);

	//Draw shapes
	camera.Draw(g_Rectangle);

	camera.Draw(g_Hexagon);

	//copy current hex to keep most data stored
	CShape tempShape(*g_Hexagon);

	//Change relevant data
	g_Hexagon->m_position = { -0.7, 0.7, 0 };
	g_Hexagon->m_rotation = 90.0f;
	g_Hexagon->m_scale = { 0.3, 0.3, 1 };

	//Draw hex again with new data
	camera.Draw(g_Hexagon);

	//Reset hex values
	g_Hexagon->m_position = tempShape.m_position;
	g_Hexagon->m_rotation = tempShape.m_rotation;
	g_Hexagon->m_scale = tempShape.m_scale;

	//Draw fractal
	camera.Draw(g_Fractal);
	
	glfwSwapBuffers(window);
}

void CCamera::Draw(CShape* _shape)
{
	UpdatePVM(_shape);

	_shape->UpdateUniform(new Mat4Uniform(_shape->m_PVMMat), "PVMMat");

	_shape->Render();
}

void UpdatePVM(CShape* _shape)
{
	_shape->m_translationMat = glm::translate(glm::mat4(), _shape->m_position);
	_shape->m_rotationMat = glm::rotate(glm::mat4(), glm::radians(_shape->m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	_shape->m_scaleMat = glm::scale(glm::mat4(), _shape->m_scale);

	glm::mat4 pixelScale = (_shape->m_useScreenScale ? glm::scale(glm::mat4(), glm::vec3(width / 2, height / 2, 1)) : glm::scale(glm::mat4(), glm::vec3(1, 1, 1)));

	_shape->m_modelMat = pixelScale * _shape->m_translationMat * _shape->m_rotationMat * _shape->m_scaleMat;

	//Ortho project
	float halfWindowWidth = (float)width / 2.0f;
	float halfWindowHeight = (float)height / 2.0f;
	camera.ProjectionMat = glm::ortho(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, 0.1f, 100.0f);

	//Perspective project
	//ProjectionMat = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	camera.ViewMat = glm::lookAt(camera.CameraPos, camera.CameraPos + camera.CameraLookDir, camera.CameraUpDir);

	_shape->m_PVMMat = camera.ProjectionMat * camera.ViewMat * _shape->m_modelMat;
}