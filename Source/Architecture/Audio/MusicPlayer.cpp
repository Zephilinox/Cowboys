#include "MusicPlayer.hpp"

void MusicPlayer::play(std::string music_name)
{
	if (music_name != current_music)
	{
		//todo: create interface class for audio files so that we can delete them and stuff latah
		AudioLocator::set(AudioEngineType::IrrKlang);
		AudioLocator::get()->play("Music/" + music_name + ".wav", true);
	}
}

std::string MusicPlayer::getCurrentMusicName()
{
	return current_music;
}
