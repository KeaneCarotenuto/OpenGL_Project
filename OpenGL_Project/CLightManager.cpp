#include "CLightManager.h"

const int CLightManager::MAX_POINT_LIGHTS;
PointLight CLightManager::PointLights[MAX_POINT_LIGHTS];

const int CLightManager::MAX_SPHERES;

DirectionalLight CLightManager::directionalLight = {
	glm::vec3(-1,-1,-1),
	glm::vec3(1,1,1),
	0.1f,
	0.5f
};

int CLightManager::currentLightNum = 0;

void CLightManager::AddLight(glm::vec3 _pos, glm::vec3 _col, float _ambientStrength, float _specularStrength, float _attenDist)
{
	if (currentLightNum < MAX_POINT_LIGHTS) {
		PointLight _tempLight;
		_tempLight.Position = _pos;
		_tempLight.Colour = _col;
		_tempLight.AmbientStrength = _ambientStrength;
		_tempLight.SpecularStrength = _specularStrength;

		_tempLight.AttenuationConstant = 1.0f;
		_tempLight.AttenuationLinear = glm::pow(1.1, -_attenDist + 3);
		_tempLight.AttenuationExponent = glm::pow(1.17, -_attenDist + 10);

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
		glUniform1f(glGetUniformLocation(_program, ((std::string)("PointLights[" + std::to_string(i) + "].AttenuationConstant")).c_str()), PointLights[i].AttenuationConstant);
		glUniform1f(glGetUniformLocation(_program, ((std::string)("PointLights[" + std::to_string(i) + "].AttenuationLinear")).c_str()), PointLights[i].AttenuationLinear);
		glUniform1f(glGetUniformLocation(_program, ((std::string)("PointLights[" + std::to_string(i) + "].AttenuationExponent")).c_str()), PointLights[i].AttenuationExponent);
	}

	glUniform3fv(glGetUniformLocation(_program, "DirLight.Direction"), 1, glm::value_ptr(directionalLight.Direction));
	glUniform3fv(glGetUniformLocation(_program, "DirLight.Colour"), 1, glm::value_ptr(directionalLight.Colour));
	glUniform1f(glGetUniformLocation(_program, "DirLight.AmbientStrength"), directionalLight.AmbientStrength);
	glUniform1f(glGetUniformLocation(_program, "DirLight.SpecularStrength"), directionalLight.SpecularStrength);

	for (int i = 0; i < MAX_SPHERES; i++) {
		if (CObjectManager::GetShape("sphere" + std::to_string(i), false) == nullptr) continue;

		glm::vec3 pos = CObjectManager::GetShape("sphere" + std::to_string(i))->GetPosition();
		float rad = CObjectManager::GetShape("sphere" + std::to_string(i))->GetScale().x / 2;

		glUniform3fv(glGetUniformLocation(_program, ((std::string)("Spheres[" + std::to_string(i) + "].Position")).c_str()), 1, glm::value_ptr(pos));
		glUniform1f(glGetUniformLocation(_program, ((std::string)("Spheres[" + std::to_string(i) + "].rad")).c_str()), rad);
	}

	glUseProgram(0);



	
}
