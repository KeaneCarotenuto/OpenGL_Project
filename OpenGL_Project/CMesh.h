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

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

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
	Pos_Col
};

class CMesh
{
private:

	CMesh(VertType _type, std::vector<float> _vertices, std::vector<int> _indices, bool defaultBind);

	static std::map<std::string, CMesh*> meshMap;

	void GenBindVerts();

	CVertexArray m_VertexArray;

	int m_width;
	int m_length;

	int m_widthDivs;
	int m_lengthDivs;

	GLuint m_VBO = NULL;
	GLuint m_VAO = NULL;
	GLuint m_EBO = NULL;

	VertType type;

public:

	static void NewCMesh(std::string _name, VertType _type, std::vector<float> _vertices, std::vector<int> _indices);
	static void NewCMesh(int _verts);
	static std::vector<float> LoadImage(std::string _fileName);
	static float GetBrightness(int x, int y, int image);
	static void NewCMesh(std::string _name, float _radius, int _fidelity);
	static void NewPlane(std::string _name, float _width, float _length, int _divW, int _divL);

	static CMesh* GetMesh(std::string _name, bool* _doesExist = nullptr);

	int GetWidth() { return m_width; }
	int GetLength() { return m_length; }

	int GetWidthDivs() { return m_widthDivs; }
	int GetLengthDivs() { return m_lengthDivs; }

	GLuint GetVBO() { return m_VBO; };
	GLuint GetVAO() { return m_VAO; };
	GLuint GetEBO() { return m_EBO; };

	void SetVertices(std::vector<float> _verts) { m_VertexArray.vertices = _verts; };
	std::vector<float> GetVertices() { return m_VertexArray.vertices; };
	std::vector<int> GetIndices() { return m_VertexArray.indices; };

	void Render();
};

