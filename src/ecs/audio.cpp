#define DR_WAV_IMPLEMENTATION
#include "ecs/audio.hpp"

ecs::AudioSystem::AudioSystem() {
    device_ = alcOpenDevice(nullptr);
    if (!device_) {
        throw std::runtime_error("Failed to open OpenAL device");
    }

    context_ = alcCreateContext(device_, nullptr);
    if (!context_ || alcMakeContextCurrent(context_) == AL_FALSE) {
        if (context_) {
            alcDestroyContext(context_);
            context_ = nullptr;
        }
        alcCloseDevice(device_);
        device_ = nullptr;
        throw std::runtime_error("Failed to create OpenAL context");
    }
}

ecs::AudioSystem::~AudioSystem() {
    stopAllSounds();

    alcMakeContextCurrent(nullptr);
    if (context_) {
        alcDestroyContext(context_);
        context_ = nullptr;
    }

    if (device_) {
        alcCloseDevice(device_);
        device_ = nullptr;
    }
}

void ecs::AudioSystem::playSound(const char* filePath, bool loop, float pitch, float gain) {

    activeSources_.erase(std::remove_if(activeSources_.begin(), activeSources_.end(),
        [](const AudioSource& s) {
            ALint state;
            alGetSourcei(s.alSource, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING) {
                alDeleteSources(1, &s.alSource);
                alDeleteBuffers(1, &s.alBuffer);
                return true;
            }
            return false;
        }), activeSources_.end());

    if (activeSources_.size() >= 128) { // OpenAL Soft default limit is often 256
        std::cerr << "Audio cap reached, skipping sound: " << filePath << std::endl;
        return;
    }

    if (!device_ || !context_) {
        std::cerr << "OpenAL is not initialized" << std::endl;
        return;
    }

    if (!filePath) {
        std::cerr << "Invalid audio file path" << std::endl;
        return;
    }

    unsigned int channels = 0;
    unsigned int sampleRate = 0;
    drwav_uint64 totalFrameCount = 0;
    drwav_int16* pcmFrames = drwav_open_file_and_read_pcm_frames_s16(filePath, &channels, &sampleRate, &totalFrameCount, nullptr);
    if (!pcmFrames) {
        std::cerr << "Failed to load audio: " << filePath << std::endl;
        return;
    }

    ALenum format = AL_FORMAT_MONO16;
    if (channels == 2) {
        format = AL_FORMAT_STEREO16;
    } else if (channels != 1) {
        drwav_free(pcmFrames, nullptr);
        std::cerr << "Unsupported channel count: " << channels << std::endl;
        return;
    }

    ALuint buffer = 0;
    alGenBuffers(1, &buffer);
    if (alGetError() != AL_NO_ERROR) {
        drwav_free(pcmFrames, nullptr);
        std::cerr << "Failed to create OpenAL buffer" << std::endl;
        return;
    }

    const ALsizei bufferSize = static_cast<ALsizei>(totalFrameCount * channels * sizeof(drwav_int16));
    alBufferData(buffer, format, pcmFrames, bufferSize, static_cast<ALsizei>(sampleRate));
    if (alGetError() != AL_NO_ERROR) {
        alDeleteBuffers(1, &buffer);
        drwav_free(pcmFrames, nullptr);
        std::cerr << "Failed to upload audio buffer" << std::endl;
        return;
    }

    ALuint source = 0;
    alGenSources(1, &source);
    if (alGetError() != AL_NO_ERROR) {
        alDeleteBuffers(1, &buffer);
        drwav_free(pcmFrames, nullptr);
        std::cerr << "Failed to create OpenAL source" << std::endl;
        return;
    }

    alSourcei(source, AL_BUFFER, static_cast<ALint>(buffer));
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcef(source, AL_PITCH, pitch);
    alSourcef(source, AL_GAIN, gain);
    alSourcePlay(source);

    if (alGetError() != AL_NO_ERROR) {
        alDeleteSources(1, &source);
        alDeleteBuffers(1, &buffer);
        drwav_free(pcmFrames, nullptr);
        std::cerr << "Failed to start playback" << std::endl;
        return;
    }

    activeSources_.push_back({source, buffer, loop, pitch, gain});

    drwav_free(pcmFrames, nullptr);

}

void ecs::AudioSystem::stopAllSounds() {
    for (const auto& source : activeSources_) {
        alSourceStop(source.alSource);
        alSourcei(source.alSource, AL_BUFFER, 0);
        alDeleteSources(1, &source.alSource);
        alDeleteBuffers(1, &source.alBuffer);
    }
    activeSources_.clear();
}