#pragma once
#include <vector>

#include "CVertex.h"

class CVertexArray
{
public:
	std::vector<CVertex> vertices;
	std::vector<int[3]> indices;
	const int num;
};

