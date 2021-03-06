// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// (c) 2021 Media Design School
//
// File Name   : CMesh.h
// Description : Mesh class for shape blueprints
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#pragma once
#include <vector>
#include <map>
#include <string>
#include <glew.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "CVertexArray.h"

enum class VertType
{
	Pos_Col_Tex,
	Pos_Tex_Norm,
	Pos,
};

class CMesh
{
private:

	CMesh(VertType _type, std::vector<float> _vertices, std::vector<int> _indices, bool defaultBind);

	static std::map<std::string, CMesh*> meshMap;

	void GenBindVerts();

	CVertexArray m_VertexArray;

	GLuint m_VBO = NULL;
	GLuint m_VAO = NULL;
	GLuint m_EBO = NULL;

	VertType type;

public:

	static void NewCMesh(std::string _name, VertType _type, std::vector<float> _vertices, std::vector<int> _indices);
	static void NewCMesh(int _verts);
	static void NewCMesh(std::string _name, float _radius, int _fidelity);

	static CMesh* GetMesh(std::string _name, bool* _doesExist = nullptr);

	GLuint GetVBO() { return m_VBO; };
	GLuint GetVAO() { return m_VAO; };
	GLuint GetEBO() { return m_EBO; };

	std::vector<float> GetVertices() { return m_VertexArray.vertices; };
	std::vector<int> GetIndices() { return m_VertexArray.indices; };

	void Render();
};

