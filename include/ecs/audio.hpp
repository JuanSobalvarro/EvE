#pragma once

#include <dr_wav.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

#include "ecs/components.hpp"


namespace ecs {

class AudioSystem {
public:
    AudioSystem();
    ~AudioSystem();

    void playSound(const char* filePath, bool loop = false, float pitch = 1.0f, float gain = 1.0f);
    void stopAllSounds();
private:
    ALCdevice* device_ = nullptr;
    ALCcontext* context_ = nullptr;
    std::vector<AudioSource> activeSources_;
};

} // namespace ecs