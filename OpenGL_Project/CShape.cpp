#pragma once
#include "CShape.h"
#include "CUniform.h"

//#include <stb_image.h>

CShape::CShape(int _verts, glm::vec3 _pos, float _rot, glm::vec3 _scale, bool _screenScale, int _renderPri)
{
	m_position = _pos;
	m_rotation = _rot;
	m_scale = _scale;
	m_orthoProject = _screenScale;


	std::cout << "Finding mesh" << std::endl;

	m_mesh = CMesh::GetMesh("poly" + std::to_string(_verts));

	if (m_mesh == nullptr) {
		CMesh::NewCMesh(_verts);

		m_mesh = CMesh::GetMesh("poly" + std::to_string(_verts));

		if (m_mesh == nullptr) {
			std::cout << std::endl << "ERROR: Failed to create polygon mesh of " << _verts << " sides." << std::endl;
		}
	}

	
}

CShape::CShape(std::string _meshName ,glm::vec3 _pos, float _rot, glm::vec3 _scale, bool _screenScale, int _renderPri)
{
	m_position = _pos;
	m_rotation = _rot;
	m_scale = _scale;
	m_orthoProject = _screenScale;

	m_mesh = CMesh::GetMesh(_meshName);
}

void CShape::AddUniform(CUniform* _uniform, std::string _name)
{
	const char* val = _name.c_str();
	_uniform->location = glGetUniformLocation(m_program, val);

	m_uniforms.push_back(_uniform);
}

/// <summary>
/// Update a uniform with a new one, if it exists
/// </summary>
/// <param name="_NewUniform"></param>
/// <param name="_name"></param>
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

/// <summary>
/// Update funciton for shapes
/// </summary>
/// <param name="deltaTime"></param>
/// <param name="currentTime"></param>
void CShape::Update(float deltaTime, float currentTime)
{
	m_currentTime = currentTime;
	UpdateUniform(new FloatUniform(currentTime), "CurrentTime");
}

/// <summary>
/// Renders shape
/// </summary>
void CShape::Render()
{
	UpdatePVM();

	glUseProgram(m_program);

	for (CUniform* _uniform : m_uniforms) {
		_uniform->Send(this);
	}

	m_mesh->Render();
	
	glUseProgram(0);
}

void CShape::UpdatePVM()
{
	//Calc transformation matrices
	m_translationMat = glm::translate(glm::mat4(), m_position);
	m_rotationMat = glm::rotate(glm::mat4(), glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	m_scaleMat = glm::scale(glm::mat4(), m_scale);

	//Convert from world space to screen space for ortho
	glm::mat4 pixelScale = (m_orthoProject ? glm::scale(glm::mat4(), glm::vec3(utils::windowWidth / 2, utils::windowHeight / 2, 1)) : glm::scale(glm::mat4(), glm::vec3(1, 1, 1)));

	//Calculate model matrix for shape
	m_modelMat = pixelScale * m_translationMat * m_rotationMat * m_scaleMat ;

	//Ortho project
	float halfWindowWidth = (float)utils::windowWidth / 2.0f;
	float halfWindowHeight = (float)utils::windowHeight / 2.0f;

	//Perspective project
	if (m_orthoProject) {
		m_camera->SetCameraProjectionMat(glm::ortho(0.0f, (float)utils::windowWidth, 0.0f, (float)utils::windowHeight, 0.0f, 100.0f));

		m_modelMat = m_camera->GetCameraProjectionMat() * m_modelMat;

		UpdateUniform(new Mat4Uniform(m_modelMat), "ModelMat");
		return;
	}
	else {
		m_camera->SetCameraProjectionMat(glm::perspective(glm::radians(45.0f), (float)utils::windowWidth / (float)utils::windowHeight, 0.1f, 100.0f));
	}
	

	//Calculate the new View matrix using all camera vars
	m_camera->SetCameraViewMat(glm::lookAt(m_camera->GetCameraPos(), m_camera->GetCameraPos() + m_camera->GetCameraLookDir(), m_camera->GetCameraUpDir()));

	//Calculate the PVM mat for the shape using camera view mat
	m_PVMMat = m_camera->GetCameraProjectionMat() * m_camera->GetCameraViewMat() * m_modelMat;

	UpdateUniform(new Mat4Uniform(m_PVMMat), "PVMMat");
}
