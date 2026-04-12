// Description: Implementation file for the audio manager class.
// Related Files: Audio.h, MainUI.h, MainUI.cpp
// Date Created: 4/11/2026
// Last Edited: 4/11/2026

#include "Audio.h"

#include <algorithm>

AudioManager::AudioManager()
    : m_musicVolume(50.f),
    m_soundVolume(100.f),
    m_fadeInActive(false),
    m_fadeDuration(0.f),
    m_fadeTargetVolume(50.f)
{
}

bool AudioManager::loadSound(const std::string& id, const std::string& filename)
{
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename))
        return false;

    m_soundBuffers[id] = std::move(buffer);
    return true;
}

bool AudioManager::playMusic(const std::string& filename, bool loop, float volume)
{
    m_fadeInActive = false;

    if (!m_music.openFromFile(filename))
        return false;

    m_musicVolume = volume;
    m_music.setLooping(loop);
    m_music.setVolume(m_musicVolume);
    m_music.play();
    return true;
}

bool AudioManager::playMusicFadeIn(const std::string& filename, float fadeSeconds, bool loop, float targetVolume)
{
    if (!m_music.openFromFile(filename))
        return false;

    m_music.setLooping(loop);
    m_music.setVolume(0.f);
    m_music.play();

    m_fadeInActive = true;
    m_fadeDuration = std::max(0.001f, fadeSeconds);
    m_fadeTargetVolume = targetVolume;
    m_musicVolume = targetVolume;
    m_fadeClock.restart();

    return true;
}

void AudioManager::stopMusic()
{
    m_fadeInActive = false;
    m_music.stop();
}

void AudioManager::pauseMusic()
{
    m_music.pause();
}

void AudioManager::resumeMusic()
{
    if (m_music.getStatus() != sf::SoundSource::Status::Playing)
        m_music.play();
}

void AudioManager::playSound(const std::string& id, float volume)
{
    auto it = m_soundBuffers.find(id);
    if (it == m_soundBuffers.end())
        return;

    m_activeSounds.emplace_back(it->second);
    m_activeSounds.back().setVolume(volume * (m_soundVolume / 100.f));
    m_activeSounds.back().play();
}

void AudioManager::setMusicVolume(float volume)
{
    m_musicVolume = volume;
    m_music.setVolume(m_musicVolume);
}

void AudioManager::setSoundVolume(float volume)
{
    m_soundVolume = volume;
}

void AudioManager::update()
{
    if (m_fadeInActive)
    {
        float t = m_fadeClock.getElapsedTime().asSeconds() / m_fadeDuration;
        t = std::min(t, 1.0f);

        m_music.setVolume(m_fadeTargetVolume * t);

        if (t >= 1.0f)
        {
            m_music.setVolume(m_fadeTargetVolume);
            m_fadeInActive = false;
        }
    }

    cleanupStoppedSounds();
}

void AudioManager::cleanupStoppedSounds()
{
    m_activeSounds.erase(
        std::remove_if(
            m_activeSounds.begin(),
            m_activeSounds.end(),
            [](const sf::Sound& sound)
            {
                return sound.getStatus() == sf::SoundSource::Status::Stopped;
            }),
        m_activeSounds.end());
}