#pragma once

// Library Includes
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class CShader {
public:
	GLuint m_id;
	const char* m_fileName;
	std::string m_shaderString;

	CShader(GLuint _id, const char* _fileName, std::string _shaderString);
	~CShader();
};

class CProgram {
public:
	GLuint m_id;
	std::vector<CShader*> m_shaders;

	CProgram(GLuint _id, std::vector<CShader*> _shaders);
	~CProgram();
};

struct Globals {
	static std::vector<CShader*> shaders;
	static std::vector<CProgram*> programs;
};

class ShaderLoader
{
	
public:	
	static GLuint CreateProgram(const char* VertexShaderFilename, const char* FragmentShaderFilename);

private:
	ShaderLoader(void);
	~ShaderLoader(void);
	static GLuint CreateShader(GLenum shaderType, const char* shaderName, CShader ** _shaderReturn);
	static std::string ReadShaderFile(const char *filename);
	static void PrintErrorDetails(bool isShader, GLuint id, const char* name);
};
