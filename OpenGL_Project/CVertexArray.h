#pragma once
#include <vector>

#include "CVertex.h"

class CVertexArray
{
public:
	CVertexArray();

	std::vector<float> vertices;
	std::vector<int> indices;
	//const int num;
};

