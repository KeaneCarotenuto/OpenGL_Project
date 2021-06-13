// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// (c) 2021 Media Design School
//
// File Name   : Camera.h
// Description : Camera class for getting proj matrix
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#pragma once
#include <glm.hpp>
#include <gtx/rotate_vector.hpp>

class CCamera
{
private:
	// Camera Variables 
	glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 CameraLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 CameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 CameraUpDir = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 CameraRightDir = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::mat4 ViewMat;
	glm::mat4 ProjectionMat;

	float yaw = 0.0f;
	float pitch = 0.0f;

	float speed = 2.0f;

public:

	void SetCameraPos(glm::vec3 _pos) { CameraPos = _pos; };
	glm::vec3 GetCameraPos() { return CameraPos; };

	void SetCameraLookDir(glm::vec3 _dir) { CameraLookDir = _dir; };
	glm::vec3 GetCameraLookDir() { return CameraLookDir; };

	void SetCameraTargetPos(glm::vec3 _targetPos) { CameraTargetPos = _targetPos; };
	glm::vec3 GetCameraTargetPos() { return CameraTargetPos; };

	void SetCameraUpDir(glm::vec3 _upDir) { CameraUpDir = _upDir; };
	glm::vec3 GetCameraUpDir() { return CameraUpDir; };

	void SetCameraRightDir(glm::vec3 _rightDir) { CameraRightDir = _rightDir; };
	glm::vec3 GetCameraRightDir() { return CameraRightDir; };

	void SetCameraViewMat(glm::mat4 _viewMat) { ViewMat = _viewMat; };
	glm::mat4 GetCameraViewMat() { return ViewMat; };

	void SetCameraProjectionMat(glm::mat4 _projMat) { ProjectionMat = _projMat; };
	glm::mat4 GetCameraProjectionMat() { return ProjectionMat; };

	void SetYaw(float _rads) { yaw = _rads; };
	float GetYaw() { return yaw; };

	void SetPitch(float _rads) { pitch = _rads; };
	float GetPitch() { return pitch; };

	void SetSpeed(float _speed) { speed = _speed; };
	float GetSpeed() { return speed; };

	void UpdateRotation();
};

