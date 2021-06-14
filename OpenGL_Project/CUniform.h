// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// 
// (c) 2021 Media Design School
//
// File Name   : CUniform.h
// Description : Contains datastructures to create Uniforms for shapes
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#pragma once
#include <string>
#include <glew.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "CShape.h"

class CUniform {
public:
	GLint location = NULL;
	virtual void Send(CShape * _shape) = 0;
};

/// <summary>
/// Uniform for "static" images
/// </summary>
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

/// <summary>
/// Uniform for "static" images
/// </summary>
class CubemapUniform : public CUniform {
public:
	CubemapUniform(GLuint _val) :
		value(_val)
	{

	}

	GLuint value = NULL;
	void Send(CShape* _shape) {
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		//Activate and bind texture
		glActiveTexture(GL_TEXTURE0 + value);
		glBindTexture(GL_TEXTURE_CUBE_MAP, value);
		glUniform1i(location, value);
	}
};

/// <summary>
/// uniform for floats
/// </summary>
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

/// <summary>
/// Uniform for animated images
/// </summary>
class AnimationUniform : public CUniform { 
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="_va">Image GLuint</param>
	/// <param name="_count">Amount of frames in image</param>
	/// <param name="_speed">Seconds per frame</param>
	/// <param name="_shape">The shape that is using the image</param>
	/// <returns></returns>
	AnimationUniform(GLuint _val, int _count, float _speed, CShape* _shape) :
		value(_val),
		frameCount(_count),
		SPF(_speed)
	{
		
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

		//Increment the current frame based on speed defined
		if (utils::currentTime >= lastFrameTime + SPF) {
			lastFrameTime = utils::currentTime;
			currentFrame++;

			if (currentFrame >= frameCount) currentFrame = 0;
		}

		float newVal = (float)currentFrame * (1.0f / frameCount);

		_shape->UpdateUniform(new FloatUniform(newVal), "offset");
	}
};

/// <summary>
/// Uniform for ints...
/// </summary>
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

/// <summary>
/// Uniform for glm::mat4
/// </summary>
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

/// <summary>
/// Uniform for glm::vec3
/// </summary>
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

/// <summary>
/// Uniform for glm::vec2
/// </summary>
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

class Vec4Uniform : public CUniform {
public:
	Vec4Uniform(glm::vec4 _val) :
		value(_val)
	{

	}

	glm::vec4 value;
	void Send(CShape* _shape) {
		glUniform4fv(location, 1, glm::value_ptr(value));
	}
};