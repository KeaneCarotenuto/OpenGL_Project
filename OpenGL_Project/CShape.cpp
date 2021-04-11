#include "CShape.h"

template<class TValue>
inline void CShape::SendUniform(UniformType _type, TValue _value, char* _name, int imageNum)
{
	switch (_type)
	{
	case UniformType::Int:
		break;

	case UniformType::Float:
		GLint FloatLoc = glGetUniformLocation(this->m_program, _name);
		glUniform1f(FloatLoc, _value);
		break;

	case UniformType::Image:
		glActiveTexture((33984 + imageNum));
		glBindTexture(GL_TEXTURE_2D, _value);
		glUniform1i(glGetUniformLocation(this->m_program, _name), imageNum);
		break;

	case UniformType::Vec3:
		break;

	case UniformType::Mat4:
		GLuint Mat4Loc = glGetUniformLocation(this->m_program, _name);
		glUniformMatrix4fv(Mat4Loc, 1, GL_FALSE, glm::value_ptr(_value));
		break;

	default:
		break;
	}
}