#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <Windows.h>
#include <map>
#include <string>

#include "ShaderLoader.h"

#include "Utility.h"



class TextLabel
{
private:
	struct FontChar
	{
		GLuint textureID = 0;
		glm::ivec2 size = glm::ivec2();
		glm::ivec2 bearing = glm::ivec2();
		GLuint advance = 0;
	};

	GLuint GenerateTexture(FT_Face _face);

	static const int fontCharacterLimit = 128;
	bool m_initialized = false;

	std::string m_text;
	glm::vec2 m_scale;
	glm::vec3 m_color;
	glm::vec2 m_position;

	GLuint VAO_Text;
	GLuint VBO_DynamicQuad;
	GLuint Program_Text;
	glm::mat4 ProjectionMat;
	std::map<GLchar, FontChar> CharacterMap;
	
	

public:
	TextLabel(
		std::string _text,
		std::string _font,
		glm::ivec2 _pixelSize,
		glm::vec2 _pos,
		glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec2 _scale = glm::vec2(1.0f, 1.0f)
	);

	~TextLabel();

	void Render();
	void SetText(std::string _text) { this->m_text = _text; };
	void SetColor(glm::vec3 _color) { this->m_color = _color; };
	void SetScale(glm::vec2 _scale) { this->m_scale = _scale; };
	void SetPosition(glm::vec2 _pos) { this->m_position = _pos; };
};

