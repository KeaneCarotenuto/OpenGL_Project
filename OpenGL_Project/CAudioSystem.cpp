#include "CAudioSystem.h"

/// <summary>
/// Initialise the audio system
/// </summary>
/// <returns></returns>
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

/// <summary>
/// Add song/audio file to list for future playing
/// </summary>
/// <param name="_name">name of song</param>
/// <param name="_filePath">file path</param>
/// <param name="_mode">for repeating or not etc</param>
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

/// <summary>
/// Play sound
/// </summary>
/// <param name="_name"> name of sound</param>
void CAudioSystem::PlaySong(std::string _name)
{
	if (m_sounds[_name]) {
		m_system->playSound(m_sounds[_name], 0, false, 0);
	}
}

/// <summary>
/// Release all sounds and audio system, then clears list
/// </summary>
void CAudioSystem::ReleaseAll()
{
	for (std::map<std::string, FMOD::Sound*>::iterator it = m_sounds.begin(); it != m_sounds.end(); it++)
	{
		it->second->release();
	}

	m_system->release();

	m_sounds.clear();
}
