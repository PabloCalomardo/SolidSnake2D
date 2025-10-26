#include "SoundManager.h"
#include <iostream>

SoundManager& SoundManager::instance() {
    static SoundManager instance;
    return instance;
}

void SoundManager::loadSound(const std::string& name, const std::string& filepath) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filepath)) {
        std::cerr << "Error cargando sonido: " << filepath << std::endl;
        return;
    }
    buffers[name] = buffer;
    sounds[name].setBuffer(buffers[name]);
}

void SoundManager::playSound(const std::string& name) {
    if (sounds.count(name))
        sounds[name].play();
}

void SoundManager::playMusic(const std::string& filepath, bool loop) {
    if (!music.openFromFile(filepath)) {
        std::cerr << "Error cargando música: " << filepath << std::endl;
        return;
    }
    music.setLoop(loop);
    music.play();
}

void SoundManager::stopMusic() {
    music.stop();
}

bool SoundManager::isSoundPlaying(const std::string& name) {
    if (sounds.count(name)) {
        return sounds[name].getStatus() == sf::Sound::Playing;
    }
    return false;
}

void SoundManager::stopSound(const std::string& name) {
    // Detener el sonido solo si existe
    if (sounds.count(name)) {
        sounds[name].stop();
    }
}

void SoundManager::setSoundVolume(const std::string& name, float volume) {
    sounds[name].setVolume(volume);
}

void SoundManager::setMusicVolume(float volume) {
    music.setVolume(volume);
}