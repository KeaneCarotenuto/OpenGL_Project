#pragma once
#include <glew.h>
#include <string>
#include <iostream>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "CVertexArray.h"
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

	CVertexArray m_VertexArray;

	GLuint m_VBO = NULL;
	GLuint m_VAO = NULL;
	GLuint m_EBO = NULL;

	GLuint m_program = NULL;

	//std::vector<CUniform> m_uniforms;

	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	GLfloat m_rotation = 0.0f;
	glm::vec3 m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	bool m_useScreenScale = false;

	glm::mat4 m_modelMat = glm::mat4();
	glm::mat4 m_translationMat = glm::mat4();
	glm::mat4 m_rotationMat = glm::mat4();
	glm::mat4 m_scaleMat = glm::mat4();
	glm::mat4 m_PVMMat = glm::mat4();

	template <class TValue>
	void SendUniform(UniformType _type, TValue _value, char* _name, int imageNum);
};