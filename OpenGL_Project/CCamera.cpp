#include "CCamera.h"

void CCamera::UpdateRotation()
{
	//Reset yaw as to not infinitely increase
	if (glm::abs(GetYaw()) > 2 * glm::pi<float>()) {
		SetYaw(0);
	}
	//Clamp pitch to just before reaching 90 degrees
	if (GetPitch() > glm::half_pi<float>()) {
		SetPitch(glm::half_pi<float>() - 0.0001f);
	}
	else if (GetPitch() < -glm::half_pi<float>()) {
		SetPitch(-glm::half_pi<float>() + 0.0001f);
	}

	//Update the look direction based on new yaw and pitch
	SetCameraForwardDir(glm::vec3(
		-sin(GetYaw()) * cos(GetPitch()),
		-sin(GetPitch()),
		-cos(GetYaw()) * cos(GetPitch())
	));

	//Update the right direction based on new yaw and pitch
	SetCameraRightDir(glm::vec3(
		-cos(GetYaw()),
		0.0,
		sin(GetYaw())
	));

	//Update the up direction using cross product of up and right dir
	SetCameraUpDir(glm::cross(GetCameraForwardDir(), GetCameraRightDir()));

	//Normalise all directions
	SetCameraForwardDir(glm::normalize(GetCameraForwardDir()));
	SetCameraRightDir(glm::normalize(GetCameraRightDir()));
	SetCameraUpDir(glm::normalize(GetCameraUpDir()));
}
