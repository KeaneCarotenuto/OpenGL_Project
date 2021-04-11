#pragma once
#include <string>

enum class UniformType {
	Int,
	Float,
	Vec3,
	Mat4,
};

template <typename TValue>
class CUniform
{
	UniformType type;
	TValue value;
	std::string name;
};

