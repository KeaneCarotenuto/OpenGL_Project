#pragma once
#include <glew.h>

#include <string>
#include <iostream>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "CVertexArray.h"
#include <map>
//#include "CUniform.h"

enum class UniformType {
	Int,
	Float,
	Image,
	Vec3,
	Mat4,
};

class CShape
{
public:
	CShape();

	//std::map<std::string, void(*)()> test;

	CVertexArray m_VertexArray;

	GLuint m_VBO = NULL;
	GLuint m_VAO = NULL;
	GLuint m_EBO = NULL;

	GLuint m_program = NULL;

	//std::vector<CUniform> m_uniforms;

	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	GLfloat m_rotation = 0.0f;
	glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	bool m_useScreenScale = false;

	glm::mat4 m_modelMat = glm::mat4();
	glm::mat4 m_translationMat = glm::mat4();
	glm::mat4 m_rotationMat = glm::mat4();
	glm::mat4 m_scaleMat = glm::mat4();
	glm::mat4 m_PVMMat = glm::mat4();

	glm::mat4 m_PVMMat2 = glm::mat4();

	void SendUniform(UniformType _type, float _value, std::string _name);

	void SendUniform(UniformType _type, GLuint _value, std::string _name, int imageNum = 0);

	void SendUniform(UniformType _type, glm::mat4 _value, std::string _name);

	void SendUniform(UniformType _type, glm::vec3 _value, std::string _name);
	

	void GenBindVerts();
};