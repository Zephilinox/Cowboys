#pragma once

//STD
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

//LIB
#include <SFML/Audio.hpp>

//SELF
#include "AudioEngine.hpp"

class SoundSFML : public Sound
{
public:
	void stop() final
	{
		if (sound)
		{
			sound->stop();
		}
	}

	sf::Sound* sound;
};

class AudioEngineSFML : public AudioEngine
{
public:
	AudioEngineSFML(const std::string& audio_path);
	AudioEngineSFML() = default;

	std::unique_ptr<Sound> play(const std::string& name, bool loop = false) override final;
	
private:
	std::unordered_map<std::string, sf::SoundBuffer> buffers;
	std::vector<std::unique_ptr<sf::Sound>> sounds;
};