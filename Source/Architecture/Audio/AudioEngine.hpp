#pragma once

//STD
#include <string>
#include <utility>

class AudioEngine
{
public:
	AudioEngine(std::string  audio_path)
		: audio_path(std::move(audio_path))
	{};

	virtual ~AudioEngine() noexcept = default;

	virtual void play(const std::string& name, bool loop = false) = 0;

	const std::string audio_path;
};