#pragma once
#include <string>
#include <glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "CShape.h"

class CUniform {
public:
	GLint location;
	virtual void Send(CShape * _shape) = 0;
};

class ImageUniform : public CUniform { 
public:
	ImageUniform(GLuint _val):
		value(_val)
	{

	}

	GLuint value = NULL;
	void Send(CShape * _shape) {
		//Activate and bind texture
		glActiveTexture(GL_TEXTURE0 + value);
		glBindTexture(GL_TEXTURE_2D, value);
		glUniform1i(location, value);
	}
};

class FloatUniform : public CUniform {
public:
	FloatUniform(float _val) :
		value(_val)
	{

	}

	float value;
	void Send(CShape* _shape) {
		glUniform1f(location, value);
	}
};

class AnimationUniform : public CUniform { 
public:
	AnimationUniform(GLuint _val, int _count, float _speed, CShape* _shape) :
		value(_val),
		frameCount(_count),
		SPF(_speed)
	{
		for (int i = 6; i < _shape->m_VertexArray.vertices.size(); i += 8) {
			_shape->m_VertexArray.vertices[i] /= frameCount;
		}
	}

	GLuint value = NULL;
	int frameCount = NULL;
	float SPF = NULL;
	int currentFrame = 0;
	float lastFrameTime = 0;

	void Send(CShape * _shape) {
		//Activate and bind texture
		glActiveTexture(GL_TEXTURE0 + value);
		glBindTexture(GL_TEXTURE_2D, value);
		glUniform1i(location, value);



		if (_shape->m_currentTime >= lastFrameTime + SPF) {
			lastFrameTime = _shape->m_currentTime;
			currentFrame++;

			if (currentFrame >= frameCount) currentFrame = 0;
		}

		float newVal = (float)currentFrame * (1.0f / frameCount);

		_shape->UpdateUniform(new FloatUniform(newVal), "offset");
	}
};

class IntUniform : public CUniform { 
public:
	IntUniform(int _val) :
		value(_val)
	{

	}

	int value;
	void Send(CShape * _shape) {
		glUniform1i(location, value);
	}
};

class Mat4Uniform : public CUniform { 
public:
	Mat4Uniform(glm::mat4 _val) :
		value(_val)
	{

	}

	glm::mat4 value;
	void Send(CShape * _shape) {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
};

class Vec3Uniform : public CUniform { 
public:
	Vec3Uniform(glm::vec3 _val) :
		value(_val)
	{

	}

	glm::vec3 value;
	void Send(CShape * _shape) {
		//glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
};

class Vec2Uniform : public CUniform {
public:
	Vec2Uniform(glm::vec2 _val) :
		value(_val)
	{

	}

	glm::vec2 value;
	void Send(CShape* _shape) {
		glUniform2fv(location, 1, glm::value_ptr(value));
	}
};