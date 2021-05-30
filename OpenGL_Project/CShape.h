// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// 
// (c) 2021 Media Design School
//
// File Name   : CShape.h
// Description : Contains data and logic to create shapes in OpenGL
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#pragma once
#include <glew.h>

#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "CVertexArray.h"
#include <map>

#include "CCamera.h"
#include "Utility.h"
#include "CMesh.h"

class CUniform;

//enum class UniformType {
//	Int,
//	Float,
//	Image,
//	Vec3,
//	Mat4,
//};

class CShape
{
private:
	

	CMesh* m_mesh;

	GLuint m_program = NULL;

	CCamera* m_camera;

	//List of uniforms
	std::vector<CUniform*> m_uniforms;

	float m_currentTime;

	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	GLfloat m_rotation = 0.0f;
	glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f);

	bool isPerspective = false;
	

	glm::mat4 m_modelMat = glm::mat4();
	glm::mat4 m_translationMat = glm::mat4();
	glm::mat4 m_rotationMat = glm::mat4();
	glm::mat4 m_scaleMat = glm::mat4();
	glm::mat4 m_PVMMat = glm::mat4();

public:
	bool m_orthoProject = false;

	CShape(int _verts, glm::vec3 _pos, float _rot, glm::vec3 _scale, bool _screenScale);
	CShape(std::string _meshName, glm::vec3 _pos, float _rot, glm::vec3 _scale, bool _screenScale);

	void SetProgram(GLuint _program) { m_program = _program; };
	void SetCamera(CCamera* _camera) { m_camera = _camera; };
	void SetMesh(CMesh* _mesh) { m_mesh = _mesh; };
	void SetPosition(glm::vec3 _pos) { m_position = _pos; };

	glm::mat4 GetPVM() { return m_PVMMat; };
	glm::vec3 GetPosition() { return m_position; };
	glm::vec3 GetScale() { return m_scale; };


	//Adding/updating uniforms
	void AddUniform(CUniform* _uniform, std::string _name);
	void UpdateUniform(CUniform* _NewUniform, std::string _name);

	void Update(float deltaTime, float currentTime);
	void Render();

	void UpdatePVM();
};
