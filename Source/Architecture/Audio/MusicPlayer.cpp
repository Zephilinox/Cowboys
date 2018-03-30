#include "MusicPlayer.hpp"

//STD
#include <fstream>

//LIBS
#include <jsoncons/json.hpp>

MusicPlayer::MusicPlayer()
{
	try
	{
		std::ifstream file("../../Resources/settings.json");
		jsoncons::json settings;
		file >> settings;
		muted = settings["Audio"]["Music Muted"].as_bool();
	}
	catch (std::runtime_error& e)
	{
		std::cout << "ERROR INFO: " << e.what() << "\n";
	}
}

void MusicPlayer::play(std::string music_name)
{
	if (music_name != current_music)
	{
		current_music = music_name;

		if (current_music_sound)
		{
			current_music_sound->stop();
		}

		current_music_sound = std::move(AudioLocator::get()->play("Music/" + music_name + ".wav", true));
		current_music_sound->setMuted(muted);
	}
}

std::string MusicPlayer::getCurrentMusicName()
{
	return current_music;
}

void MusicPlayer::toggleMuted()
{
	muted = !muted;
	current_music_sound->setMuted(muted);
}
