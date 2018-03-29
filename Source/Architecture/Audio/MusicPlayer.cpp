#include "MusicPlayer.hpp"

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
	}
}

std::string MusicPlayer::getCurrentMusicName()
{
	return current_music;
}
