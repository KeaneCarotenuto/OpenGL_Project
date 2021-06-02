// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// (c) 2021 Media Design School
//
// File Name   : CAudioSystem.h
// Description : Audio management 
// Author      : Keane Carotenuto
// Mail        : KeaneCarotenuto@gmail.com

#pragma once
#include <map>
#include <string>
#include <fmod.hpp>
#include <iostream>

class CAudioSystem
{
private:
	std::map < std::string, FMOD::Sound*> m_sounds;
	FMOD::System* m_system;

	CAudioSystem();
	~CAudioSystem();

public:
	
	//Singleton class
	static CAudioSystem& GetInstance() {
		static CAudioSystem m_instance;

		return m_instance;
	}

	void AddSong(std::string _name, std::string _filePath, FMOD_MODE _mode);

	void PlaySong(std::string _name);

	void ReleaseAll();

	void Update() { m_system->update(); };
};

