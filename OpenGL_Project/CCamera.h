#pragma once
#include <glm.hpp>

class CCamera
{
public:
	// Camera Variables 
	glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 CameraLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 CameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraUpDir = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 ViewMat;
	glm::mat4 ProjectionMat;

	glm::vec3 GetPos() { return CameraPos; };
	glm::vec3 GetLookDir() { return CameraLookDir; };

	//void Draw(CShape* _shape);
};

