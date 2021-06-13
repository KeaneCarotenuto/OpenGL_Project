#include "CLightManager.h"

const int CLightManager::MAX_POINT_LIGHTS;
PointLight CLightManager::PointLights[MAX_POINT_LIGHTS];

int CLightManager::currentLightNum = 0;


void CLightManager::AddLight(glm::vec3 _pos, glm::vec3 _col, float _ambientStrength, float _specularStrength)
{
	if (currentLightNum < MAX_POINT_LIGHTS) {
		PointLight _tempLight;
		_tempLight.Position = _pos;
		_tempLight.Colour = _col;
		_tempLight.AmbientStrength = _ambientStrength;
		_tempLight.SpecularStrength = _specularStrength;

		PointLights[currentLightNum] = _tempLight;
		currentLightNum++;
	}
	else {
		std::cout << "ERROR: Too many lights being added";
	}
}

void CLightManager::UpdateUniforms(GLuint _program)
{
	glUseProgram(_program);
	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		std::string str = std::to_string(i);
		const char* num = str.c_str();
		glUniform3fv(glGetUniformLocation(_program, ((std::string)("PointLights[" + std::to_string(i) + "].Position")).c_str()), 1, glm::value_ptr(PointLights[i].Position));
		glUniform3fv(glGetUniformLocation(_program, ((std::string)("PointLights[" + std::to_string(i) + "].Colour")).c_str()), 1, glm::value_ptr(PointLights[i].Colour));
		glUniform1f(glGetUniformLocation(_program, ((std::string)("PointLights[" + std::to_string(i) + "].AmbientStrength")).c_str()), PointLights[i].AmbientStrength);
		glUniform1f(glGetUniformLocation(_program, ((std::string)("PointLights[" + std::to_string(i) + "].SpecularStrength")).c_str()), PointLights[i].SpecularStrength);
	}
	glUseProgram(0);
}
