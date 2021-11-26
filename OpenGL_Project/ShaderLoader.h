// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// 
// (c) 2021 Media Design School
//
// File Name   : ShaderLoader.h
// Description : Used for creating shaders and programs
// Author      : Keane Carotenuto, Callan Moore (Supplied Code ideas/pictures/descriptions etc)
// Mail        : KeaneCarotenuto@gmail.com

#pragma once

// Library Includes
#include <glew.h>
#include <glfw3.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

/// <summary>
/// Shader class to store info about shader
/// </summary>
class CShader {
public:
	GLuint m_id;
	const char* m_fileName;
	std::string m_shaderString;

	CShader(GLuint _id, const char* _fileName, std::string _shaderString);
	~CShader();
};

/// <summary>
/// Program class to store info about Programs
/// </summary>
class CProgram {
public:
	GLuint m_id;

	//list of shaders in program
	std::vector<CShader*> m_shaders;

	CProgram(GLuint _id, std::vector<CShader*> _shaders);
	~CProgram();
};

struct Globals {
	static std::vector<CShader* > shaders;
	static std::map<std::string, CProgram*> programs;
};

class ShaderLoader
{
	
public:	
	static GLuint CreateProgram(std::string _name, const char* vertexShaderFilename, const char* fragmentShaderFilename, const char* geometryShaderFilename = nullptr, const char* tessControlShaderFilename = nullptr, const char* tessEvalShaderFilename = nullptr);
	static GLuint CreateProgramCompute(std::string _name, const char* computeShaderFilename);
	static CProgram* GetProgram(std::string _name);

private:
	ShaderLoader(void);
	~ShaderLoader(void);
	static GLuint CreateShader(GLenum shaderType, const char* shaderName, CShader ** _shaderReturn);
	static std::string ReadShaderFile(const char *filename);
	static void PrintErrorDetails(bool isShader, GLuint id, const char* name);
};
