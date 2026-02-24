//
//  AudioController.cpp
//
//  Created by Felicia Chen on 2/23/26.
//
#include "AudioController.h"

using namespace cugl;
using namespace cugl::audio;

std::shared_ptr<AssetManager> AudioController::_assets = nullptr;
std::shared_ptr<AudioQueue> AudioController::_queue = nullptr;

void AudioController::init(const std::shared_ptr<AssetManager>& assets) {
    _assets = assets;
    
    AudioEngine::start();
    _queue = AudioEngine::get()->getMusicQueue();
}

void AudioController::shutdown() {
    _queue = nullptr;
    _assets = nullptr;
    AudioEngine::stop();
}

void AudioController::playBGM(const std::string& key, bool loop) {
    auto music = _assets->get<cugl::audio::Sound>(key);
    _queue->play(music, loop);
}

void AudioController::pauseBGM() {
    if (_queue) _queue->pause();
}

void AudioController::resumeBGM() {
    if (_queue) _queue->resume();
}

void AudioController::playSFX(const std::string& key) {
    auto sfx = _assets->get<cugl::audio::Sound>(key);
    AudioEngine::get()->play(key, sfx);
}
