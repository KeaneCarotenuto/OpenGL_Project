// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// 
// (c) 2021 Media Design School
//
// File Name   : CVertexArray.cpp
// Description : For storing vertex data
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#pragma once
#include <vector>

class CVertexArray
{
public:
	CVertexArray();

	std::vector<float> vertices;
	std::vector<int> indices;
};

