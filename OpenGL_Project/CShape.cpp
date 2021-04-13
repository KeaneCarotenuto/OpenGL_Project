#pragma once
#include "CShape.h"
#include "CUniform.h"

//#include <stb_image.h>

CShape::CShape()
{
}

void CShape::AddUniform(CUniform* _uniform, std::string _name)
{
	const char* val = _name.c_str();
	_uniform->location = glGetUniformLocation(m_program, val);

	m_uniforms.push_back(_uniform);
}

void CShape::UpdateUniform(CUniform* _NewUniform, std::string _name)
{
	GLuint loc = glGetUniformLocation(m_program, _name.c_str());
	_NewUniform->location = loc;
	for (CUniform*& _uniform : m_uniforms) {
		if (loc == _uniform->location) {
			delete _uniform;
			_uniform = _NewUniform;
			return;
		}
	}
}

void CShape::Render()
{
	glUseProgram(m_program);
	glBindVertexArray(m_VAO);

	for (CUniform* _uniform : m_uniforms) {
		_uniform->Send(this);
	}

	//Draw Elements	//Type	//Vertices
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void CShape::GenBindVerts()
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//Gen EBO 
	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	int* ind = &m_VertexArray.indices[0];
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_VertexArray.indices.size() * sizeof(int), ind, GL_DYNAMIC_DRAW);

	//Gen VBO 
	glGenBuffers(1, &m_VBO);
	//copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	float* verts = &m_VertexArray.vertices[0];
	glBufferData(GL_ARRAY_BUFFER, m_VertexArray.vertices.size() * sizeof(float), verts, GL_DYNAMIC_DRAW);

	//Set the vertex attributes pointers (How to interperet Vertex Data)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
}
