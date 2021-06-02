#include "CObjectManager.h"

std::map<std::string, CShape*> CObjectManager::m_shapes;

/// <summary>
/// Add new shape to list with name
/// </summary>
/// <param name="_name"> name of shape</param>
/// <param name="_shape"></param>
void CObjectManager::AddShape(std::string _name, CShape* _shape)
{
	if (CObjectManager::m_shapes[_name]) {
		delete CObjectManager::m_shapes[_name];
	}

	CObjectManager::m_shapes[_name] = _shape;
}

/// <summary>
/// Update all shapes
/// </summary>
/// <param name="_deltaTime"></param>
/// <param name="_currentTime"></param>
void CObjectManager::UpdateAll(float _deltaTime, float _currentTime)
{
	for (std::map<std::string, CShape*>::iterator it = m_shapes.begin(); it != m_shapes.end(); it++)
	{
		it->second->Update(_deltaTime, _currentTime);
	}
}

/// <summary>
/// Render all shapes
/// </summary>
void CObjectManager::RenderAll()
{
	for (std::map<std::string, CShape*>::iterator it = m_shapes.begin(); it != m_shapes.end(); it++)
	{
		it->second->Render();
	}
}

/// <summary>
/// Finds shape of given name
/// </summary>
/// <param name="_name"></param>
/// <returns></returns>
CShape* CObjectManager::GetShape(std::string _name)
{
	std::map<std::string, CShape*>::iterator it = m_shapes.find(_name);
	if (it != m_shapes.end()) {
		return it->second;
	}
	{
		std::cout << "ERROR: Failed to get shape named " << _name << "." << std::endl;
		return nullptr;
	}
		
}
