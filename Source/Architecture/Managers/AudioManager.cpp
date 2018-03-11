#include "AudioManager.hpp"

//STD
#include <functional>
#include <string>

//SELF
#include "../Audio/AudioEngineIrrklang.hpp"
#include "../Audio/AudioEngineSFML.hpp"
#include "../Audio/AudioEngineNone.hpp"

AudioManager::AudioManager(Engine engine, const std::string audio_path)
	: engine(engine)
	, audio_path(audio_path)
{
	setEngineType(engine);
}

AudioManager::AudioManager(const std::string audio_path)
	: audio_path(audio_path)
	, engine(Engine::None)
{
	setEngineType(Engine::None);
}

AudioEngine* AudioManager::getAudioEngine() const noexcept
{
	return audio_engine.get();
}

AudioManager::Engine AudioManager::getEngineType() const noexcept
{
	return engine;
}

void AudioManager::setEngineType(int engine)
{
	this->engine = static_cast<Engine>(engine);

	switch (engine)
	{
		//todo: handle switching to none if other engines fail to initialize? need to move from constructor to init funcs
		case Engine::IrrKlang:
			audio_engine = std::move(std::make_unique<AudioEngineIrrklang>(audio_path));
			break;
		case Engine::SFML:
			audio_engine = std::move(std::make_unique<AudioEngineSFML>(audio_path));
			break;
		case Engine::None:
		default:
			audio_engine = std::move(std::make_unique<AudioEngineNone>(audio_path));
			break;
	}
}

void AudioManager::play(const std::string& name, bool loop)
{
	//todo: pass path + name, don't worry about telling the audio engine the path?
	audio_engine->play(std::forward<const std::string&>(name), std::forward<bool>(loop));
}

void AudioManager::reset()
{
	setEngineType(engine);
}
