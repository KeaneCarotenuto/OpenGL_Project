#pragma once
#include <vector>

class CVertexArray
{
public:
	CVertexArray();

	std::vector<float> vertices;
	std::vector<int> indices;
	//const int num;
};

