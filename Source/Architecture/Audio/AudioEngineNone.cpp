#include "AudioEngineNone.hpp"

AudioEngineNone::AudioEngineNone(const std::string& audio_path)
	: AudioEngine(audio_path)
{
}

void AudioEngineNone::play(const std::string& name, bool loop) noexcept
{
}
