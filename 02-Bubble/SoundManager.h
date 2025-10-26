#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <string>

class SoundManager {
public:
    static SoundManager& instance();

    void loadSound(const std::string& name, const std::string& filepath);
    void playSound(const std::string& name);
    void playMusic(const std::string& filepath, bool loop = true);
    void stopMusic();
    bool isSoundPlaying(const std::string& name);
    void stopSound(const std::string& name);
    void setSoundVolume(const std::string& name, float volume);
    void setMusicVolume(float volume);

private:
    SoundManager() = default;
    std::map<std::string, sf::SoundBuffer> buffers;
    std::map<std::string, sf::Sound> sounds;
    sf::Music music;
};