#include "AudioEngineIrrklang.hpp"

//STD
#include <cassert>
#include <string>

//LIB
#include <irrKlang.h>

AudioEngineIrrklang::AudioEngineIrrklang(const std::string& audio_path)	
	: AudioEngine(audio_path)
{
	audio_engine.reset(irrklang::createIrrKlangDevice());

	assert(audio_engine);
}

void AudioEngineIrrklang::play(const std::string& name, bool loop)
{
	audio_engine->play2D(std::string(audio_path + name).c_str(), loop);
}
