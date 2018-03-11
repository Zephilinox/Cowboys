#pragma once

//STD
#include <string>

class AudioEngine
{
public:
	AudioEngine(const std::string& audio_path)
		: audio_path(audio_path)
	{};

	virtual ~AudioEngine() noexcept = default;

	virtual void play(const std::string& name, bool loop = false) = 0;

	const std::string audio_path;
};