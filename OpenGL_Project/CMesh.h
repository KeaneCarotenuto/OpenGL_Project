#pragma once
#include <vector>
#include <map>
#include <string>
#include <glew.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "CVertexArray.h"

class CMesh
{
private:

	CMesh(std::vector<float> _vertices, std::vector<int> _indices);

	static std::map<std::string, CMesh*> meshMap;

	void GenBindVerts();

	CVertexArray m_VertexArray;

	GLuint m_VBO = NULL;
	GLuint m_VAO = NULL;
	GLuint m_EBO = NULL;

public:
	static void NewCMesh(std::string _name, std::vector<float> _vertices, std::vector<int> _indices);
	static void NewCMesh(int _verts);

	static CMesh* GetMesh(std::string _name, bool* _doesExist = nullptr);

	GLuint GetVBO() { return m_VBO; };
	GLuint GetVAO() { return m_VAO; };
	GLuint GetEBO() { return m_EBO; };

	std::vector<float> GetVertices() { return m_VertexArray.vertices; };
	std::vector<int> GetIndices() { return m_VertexArray.indices; };

	void Render();
};

