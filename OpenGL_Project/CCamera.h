#pragma once
#include <glm.hpp>

class CCamera
{
private:
	// Camera Variables 
	glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 CameraLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 CameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraUpDir = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 ViewMat;
	glm::mat4 ProjectionMat;

public:
	void SetCameraPos(glm::vec3 _pos) { CameraPos = _pos; };
	glm::vec3 GetCameraPos() { return CameraPos; };

	void SetCameraLookDir(glm::vec3 _dir) { CameraLookDir = _dir; };
	glm::vec3 GetCameraLookDir() { return CameraLookDir; };

	void SetCameraTargetPos(glm::vec3 _targetPos) { CameraTargetPos = _targetPos; };
	glm::vec3 GetCameraTargetPos() { return CameraTargetPos; };

	void SetCameraUpDir(glm::vec3 _upDir) { CameraUpDir = _upDir; };
	glm::vec3 GetCameraUpDir() { return CameraUpDir; };

	void SetCameraViewMat(glm::mat4 _viewMat) { ViewMat = _viewMat; };
	glm::mat4 GetCameraViewMat() { return ViewMat; };

	void SetCameraProjectionMat(glm::mat4 _projMat) { ProjectionMat = _projMat; };
	glm::mat4 GetCameraProjectionMat() { return ProjectionMat; };
};

