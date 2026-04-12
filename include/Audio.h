// Description: Header file for the audio manager class.
// Related Files: Audio.cpp, MainUI.h, MainUI.cpp
// Date Created: 4/11/2026
// Last Edited: 4/12/2026

#pragma once

#include <SFML/Audio.hpp>
#include <list>
#include <string>
#include <unordered_map>

class AudioManager
{
public:
    AudioManager();

    bool loadSound(const std::string& id, const std::string& filename);

    bool playMusic(const std::string& filename, bool loop = true, float volume = 50.f);
    bool playMusicFadeIn(const std::string& filename, float fadeSeconds, bool loop = true, float targetVolume = 50.f);

    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    void playSound(const std::string& id, float volume = 100.f);

    void setMusicVolume(float volume);
    void setSoundVolume(float volume);

    void update();
    void cleanupStoppedSounds();

private:
    sf::Music m_music;
    std::unordered_map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::list<sf::Sound> m_activeSounds;

    float m_musicVolume;
    float m_soundVolume;

    bool m_fadeInActive;
    float m_fadeDuration;
    float m_fadeTargetVolume;
    sf::Clock m_fadeClock;
};