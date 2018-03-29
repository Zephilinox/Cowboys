#pragma once

//STD
#include <string>

//SELF
#include "AudioLocator.hpp"

class MusicPlayer
{
public:
	void play(std::string music_name);
	std::string getCurrentMusicName();

private:
	std::string current_music;
};