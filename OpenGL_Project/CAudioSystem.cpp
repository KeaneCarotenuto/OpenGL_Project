#include "CAudioSystem.h"

CAudioSystem::CAudioSystem()
{
	if (FMOD::System_Create(&m_system) != FMOD_OK) {
		std::cout << "FMOD ERROR: Audio System failed to create." << std::endl;
	}

	if (m_system->init(100, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0) != FMOD_OK) {
		std::cout << "FMOD ERROR: Audio system failed to initialize." << std::endl;
	}
}

CAudioSystem::~CAudioSystem()
{
}

void CAudioSystem::AddSong(std::string _name, std::string _filePath, FMOD_MODE _mode)
{
	FMOD::Sound* tempSound;
	if (m_system->createSound(_filePath.c_str(), _mode, 0, &tempSound) != FMOD_OK) {
		std::cout << "FMOD ERROR: Failed to load sound using createsound(...)" << std::endl;
	}
	else {
		if (m_sounds[_name]) {
			delete m_sounds[_name];
		}

		m_sounds[_name] = tempSound;
	}
}

void CAudioSystem::PlaySong(std::string _name)
{
	if (m_sounds[_name]) {
		m_system->playSound(m_sounds[_name], 0, false, 0);
	}
}

void CAudioSystem::ReleaseAll()
{
	for (std::map<std::string, FMOD::Sound*>::iterator it = m_sounds.begin(); it != m_sounds.end(); it++)
	{
		it->second->release();
	}

	m_system->release();

	m_sounds.clear();
}
