#pragma once

//STD
#include <string>
#include <memory>

//SELF
#include "../Audio/AudioEngine.hpp"

//This is a factory atm, might become more later though.
class AudioManager
{
public:
	enum Engine
	{
		None = 0,
		IrrKlang = 1,
		SFML = 2
	};

	AudioManager(Engine engine, const std::string audio_path);
	AudioManager(const std::string audio_path);
	AudioEngine* getAudioEngine() const noexcept;
	Engine getEngineType() const noexcept;

	void setEngineType(int engine);

	void play(const std::string& name, bool loop = false);
	void reset();

private:
	std::unique_ptr<AudioEngine> audio_engine;
	Engine engine;
	std::string audio_path;
};