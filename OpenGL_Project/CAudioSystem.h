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
	

	static CAudioSystem& GetInstance() {
		static CAudioSystem m_instance;

		return m_instance;
	}

	void AddSong(std::string _name, std::string _filePath, FMOD_MODE _mode);

	void PlaySong(std::string _name);

	void ReleaseAll();

	void Update() { m_system->update(); };
};

