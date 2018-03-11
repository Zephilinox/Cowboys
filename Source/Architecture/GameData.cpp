#include "GameData.hpp"

//STD
#include <assert.h>

//LIB
#include <ini_parser.hpp>

GameData::GameData(ASGE::Renderer* renderer, ASGE::Input* input, int width, int height)
	: renderer(renderer)
	, state_manager(this)
	, input_manager(input)
	, font_manager(renderer)
	, audio_manager("Resources/Sounds/")
	, network_manager(this) //todo: evaluate if needed
	, window_width(width)
	, window_height(height)
{
	assert(renderer);

	ini_parser ini("settings.ini");

	try
	{
		audio_manager.setEngineType(ini.get_int("AudioEngine"));
	}
	catch (std::runtime_error& e)
	{
		std::cout << "ERROR: Setting 'AudioEngine' not found in 'settings.ini'.\n";
		std::cout << "ERROR INFO: " << e.what() << "\n";
	}
}

ASGE::Renderer* GameData::getRenderer() const noexcept
{
	return renderer;
}

InputManager* GameData::getInputManager() noexcept
{
	return &input_manager;
}

FontManager* GameData::getFontManager() noexcept
{
	return &font_manager;
}

MessageQueue* GameData::getMessageQueue() noexcept
{
	return &message_queue;
}

StateManager* GameData::getStateManager() noexcept
{
	return &state_manager;
}

AudioManager* GameData::getAudioManager() noexcept
{
	return &audio_manager;
}

Rng* GameData::getRandomNumberGenerator() noexcept
{
	return &random_number_gen;
}

NetworkManager* GameData::getNetworkManager() noexcept
{
	return &network_manager;
}

int GameData::getWindowWidth() noexcept
{
	return window_width;
}

int GameData::getWindowHeight() noexcept
{
	return window_height;
}