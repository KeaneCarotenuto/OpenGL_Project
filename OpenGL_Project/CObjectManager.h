// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// (c) 2021 Media Design School
//
// File Name   : CObjectManager.h
// Description : Manages all/most shapes update and render
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#pragma once
#include <map>
#include <string>
#include <algorithm>
#include "CShape.h"

class CObjectManager
{
private:

	static std::map<std::string, CShape*> m_shapes;

public:

	static void AddShape(std::string _name, CShape* _shape);
	static void UpdateAll(float _deltaTime, float _currentTime);
	static void RenderAll();

	static CShape* GetShape(std::string _name);
};

