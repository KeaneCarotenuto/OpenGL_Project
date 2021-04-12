#pragma once
#include <string>
#include <glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

struct CUniform
{
	GLint location;
	virtual void Send() = 0;
};

struct ImageUniform : CUniform {
	ImageUniform(GLuint _val):
		value(_val)
	{

	}

	GLuint value = NULL;
	void Send() {
		//Activate and bind texture
		glActiveTexture(33984 + value);
		glBindTexture(GL_TEXTURE_2D, value);
		glUniform1i(location, value);
	}
};

struct IntUniform : CUniform {
	IntUniform(int _val) :
		value(_val)
	{

	}

	int value;
	void Send() {
		glUniform1i(location, value);
	}
};

struct FloatUniform : CUniform {
	FloatUniform(float _val) :
		value(_val)
	{

	}

	float value;
	void Send() {
		glUniform1f(location, value);
	}
};

struct Mat4Uniform : CUniform {
	Mat4Uniform(glm::mat4 _val) :
		value(_val)
	{

	}

	glm::mat4 value;
	void Send() {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
};

struct Vec3Uniform : CUniform {
	Vec3Uniform(glm::vec3 _val) :
		value(_val)
	{

	}

	glm::vec3 value;
	void Send() {
		//glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
};