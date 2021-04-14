#pragma once
#include "CShape.h"
#include "CUniform.h"

//#include <stb_image.h>

CShape::CShape(int _verts, glm::vec3 _pos, float _rot, glm::vec3 _scale, bool _screenScale)
{
	m_position = _pos;
	m_rotation = _rot;
	m_scale = _scale;
	m_useScreenScale = _screenScale;


	float angle = 360 / _verts;

	m_VertexArray.vertices = { 0.0f, 0.0f, 0.0f,		1.0f, 1.0f, 1.0f,		0.5f, 0.5f, };

	float near1 = 0;

	for (int i = 1; i <= _verts; i++) {
		float xCoord = cos((((float)i - 2) * angle + angle * 0.5f) * M_PI / 180.0);
		float yCoord = sin((((float)i - 2) * angle + angle * 0.5f) * M_PI / 180.0);
		
		if (abs(xCoord) >= 0.99f) continue;

		if (abs(xCoord) > near1) near1 = abs(xCoord);
		else if (abs(yCoord) > near1) near1 = abs(yCoord);
	}

	for (int i = 1; i <= _verts; i++) {
		float xCoord = cos((((float)i - 2) * angle + angle * 0.5f) * M_PI / 180.0);
		float yCoord = sin((((float)i - 2) * angle + angle * 0.5f) * M_PI / 180.0);

		float xPos = xCoord;
		float yPos = yCoord;
		float zPos = 0;

		float rCol = 1.0f;
		float gCol = 1.0f;
		float bCol = 1.0f;

		float xTex = (xCoord / (near1 > 0 ? near1 : 1) + 1) / 2;
		float yTex = (yCoord / (near1 > 0 ? near1 : 1) + 1) / 2;

		m_VertexArray.vertices.push_back(xPos);
		m_VertexArray.vertices.push_back(yPos);
		m_VertexArray.vertices.push_back(zPos);

		m_VertexArray.vertices.push_back(rCol);
		m_VertexArray.vertices.push_back(gCol);
		m_VertexArray.vertices.push_back(bCol);

		m_VertexArray.vertices.push_back(xTex);
		m_VertexArray.vertices.push_back(yTex);

		m_VertexArray.indices.push_back(0);
		m_VertexArray.indices.push_back(i);
		m_VertexArray.indices.push_back((i + 1 > _verts ? 1 : i + 1));
	}
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
	glDrawElements(GL_TRIANGLES, m_VertexArray.indices.size(), GL_UNSIGNED_INT, 0);

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
