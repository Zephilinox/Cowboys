#pragma once

//STD
#include <memory>

//LIB
#include <irrKlang.h>

//SELF
#include "AudioEngine.hpp"

namespace irrklang
{
	class ISoundEngine;
}

class AudioEngineIrrklang : public AudioEngine
{
public:
	AudioEngineIrrklang(const std::string& audio_path);
	~AudioEngineIrrklang() noexcept override = default;

	void play(const std::string& name, bool loop = false) final;

private:
	std::unique_ptr<irrklang::ISoundEngine> audio_engine = nullptr;
};
