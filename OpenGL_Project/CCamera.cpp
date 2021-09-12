#include "CCamera.h"
#include "Utility.h"
#include <glm.hpp>

glm::vec3 CCamera::GetWorldRay()
{
	float x = (2.0f * utils::mousePos.x) / utils::windowHeight - 1.0f;
	float y = -(1.0f - (2.0f * utils::mousePos.y) / utils::windowWidth);
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);

	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

	glm::vec4 ray_eye = glm::inverse(GetCameraProjectionMat()) * ray_clip;

	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	glm::vec3 ray_wor = glm::vec3(glm::inverse(GetCameraViewMat()) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);

	return ray_wor;
}

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
