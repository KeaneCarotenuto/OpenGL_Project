#pragma once
#include "CShape.h"

//#include <stb_image.h>

CShape::CShape()
{
}


void CShape::SendUniform(UniformType _type, float _value, std::string _name) {
	if (_type != UniformType::Float) {
		std::cout << "Got Float, expected type: " << std::to_string((int)_type) << ". See CShape.h for types..." << std::endl;
	}

	const char* val = _name.c_str();

	GLint FloatLoc = glGetUniformLocation(m_program, val);
	glUniform1f(FloatLoc, (float)_value);
}

void CShape::SendUniform(UniformType _type, GLuint _value, std::string _name, int imageNum)
{
	const char* val = _name.c_str();

	switch (_type)
	{
	case UniformType::Int: {
		GLint FloatLoc = glGetUniformLocation(m_program, val);
		glUniform1i(FloatLoc, (int)_value);
	}
						 break;

	case UniformType::Image: {
		//Activate and bind texture
		glActiveTexture(33984 + imageNum);
		glBindTexture(GL_TEXTURE_2D, (GLuint)_value);
		glUniform1i(glGetUniformLocation(m_program, val), imageNum);
	}
						   break;

	default: {
		std::cout << "Got GLuint, expected type: " << std::to_string((int)_type) << ". See CShape.h for types..." << std::endl;
	}
		   break;
	}
}


void CShape::SendUniform(UniformType _type, glm::mat4 _value, std::string _name) {
	if (_type != UniformType::Mat4) {
		std::cout << "Got Mat4, expected type: " << std::to_string((int)_type) << ". See CShape.h for types..." << std::endl;
	}

	const char* val = _name.c_str();

	GLuint Mat4Loc = glGetUniformLocation(m_program, val);
	glUniformMatrix4fv(Mat4Loc, 1, GL_FALSE, glm::value_ptr((glm::mat4)_value));
}

void CShape::SendUniform(UniformType _type, glm::vec3 _value, std::string _name) {
	if (_type != UniformType::Mat4) {
		std::cout << "Got Vec3, expected type: " << std::to_string((int)_type) << ". See CShape.h for types..." << std::endl;
	}

	const char* val = _name.c_str();

	//GLint FloatLoc = glGetUniformLocation(m_program, val);
	//glUniform3fv(FloatLoc, (glm::vec3)_value);
}

void CShape::GenBindVerts()
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//Gen EBO 
	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	int* ind = &m_VertexArray.indices[0];
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_VertexArray.indices.size() * sizeof(int), ind, GL_STATIC_DRAW);

	//Gen VBO 
	glGenBuffers(1, &m_VBO);
	//copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	float* verts = &m_VertexArray.vertices[0];
	glBufferData(GL_ARRAY_BUFFER, m_VertexArray.vertices.size() * sizeof(float), verts, GL_STATIC_DRAW);

	//Set the vertex attributes pointers (How to interperet Vertex Data)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
}
